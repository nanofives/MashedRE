"""Plan + generate PortTop bindings for the audio-tick closure (PORT-TOP-AUDIOTICK-STOPS).
usage: gen.py <wt> <exeidx.json> <roots,comma> [--write]"""
import csv, re, sys, os, json, glob
wt, idxp, roots = sys.argv[1], sys.argv[2], [r.lower() for r in sys.argv[3].split(',')]
WRITE = '--write' in sys.argv
os.chdir(wt)
idx = json.load(open(idxp))
rows = {}
with open('hooks.csv', encoding='utf-8', errors='replace') as f:
    for r in csv.reader(f):
        if r and re.fullmatch(r'[0-9a-fA-F]{8}', r[0]):
            rows[r[0].lower()] = r
adm = {}
with open('re/match/ADMITTED.tsv', encoding='utf-8') as f:
    for r in csv.reader((l for l in f if not l.startswith('#')), delimiter='\t'):
        if r and r[0].startswith('0x'):
            adm[r[0][2:].lower()] = r
prov = {}
with open('re/match/PROVISIONAL.tsv', encoding='utf-8') as f:
    for r in csv.reader((l for l in f if not l.startswith('#')), delimiter='\t'):
        if r and r[0].startswith('0x') and len(r) > 10 and r[10] == 'provisional':
            prov[r[0][2:].lower()] = r
# PORT-TOP-AUDIO-RWA: L-126 provisional bodies bind on the knob path only for the rvas named here (kept narrow so the
# closure grows one reached stop at a time); c4=frida-replay rows (L-128) bind from their one re/match source.
PROV_BIND = {'005a6ea0', '005a6f30', '004627f0', '005aa0c0'}   # 004627f0 + its reached callee 005aa0c0: PORT-TOP-AUDIO-HWINIT-4627F0
def msrc(rva):
    r = rows.get(rva)
    if not r: return None, None
    if r[3] == 'C4' and 'c4=frida-replay' in r[8] and 'c4=match' not in r[8]:
        hits = glob.glob('re/match/*/%s.c' % rva)
        if len(hits) == 1:
            return hits[0].replace(os.sep, '/'), 'replay'
    if rva in PROV_BIND and rva in prov and os.path.exists(prov[rva][2]):
        return prov[rva][2], 'provisional'
    if 'c4=match' in r[8] and ('BYTE-IDENTICAL' in r[8] or 'match evidence: identical' in r[8] or ('100.0%' in r[8] and 'relocheck OK' in r[8])):
        for m in re.finditer(r'(re/match/[^\s;,()]+?\.c)', r[8]):
            if os.path.exists(m.group(1)):
                return m.group(1), 'match'
    if rva in adm and os.path.exists(adm[rva][2]):
        return adm[rva][2], 'admitted'
    return None, None
def callees(path, rva):
    t = open(path, encoding='utf-8', errors='replace').read()
    cs = sorted(set(c.lower() for c in re.findall(r'FUN_([0-9a-fA-F]{8})', t)) - {rva})
    local = set()
    for c in cs:
        if re.search(r'^[A-Za-z_][^\n;=(]*\bFUN_%s\s*\([^;{]*\)\s*\{' % c, t, re.I | re.M):
            local.add(c)
    return [c for c in cs if c not in local], sorted(local), t
CRT = {}
for _p in glob.glob('mashedmod/src/mashed_re/Match/PortTop_Crt*.cpp'):
    for _m in re.finditer(r'\b(MashedCrt_\w+?_([0-9a-f]{8}))\s*\(', open(_p, encoding='utf-8', errors='replace').read()):
        CRT.setdefault(_m.group(2), (_m.group(1), 'Match/' + os.path.basename(_p)))
def reuse(rva):
    """existing exe body usable as-is: a PortTop body, or a match-include body whose source is a leaf."""
    if rva in CRT:
        return CRT[rva]
    for sym, f, how in idx.get(rva, []):
        if f.startswith('Match/PortTop_') and how == 'match-include':
            return sym, f
    src, kind = msrc(rva)
    if src:
        cs, _, _ = callees(src, rva)
        if not cs:
            for sym, f, how in idx.get(rva, []):
                if how == 'match-include':
                    return sym, f
            r = rows.get(rva)
            if r and r[3] == 'C4':
                for sym, f, how in idx.get(rva, []):
                    if how == 'rh' and '::' not in sym:
                        return sym, f
    return None
EXTRA_PRE = {
    '004645c0': ['// The source declares its globals extern under #ifndef MATCH_PORT; the port reads them at their original addresses.',
                 '#define DAT_00604a10 (*(int *)0x00604a10u)', '#define DAT_0068f640 ((Slot *)0x0068f640u)'],
    '0045d250': ['// The source declares its globals extern under #ifndef MATCH_PORT; the port reads them at their original addresses.',
                 '#define DAT_007f0fd0 (*(int *)0x007f0fd0u)', '#define DAT_0088fbc0 (*(int *)0x0088fbc0u)'],
    '00407600': ['// MASHED_RE_PORT drops the source extern block (same as Match/MatchS01.cpp); the table is read at its original address.',
                 '#define MASHED_RE_PORT 1', '#define DAT_00639d80 ((Rec639d80 *)0x00639d80u)'],
}
EXTRA_PRE['005ab040'] = ['#pragma comment(lib, "winmm.lib")   // timeGetTime (the original imports it from WINMM.dll)']
EXTRA_IN = {'00407600': ['char *FUN_0047d150(int h);   // declared here: the source declares it only outside MASHED_RE_PORT']}
FORCE = {"00466b50", "0045d3f0", "0045d430"}
# PORT-TOP-AUDIO-RWA: re-runs regenerate every wrapper this generator wrote before (the previous plan.json 'bind' rows),
# so a callee that moved from stop to bind is rebound in all its callers instead of the old wrapper being reused.
_pp = os.path.join(os.path.dirname(idxp), 'plan.json')
OLD_PLAN = set()
if os.path.exists(_pp):
    OLD_PLAN = set(json.load(open(_pp)))
    FORCE |= {k for k, v in json.load(open(_pp)).items() if v and v[0] == 'bind'}
# PORT-TOP-AUDIO-HWINIT-4627F0: 0x004627f0 is a root (reached from 0x004669b0 under MASHED_OT_AUDIOINIT); wrappers new to
# its closure say so in their header, the older wrappers keep their text.
INIT_ROOT = '004627f0'
# PORT-TOP-AUDIO-RWA: re/match/s10/0045cbe0.c carries a second copy of 0x0045c4e0 (re/match/s10/0045cb20.c has the other).
# The copy sits under #ifndef MATCH_PORT_SHARED_0045c4e0 (DDK object unchanged, match.py --bytes BYTE-IDENTICAL), so the
# port defines that macro and binds the call to the one exe copy in the 0045cb20 TU (one exe body per rva).
STOP_FORCE = set()
LOCAL_SHARED = {("0045cbe0", "0045c4e0"): "PortTop_Local_0045cb20_0045c4e0"}
EXTRA_PRE["0045cbe0"] = ["#define MATCH_PORT_SHARED_0045c4e0 1   // the exe copy of 0x0045c4e0 is the one in Match/PortTop_0045cb20.cpp"]
# re/match/s10/0045c6b0.c carries 0x0045c640 (cast-only text difference from re/match/s10/0045c640.c): that copy is the exe body.
ALIAS = {"0045c640": ("PortTop_Bound_0045c640", "Match/PortTop_0045c6b0.cpp"), "004671a0": ("RwCore_004671a0", "RwCore/RwCore_004671a0.cpp")}
FORCE -= set(ALIAS)
# PORT-TOP-AUDIO-RWA: 0x004671a0 reuses the RW core body (RWC-FRAME-492E90, same source re/match/s11/004671a0.c): two
# ORIGTOP-only exe copies of one rva are not a class the lint takes.
LOCAL_AS_BOUND = {("0045c6b0", "0045c640")}
plan = {}   # rva -> ('reuse', sym, file) | ('bind', src, kind, callees, local) | ('stop',)
q = list(roots)
while q:
    rva = q.pop(0)
    if rva in plan: continue
    if rva in STOP_FORCE:
        plan[rva] = ('stop',); continue
    if rva in ALIAS:
        plan[rva] = ('reuse',) + ALIAS[rva]; continue
    ru = None if rva in FORCE else reuse(rva)
    if ru:
        plan[rva] = ('reuse', ru[0], ru[1]); continue
    src, kind = msrc(rva)
    if src:
        cs, local, _ = callees(src, rva)
        plan[rva] = ('bind', src, kind, cs, local)
        q.extend(cs)
    else:
        plan[rva] = ('stop',)
def sym(rva):
    p = plan[rva]
    if p[0] == 'reuse': return p[1]
    if p[0] == 'bind': return 'PortTop_Bound_%s' % rva
    return 'PortTop_AStop_%s' % rva
from collections import Counter
print(Counter(p[0] for p in plan.values()), Counter(p[2] for p in plan.values() if p[0] == 'bind'))
for rva in sorted(plan):
    p = plan[rva]
    print(rva, p[0], p[1] if p[0] != 'stop' else '', (rows.get(rva) or ['', '', '', '-'])[3])
if not WRITE: sys.exit(0)
stops = sorted(r for r, p in plan.items() if p[0] == 'stop')
out = []
for rva, p in sorted(plan.items()):
    if p[0] != 'bind': continue
    src, kind, cs, local = p[1], p[2], p[3], p[4]
    fn = 'mashedmod/src/mashed_re/Match/PortTop_%s.cpp' % rva
    keep_hdr = None
    if os.path.exists(fn):
        old = open(fn, encoding='utf-8').read()
        keep_hdr = [l for l in old.splitlines() if l.startswith('//')]
    r = rows.get(rva)
    m = None
    ev = 'match.py BYTE-IDENTICAL, hooks.csv C4 c4=match' if kind == 'match' else \
         ('L-126 admitted (re/match/ADMITTED.tsv, %s)' % adm[rva][6] if kind == 'admitted' else
          'hooks.csv C4 c4=frida-replay (L-128)' if kind == 'replay' else
          'L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4' if kind == 'provisional' else kind)
    L = []
    if keep_hdr and rva in ('00466b50', '0045d3f0', '0045d430'):
        L += [l for l in keep_hdr if not l.startswith('// RVA-LINT')]
        L.append('// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): callees rebound to matched / admitted exe bodies; the rest stay named stops')
        L.append('// (PortTop_AStop_<rva>, Match/PortTop_AudioStops.cpp).')
    elif rva not in OLD_PLAN:
        L.append('// PORT-TOP-AUDIO-HWINIT-4627F0 (2026-11-13): exe-only port body of rva 0x%s = %s' % (rva, src))
        L.append('// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT' % ev)
        L.append('// through the closure of 0x%s (sound engine HW init, called by 0x004669b0). Callees bind to exe bodies by name;' % INIT_ROOT)
        L.append('// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).')
    else:
        L.append('// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x%s = %s' % (rva, src))
        L.append('// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK' % ev)
        L.append('// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;')
        L.append('// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).')
    L.append('// RVA-LINT: ORIGTOP-ONLY')
    L.append('#ifdef MASHED_STANDALONE')
    L.append('#define MATCH_PORT 1')
    L.append('#define FUN_%s PortTop_Bound_%s' % (rva, rva))
    for c in cs:
        note = {'reuse': '   // exe body (%s)' % (plan[c][2] if plan[c][0] == 'reuse' else ''), 'bind': '', 'stop': '   // named stop'}[plan[c][0]]
        if c in STOP_FORCE:
            note = '   // named stop: re/match/s10/0045cbe0.c carries a second copy of 0x0045c4e0 (one exe body per rva)'
        L.append('#define FUN_%s %s%s' % (c, sym(c), note))
    for c in local:
        if (rva, c) in LOCAL_SHARED:
            L.append('#define FUN_%s %s   // the one exe copy (Match/PortTop_0045cb20.cpp); this source compiles its copy out' % (c, LOCAL_SHARED[(rva, c)]))
        elif (rva, c) in LOCAL_AS_BOUND:
            L.append('#define FUN_%s PortTop_Bound_%s   // the exe body of 0x%s: this source carries its only exe copy' % (c, c, c))
        else:
            L.append('#define FUN_%s PortTop_Local_%s_%s   // file-local in this source' % (c, rva, c))
    if 'FID_conflict__wprintf' in open(src, encoding='utf-8', errors='replace').read():
        L.append('#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)')
    for x in EXTRA_PRE.get(rva, []):
        L.append(x)
    L.append('extern "C" {')
    for x in EXTRA_IN.get(rva, []):
        L.append(x)
    L.append('#include "../../../../%s"' % src)
    L.append('}')
    L.append('#endif // MASHED_STANDALONE')
    open(fn, 'w', encoding='utf-8', newline='\n').write('\n'.join(L) + '\n')
    out.append(fn)
S = ['// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): named stops of the audio-tick closure (0x00466b50 / 0x0045d3f0 / 0x0045d430).',
     '// Each is a callee with no matched (c4=match) or L-126 admitted body yet. A stop logs and exits with code = rva',
     '// (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so a reached stop names the next body to bind.',
     '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#include <intrin.h>', 'extern "C" {',
     'void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp']
for s in stops:
    S.append('void PortTop_AStop_%s(void) { PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); }' % (s, s))
S += ['}', '#endif // MASHED_STANDALONE']
open('mashedmod/src/mashed_re/Match/PortTop_AudioStops.cpp', 'w', encoding='utf-8', newline='\n').write('\n'.join(S) + '\n')
json.dump({k: list(v) for k, v in plan.items()}, open(os.path.join(os.path.dirname(idxp), 'plan.json'), 'w'), indent=0)
print('wrote', len(out), 'bind files +', len(stops), 'stops')
for f in out: print('NEWFILE', f)

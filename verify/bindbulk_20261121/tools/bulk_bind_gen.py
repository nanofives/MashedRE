"""PORT-OT-RACE: plan + generate PortTop bindings for the race-load closure under 0x0040d270.
Reuses the definitions of verify/portorigtop_20261113/tools/audiotick_bind_gen.py (msrc / callees / reuse / ALIAS /
EXTRA_PRE / PROV_BIND), with its own roots, its own plan file (rltools/plan_rl.json) and its own stop file
(Match/PortTop_BulkStops.cpp). Audio-plan wrappers that already exist are reused as exe bodies, never rewritten.
usage: py -3.12 rl_bind_gen.py <wt> <roots,comma> [--write] [--counting <rva,rva>]
Run from the worktree root. --counting names roots kept as checkpoint-1 counting stand-ins (not planned)."""
import os, sys, json, re
from collections import Counter
HERE = os.path.dirname(os.path.abspath(__file__))
wt, roots_arg = sys.argv[1], sys.argv[2]
WRITE_RL = '--write' in sys.argv
roots = [r.lower().replace('0x', '') for r in roots_arg.split(',') if r]
AUD = os.path.join(wt, 'verify', 'portorigtop_20261113', 'tools', 'audiotick_bind_gen.py')
src = open(AUD, encoding='utf-8').read()
prefix = src[:src.index("plan = {}   # rva ->")]
sys.argv = [AUD, wt, os.path.join(HERE, 'exeidx.json'), ','.join(roots)]
g = {'__name__': 'audgen', '__file__': AUD}
exec(compile(prefix, AUD, 'exec'), g)
msrc, callees, reuse, ALIAS, EXTRA_PRE, EXTRA_IN = g['msrc'], g['callees'], g['reuse'], g['ALIAS'], g['EXTRA_PRE'], g['EXTRA_IN']
LOCAL_SHARED, LOCAL_AS_BOUND, adm, rows = g['LOCAL_SHARED'], g['LOCAL_AS_BOUND'], g['adm'], g['rows']
idx = g['idx']
FORCE = set()   # RACE-BIND-C4-BULK: an rva that already has an exe body is reused, never bound twice
ROOTS = set(roots)
import glob as _gl
def existing_stop(rva):
    pat = re.compile(r'\b(PortTop_\w*Stop\w*_%s)\s*\(' % rva)
    for f in _gl.glob('mashedmod/src/mashed_re/Match/PortTop_*.cpp'):
        m = pat.search(open(f, encoding='utf-8', errors='replace').read())
        if m: return m.group(1)
    return None
# The exe's file layer on the top-down route (no RtFSManager): the piz mount stack of Match/PortTop_00402750_Pieces.cpp
# (PIZ_FS) and the RW stream open / close bindings over it (Match/PortTop_RwStream04F.cpp, PORT-TOP-04F). The matched
# RwStreamRead / FindChunk bodies (RwCore) read the type-3 memory stream the open binding returns.
ALIAS = dict(ALIAS)
ALIAS['00495280'] = ('PortTop_PizFs_Mount_00495280', 'Match/PortTop_00402750_Pieces.cpp')
ALIAS['004952f0'] = ('PortTop_PizFs_Unmount_004952f0', 'Match/PortTop_00402750_Pieces.cpp')
ALIAS['004cc230'] = ('PortTop_RwStreamOpen_004cc230', 'Match/PortTop_RwStream04F.cpp')
ALIAS['004cc160'] = ('PortTop_RwStreamClose_004cc160', 'Match/PortTop_RwStream04F.cpp')
ALIAS['00550b00'] = ('RwCoreBind_FsFileExists_00550b00', 'RwCore/RwCore_Bind_00550b00.cpp')
# Checkpoint 3: 0x0042fe30 already has one exe body compiled from the same matched source (re/match/s07/0042fe30.c,
# MATCH-S07 TU in exe_sources.rsp, its one callee 0x0042f6a0 bound to GetRaceSubMode). reuse() skips it because the
# source is not a leaf; binding a second copy would be a dual copy of the rva in the exe.
ALIAS['0042fe30'] = ('RaceEndFlagIfEndMode', 'Match/MatchS07_0042fe30.cpp')
# Checkpoint 3 (lead decision (b), 2026-11-16): 0x00458c70 stays the named counting stand-in. Its bound body faulted in
# RwCore_004c15c0+0x85 (original 0x004c1645): [RwGlobals+0xbc] is 0 on the librw RwGlobals record of this arm.
ALIAS['00458c70'] = ('PortTop_StandInRL_00458c70', 'Match/PortTop_StandInsRL.cpp')
# Exe bodies that exist but are not match-include PortTop bodies: an rh or extern "C" body compiled into the exe for the
# same rva. Reused when the rva is matched (C4) and the body is a C function (no ::).
def reuse_any(rva):
    r = rows.get(rva)
    for sym, f, how in idx.get(rva, []):
        if how == 'match-include':
            return sym, f
    return None
# Matched sources that do not compile as C++ under MATCH_PORT (the exe build compiles .cpp TUs only): named stops, with
# the first compiler error recorded in rltools/cpp_unclean.txt (rva <tab> reason).
UNCLEAN = {}
_up = os.path.join(HERE, 'cpp_unclean.txt')
if os.path.exists(_up):
    for l in open(_up, encoding='utf-8'):
        if l.strip() and not l.startswith('#'):
            k, _, why = l.rstrip(chr(13) + chr(10)).partition(chr(9))
            UNCLEAN[k.strip().lower()] = why.strip()
# Checkpoint 3: a callee whose exe body takes a different argument list than the caller's source declares binds to the
# existing forwarder that adapts it (same binding as the caller's other exe TU).
CALLEE_OVERRIDE = {
    ('0042a5d0', '004b3bf0'): ('MatchS06_0042a5d0_Call004b3bf0',
                               'Match/MatchS06_0042a5d0.cpp: the source passes 1 argument, Orig_004b3bf0 takes 3; '
                               'declared deviation there, 2nd / 3rd passed as 0'),
}
NOT_LOCAL = {('0047bf70', '0047be80'), ('00411d90', '00411350')}   # 00411d90.c: 00411350 is only forward-declared under MATCH_PORT   # re/match/s13/0047bf70.c: 0x0047be80 HELD (86.4%), its copy is #ifndef MATCH_PORT
plan = {}
q = list(roots)
while q:
    rva = q.pop(0)
    if rva in plan:
        continue
    if rva in UNCLEAN:
        plan[rva] = ('stop', 'cpp-unclean: ' + UNCLEAN[rva]); continue
    if rva in ALIAS:
        plan[rva] = ('reuse',) + ALIAS[rva]; continue
    ru = None if rva in FORCE else reuse(rva)
    if not ru and rva not in FORCE and os.path.exists('mashedmod/src/mashed_re/RwCore/RwCore_%s.cpp' % rva):
        ru = ('RwCore_%s' % rva, 'RwCore/RwCore_%s.cpp' % rva)   # the RW core's one exe body per rva (L-124)
    if ru:
        plan[rva] = ('reuse', ru[0], ru[1]); continue
    s, kind = msrc(rva)
    if s and rva not in ROOTS:
        plan[rva] = ('stop', 'outside the bulk cp1 list (RVA ownership: bind only the listed rvas), source %s' % s); continue
    if s:
        cs, local, _ = callees(s, rva)
        # a callee defined only under #ifndef MATCH_PORT (a HELD copy the DDK needs) is external on the port
        for c in [c for c in local if (rva, c) in NOT_LOCAL]:
            local = [x for x in local if x != c]; cs = sorted(set(cs) | {c})
        plan[rva] = ('bind', s, kind, cs, local)
        q.extend(cs)
        continue
    # no matched / admitted source: an existing match-include exe body (a matched source compiled elsewhere) is reused;
    # a hand-path body (rh only) is not bound on the top-down route, the rva is a named stop.
    ru = reuse_any(rva)
    if ru and any(h == 'match-include' for _, _, h in idx.get(rva, [])):
        plan[rva] = ('reuse', ru[0], ru[1]); continue
    plan[rva] = ('stop',)
# RACE-BIND-C4-BULK: one body per RVA (scripts/lint_rva_bodies.py: two ORIG_TOP bodies of one RVA are a real duplicate).
FUNDEF = r'^[A-Za-z_][^\n;=(]*\bFUN_%s\s*\([^;{]*\)\s*\{'
REFUSED = {}   # rva -> reason (a root that ends up unbound)
_inc = {}      # source path -> [(tu, text)] over every existing exe TU (generated or not)
for _l in open('mashedmod/exe_sources.rsp', encoding='utf-8'):
    _tu = _l.strip().strip('"').replace(chr(92), '/')
    if not _tu.endswith('.cpp'):
        continue
    _p = os.path.join('mashedmod/src/mashed_re', _tu)
    if not os.path.exists(_p):
        continue
    _t = open(_p, encoding='utf-8', errors='replace').read()
    for _m in re.finditer(r'#include\s+"(?:\.\./)+(re/match/[^"]+)"', _t):
        _inc.setdefault(_m.group(1), []).append((_tu, _t))
# 1. the matched source is already compiled by an existing exe TU: that TU carries the rva, a second copy would duplicate it
for rva, p in list(plan.items()):
    if p[0] != 'bind':
        continue
    for tu, t in _inc.get(p[1], []):
        m = re.search(r'#define\s+FUN_%s\s+(\w+)' % rva, t)
        plan[rva] = ('reuse', m.group(1) if m else 'FUN_%s' % rva, tu)
        if rva in ROOTS:
            REFUSED[rva] = 'already carried by %s (same source %s)' % (tu, p[1])
        break
# 2. several planned binds share one source: the lowest rva carries it, the others are its members
GROUP = {}     # primary -> [members]
_bysrc = {}
for rva, p in sorted(plan.items()):
    if p[0] == 'bind':
        _bysrc.setdefault(p[1], []).append(rva)
for s_, rv in _bysrc.items():
    if len(rv) > 1:
        GROUP[rv[0]] = rv[1:]
        for m_ in rv[1:]:
            plan[m_] = ('reuse', 'PortTop_Bound_%s' % m_, 'Match/PortTop_%s.cpp' % rv[0])
# 3. a planned bind whose source carries a file-local copy of another planned bind: one body per RVA, the separate body is dropped
for rva, p in list(plan.items()):
    if p[0] != 'bind':
        continue
    for c in p[4]:
        if (rva, c) in LOCAL_SHARED or (rva, c) in LOCAL_AS_BOUND or c in GROUP.get(rva, []):
            continue
        if plan.get(c, ('x',))[0] == 'bind':
            plan[c] = ('stop', 'file-local copy of this rva in the source of 0x%s (one body per RVA)' % rva)
            if c in ROOTS:
                REFUSED[c] = 'file-local static copy inside the source of 0x%s; a second body of the rva would fail rva-lint' % rva
def sym(rva):
    p = plan[rva]
    if p[0] == 'reuse': return p[1]
    if p[0] == 'bind': return 'PortTop_Bound_%s' % rva
    return existing_stop(rva) or 'PortTop_BulkStop_%s' % rva
print(Counter(p[0] for p in plan.values()), Counter(p[2] for p in plan.values() if p[0] == 'bind'))
for rva in sorted(plan):
    p = plan[rva]
    r = rows.get(rva) or ['', '', '', '-']
    extra = p[1] if len(p) > 1 else ''
    print(rva, p[0], extra, r[3], (r[1] if len(r) > 1 else ''))
if not WRITE_RL:
    sys.exit(0)
out = []
KEEP = []
for rva, p in sorted(plan.items()):
    if p[0] != 'bind':
        continue
    s, kind, cs, local = p[1], p[2], p[3], p[4]
    fn = 'mashedmod/src/mashed_re/Match/PortTop_%s.cpp' % rva
    if os.path.exists(fn):
        print('REFUSE existing non-RL wrapper', fn); sys.exit(2)
    ev = 'match.py BYTE-IDENTICAL, hooks.csv C4 c4=match' if kind == 'match' else \
         ('L-126 admitted (re/match/ADMITTED.tsv, %s)' % adm[rva][6] if kind == 'admitted' else
          'hooks.csv C4 c4=frida-replay (L-128)' if kind == 'replay' else
          'L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4' if kind == 'provisional' else kind)
    L = ['// RACE-BIND-C4-BULK (2026-11-21): exe-only port body of rva 0x%s = %s' % (rva, s),
         '// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD' % ev,
         '// through the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py). Callees bind to exe bodies',
         '// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_BulkStops.cpp).',
         '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#define MATCH_PORT 1',
         '#define FUN_%s PortTop_Bound_%s' % (rva, rva)]
    for c in cs:
        if (rva, c) in CALLEE_OVERRIDE:
            L.append('#define FUN_%s %s   // %s' % ((c,) + CALLEE_OVERRIDE[(rva, c)]))
            continue
        note = {'reuse': '   // exe body (%s)' % (plan[c][2] if plan[c][0] == 'reuse' else ''), 'bind': '', 'stop': '   // named stop'}[plan[c][0]]
        L.append('#define FUN_%s %s%s' % (c, sym(c), note))
    for c in local:
        if (rva, c) in LOCAL_SHARED:
            L.append('#define FUN_%s %s   // the one exe copy' % (c, LOCAL_SHARED[(rva, c)]))
        elif (rva, c) in LOCAL_AS_BOUND:
            L.append('#define FUN_%s PortTop_Bound_%s   // this source carries its only exe copy' % (c, c))
        else:
            L.append('#define FUN_%s PortTop_Local_%s_%s   // file-local in this source' % (c, rva, c))
    _stxt = open(s, encoding='utf-8', errors='replace').read()
    _done = {rva} | set(cs) | set(local)
    if not re.search(FUNDEF % rva, _stxt, re.I | re.M):
        _th = re.findall(r'^[A-Za-z_][^\n;=(]*\b(thunk_FUN_[0-9a-fA-F]{8})\s*\([^;{]*\)\s*\{', _stxt, re.I | re.M)
        if len(_th) == 1:
            L.append('#define %s PortTop_Bound_%s   // the source names this body %s (a jump thunk)' % (_th[0], rva, _th[0]))
    for c in GROUP.get(rva, []):
        L.append('#define FUN_%s PortTop_Bound_%s   // member of this source, its only exe copy' % (c, c))
        _done.add(c)
    for c in sorted(set(x.lower() for x in re.findall(r'FUN_([0-9a-fA-F]{8})', _stxt)) - _done):
        if re.search(FUNDEF % c, _stxt, re.I | re.M):
            L.append('#define FUN_%s PortTop_Local_%s_%s   // defined in this source, not a bound rva' % (c, rva, c))
    if 'FID_conflict__wprintf' in open(s, encoding='utf-8', errors='replace').read():
        L.append('#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)')
    L += EXTRA_PRE.get(rva, [])
    L.append('extern "C" {')
    L += EXTRA_IN.get(rva, [])
    L += ['#include "../../../../%s"' % s, '}']
    # a body the matched source defines `static` has no external symbol and no caller in its own TU, so the compiler
    # drops it: take its address once (the object build then promotes it, ddkobj.py "externalized static")
    for c in [rva] + GROUP.get(rva, []):
        KEEP.append(c)
        L.append('extern "C" { void *PortTop_Keep_%s = (void *)PortTop_Bound_%s; }   // anchor: BulkRefs /include%s' % (
            c, c, ' (the source defines it static)' if re.search(r'^\s*static\b[^\n;{(]*\bFUN_%s\s*\(' % c, _stxt, re.I | re.M) else ''))
    L.append('#endif // MASHED_STANDALONE')
    open(fn, 'w', encoding='utf-8', newline='\n').write('\n'.join(L) + '\n')
    out.append(fn)
stops = sorted(r for r, p in plan.items() if p[0] == 'stop')
# Return type of each stop as its callers' sources declare it. A stop declared float / double anywhere is exit-only in
# count mode too (a cdecl return of 0 in eax would leave the caller's x87 stack one short).
def ret_types(stop):
    ts = set()
    for rva, p in plan.items():
        if p[0] != 'bind' or stop not in p[3]:
            continue
        t = open(p[1], encoding='utf-8', errors='replace').read()
        for m in re.finditer(r'^\s*(?:extern\s+)?([A-Za-z_][\w\s\*]*?)\s*\bFUN_%s\s*\(' % stop, t, re.M | re.I):
            ts.add(' '.join(m.group(1).split()))
    return ts
def callers(stop):
    return sorted(r for r, p in plan.items() if p[0] == 'bind' and stop in p[3])
S = ['// RACE-BIND-C4-BULK (2026-11-21): named stops of the race-load census (verify/bindbulk_20261121/tools/bulk_bind_gen.py).',
     '// Each is a callee with no matched (c4=match) or L-126 admitted body yet. None is a body of its rva.',
     '// Default: a reached stop logs and exits with code = rva (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so it',
     '// names the next body to bind. MASHED_OT_RL_STOPS=count: a stop logs its first call and returns 0 (eax), so the load',
     '// runs on past it with that callee\'s effects missing; stops declared float / double by a caller always exit.',
     '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#include <intrin.h>', '#include <cstdio>', '#include <cstdlib>',
     'namespace {', 'int BulkStopCount() {', '    static int s = -1;',
     '    if (s < 0) { const char* const v = std::getenv("MASHED_OT_RL_STOPS"); s = (v && v[0] == \'c\') ? 1 : 0; }',
     '    return s;', '}', 'unsigned g_bulkStopHits = 0;',
     'void BulkStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {',
     '    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {',
     '        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: race-load stop (RACE-BIND-C4-BULK), call %u, callers %s, return %p, %s\\n", rva, n, callers, ret,',
     '                     exits ? "exits" : "returns 0 (MASHED_OT_RL_STOPS=count)");', '        std::fclose(f);', '    }', '}', '} // namespace',
     'extern "C" {',
     'void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp',
     'unsigned PortTop_Bulk_StopHits(void) { return g_bulkStopHits; }']
for s_ in stops:
    rt = ret_types(s_)
    fl = any(('float' in x or 'double' in x) for x in rt)
    cl = ' '.join('0x' + c for c in callers(s_))
    S.append('// stop rva %s: declared %s by %s%s' % (s_, ' / '.join(sorted(rt)) or '(none found)', cl,
             ('; ' + plan[s_][1]) if len(plan[s_]) > 1 else '; no matched or admitted source'))
    if fl:
        S.append('int PortTop_BulkStop_%s(void) { BulkStopLog(0x%su, "%s", 1u, _ReturnAddress(), 1); PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % (s_, s_, cl, s_))
    else:
        S.append('int PortTop_BulkStop_%s(void) { static unsigned n = 0; ++n; ++g_bulkStopHits; const int ex = !BulkStopCount();' % s_)
        S.append('    if (n == 1 || ex) BulkStopLog(0x%su, "%s", n, _ReturnAddress(), ex);' % (s_, cl))
        S.append('    if (ex) PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % s_)
S += ['}', '#endif // MASHED_STANDALONE']
open('mashedmod/src/mashed_re/Match/PortTop_BulkStops.cpp', 'w', encoding='utf-8', newline='\n').write('\n'.join(S) + '\n')
json.dump({k: list(v) for k, v in plan.items()}, open(os.path.join(HERE, 'plan_cp.json'), 'w'), indent=0)
json.dump({'refused': REFUSED, 'group': GROUP, 'keep': sorted(set(KEEP)), 'unclean': UNCLEAN}, open(os.path.join(HERE, 'disposition_cp.json'), 'w'), indent=0)
with open(os.path.join(HERE, 'newfiles_cp.txt'), 'w') as f:
    f.write('\n'.join(out + ['mashedmod/src/mashed_re/Match/PortTop_BulkStops.cpp']) + '\n')
print('wrote', len(out), 'bind files +', len(stops), 'stops')

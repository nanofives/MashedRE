"""PORT-OT-RACE: plan + generate PortTop bindings for the race-load closure under 0x0040d270.
Reuses the definitions of verify/portorigtop_20261113/tools/audiotick_bind_gen.py (msrc / callees / reuse / ALIAS /
EXTRA_PRE / PROV_BIND), with its own roots, its own plan file (rltools/plan_rl.json) and its own stop file
(Match/PortTop_RaceLoadStops.cpp). Audio-plan wrappers that already exist are reused as exe bodies, never rewritten.
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
FORCE = set(roots)
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
NOT_LOCAL = {('0047bf70', '0047be80')}   # re/match/s13/0047bf70.c: 0x0047be80 HELD (86.4%), its copy is #ifndef MATCH_PORT
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
def sym(rva):
    p = plan[rva]
    if p[0] == 'reuse': return p[1]
    if p[0] == 'bind': return 'PortTop_Bound_%s' % rva
    return 'PortTop_RLStop_%s' % rva
print(Counter(p[0] for p in plan.values()), Counter(p[2] for p in plan.values() if p[0] == 'bind'))
for rva in sorted(plan):
    p = plan[rva]
    r = rows.get(rva) or ['', '', '', '-']
    extra = p[1] if len(p) > 1 else ''
    print(rva, p[0], extra, r[3], (r[1] if len(r) > 1 else ''))
if not WRITE_RL:
    sys.exit(0)
out = []
# remove this generator's wrappers whose rva is no longer bound (and their exe_sources.rsp lines)
import glob as _g
_rsp = 'mashedmod/exe_sources.rsp'
_rr = open(_rsp, 'rb').read(); _nl = chr(13) + chr(10) if b'\r\n' in _rr else chr(10)
_rl = _rr.decode().split(_nl)
for f in _g.glob('mashedmod/src/mashed_re/Match/PortTop_*.cpp'):
    m = re.search(r'PortTop_([0-9a-f]{8})\.cpp$', f)
    if not m: continue
    txt = open(f, encoding='utf-8', errors='replace').read()
    if not txt.startswith('// PORT-OT-RACE (2026-11-16): exe-only port body'): continue
    r_ = m.group(1)
    if plan.get(r_, ('x',))[0] != 'bind':
        os.remove(f)
        _rl = [l for l in _rl if l.strip() != '"Match' + chr(92) + 'PortTop_%s.cpp"' % r_]
        print('REMOVED', f)
open(_rsp, 'wb').write(_nl.join(_rl).encode())
for rva, p in sorted(plan.items()):
    if p[0] != 'bind':
        continue
    s, kind, cs, local = p[1], p[2], p[3], p[4]
    fn = 'mashedmod/src/mashed_re/Match/PortTop_%s.cpp' % rva
    if os.path.exists(fn) and 'PORT-OT-RACE' not in open(fn, encoding='utf-8').read():
        print('REFUSE existing non-RL wrapper', fn); sys.exit(2)
    ev = 'match.py BYTE-IDENTICAL, hooks.csv C4 c4=match' if kind == 'match' else \
         ('L-126 admitted (re/match/ADMITTED.tsv, %s)' % adm[rva][6] if kind == 'admitted' else
          'hooks.csv C4 c4=frida-replay (L-128)' if kind == 'replay' else
          'L-126 provisional (re/match/PROVISIONAL.tsv), knob path only, not C4' if kind == 'provisional' else kind)
    L = ['// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x%s = %s' % (rva, s),
         '// (%s). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD' % ev,
         '// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies',
         '// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).',
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
    if 'FID_conflict__wprintf' in open(s, encoding='utf-8', errors='replace').read():
        L.append('#define FID_conflict__wprintf PortTop_Printf04G   // printf -> log (Match/PortTop_0040bbb0.cpp)')
    L += EXTRA_PRE.get(rva, [])
    L.append('extern "C" {')
    L += EXTRA_IN.get(rva, [])
    L += ['#include "../../../../%s"' % s, '}', '#endif // MASHED_STANDALONE']
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
S = ['// PORT-OT-RACE (2026-11-16): named stops of the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py).',
     '// Each is a callee with no matched (c4=match) or L-126 admitted body yet. None is a body of its rva.',
     '// Default: a reached stop logs and exits with code = rva (PortTop_AudioCalleeStop, Match/PortTop_00492370.cpp), so it',
     '// names the next body to bind. MASHED_OT_RL_STOPS=count: a stop logs its first call and returns 0 (eax), so the load',
     '// runs on past it with that callee\'s effects missing; stops declared float / double by a caller always exit.',
     '// RVA-LINT: ORIGTOP-ONLY', '#ifdef MASHED_STANDALONE', '#include <intrin.h>', '#include <cstdio>', '#include <cstdlib>',
     'namespace {', 'int RlStopCount() {', '    static int s = -1;',
     '    if (s < 0) { const char* const v = std::getenv("MASHED_OT_RL_STOPS"); s = (v && v[0] == \'c\') ? 1 : 0; }',
     '    return s;', '}', 'unsigned g_rlStopHits = 0;',
     'void RlStopLog(unsigned rva, const char* callers, unsigned n, void* ret, int exits) {',
     '    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {',
     '        std::fprintf(f, "[ORIG_TOP] rva 0x%08X: race-load stop (PORT-OT-RACE), call %u, callers %s, return %p, %s\\n", rva, n, callers, ret,',
     '                     exits ? "exits" : "returns 0 (MASHED_OT_RL_STOPS=count)");', '        std::fclose(f);', '    }', '}', '} // namespace',
     'extern "C" {',
     'void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp',
     'unsigned PortTop_RL_StopHits(void) { return g_rlStopHits; }']
for s_ in stops:
    rt = ret_types(s_)
    fl = any(('float' in x or 'double' in x) for x in rt)
    cl = ' '.join('0x' + c for c in callers(s_))
    S.append('// stop rva %s: declared %s by %s%s' % (s_, ' / '.join(sorted(rt)) or '(none found)', cl,
             ('; ' + plan[s_][1]) if len(plan[s_]) > 1 else '; no matched or admitted source'))
    if fl:
        S.append('int PortTop_RLStop_%s(void) { RlStopLog(0x%su, "%s", 1u, _ReturnAddress(), 1); PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % (s_, s_, cl, s_))
    else:
        S.append('int PortTop_RLStop_%s(void) { static unsigned n = 0; ++n; ++g_rlStopHits; const int ex = !RlStopCount();' % s_)
        S.append('    if (n == 1 || ex) RlStopLog(0x%su, "%s", n, _ReturnAddress(), ex);' % (s_, cl))
        S.append('    if (ex) PortTop_AudioCalleeStop(0x%su, 0u, _ReturnAddress()); return 0; }' % s_)
S += ['}', '#endif // MASHED_STANDALONE']
open('mashedmod/src/mashed_re/Match/PortTop_RaceLoadStops.cpp', 'w', encoding='utf-8', newline='\n').write('\n'.join(S) + '\n')
json.dump({k: list(v) for k, v in plan.items()}, open(os.path.join(HERE, 'plan_rl.json'), 'w'), indent=0)
with open(os.path.join(HERE, 'newfiles_rl.txt'), 'w') as f:
    f.write('\n'.join(out + ['mashedmod/src/mashed_re/Match/PortTop_RaceLoadStops.cpp']) + '\n')
print('wrote', len(out), 'bind files +', len(stops), 'stops')

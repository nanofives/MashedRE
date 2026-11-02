"""D4-DC-77b0-RUNTIME-PARKED, exe side: read-only Frida probe on mashed_re.exe (no build change).

Spawns the exe with run_arms.py's base_env (H_DEF recipe: Training, MASHED_ROUND=1, MASHED_ROUND_RULE, MASHED_GAME_MODE=10,
MASHED_DETERMINISTIC=1, MASHED_DET_FRAMES) and hooks, by the build's own .map:
  T  Ai_Standalone_Tick entry      B  AiPreTickRubberBand entry      S  TrackRenderer::StartRound entry
Each call reads 0x0063ba8c, s_roundEnd7 (byte), the four cells *(*(0x005f2770) + 0x34 + 4v), 0x00897ffc, 0x0067e9fc,
0x007f0fd0. Rows are run-length per (kind, tuple). Hardware WRITE watchpoints on 0x0063ba8c / 0x00897ffc / 0x005f275c /
0x0067e9fc log pc (resolved to a map symbol here) and the four dwords. Nothing is written to the process.

usage: py -3.12 exe_vsprobe.py --exe <mashed_re.exe> --root <private root> --out <prefix> [--rule 0] [--frames 14400]
"""
import argparse, bisect, os, re, subprocess, sys, time
import frida

AGENT = r'''
'use strict';
let DELTA = 0;
const R = { runs:[], key:{}, cur:{}, n:0, w:[], nw:0, err:null };
function A(va){ return ptr(va); }                       // pad addresses are absolute (image pad owns the RVA range)
function cells(){
  const p = A(0x005f2770).readU32() >>> 0;
  if (p === 0) return ['', '', '', ''];
  return [0, 1, 2, 3].map(function(v){ return ptr(p + 0x34 + 4 * v).readS32(); });
}
function tup(re7){ return [A(0x0063ba8c).readS32(), re7.readU8()].concat(cells()).concat([A(0x00897ffc).readS32(), A(0x0067e9fc).readS32(), A(0x007f0fd0).readS32()]); }
function rec(kind, re7){
  R.n++;
  const t = tup(re7), k = t.join(',');
  if (R.key[kind] !== k) {
    if (R.cur[kind]) R.runs.push(R.cur[kind].join(','));
    R.key[kind] = k; R.cur[kind] = [kind, R.n, 1, R.n].concat(t);
  } else { R.cur[kind][2]++; R.cur[kind][3] = R.n; }
}
rpc.exports = {
  arm: function(pref, tick, pre, start, re7va){
    try {
      const m = Process.findModuleByName('mashed_re.exe');
      DELTA = m.base.toUInt32() - pref;
      const re7 = ptr(re7va + DELTA);
      const at = function(va, kind){ Interceptor.attach(ptr(va + DELTA), { onEnter(){ try { rec(kind, re7); } catch(e){ if (!R.err) R.err = kind + ' ' + e; } } }); };
      at(tick, 'T'); at(pre, 'B'); at(start, 'S');
      const W = [A(0x0063ba8c), A(0x00897ffc), A(0x005f275c), A(0x0067e9fc)];
      Process.setExceptionHandler(function(d){
        if (d.type !== 'single-step') return false;
        try {
          R.nw++;
          if (R.w.length < 200000)
            R.w.push([R.nw, R.n, (d.context.pc.toUInt32() - DELTA) >>> 0].concat(W.map(function(a){ return a.readS32(); })).join(','));
        } catch(e){ if (!R.err) R.err = 'hit ' + e; }
        return true;
      });
      let n = 0;
      Process.enumerateThreads().forEach(function(t){
        for (let k = 0; k < 4; k++) { try { t.setHardwareWatchpoint(k, W[k], 4, 'w'); if (k === 0) n++; } catch(e){ if (!R.err) R.err = 'arm ' + e.message; } }
      });
      return 'armed delta=' + DELTA + ' threads=' + n;
    } catch(e){ return 'ERR ' + e; }
  },
  drain: function(final){
    const r = R.runs; R.runs = [];
    if (final) { for (const k in R.cur) if (R.cur[k]) r.push(R.cur[k].join(',')); R.cur = {}; R.key = {}; }
    const w = R.w; R.w = [];
    const cur = []; for (const k in R.cur) if (R.cur[k]) cur.push(R.cur[k].join(','));
    return [r, w, '' + R.err + ' calls=' + R.n + ' hits=' + R.nw, cur];
  }
};
'''


def load_map(path):
    syms, pref = [], 0x10000
    rx = re.compile(r'^\s*000[1-9]:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+(f\s+)?')
    for ln in open(path, encoding='latin-1'):
        if 'Preferred load address is' in ln:
            pref = int(ln.split()[-1], 16)
        m = rx.match(ln)
        if m: syms.append((int(m.group(2), 16), m.group(1)))
    syms.sort()
    return pref, syms


def find(syms, name):
    for a, s in syms:
        if name in s: return a
    raise SystemExit('symbol not found: ' + name)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', required=True); ap.add_argument('--root', required=True); ap.add_argument('--out', required=True)
    ap.add_argument('--rule', default='0'); ap.add_argument('--frames', type=int, default=14400)
    ap.add_argument('--track', default='Training'); ap.add_argument('--limit', type=int, default=1200)
    ap.add_argument('--env', action='append', default=[], help='extra K=V for the exe (e.g. MASHED_R7_INPUT_PRESS=150)')
    a = ap.parse_args()
    pref, syms = load_map(os.path.splitext(a.exe)[0] + '.map')
    addrs = [find(syms, '?Ai_Standalone_Tick@Ai@@'), find(syms, '?AiPreTickRubberBand@'),
             find(syms, '?StartRound@TrackRenderer@'), find(syms, '?s_roundEnd7@')]
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env.update(MASHED_ROOT=a.root, MASHED_MUTE='1', MASHED_TRACK_VIEW=a.track, MASHED_CAR='1', MASHED_ROUND='1',
               MASHED_ROUND_RULE=a.rule, MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
               MASHED_DET_FRAMES=str(a.frames))
    for kv in a.env:
        k, v = kv.split('=', 1); env[k] = v
    pid = frida.spawn([a.exe], cwd=a.root, env=env)
    print('spawned pid', pid, 'map pref', hex(pref), [hex(x) for x in addrs], flush=True)
    s = frida.attach(pid); sc = s.create_script(AGENT); sc.load(); E = sc.exports_sync
    print(E.arm(pref, addrs[0], addrs[1], addrs[2], addrs[3]), flush=True)
    runs, hits, cur = [], [], []
    frida.resume(pid)
    t0, stat = time.time(), ''
    gone = False
    while time.time() - t0 < a.limit:
        time.sleep(2)
        try:
            r, w, stat, cur = E.drain(False); runs += r; hits += w
        except Exception as e:
            gone = True; runs += cur; print('drain stopped (process exit):', e, flush=True); break
    if not gone:
        try:
            r, w, stat, _c = E.drain(True); runs += r; hits += w
        except Exception as e: print('final drain failed:', e)
        print('limit reached, killing own pid', pid, flush=True)
        try: frida.kill(pid)
        except Exception: pass
    keys = [x[0] for x in syms]
    def sym(pc):
        i = bisect.bisect_right(keys, pc) - 1   # pc is a VA at the preferred base, the map's Rva+Base column
        return '%s+0x%x' % (syms[i][1], pc - syms[i][0]) if i >= 0 else '?'
    with open(a.out + '.vs.csv', 'w', newline='') as f:
        f.write('kind,call0,ncalls,call1,ba8c,re7,c0,c1,c2,c3,ffc,rt,fd0\n')
        for r in runs: f.write(r + '\n')
    with open(a.out + '.vsw.csv', 'w', newline='') as f:
        f.write('hit,call,pc_va,ba8c,ffc,c0,rt,sym\n')
        for h in hits:
            p = h.split(','); f.write(h + ',' + sym(int(p[2])) + '\n')
    print('runs', len(runs), 'hits', len(hits), 'stat', stat, 'secs', int(time.time() - t0), flush=True)


if __name__ == '__main__':
    main()

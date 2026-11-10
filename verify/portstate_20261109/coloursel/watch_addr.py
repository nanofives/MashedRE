"""watch_0a40.py --root <root> --out <json> [--addrs 0x007f0a40,0x007f0a44,0x007f0a7c] [--menu-s 4]

PORT-SAVE-SPAN-MIRROR arm 1: who writes the 0x007f0a40 table on the original. Spawns <root>/original/MASHED.exe (hooks
OFF, MASHED_RE_NO_AUTO_HOOK=1), arms hardware WRITE watchpoints (debug registers, 4 bytes each, up to 4 addresses) on
every thread BEFORE resume, re-arms every 50 ms so threads created later are covered, and logs each hit: hit#, pc rva
(the instruction AFTER the store, data breakpoints trap after the write), the u32 at every watched address after the
write, and the accurate backtrace as image rvas. Runs until the title screen is up (phase 3, depth >= 1, via
re/frida/nav_agent.js), then --menu-s more seconds. With --walk it then presses confirm (control 4) to each next depth
(main menu, single player, colour select: the exe walk), then kills its own pid. Same method as scenario_launch.py
--watch-write / --watch-global. Read-only: no Interceptor on the watched path, no code patch.
Run inside test-slot and mashed_lock.py run.
"""
import argparse, json, os, time
from pathlib import Path
import frida

HERE = Path(__file__).resolve().parent  # copy of savespan/watch_0a40.py (PORT-SAVE-SPAN-MIRROR)
REPO = HERE.parents[2]
ap = argparse.ArgumentParser()
ap.add_argument('--root', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--addrs', default='0x007f0a40,0x007f0a44,0x007f0a7c')
ap.add_argument('--menu-s', type=float, default=4)
ap.add_argument('--walk', action='store_true', help='then confirm x3 to the colour select (the exe walk), logging hits per step')
a = ap.parse_args()
ADDRS = [int(x, 16) for x in a.addrs.split(',') if x][:4]

AGENT = r'''
'use strict';
const IMG = 0x00400000; let DELTA = 0;
const W = { rows: [], n: 0, err: null, addrs: [], armed: {} };
function rva(p) { const x = (p.toUInt32 ? p.toUInt32() : p) >>> 0; return '0x' + ((x - DELTA) >>> 0).toString(16); }
function armAll() {
  let k = 0;
  Process.enumerateThreads().forEach(function (t) {
    if (W.armed[t.id]) return;
    try { for (let i = 0; i < W.addrs.length; i++) t.setHardwareWatchpoint(i, W.addrs[i], 4, 'w'); W.armed[t.id] = 1; k++; }
    catch (e) { if (!W.err) W.err = 'arm ' + t.id + ' ' + e.message; }
  });
  return k;
}
rpc.exports = {
  arm: function (list) {
    const m = Process.findModuleByName('MASHED.exe') || Process.enumerateModules()[0];
    DELTA = m.base.toUInt32() - IMG;
    W.addrs = list.map(x => ptr(x + DELTA));
    Process.setExceptionHandler(function (d) {
      if (d.type !== 'single-step' && d.type !== 'breakpoint') return false;
      try {
        const c = d.context;
        let bt = '';
        try { bt = Thread.backtrace(c, Backtracer.ACCURATE).slice(0, 12).map(rva).join('|'); } catch (e) { bt = 'bt-err'; }
        W.n++;
        if (W.rows.length < 20000) W.rows.push([W.n, rva(c.pc), W.addrs.map(x => x.readU32() >>> 0), bt, Process.getCurrentThreadId()]);
      } catch (e) { if (!W.err) W.err = 'hit ' + e; }
      return true;
    });
    const k = armAll();
    setInterval(armAll, 50);
    return [DELTA, k];
  },
  drain: function () { const r = W.rows; W.rows = []; return [r, W.n, W.err]; }
};
'''


def main():
    res = {'addrs': ['0x%08x' % x for x in ADDRS], 'hits': []}
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env.update({'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_MUTE': '1'})
    exe = Path(a.root) / 'original' / 'MASHED.exe'
    dev = frida.get_local_device()
    pid = dev.spawn(str(exe), cwd=str(exe.parent), env=env)
    res['pid'] = pid
    print('spawned pid', pid, flush=True)
    try:
        s = dev.attach(pid)
        sc = s.create_script(AGENT); sc.load(); E = sc.exports_sync
        print('armed', E.arm(ADDRS), flush=True)
        nav = s.create_script((REPO / 're' / 'frida' / 'nav_agent.js').read_text(encoding='utf-8')); nav.load()
        N = nav.exports_sync; N.init()
        dev.resume(pid)
        t0 = time.time()
        while time.time() - t0 < 60:
            r, n, err = E.drain(); res['hits'] += r
            if N.phase() == 3 and N.depth() >= 1:
                break
            time.sleep(0.2)
        res['title_t'] = round(time.time() - t0, 1)
        time.sleep(a.menu_s)
        r, n, err = E.drain(); res['hits'] += r
        res['marks'] = [['title', n]]
        if a.walk:
            for step in ('mainmenu', 'singleplayer', 'colourselect'):
                d = N.depth()
                for _ in range(6):
                    if N.depth() > d:
                        break
                    N.press(4, 180); time.sleep(2.5)
                time.sleep(2.0)
                r, n, err = E.drain(); res['hits'] += r
                res['marks'].append([step, n, N.depth()])
                print('step', step, 'depth', N.depth(), 'hits so far', n, flush=True)
        res['n'] = n; res['err'] = err
        res['final'] = N.snap(ADDRS)
        print('hits', n, 'err', err, 'title at', res['title_t'], 's', flush=True)
        for h in res['hits'][:60]:
            print(h[0], h[1], ' '.join('%08x' % v for v in h[2]), h[3], flush=True)
    finally:
        try:
            dev.kill(pid); print('killed own pid', pid, flush=True)
        except Exception as e:
            print('kill', e, flush=True)
        Path(a.out).write_text(json.dumps(res, indent=1))
        print('->', a.out, flush=True)


if __name__ == '__main__':
    main()

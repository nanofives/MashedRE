"""icon_trace.py --root <root> --out <json> [--n 40]

PORT-COLOURSEL-ICON arm 1 (original side). Boots <root>/original/MASHED.exe (hooks OFF), walks to the colour select with
re/frida/nav_agent.js (title -> confirm x3, the exe nav-demo walk), then:
  A. Interceptor on 0x0042bcb0 (the icon glyph draw 0x004335f0 calls at 0x004339d3) for the next --n calls: the 5 stack
     args, the return address, and the globals 0x004335f0 reads for it: 0x0067f0c0 (choice, cmp 6 at 0x00433979),
     0x0067f0c4 (x), 0x0067f0c8 (y), 0x007f1a14 (profile number). Detached after --n calls (hot-path rule).
  B. Hardware WRITE watchpoints on 0x0067f0c0 / 0x0067f0c4 / 0x0067f0c8 for 1.5 s: pc (after the store), the values, the
     accurate backtrace, first 60 hits.
  C. One read of 0x0067eaf0..0x0067ebff (the port comment's choice records, 12 B per profile) and 0x007f1a00..0x007f1a3f.
Spawned pid only, killed by pid. Run inside test-slot and mashed_lock.py run.
"""
import argparse, json, os, time
from pathlib import Path
import frida

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[2]
ap = argparse.ArgumentParser()
ap.add_argument('--root', required=True)
ap.add_argument('--out', required=True)
ap.add_argument('--n', type=int, default=40)
a = ap.parse_args()

AGENT = r'''
'use strict';
const IMG = 0x00400000; let DELTA = 0; let L = null;
const R = { calls: [], hits: [] };
function A(x) { return ptr(x + DELTA); }
function rva(p) { return '0x' + (((p.toUInt32 ? p.toUInt32() : p) >>> 0) - DELTA >>> 0).toString(16); }
function g() { return { c0: A(0x0067f0c0).readS32(), c4: A(0x0067f0c4).readFloat(), c8: A(0x0067f0c8).readFloat(), p: A(0x007f1a14).readS32() }; }
rpc.exports = {
  init: function () { const m = Process.findModuleByName('MASHED.exe'); DELTA = m.base.toUInt32() - IMG; return DELTA; },
  hook: function (n) {
    L = Interceptor.attach(A(0x0042bcb0), { onEnter: function () {
      const sp = this.context.esp; const args = [];
      for (let i = 1; i <= 5; i++) args.push(sp.add(4 * i).readU32() >>> 0);
      const f = []; for (let i = 1; i <= 5; i++) f.push(sp.add(4 * i).readFloat());
      R.calls.push({ ret: rva(this.returnAddress), args: args, argsf: f, g: g() });
      if (R.calls.length >= n && L) { L.detach(); L = null; }
    }});
    return 1;
  },
  watch: function () {
    const addrs = [A(0x0067f0c0), A(0x0067f0c4), A(0x0067f0c8)];
    Process.setExceptionHandler(function (d) {
      if (d.type !== 'single-step' && d.type !== 'breakpoint') return false;
      if (R.hits.length < 60) {
        let bt = ''; try { bt = Thread.backtrace(d.context, Backtracer.ACCURATE).slice(0, 8).map(rva).join('|'); } catch (e) { bt = 'bt-err'; }
        R.hits.push([rva(d.context.pc), addrs.map(x => x.readU32() >>> 0), bt]);
      }
      return true;
    });
    Process.enumerateThreads().forEach(function (t) { for (let i = 0; i < 3; i++) { try { t.setHardwareWatchpoint(i, addrs[i], 4, 'w'); } catch (e) {} } });
    return 1;
  },
  unwatch: function () { Process.enumerateThreads().forEach(function (t) { for (let i = 0; i < 3; i++) { try { t.unsetHardwareWatchpoint(i); } catch (e) {} } }); return 1; },
  rd: function (lo, hi) { const r = []; for (let x = lo; x < hi; x += 4) r.push(A(x).readU32() >>> 0); return r; },
  res: function () { return R; }
};
'''


def main():
    res = {}
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env.update({'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_MUTE': '1'})
    exe = Path(a.root) / 'original' / 'MASHED.exe'
    dev = frida.get_local_device()
    pid = dev.spawn(str(exe), cwd=str(exe.parent), env=env)
    print('spawned pid', pid, flush=True)
    try:
        s = dev.attach(pid)
        nav = s.create_script((REPO / 're' / 'frida' / 'nav_agent.js').read_text(encoding='utf-8')); nav.load()
        N = nav.exports_sync; N.init()
        sc = s.create_script(AGENT); sc.load(); E = sc.exports_sync; E.init()
        dev.resume(pid)

        def wait(pred, t):
            end = time.time() + t
            while time.time() < end:
                if pred():
                    return True
                time.sleep(0.1)
            return False
        if not wait(lambda: N.phase() == 3 and N.depth() >= 1, 60):
            raise SystemExit('no title')
        time.sleep(1.5)
        for _ in range(3):
            d = N.depth()
            for _ in range(6):
                if N.depth() > d:
                    break
                N.press(4, 180); time.sleep(2.5)
        time.sleep(2.5)
        res['depth'] = N.depth()
        E.hook(a.n); time.sleep(2.0)
        E.watch(); time.sleep(1.5); E.unwatch()
        res['trace'] = E.res()
        res['eaf0'] = E.rd(0x0067eaf0, 0x0067ec00)
        res['f1a00'] = E.rd(0x007f1a00, 0x007f1a40)
        print('depth', res['depth'], 'calls', len(res['trace']['calls']), 'hits', len(res['trace']['hits']), flush=True)
        for c in res['trace']['calls'][:8]:
            print('call', c['ret'], ['%08x' % x for x in c['args']], c['g'], flush=True)
        for h in res['trace']['hits'][:12]:
            print('hit', h, flush=True)
    finally:
        try:
            dev.kill(pid); print('killed own pid', pid, flush=True)
        except Exception as e:
            print('kill', e, flush=True)
        Path(a.out).write_text(json.dumps(res, indent=1))
        print('->', a.out, flush=True)


if __name__ == '__main__':
    main()

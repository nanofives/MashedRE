# probe_chalrow.py <screen> <out.json> -- text4: which TrackImages name FUN_0042e590 resolves on the original.
# Spawns original\MASHED.exe (its own PID, MASHED_RE_NO_AUTO_HOOK=1), waits for [0x0067eca4] == 3, pushes
# <screen> the way ../scr32_texty_recheck/recheck_frame_probe.py does (FUN_0043d2a0(screen, 0) +
# [0x0067ecb0] = screen, no key input), settles 6 s, then samples with plain Memory reads (no Interceptor):
#   [0x0067f17c]  row used by FUN_0042e590 (0x0042e594) and by the power-up table (0x0043b956)
#   names [0x005f79d8 + (n + row*2)*4] for n = 0, 1 (FUN_0042e590 0x0042e59a..0x0042e59d)
#   [0x0067ea74] power-up mode, [0x0067e9f8] stack depth, [0x0067ea58] / [0x0067f0bc] preview slot counters
# Writes nothing else into game memory. Kills only the PID it spawned.
import sys, json, time, os
from pathlib import Path
import frida

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EXE = ROOT / "original" / "MASHED.exe"
AGENT = r"""
'use strict';
let DELTA = 0;
function abs(r) { return ptr(r + DELTA); }
const nav = () => new NativeFunction(abs(0x0043d2a0), 'void', ['int', 'int']);
rpc.exports = {
  init: function () { const m = Process.getModuleByName('MASHED.exe'); DELTA = m.base.toInt32() - 0x00400000; return DELTA; },
  phase: function () { return abs(0x0067eca4).readS32(); },
  push: function (scr) { nav()(scr, 0); abs(0x0067ecb0).writeS32(scr); return abs(0x0067e9f8).readS32(); },
  sample: function () {
    const row = abs(0x0067f17c).readS32();
    const names = [];
    for (let n = 0; n < 2; n++) {
      const idx = n + row * 2;
      let s = null;
      if (idx >= 0 && idx < 26) { try { s = ptr(abs(0x005f79d8 + idx * 4).readU32() + DELTA).readCString(32); } catch (e) {} }
      names.push({ n: n, idx: idx, name: s });
    }
    return { row: row, names: names, ea74: abs(0x0067ea74).readS32(), depth: abs(0x0067e9f8).readS32(),
             slotA: abs(0x0067ea58).readS32(), slotB: abs(0x0067f0bc).readS32(), screen: abs(0x0067ecb0).readS32() };
  }
};
"""

def main():
    screen = int(sys.argv[1]); out = Path(sys.argv[2])
    env = dict(os.environ); env.setdefault("MASHED_RE_NO_AUTO_HOOK", "1")
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(EXE.parent), env=env)
    res = {"pid": pid, "screen": screen, "before_push": None, "samples": []}
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT); scr.load()
        dev.resume(pid)
        scr.exports_sync.init()
        t0 = time.time()
        while scr.exports_sync.phase() != 3 and time.time() - t0 < 60:
            time.sleep(0.25)
        time.sleep(2.0)
        res["before_push"] = scr.exports_sync.sample()
        res["push_depth"] = scr.exports_sync.push(screen)
        time.sleep(6.0)
        for _ in range(5):
            s = scr.exports_sync.sample(); s["t"] = time.time(); res["samples"].append(s)
            time.sleep(1.0)
    finally:
        try:
            dev.kill(pid)
        except Exception:
            pass
    out.write_text(json.dumps(res, indent=1))
    print("pid", pid, "before", res["before_push"], "after", res["samples"][-1] if res["samples"] else None)

if __name__ == "__main__":
    main()

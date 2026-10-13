# probe_devcount.py <out.json> [--wait S] -- READ-ONLY live read of the screen-4 device count on the original.
# Spawns original\MASHED.exe (its own PID, MASHED_RE_NO_AUTO_HOOK=1), waits for boot, then samples with plain
# Memory reads (no Interceptor, no writes, no nav push) and kills only that PID.
#   [0x00772fac]  attached game-controller count, written by FUN_00495ee0 (inc [pvRef] @0x00495fcb)
#   [0x00772ffc]  active-slot count, written by FUN_004967e0 (@0x0049680d / @0x00496825), read by FUN_00496910
#   slot i = 0x007e95c0 + i*0x200: +0 dword, +4 name (cstr), +0x13c type, +0x140 joystick index (FUN_00498510)
#   device n = 0x00771e88 + n*0x448: +0x28 / +0x12c instance / product name (FUN_00495ee0 @0x00495f5a..0x00495f61)
import sys, json, time, os
from pathlib import Path
import frida

ROOT = Path(__file__).resolve().parents[3]
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"
AGENT = r"""
function cstr(a) { try { return ptr(a).readCString(64); } catch (e) { return null; } }
rpc.exports = {
  sample: function () {
    var r = { c772fac: ptr(0x772fac).readS32(), c772ffc: ptr(0x772ffc).readS32(),
              d7719e8: ptr(0x7719e8).readS32(), slots: [], devs: [] };
    for (var i = 0; i < 4; i++) {
      var b = 0x7e95c0 + i * 0x200;
      r.slots.push({ i: i, w0: ptr(b).readS32(), name: cstr(b + 4),
                     type13c: ptr(b + 0x13c).readS32(), idx140: ptr(b + 0x140).readS32() });
    }
    var n = r.c772fac; if (n > 8) n = 8;
    for (var k = 0; k < n; k++) {
      var d = 0x771e88 + k * 0x448;
      r.devs.push({ k: k, iface: ptr(d).readU32(), inst: cstr(d + 4 + 0x28), prod: cstr(d + 4 + 0x12c) });
    }
    return r;
  }
};
"""

def main():
    out = Path(sys.argv[1])
    wait = float(sys.argv[sys.argv.index("--wait") + 1]) if "--wait" in sys.argv else 15.0
    env = dict(os.environ); env.setdefault("MASHED_RE_NO_AUTO_HOOK", "1")
    dev = frida.get_local_device()
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    res = {"pid": pid, "samples": []}
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT); scr.load()
        dev.resume(pid)
        time.sleep(wait)
        for _ in range(5):
            s = scr.exports_sync.sample(); s["t"] = time.time(); res["samples"].append(s)
            time.sleep(1.0)
    finally:
        try:
            dev.kill(pid)
        except Exception:
            pass
    out.write_text(json.dumps(res, indent=1))
    last = res["samples"][-1] if res["samples"] else {}
    print("pid", pid, "772fac", last.get("c772fac"), "772ffc", last.get("c772ffc"),
          "types", [s["type13c"] for s in last.get("slots", [])],
          "devs", [d["prod"] for d in last.get("devs", [])])

if __name__ == "__main__":
    main()

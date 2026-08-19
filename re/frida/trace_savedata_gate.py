# trace_savedata_gate.py — settle WHO sets DAT_007f0f2c before the frontend reads it.
#
# WHY: the scr1 draw-list gate is RED on 20 quads because the standalone enables the
# menu row the original greys. Root cause (verify/parity_20260819/DRAWLIST_scr1.md):
# DAT_007f0f2c is RUNTIME state with three writers, and the port derives it from the
# restored save span instead. FUN_00432800 case 1 reads it and clears avail[3].
#
#   READ  0x004328df  FUN_00432800   the gate, its only reader
#   WRITE 0x004305ba  FUN_00430290
#   WRITE 0x00492504  FUN_004924f0   assigns 0
#   WRITE 0x00492991  FUN_004927c0   assigns 1
#
# The one open question is CALL ORDER on a fresh boot: which writer lands last before
# the frontend reads it. This logs the sequence with the flag's value at each point.
#
# Not a hot path -- these are init/screen-change functions, so Interceptor is safe
# here (the >1000 calls/s rule in CLAUDE.md targets FastSqrt-class leaves).
#
# Usage: py -3.12 re/frida/trace_savedata_gate.py [seconds]
import os
import pathlib
import sys
import time

import frida

ROOT = pathlib.Path(__file__).resolve().parents[2]
ORIG = ROOT / "original"
EXE = ORIG / "MASHED.exe"
GATE = 0x007F0F2C

AGENT = r"""
// Frida 17 removed the legacy Module.* statics (Module.findBaseAddress throws
// "TypeError: not a function"). Process.findModuleByName is what every working
// agent in re/frida/ uses.
const BASE = Process.findModuleByName('MASHED.exe').base;
const GATE = BASE.add(0x3F0F2C);              // 0x007f0f2c - 0x00400000
// Frida 17: Memory.readU32(ptr) was removed; it is ptr.readU32() now.
function val() { return GATE.readU32(); }
const seq = [];
function note(tag, extra) {
  seq.push({ n: seq.length, t: Date.now(), tag: tag, gate: val(), extra: extra || '' });
  send({ type: 'evt', tag: tag, gate: val(), extra: extra || '' });
}
function hook(rva, tag) {
  Interceptor.attach(BASE.add(rva - 0x400000), {
    onEnter: function () { note(tag + ':enter'); },
    onLeave: function () { note(tag + ':leave'); }
  });
}
rpc.exports = {
  init: function () {
    hook(0x00430290, 'W_FUN_00430290');
    hook(0x004924f0, 'W_FUN_004924f0');
    hook(0x004927c0, 'W_FUN_004927c0');
    Interceptor.attach(BASE.add(0x00432800 - 0x400000), {
      onEnter: function (a) { note('R_FUN_00432800:enter', 'slot=' + a[0].toInt32()); },
      onLeave: function () { note('R_FUN_00432800:leave'); }
    });
    note('init');
  },
  dump: function () { return seq; }
};
"""


def main():
    secs = int(sys.argv[1]) if len(sys.argv) > 1 else 25
    dev = frida.get_local_device()
    env = dict(os.environ)
    env["MASHED_RE_NO_AUTO_HOOK"] = "1"        # stock behaviour, no .asi hooks
    env["MASHED_WIN_POS"] = "left-bl"
    pid = dev.spawn(str(EXE), cwd=str(ORIG), env=env)
    print(f"spawned pid {pid}")
    sess = dev.attach(pid)
    events = []

    def on_msg(m, d):
        # STREAM immediately: an earlier run was killed by an outer timeout and the
        # end-of-run RPC dump was lost with it. Printing per event survives that.
        if m.get("type") == "send":
            p = m.get("payload") or {}
            events.append(p)
            print(f"  gate={p.get('gate'):>4}  {p.get('tag')} {p.get('extra','')}",
                  flush=True)
        else:
            print(f"  [frida] {m}", flush=True)

    scr = sess.create_script(AGENT)
    scr.on("message", on_msg)
    scr.load()
    scr.exports_sync.init()
    dev.resume(pid)
    print(f"tracing {secs}s ...")
    time.sleep(secs)
    try:
        seq = scr.exports_sync.dump()
    except Exception as e:
        seq = events            # streamed copy is authoritative if the RPC is lost
        print(f"dump failed ({e}); using {len(seq)} streamed events")
    try:
        sess.detach()
    except Exception:
        pass
    # kill ONLY the pid we spawned (CLAUDE.md process hygiene)
    try:
        dev.kill(pid)
    except Exception:
        pass

    lines = [f"{len(seq)} events", f"{'#':>3}  {'gate':>4}  event"]
    prev = None
    for e in seq:
        mark = "  <-- CHANGED" if prev is not None and e["gate"] != prev else ""
        lines.append(f"{e.get('n','?'):>3}  {e['gate']:>4}  {e['tag']} {e['extra']}{mark}")
        prev = e["gate"]
    reads = [e for e in seq if e["tag"].startswith("R_")]
    if reads:
        lines.append(f"\ngate at FIRST read of FUN_00432800: {reads[0]['gate']}")
        lines.append(f"gate at LAST  read of FUN_00432800: {reads[-1]['gate']}")
    out = "\n".join(lines) + "\n"
    # Write to a file directly: an outer timeout can kill the wrapper before piped
    # stdout flushes, so the file is the durable record.
    res = ROOT / "log" / "trace_savedata_gate.txt"
    res.write_text(out, encoding="utf-8")
    print(out, flush=True)
    print(f"-> {res}", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())

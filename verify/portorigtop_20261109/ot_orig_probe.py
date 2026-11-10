"""PORT-OT-RWG-SLOTS: original-side twin of `ot_measure.py probe`.

  py -3.12 verify/portorigtop_20261109/ot_orig_probe.py --data <dir with ot_rows.json> --root <private root> [--secs 30]

Spawns <root>/original/MASHED.exe (the patched install, cwd = <root>/original, MASHED_RE_NO_AUTO_HOOK=1) and, at the
same sample marks as ot_measure.py, reads with ReadProcessMemory:
  - the dword at 0x007d3ff8 (RwGlobals),
  - every RwGlobals offset the ot_rows.json rows use (call slots, data fields, "m<addr>+<disp>" module reads),
  - the four module-offset globals and 0x40 bytes of module data at RwGlobals + offset,
  - the whole RwGlobals block 0x0..0x200 (hex) at each sample.
Writes <data>/orig_probe.json. Only reads; kills only the PID it spawned. Run it inside test-slot + mashed_lock.
"""
from __future__ import annotations

import json
import os
import pathlib
import subprocess
import sys
import time

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import ot_measure as M    # noqa: E402  (rpm, k32, RWG, opt)

MODS = (0x007d3ffc, 0x007d4028, 0x007d4054, 0x007d4628)


def main(argv):
    data = pathlib.Path(M.opt(argv, "--data"))
    root = pathlib.Path(M.opt(argv, "--root"))
    secs = float(M.opt(argv, "--secs", "30"))
    rows = json.load(open(data / "ot_rows.json"))
    offs = sorted({o for r in rows.values() for o in r.get("rwg_call_slots", []) + r.get("rwg_data_offs", [])},
                  key=lambda o: (isinstance(o, str), str(o)))
    exe = root / "original" / "MASHED.exe"
    env = dict(os.environ, MASHED_RE_NO_AUTO_HOOK="1")
    p = subprocess.Popen([str(exe)], cwd=str(root / "original"), env=env)
    print("spawned pid", p.pid, flush=True)
    h = M.k32.OpenProcess(M.PROCESS_VM_READ | M.PROCESS_QUERY_INFORMATION, False, p.pid)
    samples = []
    t0 = time.time()
    for m in [0.5, 1.5, 3, 6, 10, 15, 22, secs]:
        while time.time() - t0 < m and p.poll() is None:
            time.sleep(0.05)
        if p.poll() is not None:
            break
        s = {"t": round(time.time() - t0, 2)}
        g = M.rpm(h, M.RWG, 4)
        s["rwg"] = int.from_bytes(g, "little") if g else None
        s["mods"] = {}
        for ma in MODS:
            mv = M.rpm(h, ma, 4)
            s["mods"]["%08x" % ma] = int.from_bytes(mv, "little") if mv else None
        if s["rwg"]:
            blk = M.rpm(h, s["rwg"], 0x200)
            s["block"] = blk.hex() if blk else None
            s["slots"], s["moddata"] = {}, {}
            for o in offs:
                if isinstance(o, str):
                    ma, d = o[1:].split("+")
                    mo = s["mods"].get(ma)
                    v = M.rpm(h, s["rwg"] + mo + int(d, 16), 4) if mo is not None else None
                else:
                    v = M.rpm(h, s["rwg"] + o, 4)
                s["slots"][str(o)] = int.from_bytes(v, "little") if v else None
            for ma, mo in s["mods"].items():
                if mo is not None:
                    v = M.rpm(h, s["rwg"] + mo, 0x40)
                    s["moddata"][ma] = v.hex() if v else None
        samples.append(s)
        print("sample t=%.1f rwg=%s mods=%s" % (s["t"], hex(s["rwg"]) if s["rwg"] else s["rwg"],
                                                 {k: (hex(v) if v is not None else v) for k, v in s["mods"].items()}),
              flush=True)
    rc = p.poll()
    if rc is None:
        p.kill()
        rc = p.wait()
        print("killed own pid", p.pid)
    M.k32.CloseHandle(h)
    out = data / "orig_probe.json"
    json.dump({"exe": str(exe), "root": str(root), "rc": rc, "samples": samples}, open(out, "w"), indent=1)
    print("orig probe rc=%s samples=%d -> %s" % (rc, len(samples), out))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

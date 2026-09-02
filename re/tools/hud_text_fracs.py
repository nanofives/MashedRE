# hud_text_fracs.py — print the ORIGINAL's captured standings text as
# (first code units, xy fracs, scale) so the normalised coordinates baked into
# the port can be checked against a second scenario.
#
# The port hardcodes these fracs (exe_main.cpp standings block), so they must be
# shown to be scenario-invariant rather than assumed. See
# re/analysis/race_hud_capture_20260902.md Finding 12.
import json
import struct
import sys
from pathlib import Path


def main():
    src = Path(sys.argv[1] if len(sys.argv) > 1 else "log/race_hud_text.json")
    d = json.loads(src.read_text(encoding="utf-8"))
    lbl = sorted(d)[0]

    seen = set()
    for r in d[lbl]:
        raw = r.get("str_raw")
        if not raw or r.get("chan") != "thunk_00556ca0":
            continue
        b = bytes.fromhex(raw)
        units = tuple(b[i] | (b[i + 1] << 8) for i in range(0, 24, 2))
        if units in seen:
            continue
        seen.add(units)

        txt = "".join(chr(u) if 32 <= u < 127 else f"<{u:04x}>"
                      for u in units[:12])
        fx = fy = float("nan")
        if r.get("xy_raw"):
            fx, fy = struct.unpack_from("<ff", bytes.fromhex(r["xy_raw"]))
        sc = float("nan")
        if r.get("scale_bits"):
            sc = struct.unpack("<f", struct.pack("<I",
                 int(r["scale_bits"], 16)))[0]
        print(f"  {txt:<26} fx={fx:.4f} fy={fy:.4f} scale={sc:.4f}  "
              f"p5={sc / 0.0708:.3f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

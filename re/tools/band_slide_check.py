# band_slide_check.py — track the standings letterbox band's top-y per FRAME
# INDEX in a capture, to see whether the chrome slides in (U-9075).
#
# Works on either side's capture:
#   * original-side race_hud_burst.py output ({label: [{f,v,r}]}), 640-space
#   * standalone MASHED_DBG_DRAWSTREAM output ({f<N>: [{v,r,s}]}), 800-space
# and PRINTS THE BAND WIDTH so the render resolution is never in doubt.
#
# The width tells you the RESOLUTION, not which side produced the file. Measured
# 2026-09-02: the ORIGINAL emits 640-wide bands with MASHED_RES unset, but
# 800-wide bands when MASHED_RES=800x600 is in the environment — the original's
# ChromeBaseDraw scales with the screen getters, so its chrome geometry follows
# the resolution too. An earlier version of this comment asserted "w=800 =>
# standalone", which is wrong and would misattribute an original capture.
import json
import struct
import sys
from pathlib import Path

STRIDE = 0x1c


def quads(rows):
    out = []
    for r in rows:
        raw = bytes.fromhex(r["v"])
        n = len(raw) // STRIDE
        xs, ys = [], []
        col = 0
        for i in range(n):
            x, y, _z, _w, c = struct.unpack_from("<ffffI", raw, i * STRIDE)
            xs.append(x); ys.append(y)
            if i == 0:
                col = c
        out.append({"x": min(xs), "y": min(ys), "w": max(xs) - min(xs),
                    "h": max(ys) - min(ys), "col": col, "f": r.get("f")})
    return out


def main():
    src = Path(sys.argv[1])
    d = json.loads(src.read_text(encoding="utf-8"))

    # Flatten to (frame, quad). Frame comes from the "f" tag (original side) or
    # the label (standalone side).
    flat = []
    for label, rows in d.items():
        lab_f = None
        if label.startswith("f") and label[1:].isdigit():
            lab_f = int(label[1:])
        for q in quads(rows):
            f = q["f"] if q["f"] is not None and q["f"] >= 0 else lab_f
            flat.append((f, q))

    bands = [(f, q) for f, q in flat
             if q["col"] == 0xff000000 and q["w"] > 400 and q["y"] < q["h"] * 3]
    if not bands:
        print("no opaque-black wide band found; quad inventory of one frame:")
        seen = set()
        for f, q in flat[:40]:
            key = (round(q["w"]), round(q["h"]), q["col"])
            if key in seen:
                continue
            seen.add(key)
            print(f"   w={q['w']:7.2f} h={q['h']:7.2f} col={q['col']:08x}")
        return 1

    widths = sorted({round(q["w"]) for _, q in bands})
    space = ("640-space" if widths == [640] else
             "800-space" if widths == [800] else
             "MIXED/UNKNOWN — do not compare")
    print(f"band width(s) present: {widths}  -> {space} render resolution "
          f"(this says nothing about WHICH SIDE produced the file: the original "
          f"also emits 800 when MASHED_RES=800x600 is set)")

    by_f = {}
    for f, q in bands:
        if f is None:
            continue
        by_f.setdefault(f, []).append(q)

    print(f"\nframes with a band: {len(by_f)}  range "
          f"{min(by_f)}..{max(by_f)}")
    print(f"{'frame':>8}{'top_y':>10}{'alpha':>8}  (top band)")
    prev = None
    moved = 0
    for f in sorted(by_f):
        top = min(by_f[f], key=lambda q: q["y"])
        a = (top["col"] >> 24) & 0xff
        delta = "" if prev is None else f"   d={top['y'] - prev:+.2f}"
        if prev is not None and abs(top["y"] - prev) > 0.01:
            moved += 1
        print(f"{f:>8}{top['y']:>10.2f}{a:>8}{delta}")
        prev = top["y"]

    print(f"\nframes where top_y changed: {moved}")
    if moved == 0:
        print("VERDICT: NO SLIDE in this capture — band top is constant. "
              "If a slide is expected, the captured window did not include the "
              "state transition (the slide lives in the first frames after it).")
    else:
        print("VERDICT: the band top MOVES — a slide is present in this capture.")
    return 0


if __name__ == "__main__":
    sys.exit(main())

# hud_rows_check.py — print the standalone's non-glyph standings draws from a
# MASHED_DBG_DRAWSTREAM capture, alongside the same values scaled into the
# original's 640x480 space, so the ported row layout can be compared against the
# measurements taken from verify/race_hud/orig_drive_late.png.
#
# Why a script and not an inline one-liner: the standings row layout could not be
# measured from a draw list at all (that layer is on the RW pipeline slots, see
# re/analysis/race_hud_capture_20260902.md Finding 4), so the check is
# "did we emit what the pixel measurement said", which needs the scale applied.
import json
import struct
import sys
from pathlib import Path

# Measured from the original reference backbuffer (640x480).
ORIG_ROW_CENTRES = (107.0, 160.0, 213.0, 267.0)
ORIG_ICON = "x=33..74 (w=42, h=48)"
ORIG_BAR = "x=87..179 (w=93, h=17)"


def parse(rows):
    out = []
    for r in rows:
        raw = bytes.fromhex(r["v"])
        n = len(raw) // 0x1c
        xs, ys, col = [], [], 0
        for i in range(n):
            x, y, _z, _w, c = struct.unpack_from("<ffffI", raw, i * 0x1c)
            xs.append(x); ys.append(y)
            if i == 0:
                col = c
        out.append((min(xs), min(ys), max(xs) - min(xs), max(ys) - min(ys),
                    col, r.get("s")))
    return out


def main():
    src = Path(sys.argv[1] if len(sys.argv) > 1 else "log/drawstream_re.json")
    scale = float(sys.argv[2]) if len(sys.argv) > 2 else 0.8
    d = json.loads(src.read_text(encoding="utf-8"))

    # standings frames carry a full-width opaque black band
    keys = [k for k, rows in d.items()
            if any(c == 0xff000000 and w > 700 for _, _, w, _, c, _ in parse(rows))]
    if not keys:
        print("no standings frame found (no full-width black band)")
        return 1
    keys.sort(key=lambda s: int(s[1:]))
    k = keys[len(keys) // 2]

    print(f"=== {k} : non-glyph draws, and the same values scaled x{scale} "
          f"into the original's 640x480 ===")
    print(f"{'x':>8}{'y':>9}{'w':>9}{'h':>8}   |{'x*s':>8}{'cy*s':>9}"
          f"{'w*s':>8}{'h*s':>7}  col")
    for (x, y, w, h, col, s) in parse(d[k]):
        if s and s[0] == 9:
            continue          # font glyphs handled separately
        print(f"{x:8.2f}{y:9.2f}{w:9.2f}{h:8.2f}   |{x*scale:8.2f}"
              f"{(y + h / 2) * scale:9.2f}{w*scale:8.2f}{h*scale:7.2f}  {col:08x}")

    print(f"\noriginal MEASURED row centres (640-space): "
          f"{'  '.join(f'{c:.1f}' for c in ORIG_ROW_CENTRES)}")
    print(f"original MEASURED icon box {ORIG_ICON}; bar frame {ORIG_BAR}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

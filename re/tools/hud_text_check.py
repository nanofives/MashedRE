# hud_text_check.py — summarise the standalone's font-glyph draws per text line
# from a MASHED_DBG_DRAWSTREAM capture: glyph count, left edge, cell height, and
# the set of per-glyph colours.
#
# Used to verify the ported standings text, which drawlist_diff.py cannot check
# (--exclude-tex 9 drops the standalone's glyphs and the original's Im2D stream
# contains no text at all — glyphs are on Im3D). See
# re/analysis/race_hud_capture_20260902.md Findings 8 and 11.
#
# The colour set is the point of this tool: the prompt line must show TWO
# colours — the ctrl green on the leading 0x81 nav glyph and white on the word —
# which is how "the glyph rendered and is coloured as a control glyph" is
# distinguished from "a glyph-shaped blob in the right place".
import json
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

STRIDE = 0x1c
FONT_TEX = 9


def parse(rows):
    out = []
    for r in rows:
        raw = bytes.fromhex(r["v"])
        n = len(raw) // STRIDE
        xs, ys, col = [], [], 0
        for i in range(n):
            x, y, _z, _w, c = struct.unpack_from("<ffffI", raw, i * STRIDE)
            xs.append(x); ys.append(y)
            if i == 0:
                col = c
        out.append((min(xs), min(ys), max(xs) - min(xs), max(ys) - min(ys),
                    col, r.get("s")))
    return out


def main():
    src = Path(sys.argv[1] if len(sys.argv) > 1 else "log/drawstream_re.json")
    min_glyphs = int(sys.argv[2]) if len(sys.argv) > 2 else 5
    d = json.loads(src.read_text(encoding="utf-8"))

    keys = [k for k, rows in d.items()
            if any(c == 0xff000000 and w > 700
                   for _, _, w, _, c, _ in parse(rows))]
    if not keys:
        print("no standings frame found (no full-width black band)")
        return 1
    keys.sort(key=lambda s: int(s[1:]))
    k = keys[len(keys) // 2]

    lines = defaultdict(list)
    for (x, y, w, h, col, s) in parse(d[k]):
        if s and s[0] == FONT_TEX:
            lines[round(y, 1)].append((x, w, h, col))

    print(f"=== {k}: text lines with >= {min_glyphs} glyphs ===")
    for y in sorted(lines):
        g = sorted(lines[y])
        if len(g) < min_glyphs:
            continue
        cols = Counter(f"{c:08x}" for _, _, _, c in g)
        print(f"  y={y:8.2f} glyphs={len(g):3} x_left={g[0][0]:7.2f} "
              f"cell_h={g[0][2]:6.2f}")
        print(f"      colours={dict(cols)}")
        print(f"      first glyph: x={g[0][0]:.2f} w={g[0][1]:.2f} "
              f"col={g[0][3]:08x}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

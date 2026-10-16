"""d1r1: which rule produces the original's per-raster D3D9 alpha flag (raster ext +8)?

    py -3.12 texalpha_rule.py <texalpha.json> <piz> [<piz> ...]

<texalpha.json> is camera_probe.py --drawrec's dump: texture name -> [ext+8 alpha byte, width,
height, depth, cFormat] read live from the original. The pizzes are decoded offline with
re/tools/txd_format_census.py's _walk_full (base mip, exactly TxdDecoder.h's layout) and each
texture's alpha is measured four ways:
  used_lt255  any texel the base mip actually uses has alpha < 255
  pal_lt255   (paletted only) any palette entry has alpha < 255, used or not
  depth32     the texture is 32-bit
  any_lt255   used_lt255 for 32-bit, pal_lt255 for paletted
Prints a confusion table per rule against the original's flag.
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools"))
from piz_extract import read_archive  # noqa: E402
from txd_format_census import _walk_full  # noqa: E402


def measure(base):
    w, h, depth, stride, px, pal = base
    used = set()
    amin_used = 255
    for y in range(h):
        row = px[y * stride:(y + 1) * stride]
        for x in range(w):
            if depth == 32:
                a = row[x * 4 + 3]
            else:
                idx = (row[x] & 0x0F) if depth == 4 else row[x]
                used.add(idx)
                a = pal[idx * 4 + 3]
            if a < amin_used:
                amin_used = a
    pal_lt = depth != 32 and any(pal[i * 4 + 3] < 255 for i in range(len(pal) // 4))
    return {"depth": depth, "used_lt255": amin_used < 255, "amin_used": amin_used,
            "pal_lt255": pal_lt, "depth32": depth == 32,
            "any_lt255": (amin_used < 255) if depth == 32 else pal_lt}


def main() -> int:
    orig = json.loads(Path(sys.argv[1]).read_text())
    meas = {}
    for piz in sys.argv[2:]:
        data, _v, _a, entries, _m = read_archive(Path(piz))
        for name, off, length, _i in entries:
            if not name.upper().endswith(".TXD"):
                continue
            try:
                _dev, texs = _walk_full(data[off:off + length])
            except Exception as e:  # noqa: BLE001
                print(f"# {piz}::{name}: {e}")
                continue
            for tname, base in texs:
                if base is not None:
                    meas.setdefault(tname.lower(), measure(base))
    rules = ["used_lt255", "pal_lt255", "depth32", "any_lt255"]
    both = [n for n in orig if n.lower() in meas]
    print(f"original textures {len(orig)}, decoded {len(meas)}, in both {len(both)}")
    for r in rules:
        tab = {(0, 0): 0, (0, 1): 0, (1, 0): 0, (1, 1): 0}
        bad = []
        for n in both:
            o = 1 if orig[n][0] else 0
            m = 1 if meas[n.lower()][r] else 0
            tab[(o, m)] += 1
            if o != m:
                bad.append(f"{n}(orig={o},d{meas[n.lower()]['depth']},amin={meas[n.lower()]['amin_used']})")
        print(f"rule {r:<11} agree {tab[(0,0)] + tab[(1,1)]}/{len(both)}  "
              f"orig1&rule0 {tab[(1,0)]}  orig0&rule1 {tab[(0,1)]}  first mismatches: {bad[:8]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

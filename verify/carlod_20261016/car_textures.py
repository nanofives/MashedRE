"""carlod follow-up 3 premise: which vehicle textures appear in the original's 9 cited frames.
rtex follow-up 3 (48 ZERO/ONE dual materials on the Atmos car, a dual-UV-transform material on Storm)
is only in scope if a cited row involves those models. Lists every texture name of every original draw
whose name is one of the vehicle families' (case-insensitive prefix match), per frame.

    py -3.12 car_textures.py
"""
import collections
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import drawlist3d  # noqa: E402

TRACKS = {"training": [884, 924, 959], "arctic": [1051, 1091, 1126], "city": [761, 801, 836]}
FAMILIES = ["advantag", "atmos", "stallion", "storm", "bullet", "sputter", "cooler", "shorty", "crusader",
            "creeper", "bambam", "kustom", "formula", "shuriken", "glass"]

for t, rows in TRACKS.items():
    for k in rows:
        doc = json.load(open(ROOT / "verify" / "d1r1_20261016" / "evidence" / f"{t}_orig_row_{k:05d}.draws.json"))
        ds = [drawlist3d.norm(e, i) for i, e in enumerate(drawlist3d.cut_frame(doc))]
        c = collections.Counter(d["texl"] for d in ds if any(d["texl"].startswith(f) for f in FAMILIES))
        print(f"{t:<9}{k:>5} {dict(c)}")

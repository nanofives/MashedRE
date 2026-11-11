# RO-STACKROW-READERS: event-ring census from a detorig.py run's checkpoint dumps. Ring 0x007ec9e4 (256 x 0x40),
# type DAT_007e9de4[row], time DAT_007ea1e4[row] (DAT_007f1030 when produced), cursor DAT_007e9de0. Counts distinct
# (row, type, time) entries seen across the checkpoints, by type, and for poisoned runs how many rows hold the poison.
#   py -3.12 verify/replayoracle_20261111/ring_census.py RUN_DIR [RUN_DIR ...] [--poison 0x7fc0dead]
import argparse, json, struct
from collections import Counter
from pathlib import Path
DATA_LO = 0x005ea000
ap = argparse.ArgumentParser(); ap.add_argument("runs", nargs="+"); ap.add_argument("--poison", default="0x7fc0dead"); a = ap.parse_args()
PZ = int(a.poison, 16)
for d in a.runs:
    meta = json.loads((Path(d) / "meta.json").read_text())
    seen = set(); pz = Counter(); stale_fields = Counter()
    for c in sorted(meta["ckpt"], key=lambda c: c["k"]):
        b = (Path(d) / c["file"]).read_bytes()
        w = lambda va: struct.unpack_from("<I", b, va - DATA_LO)[0]
        for r in range(256):
            t, tm = w(0x007e9de4 + 4 * r), w(0x007ea1e4 + 4 * r)
            if t == 0 and tm == 0: continue
            seen.add((r, t, tm))
            row = [w(0x007ec9e4 + 0x40 * r + 4 * j) for j in range(16)]
            for j, v in enumerate(row):
                if v == PZ: pz[(t, j)] += 1
    by = Counter(t for _, t, _ in seen)
    print(f"{Path(d).name}: {len(seen)} distinct rows seen;  by type " + ", ".join(f"{t:#x}:{n}" for t, n in sorted(by.items())))
    if pz:
        print("   poison dwords seen (type, dword index): " + ", ".join(f"({t:#x},{j}):{n}" for (t, j), n in sorted(pz.items())))

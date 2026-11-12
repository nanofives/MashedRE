"""RO-BOOT-CRASH-3: per-tick extent of the block differences between one run and one reference (same root, same
boot class): for every block that differs at any tick, the ticks where it differs (first, last, count), plus the raw
dwords (the run's --tick-raw ranges) that differ at those ticks.

    py -3.12 verify/robootcrash3_20261112/block_extent.py <run> <ref>
"""
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1] / "re" / "frida"))
from detorig import DATA_LO  # noqa: E402
from detorig_ticks import load  # noqa: E402

_, tr, rr = load(sys.argv[1])
_, ta, ra = load(sys.argv[2])
nblk, blk = ta["nblk"], ta["block"]
raw_va = []
for lo, hi in ta.get("raw") or []:
    raw_va += list(range(lo, hi, 4))
n = min(len(rr), len(ra))
ext = {}
for i in range(n):
    for j in range(2, 2 + nblk):
        if rr[i][j] != ra[i][j]:
            ext.setdefault(j, []).append(i)
print(f"ticks compared {n}; blocks differing at any tick: {len(ext)}")
for j, ts in sorted(ext.items()):
    print(f"  block {DATA_LO + (j - 2) * blk:#010x}: ticks {ts[0]}..{ts[-1]} ({len(ts)} ticks)")
rawd = {}
for i in range(n):
    for k, v in enumerate(raw_va):
        p = 2 + nblk + k
        if rr[i][p] != ra[i][p]:
            rawd.setdefault(v, []).append(i)
for v, ts in sorted(rawd.items()):
    print(f"  raw {v:#010x}: ticks {ts[0]}..{ts[-1]} ({len(ts)} ticks)")

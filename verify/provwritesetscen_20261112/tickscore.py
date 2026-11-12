"""PROV-WRITESET-SCENARIO: per-tick block-hash score of a run against the PROV-INSITU references, every tick.
prov_compare.py stops at the first divergence. This counts every tick whose non-allowed blocks differ from r1, so a
stepped run can be set beside the unstepped control of the same harness (block hashes only, no per-dword masks).
usage: py -3.12 tickscore.py <run> [<run> ...]"""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "frida"))
from detorig_ticks import load

I = "D:/MashedWT/PROV-INSITU_data/"
_, ta, r1 = load(I + "r1"); _, _, r2 = load(I + "r2"); _, _, x1 = load(I + "x_ref"); _, _, x2 = load(I + "x_ref2")
nblk, blk = ta["nblk"], ta["block"]
allowed = set()
for a, b in ((r1, r2), (x1, x2)):
    for i in range(min(len(a), len(b))):
        allowed |= {j for j in range(2, 2 + nblk) if a[i][j] != b[i][j]}
def va(j): return hex(0x005ea000 + (j - 2) * blk) if False else j
for run in sys.argv[1:]:
    _, tr, rr = load(run)
    n = min(len(rr), len(r1))
    bad = [(i, sorted(j for j in range(2, 2 + nblk) if j not in allowed and rr[i][j] != r1[i][j])) for i in range(n)]
    bad = [(i, b) for i, b in bad if b]
    after0 = [x for x in bad if x[0] > 0]
    print(f"{Path(run).name}\tticks {n}\tdiverging_ticks {len(bad)}\tdiverging_after_tick0 {len(after0)}\t"
          f"tick0_blocks {len(bad[0][1]) if bad and bad[0][0] == 0 else 0}\tfirst_after0 {after0[0][0] if after0 else '-'}"
          f"\tblocks_after0 {sorted({j for _, b in after0 for j in b})[:8]}")

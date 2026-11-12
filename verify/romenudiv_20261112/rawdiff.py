"""RO-MENU-DIV-43DFD0: list the raw dwords (detorig --tick-raw) that differ between a run and a reference, per tick.

    py -3.12 verify/romenudiv_20261112/rawdiff.py <run> <ref> [--from T] [--ticks K] [--lo 0x.. --hi 0x..]

Prints, for each of K ticks from T on, every raw dword whose run value differs from the reference value, as
`tick va run ref` (hex). Heap-pointer pairs are kept (unlike prov_compare), so the listing is complete.
"""
import argparse, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "re" / "frida"))
from detorig_ticks import load  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("run"); ap.add_argument("ref")
    ap.add_argument("--from", dest="t0", type=int, default=0)
    ap.add_argument("--ticks", type=int, default=3)
    ap.add_argument("--lo", type=lambda s: int(s, 16), default=0)
    ap.add_argument("--hi", type=lambda s: int(s, 16), default=0xffffffff)
    a = ap.parse_args()
    _, tr, rr = load(a.run)
    _, ta, ra = load(a.ref)
    nblk = ta["nblk"]
    raw_va = []
    for lo, hi in ta.get("raw") or []:
        raw_va += list(range(lo, hi, 4))
    for i in range(a.t0, min(a.t0 + a.ticks, len(rr), len(ra))):
        x, y = rr[i], ra[i]
        for k, v in enumerate(raw_va):
            j = 2 + nblk + k
            if a.lo <= v < a.hi and x[j] != y[j]:
                print(f"{i}\t{v:#010x}\t{x[j]:08x}\t{y[j]:08x}")


if __name__ == "__main__":
    main()

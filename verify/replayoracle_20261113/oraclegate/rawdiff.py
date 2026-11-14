"""RO-ORACLE-GATE run 46: list the --tick-raw words that differ between two detorig runs, per record.

py -3.12 verify/replayoracle_20261113/oraclegate/rawdiff.py DIR_A DIR_B [--recs 0-40] [--max 200]
Prints: record, va, A value, B value (hex and signed decimal). Raw words are recorded every tick, so this needs no dump.
Uses detorig_firstdiv.load_run for the ticks.bin layout and the meta.json raw ranges.
"""
import argparse, sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "re" / "frida"))
import detorig_firstdiv as fd  # noqa: E402


def s32(v):
    v = int(v)
    return v - (1 << 32) if v & 0x80000000 else v


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a"); ap.add_argument("b")
    ap.add_argument("--recs", default="0-40")
    ap.add_argument("--max", type=int, default=200)
    a = ap.parse_args()
    A, B = fd.load_run(a.a), fd.load_run(a.b)
    ta = A["t"]
    raw_va = []
    for lo, hi in ta.get("raw") or []:
        raw_va += list(range(lo, hi, 4))
    off = ta.get("raw_off")
    if off is None:   # raw words sit at the end of each record
        off = ta["recw"] - len(raw_va)
    lo_r, hi_r = (int(x) for x in a.recs.split("-"))
    n = min(len(A["recs"]), len(B["recs"]), hi_r + 1)
    shown = 0
    print(f"raw words {len(raw_va)}, raw offset {off}, recw {ta['recw']}, records {lo_r}..{n - 1}")
    for r in range(lo_r, n):
        ra, rb = A["recs"][r][off:off + len(raw_va)], B["recs"][r][off:off + len(raw_va)]
        for k in (ra != rb).nonzero()[0]:
            x, y = int(ra[k]), int(rb[k])
            print(f"rec {r}\t0x{raw_va[k]:08x}\tA 0x{x:08x} ({s32(x)})\tB 0x{y:08x} ({s32(y)})")
            shown += 1
            if shown >= a.max:
                return


if __name__ == "__main__":
    main()

"""RO-REF-CONTROL-NOISE: dwords of given blocks that differ between runs in the tick checkpoint dumps (base 0x005ea000).

py -3.12 verify/ro_20261115/dumpdiff.py --blocks 0x00771800,0x00614400 --ticks 1,17,18,299,301,600 NAME=DIR ...
For each block, tick and dword where not every run holds the same value: the value of each run (hex and signed decimal).
Pointer-range values (heap rule of detorig_firstdiv.block_hashes: 0x00995000..0x2fffffff, low 2 bits clear) are marked PTR.
"""
import argparse, pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "frida"))
import detorig_firstdiv as fd  # noqa: E402

DATA_LO = 0x005ea000


def s32(v):
    return v - (1 << 32) if v & 0x80000000 else v


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("runs", nargs="+")
    ap.add_argument("--blocks", required=True)
    ap.add_argument("--ticks", default="1,17,18,299,301,600")
    ap.add_argument("--block", type=int, default=1024)
    a = ap.parse_args()
    runs = {}
    for s in a.runs:
        n, d = s.split("=", 1)
        runs[n] = fd.load_run(d)
    print("runs:", " ".join(runs))
    for b in [int(x, 16) for x in a.blocks.split(",")]:
        for t in [int(x) for x in a.ticks.split(",")]:
            dumps = {n: fd.read_dump(r["dumps"][t]) for n, r in runs.items() if t in r["dumps"]}
            if len(dumps) < 2:
                continue
            for va in range(b, b + a.block, 4):
                i = (va - DATA_LO) // 4
                vals = {n: int(w[i]) for n, w in dumps.items()}
                if len(set(vals.values())) > 1:
                    ptr = all(0x00995000 <= v < 0x30000000 and v & 3 == 0 for v in vals.values() if v)
                    print(f"block 0x{b:08x} tick {t} 0x{va:08x}{' PTR' if ptr else ''}: " +
                          " ".join(f"{n}=0x{v:08x}({s32(v)})" for n, v in vals.items()))


if __name__ == "__main__":
    main()

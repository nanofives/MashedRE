"""Summarize the seek-probe captures (PREREG_seek414f00.md section 1).

    py -3.12 summarize.py O1 [O2 ...]

Per arm: obj rows, state histogram, active flag, frames with any DAT_0088ff50[v] != 0 (per v), range of obj+0xc8 (= DAT_00684de0),
and for FUN_00414f00: calls per v, how many pass gate 1 (0.5 <= de0 <= 30.0 or NaN), gate 2 (ff50v != 0), returns by value and
which output branch (cand == target XZ / cand == car-0 XZ).
"""
import csv
import math
import struct
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent


def f(u: int) -> float:
    return struct.unpack("<f", struct.pack("<I", int(u) & 0xFFFFFFFF))[0]


def main() -> int:
    for arm in sys.argv[1:]:
        print(f"=== {arm}")
        objp = HERE / f"{arm}.msd.seekobj.csv"
        rows = list(csv.DictReader(objp.open())) if objp.exists() else []
        print(f"obj rows {len(rows)}")
        if rows:
            st = Counter((r["state_in"], r["state_out"]) for r in rows)
            act = Counter(r["active"] for r in rows)
            print("  state in->out:", dict(st.most_common(12)))
            print("  active(+0x4c):", dict(act))
            for v in range(4):
                n = sum(1 for r in rows if int(r[f"ff50_{v}"]) != 0)
                print(f"  ff50[{v}] != 0 on {n} rows")
            c8 = [f(int(r["c8"])) for r in rows]
            nz = [x for x in c8 if x != 0.0]
            print(f"  c8 (0x00684de0) nonzero rows {len(nz)}  min {min(c8):.6g} max {max(c8):.6g}")
            print(f"  mat ptr distinct: {len(set(r['mat'] for r in rows))}  first {rows[0]['mat']}  last {rows[-1]['mat']}")
            dts = Counter(r["dt"] for r in rows)
            print("  dt top:", dict(dts.most_common(4)))
        sp = HERE / f"{arm}.msd.seek.csv"
        srows = list(csv.DictReader(sp.open())) if sp.exists() else []
        print(f"seek rows {len(srows)}")
        if srows:
            byv = Counter(r["v"] for r in srows)
            g1 = [r for r in srows if (lambda h: math.isnan(h) or (0.5 <= h <= 30.0))(f(int(r["de0"])))]
            g2 = [r for r in g1 if int(r["ff50v"]) != 0]
            rets = Counter(r["ret"] for r in srows)
            print(f"  calls per v {dict(byv)}  gate1 pass {len(g1)}  gate1+2 pass {len(g2)}  ret {dict(rets)}")
            de0 = Counter(r["de0"] for r in srows)
            print("  de0 bits top:", dict(de0.most_common(4)))
            r1 = [r for r in srows if r["ret"] == "1"]
            tgt = sum(1 for r in r1 if (r["cout_x"], r["cout_z"]) == (r["tx"], r["tz"]))
            car0 = sum(1 for r in r1 if (r["cout_x"], r["cout_z"]) == (r["x0"], r["z0"]))
            print(f"  ret=1 rows {len(r1)}: cand==target {tgt}, cand==car0 {car0}")
            unchanged = sum(1 for r in srows if r["ret"] == "0" and (r["cin_x"], r["cin_z"]) != (r["cout_x"], r["cout_z"]))
            print(f"  ret=0 rows with cand changed: {unchanged}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

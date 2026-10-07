# U-9186 leg G1 gate checker. Gates: PREREG_G1.md section 3.
#
# Evaluates FUN_00442a60's gate chain OFFLINE from the probe dump, so the question
# "would porting it write anything?" is answered by measurement rather than by
# transcribing the function.
#
# Usage: py -3.12 verify/d3_u9186_20261008/check_g1.py
import csv
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
ARMS = ["G1base", "G1seed", "G1both"]
PTR_SEEDED = 0x005F2728


def load(name):
    p = HERE / f"{name}.gates.csv"
    if not p.exists():
        return None
    with p.open(newline="") as f:
        return list(csv.DictReader(f))


def main():
    runs = {a: load(a) for a in ARMS}
    have = [a for a in ARMS if runs[a]]
    if not have:
        print("no G1 captures found")
        return 2
    for a in ARMS:
        if not runs[a]:
            print(f"NOTE: {a} missing - reported, not inferred")

    for a in have:
        rows = runs[a]
        n = len(rows)
        print(f"\n########## {a}  ({n} rows) ##########")

        # ---- G1-PTR ----
        ptrs = Counter(r["p5f2770"] for r in rows)
        print(f"  G1-PTR   p5f2770 values: {dict(ptrs)}")

        # ---- G1-GATE1/2/3 ----
        g1 = sum(1 for r in rows if int(r["slot_state"]) != 0 and int(r["slot_state"]) != -1)
        unev = sum(1 for r in rows if int(r["slot_state"]) == -1)
        g2 = sum(1 for r in rows if int(r["veh_type"]) == 1)
        g3 = sum(1 for r in rows if int(r["state0"]) == 0)
        print(f"  G1-GATE1 slot_state != 0 : {g1}/{n} ({100.0*g1/n:.4f}%)"
              f"   [unevaluable (ptr null): {unev}]")
        print(f"           slot_state values: {dict(Counter(r['slot_state'] for r in rows))}")
        print(f"  G1-GATE2 veh_type  == 1 : {g2}/{n} ({100.0*g2/n:.4f}%)"
              f"   values: {dict(Counter(r['veh_type'] for r in rows))}")
        print(f"  G1-GATE3 state0    == 0 : {g3}/{n} ({100.0*g3/n:.4f}%)"
              f"   values: {dict(Counter(r['state0'] for r in rows))}")

        # ---- G1-ALL ----
        allp = sum(1 for r in rows
                   if int(r["slot_state"]) not in (0, -1)
                   and int(r["veh_type"]) == 1
                   and int(r["state0"]) == 0)
        print(f"  G1-ALL   all three pass : {allp}/{n} ({100.0*allp/n:.4f}%)"
              f"   <- rows a ported FUN_00442a60 would write")

        # ---- G1-OUT ----
        out = Counter(r["refdist"] for r in rows)
        print(f"  G1-OUT   refdist distinct values: {len(out)}  "
              f"{dict(list(out.items())[:6])}")

        # ---- G1-POS control ----
        xs = {r["rec_x"] for r in rows}
        zs = {r["rec_z"] for r in rows}
        percar = {v: len({r["rec_x"] for r in rows if r["v"] == v}) for v in "0123"}
        frame0 = [r for r in rows if r["frame"] == rows[0]["frame"]]
        samef = len({(r["rec_x"], r["rec_z"]) for r in frame0})
        print(f"  G1-POS   rec_x distinct {len(xs)}, rec_z distinct {len(zs)}, "
              f"per-car rec_x distinct {percar}")
        print(f"           cars at a single frame occupy {samef} distinct (x,z) "
              f"of {len(frame0)}")
        if len(xs) <= 1 or samef <= 1:
            print("           G1-POS FAIL - the position source is dead; gate counts "
                  "say nothing about FUN_00442a60 specifically (PREREG_G1 section 4)")
        else:
            print("           G1-POS PASS - positions vary over time and between cars")

        # ---- the race_pct input ----
        rp = Counter(r["racepct_ec"] for r in rows)
        print(f"  input    racepct_ec distinct values: {len(rp)}  "
              f"{dict(list(rp.items())[:4])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

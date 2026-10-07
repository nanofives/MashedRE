# Leg E2 gate checker. Gates: PREREG_CONSUMER.md section 4 as amended by AMEND_E2.md.
#
# Scores E2-WROTE, E2-DIFF, E2-EFFECT, E2-LAPAGREE and the G-TOOK determinism witness.
# E2-KNOBOFF and E2-DET are cell-for-cell identity and are scored by det_prefix.py;
# the exact commands are printed at the end.
#
# Usage: py -3.12 verify/d3_consumer_20261007/check_e2.py
import csv
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
OFF = ["E2off_1", "E2off_2", "E2off_3"]
ON = ["E2on_1", "E2on_2", "E2on_3"]
DET_STEP_UNITS = 50            # RESULT_F2.md section 1: dt pinned to 1/60
WROTE_TOL = 1e-6               # float32 round-trip through %.9g
DIFF_BAR = 0.01                # E2-DIFF: >= 1% of 3-car frames


def load(name):
    p = HERE / f"{name}.csv"
    if not p.exists():
        return None
    with p.open(newline="") as f:
        return list(csv.DictReader(f))


def rounds(name):
    p = HERE / f"{name}.log"
    if not p.exists():
        return None
    return [l.strip() for l in p.read_text(errors="replace").splitlines()
            if "RULE-EVAL" in l]


def by_frame(rows):
    d = {}
    for r in rows:
        d.setdefault(int(r["frame"]), {})[r["v"]] = r
    return d


def main():
    runs = {n: load(n) for n in OFF + ON}
    missing = [n for n, r in runs.items() if not r]
    if missing:
        print(f"MISSING captures: {', '.join(missing)} - nothing is scored.")
        return 2
    rd = {n: rounds(n) for n in OFF + ON}

    # ---- G-TOOK (determinism witness, RESULT_F2.md section 1) --------------
    print("== G-TOOK (dt pinned to 1/60 on every frame?) ==")
    bad = []
    for n in OFF + ON:
        steps = {r["step_1008"] for r in runs[n]}
        ok = steps == {str(DET_STEP_UNITS)}
        print(f"  {n}: rows={len(runs[n])} step_1008={sorted(steps)}"
              f"{'   ok' if ok else '   NOT PINNED'}")
        if not ok:
            bad.append(n)
    if bad:
        print(f"\nG-TOOK FAIL: {', '.join(bad)}. No other gate is read.")
        return 1
    print("  G-TOOK PASS")

    # ---- E2-WROTE ----------------------------------------------------------
    # ON  arm: rmetric == lap + arcpct  * 0.01   on 100% of rows
    # OFF arm: rmetric == lap + racepct * 0.01   on 100% of rows
    print("\n== E2-WROTE (does rmetric carry the formula the arm claims?) ==")
    wrote_fail = []
    for n in OFF + ON:
        src = "arcpct" if n in ON else "racepct"
        tot = good = 0
        worst = 0.0
        for r in runs[n]:
            tot += 1
            want = float(r["lap"]) + float(r[src]) * 0.01
            err = abs(float(r["rmetric"]) - want)
            worst = max(worst, err)
            if err <= WROTE_TOL:
                good += 1
        pct = 100.0 * good / tot if tot else 0.0
        ok = good == tot
        print(f"  {n}: lap + {src}*0.01 matches rmetric on {good}/{tot} "
              f"({pct:.4f}%)  max|err|={worst:.3e}{'   ok' if ok else '   FAIL'}")
        if not ok:
            wrote_fail.append(n)
    print("  E2-WROTE " + ("PASS" if not wrote_fail else f"FAIL: {', '.join(wrote_fail)}"))

    # ---- E2-LAPAGREE (AMEND_E2.md A4; can fail, does not block) ------------
    print("\n== E2-LAPAGREE (does the ON formula mix two lap counters?) ==")
    for n in ON + OFF:
        tot = len(runs[n])
        dis = sum(1 for r in runs[n] if int(r["lap"]) != int(r["arclaps"]))
        print(f"  {n}: lap != arclaps on {dis}/{tot} rows "
              f"({100.0 * dis / tot:.4f}%)")
    print("  (>0% means the pre-registered ON formula is internally inconsistent;")
    print("   any E2-EFFECT result then carries that caveat - AMEND_E2.md A4)")

    # ---- E2-DIFF (the control that can fail) -------------------------------
    print("\n== E2-DIFF (does the swap change car ordering?) ==")
    fa, fb = by_frame(runs[OFF[0]]), by_frame(runs[ON[0]])
    shared = sorted(set(fa) & set(fb))
    denom = diff = 0
    for f in shared:
        ra, rb = fa[f], fb[f]
        if not all(v in ra and v in rb for v in ("1", "2", "3")):
            continue
        denom += 1
        oa = sorted(("1", "2", "3"), key=lambda v: -float(ra[v]["rmetric"]))
        ob = sorted(("1", "2", "3"), key=lambda v: -float(rb[v]["rmetric"]))
        if oa != ob:
            diff += 1
    frac = (diff / denom) if denom else 0.0
    print(f"  {OFF[0]} vs {ON[0]}: ordering of cars 1..3 differs on {diff}/{denom} "
          f"3-car frames ({100.0 * frac:.4f}%)   bar = {100.0 * DIFF_BAR:.0f}%")
    if denom == 0:
        print("  E2-DIFF VOID: no frame dumped all three cars.")
    elif frac >= DIFF_BAR:
        print("  E2-DIFF PASS - the swap is NOT inert.")
    else:
        print("  E2-DIFF FAIL - the swap is INERT; no behaviour claim follows.")

    # ---- E2-EFFECT ---------------------------------------------------------
    print("\n== E2-EFFECT (does the match outcome change?) ==")
    for n in [OFF[0], ON[0]]:
        print(f"  --- {n} ---")
        for l in rd[n] or []:
            print(f"      {l}")
    if rd[OFF[0]] == rd[ON[0]]:
        print("  E2-EFFECT: arms produced IDENTICAL R-ROUND -> the swap is INERT "
              "at the outcome level.")
    else:
        print("  E2-EFFECT: R-ROUND DIFFERS between arms - the swap reaches the "
              "match outcome.")

    # ---- cell-for-cell gates ----------------------------------------------
    print("\n== E2-KNOBOFF / E2-DET (run det_prefix.py) ==")
    print("  E2-KNOBOFF, cross-build on the PRE-EXISTING columns "
          "(F2a is the previous build, same knobs, same scenario):")
    print("    py -3.12 re/tools/det_prefix.py "
          "verify/d3_determinism_20261007/F2a.csv "
          "verify/d3_consumer_20261007/E2off_1.csv --common-cols")
    print("  E2-DET, within each arm:")
    for a, b in [(OFF[0], OFF[1]), (OFF[0], OFF[2]), (ON[0], ON[1]), (ON[0], ON[2])]:
        print(f"    py -3.12 re/tools/det_prefix.py "
              f"verify/d3_consumer_20261007/{a}.csv "
              f"verify/d3_consumer_20261007/{b}.csv")
    print("  E2-NOREG-E / E2-NOREG-B: ai_speed_env.py --check / "
          "ai_ctrl_window.py --check on the OFF arm.")
    return 0 if not wrote_fail else 1


if __name__ == "__main__":
    sys.exit(main())

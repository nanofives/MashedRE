# Leg F2 gate checker. Evaluates G-TOOK, VOID-*, G-REPRO and reports the inputs
# G-CTRL needs (det_prefix.py produces G-CTRL/G-PREFIX itself).
#
# Gates and void conditions are PREREG_F2.md sections 2 and 3. This script does not
# interpret a downstream gate when an upstream one fails, per PREREG_F2 section 4.
#
# Usage: py -3.12 verify/d3_determinism_20261007/check_f2.py
import csv
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
RUNS = ["F2a", "F2b", "F2c", "F2x"]
REPEATS = ["F2a", "F2b", "F2c"]
FRAMES = 14400                      # PREREG_F2 section 1

# G-TOOK, CORRECTED 2026-10-07 after the first run. PREREG_F2 section 2 defined it as
# "every run ends at frame 14399", but AiStepDump's `frame` is a DUMP-LOCAL static
# (TrackRenderer.cpp:3994) that starts at the first dumped frame, not g_det_frame, so
# it can never equal DET_FRAMES-1 (902 boot frames precede the race). The gate named
# the wrong counter; its subject is unchanged.
#
# The corrected witness is per-frame and direct: step_1008 is the AI clock advance
# int(in.dt * 3000 + 0.5) (TrackRenderer.cpp:3460), which is exactly 50 iff dt == 1/60,
# i.e. iff exe_main.cpp:2864 pinned sim_real_dt. Requiring 50 on EVERY row proves the
# pin held for the whole run, which the old frame-index check never did.
# The deviation is recorded in RESULT_F2.md section 1.
DET_STEP_UNITS = 50


def rounds(name):
    """R-ROUND: the ordered RULE-EVAL lines, compared as text (PREREG_F1 section 1)."""
    p = HERE / f"{name}.log"
    if not p.exists():
        return None
    return [l.strip() for l in p.read_text(errors="replace").splitlines()
            if "RULE-EVAL" in l]


def csv_stats(name):
    p = HERE / f"{name}.csv"
    if not p.exists():
        return None
    n = 0
    maxf = -1
    steps = set()
    last_clk = None
    with p.open(newline="") as f:
        for row in csv.DictReader(f):
            n += 1
            fr = int(row["frame"])
            if fr > maxf:
                maxf = fr
            steps.add(row["step_1008"])
            last_clk = row["clk_0ff4"]
    return n, maxf, steps, last_clk


def main():
    st = {r: csv_stats(r) for r in RUNS}
    rd = {r: rounds(r) for r in RUNS}

    # ---- VOID-EMPTY -------------------------------------------------------
    empty = [r for r in RUNS if st[r] is None or st[r][0] == 0]
    print("== inputs ==")
    for r in RUNS:
        s = st[r]
        print(f"  {r}: csv={'MISSING' if s is None else f'{s[0]} rows, max frame {s[1]}'}"
              f"   RULE-EVAL lines={'MISSING log' if rd[r] is None else len(rd[r])}")
    print("  (max frame is the DUMP-LOCAL counter, TrackRenderer.cpp:3994 - not g_det_frame)")
    if empty:
        print(f"\nVOID-EMPTY: {', '.join(empty)} produced no stepdump. "
              f"PREREG_F2 section 3 says re-take once, then report. Nothing else is read.")
        return 2

    # ---- G-TOOK -----------------------------------------------------------
    print("\n== G-TOOK (did MASHED_DETERMINISTIC pin the timestep?) ==")
    # The control deliberately runs at a different step rate, so it is exempt.
    took_bad = []
    for r in RUNS:
        _, maxf, steps, clk = st[r]
        nf = maxf + 1
        ratio = (int(clk) / nf) if clk is not None and nf else float("nan")
        exempt = (r == "F2x")
        ok = exempt or (steps == {str(DET_STEP_UNITS)})
        print(f"  {r}: frames={nf} step_1008 values={sorted(steps)} "
              f"last clk_0ff4={clk} clk/frames={ratio:.4f}"
              f"{'   (control - exempt)' if exempt else ('   ok' if ok else '   NOT PINNED')}")
        if not ok:
            took_bad.append(r)
    if took_bad:
        print(f"\nG-TOOK FAIL: {', '.join(took_bad)} carry a step_1008 other than "
              f"{DET_STEP_UNITS}, so dt was not pinned to 1/60 on every frame.")
        print("PREREG_F2 section 4: report the knob failure only. No other gate is read.")
        return 1
    print(f"  G-TOOK PASS - every repeat frame advanced the AI clock by exactly "
          f"{DET_STEP_UNITS} units (dt == 1/60, exe_main.cpp:2864).")

    # ---- VOID-ROUNDS ------------------------------------------------------
    print("\n== VOID-ROUNDS (did the match progress?) ==")
    thin = [r for r in RUNS if len(rd[r] or []) < 2]
    for r in RUNS:
        print(f"  {r}: {len(rd[r] or [])} RULE-EVAL lines")
    if thin:
        print(f"\nVOID-ROUNDS: {', '.join(thin)} produced fewer than 2 rounds. "
              f"R-ROUND is vacuous there; G-REPRO cannot pass. Reported VOID.")
        return 2
    print("  no VOID-ROUNDS - every run reached at least 2 rounds.")

    # ---- G-REPRO (the registered target) ----------------------------------
    print("\n== G-REPRO (R-ROUND identical across 3 repeats) ==")
    base = rd[REPEATS[0]]
    diffs = []
    for r in REPEATS[1:]:
        if rd[r] != base:
            diffs.append(r)
    for r in REPEATS:
        print(f"  --- {r} ---")
        for l in rd[r]:
            print(f"      {l}")
    if diffs:
        print(f"\nG-REPRO FAIL: {', '.join(diffs)} differ from {REPEATS[0]}.")
        n = max(len(rd[r]) for r in REPEATS)
        for i in range(n):
            vals = {r: (rd[r][i] if i < len(rd[r]) else "<absent>") for r in REPEATS}
            if len(set(vals.values())) > 1:
                print(f"  first divergence at round index {i}:")
                for r in REPEATS:
                    print(f"      {r}: {vals[r]}")
                break
        print("PREREG_F2 section 4: determinism mode is insufficient; the residual carrier is")
        print("not a wall-clock or PRNG read visible in mashedmod/src/mashed_re/ (SURVEY_F2 section 5).")
        rc = 1
    else:
        print(f"\nG-REPRO PASS: all three repeats produced identical R-ROUND "
              f"({len(base)} rounds).")
        rc = 0

    # ---- G-CTRL / G-PREFIX inputs -----------------------------------------
    print("\n== G-PREFIX / G-CTRL ==")
    print("Run det_prefix.py (registered definition, membership differences included):")
    for a, b in [("F2a", "F2b"), ("F2a", "F2c"), ("F2b", "F2c"), ("F2a", "F2x")]:
        print(f"  py -3.12 re/tools/det_prefix.py "
              f"verify/d3_determinism_20261007/{a}.csv "
              f"verify/d3_determinism_20261007/{b}.csv")
    print("G-PREFIX passes on full overlap for the three repeat pairs.")
    print(f"G-CTRL passes when R-PREFIX(F2a,F2x) < {int(FRAMES * 0.10)} "
          f"(10% of the run) AND is materially below the repeat pairs.")
    return rc


if __name__ == "__main__":
    sys.exit(main())

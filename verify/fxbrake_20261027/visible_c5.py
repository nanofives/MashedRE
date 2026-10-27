"""D4-FX-BRAKEBYTE-WRITER: per-slot override onsets and the FX-visible brake byte over every AI call.

Usage: py -3.12 visible_c5.py [--orig] <step.csv> [<step.csv> ...]

--orig: an original aistep capture (scenario_launch.py --statediff-aistep). Its probe sits inside FUN_00416250, before
the tail's countdown at 0x004187e2, so ai_override is the value entering the tail and replay(N) = ai_override(N) != 0.

Port step dump (MASHED_AI_STEPDUMP) columns frame, v, c5 (= FUN_00416250's output under MASHED_AI_CONTACT_EVENT,
Ai_RecordStepOut), ai_override (0x0089a4fc + v*0x74 after this call's countdown).
Model (FUN_00418560 0x004187d5..0x0041883a; FUN_00417640 returns at 0x00417643 off course 0x21, Training here):
  replay(N) = ai_override(N-1) != 0 or ai_override(N) != 0
  visible(N) = visible(N-1) if replay(N) else c5(N)            (0x0041880a / 0x00418816 then 0x0041882a / 0x0041883a)
Every slot is cut to its (b) window: 220 calls from the first call with c4 != 0 (the ovon.py / ai_ctrl_window.py rule).
Prints onsets (rises from 0) by window call index and the share of calls with c5 > 0x40 before and after the tail.
"""
import csv
import sys
from collections import defaultdict


def run(path, orig=False):
    seq = defaultdict(list)
    with open(path, newline="") as f:
        for r in csv.DictReader(f):
            seq[int(r["v"])].append((int(r["frame"]), int(r["c5"]), int(float(r["ai_override"])), int(r["c4"])))
    print(path)
    for v in sorted(seq):
        allr = seq[v]
        i0 = next((i for i, x in enumerate(allr) if x[3] != 0), None)
        if i0 is None:
            continue
        rows = allr[i0:i0 + 220]
        prev_ov = int(allr[i0 - 1][2]) if i0 > 0 else 0
        onsets, vis, n_rep = [], 0, 0
        step_brk = vis_brk = 0
        for i, (fr, c5, ov, _c4) in enumerate(rows):
            if ov != 0 and prev_ov == 0:
                onsets.append((i, fr, ov))
            rep = (ov != 0) if orig else (prev_ov != 0 or ov != 0)
            n_rep += rep
            vis = vis if rep else c5
            step_brk += c5 > 0x40
            vis_brk += vis > 0x40
            prev_ov = ov
        n = len(rows)
        print(f"  v={v} calls {n} | replay calls {n_rep} | c5>0x40 step {step_brk} ({100*step_brk/n:.1f}%) "
              f"visible {vis_brk} ({100*vis_brk/n:.1f}%) | onsets (call idx, frame, timer): {onsets[:8]}")


if __name__ == "__main__":
    orig = "--orig" in sys.argv
    for p in [a for a in sys.argv[1:] if a != "--orig"]:
        run(p, orig)

"""D4-FX-BRAKEBYTE-WRITER: join the Z0 pair log with the step dump's override timer.

Usage: py -3.12 replay_join.py <H_Z0.pair.csv> <H_Z0.step.csv>

Model under test (port AiStandalone.cpp step tail = FUN_00418560 0x00418790..0x00418844):
  c5_step = FUN_00416250's output byte (Ai_RecordStepOut, sampled before FUN_00417640 and the replay).
  If the override timer (0x0089a4fc + v*0x74, step.csv ai_override) is nonzero when the tail runs,
  ctrl[5] = stored b5 (0x0089a518 + v*0x74) = the previous call's final ctrl[5].
  So on a replay call the block byte the FX gate reads is the byte frozen at the override's onset.
Reads only. Prints per-slot counts.
"""
import csv
import sys
from collections import defaultdict


def main(pair_path, step_path):
    ovr = {}      # (frame, v) -> ai_override after this call's countdown
    step_c5 = {}
    with open(step_path, newline="") as f:
        for r in csv.DictReader(f):
            k = (int(r["frame"]), int(r["v"]))
            ovr[k] = int(r["ai_override"])
            step_c5[k] = int(r["c5"])
    rows = []
    with open(pair_path, newline="") as f:
        for r in csv.DictReader(f):
            if r["alive"] != "1":
                continue
            rows.append({k: int(v) for k, v in r.items()})
    # final block byte per (frame, v): the FX read of that frame (pre_race == car_end == fx on every row)
    final = {(r["step_frame"], r["v"]): r["c5_fx"] for r in rows}
    stats = defaultdict(lambda: defaultdict(int))
    for r in rows:
        fr, v = r["step_frame"], r["v"]
        s = stats[v]
        s["rows"] += 1
        mism = r["c5_step"] != r["c5_fx"]
        # replay ran on this call iff the timer was nonzero entering the tail: after-countdown value of this
        # call > 0, or the previous call left it nonzero (it can reach 0 on this call's countdown)
        prev = ovr.get((fr - 1, v), 0)
        cur = ovr.get((fr, v), 0)
        replay = prev != 0 or cur != 0
        s["mism"] += mism
        s["replay"] += replay
        s["mism_and_replay"] += mism and replay
        s["mism_no_replay"] += mism and not replay
        s["match_in_replay"] += (not mism) and replay
        if replay and (fr - 1, v) in final:
            s["replay_with_prev"] += 1
            s["replay_fx_eq_prev_final"] += r["c5_fx"] == final[(fr - 1, v)]
        if mism and not replay:
            s.setdefault("examples", [])
            if len(s["examples"]) < 5:
                s["examples"].append((fr, r["c5_step"], r["c5_fx"], prev, cur))
        s["step_csv_agrees"] += step_c5.get((fr, v)) == r["c5_step"]
    for v in sorted(stats):
        s = stats[v]
        print(f"v={v} rows {s['rows']} | mismatch {s['mism']} | replay calls {s['replay']} | "
              f"mismatch&replay {s['mism_and_replay']} | mismatch&no-replay {s['mism_no_replay']} | "
              f"match-in-replay {s['match_in_replay']} | replay fx==prev final {s['replay_fx_eq_prev_final']}/"
              f"{s['replay_with_prev']} | step.csv c5==c5_step {s['step_csv_agrees']}")
        for e in s.get("examples", []):
            print("   no-replay mismatch (frame, c5_step, c5_fx, ovr_prev, ovr_cur):", e)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])

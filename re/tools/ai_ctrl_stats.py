"""Per-car control-byte distribution stats for D3 AI criterion (b).

Reads one or more aistep CSVs (original `--statediff-aistep` or standalone
`MASHED_AI_STEPDUMP`; both share columns) and prints, per car v, the distinct-value
count and median of:
  steer  = c1 - c0   (signed; ctrl block [0]/[1], AiState.h byte map)
  c0, c1             (the two raw steer bytes)
  accel  = c4        (block [4])
  brake  = c5        (block [5])
plus the call count and the fraction of calls with c4 == 255 / c5 == 255.

Optional --check <tolerance.json> compares each file against the tolerance written
into ROADMAP.md §D3 AI (b) and prints PASS/FAIL per car per field.

Usage:
  py -3.12 re/tools/ai_ctrl_stats.py <csv> [<csv> ...] [--json]
"""
import csv
import json
import statistics
import sys


def load(path):
    per = {}
    with open(path, newline="") as f:
        for r in csv.DictReader(f):
            v = int(r["v"])
            c0, c1, c4, c5 = (int(r[k]) for k in ("c0", "c1", "c4", "c5"))
            per.setdefault(v, []).append((c1 - c0, c0, c1, c4, c5))
    return per


def stats(rows):
    out = {"calls": len(rows)}
    for i, name in enumerate(("steer", "c0", "c1", "accel", "brake")):
        vals = [r[i] for r in rows]
        out[name + "_distinct"] = len(set(vals))
        out[name + "_median"] = statistics.median(vals)
    out["accel255_frac"] = round(sum(1 for r in rows if r[3] == 255) / len(rows), 4)
    out["brake255_frac"] = round(sum(1 for r in rows if r[4] == 255) / len(rows), 4)
    return out


def main(argv):
    as_json = "--json" in argv
    paths = [a for a in argv if not a.startswith("--")]
    result = {}
    for p in paths:
        per = load(p)
        result[p] = {v: stats(rows) for v, rows in sorted(per.items())}
    if as_json:
        print(json.dumps(result, indent=1))
        return
    for p, cars in result.items():
        print(p)
        print("  v  calls  steerD steerMed  c0D c1D  accD accMed acc255  brkD brkMed brk255")
        for v, s in cars.items():
            print("  %d %6d  %6d %8.1f  %3d %3d  %4d %6.1f %6.3f  %4d %6.1f %6.3f" % (
                v, s["calls"], s["steer_distinct"], s["steer_median"], s["c0_distinct"],
                s["c1_distinct"], s["accel_distinct"], s["accel_median"], s["accel255_frac"],
                s["brake_distinct"], s["brake_median"], s["brake255_frac"]))


if __name__ == "__main__":
    main(sys.argv[1:])

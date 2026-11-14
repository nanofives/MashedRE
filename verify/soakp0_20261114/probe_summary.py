# SOAK-P0: summarise a detorig --probe run (probe.jsonl). py -3.12 probe_summary.py <probe.jsonl> [--every N]
import json, math, sys

rows = [json.loads(l) for l in open(sys.argv[1]) if l.strip()]
every = int(sys.argv[sys.argv.index("--every") + 1]) if "--every" in sys.argv else 10


def disp(a, b):
    return math.dist(a, b) if a and b else None


race = [r for r in rows if r["phase"] == 3]
print("samples", len(rows), "race samples", len(race))
if race:
    r0 = race[0]
    print("race first sample: tick", r0["tick"], "wall", r0["wall"], "cars", r0["cars"])
    for r in race[::every] + [race[-1]]:
        d = [disp(r0["cars"][k], r["cars"][k]) for k in range(4)]
        print(f"tick {r['tick']} wall {r['wall']} ui {r['ui']} ended {r['ended']} winner {r['winner']} "
              f"fphase {r['fphase']} screen {r['screen']} disp0..3 " + " ".join("%.2f" % x if x is not None else "NA" for x in d))
trans = []
prev = None
for r in rows:
    key = (r["phase"], r["ui"], r["ended"], r["fphase"], r["screen"], r["depth"])
    if key != prev:
        trans.append((r["tick"], r["wall"]) + key)
        prev = key
print("transitions (tick, wall, phase, ui, ended, fphase, screen, depth):")
for t in trans:
    print(" ", t)

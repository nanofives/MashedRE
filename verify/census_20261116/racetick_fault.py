# T2-RACETICK-CENSUS: read a run's crash.jsonl (detorig --crash-catch with --prov-count) and print the fault, the exit, and
# the census counters that moved between the race anchor snapshot and the first fault (detorig.js 'fault-counts').
#   py -3.12 verify/census_20261116/racetick_fault.py <run dir> [--out <tsv>]
# "new" = 0 calls at the anchor, so first run in the race tick before the fault.
import json, sys
from pathlib import Path

d = Path(sys.argv[1])
out = sys.argv[sys.argv.index("--out") + 1] if "--out" in sys.argv else None
recs = [json.loads(l) for l in (d / "crash.jsonl").read_text(errors="replace").splitlines() if l.strip()] if (d / "crash.jsonl").exists() else []
for r in recs:
    if r.get("what") == "fault":
        print("fault n=%s tid=%s eip=%s esp=%s ebp=%s stack0=%s" % (r["n"], r["tid"], r["ctx"]["eip"], r["ctx"]["esp"], r["ctx"]["ebp"], r["stack"][:4]))
    elif r.get("what") == "exit":
        print("exit %s a0=%s n=%s" % (r["fn"], r["a0"], r["n"]))
    elif r.get("what") in ("system", "fault-err", "fault-counts-err", "stall"):
        print(r.get("what"), {k: r.get(k) for k in ("n", "type", "address", "eip", "msg")})
fc = next((r for r in recs if r.get("what") == "fault-counts"), None)
if not fc:
    print("no fault-counts record"); sys.exit(0)
rows = sorted(fc["rows"], key=lambda x: (x[2] != 0, -(x[1] - x[2])))
new = [x for x in rows if x[2] == 0]
print("fault-counts n=%s moved=%d new_after_anchor=%d" % (fc["n"], fc["moved"], len(new)))
for x in new[:80]:
    print("  new %s calls=%d" % (x[0], x[1]))
if out:
    with open(out, "w") as f:
        f.write("rva\tcalls_at_fault\tcalls_at_anchor\tnew\n")
        for x in rows:
            f.write("%s\t%d\t%d\t%d\n" % (x[0], x[1], x[2], 1 if x[2] == 0 else 0))

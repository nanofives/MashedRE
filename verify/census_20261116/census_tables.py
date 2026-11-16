# T2-RACEPATH-CENSUS: markdown tables from census_counts.json (no hand-copied numbers in the RESULT).
#   py -3.12 verify/census_20261116/census_tables.py <census_counts.json> <out.md>
import json, sys
from collections import Counter
d = json.load(open(sys.argv[1])); out = open(sys.argv[2], "w", encoding="utf-8")
ORDER = ["boot", "audio-init", "menu", "race-load", "race-tick"]
ST = ["C4:match", "C4:libid", "C4:frida-replay", "C4", "C3", "C2", "C1", "none"]
BD = ["matched", "replay", "admitted", "provisional", "stand-in", "under-stand-in", "hand-port", "exe-body", "shim",
      "rw-lib", "crt-lib", "missing", "stop", "unbound"]
def table(key, cols, label):
    out.write(f"| phase | {' | '.join(cols)} | total |\n|---|{'---|' * (len(cols) + 1)}\n")
    for p in ORDER:
        row = d[key][p]
        if not row: out.write(f"| {p} | not measured |{' |' * len(cols)}\n"); continue
        extra = sum(v for k, v in row.items() if k not in cols)
        assert extra == 0, (p, {k: v for k, v in row.items() if k not in cols})
        out.write(f"| {p} | {' | '.join(str(row.get(c, 0)) for c in cols)} | {sum(row.values())} |\n")
    out.write("\n")
out.write(f"Distinct functions executed (stage `{d['stage']}`): **{d['executed_distinct']}** of {d['instrumented']} counted; "
          f"{d['reached_today']} ran before the exe's stop at `0x005aa3d0`.\n\n")
out.write("### Phase x RE status (distinct RVAs; an RVA counts in every phase it ran in)\n\n"); table("by_phase_status", ST, "status")
out.write("### Phase x top-down binding\n\n"); table("by_phase_binding", BD, "binding")
out.write("### First phase x binding (each RVA once, in the first phase it ran in)\n\n"); table("first_phase_binding", BD, "binding")
for key, ph in (("race_load_nonmatched", "race-load"), ("race_tick_nonmatched", "race-tick")):
    L = d.get(key) or []
    if not L: continue
    out.write(f"### {ph}: functions not on a matched / replay / admitted / CRT binding, ranked by {ph} calls\n\n")
    out.write("Counts by binding: " + ", ".join(f"{k} {v}" for k, v in Counter(x[2] for x in L).most_common()) + "\n\n")
    for cat in ("provisional", "stand-in", "under-stand-in", "missing", "stop", "hand-port", "shim", "rw-lib", "exe-body"):
        rows = [x for x in L if x[2] == cat]
        if not rows: continue
        out.write(f"- **{cat}** ({len(rows)}): " + ", ".join(f"`{x[0]}` {x[4] or ''} {x[1]}" for x in rows[:15]) + (" ..." if len(rows) > 15 else "") + "\n")
    rows = [x for x in L if x[2] == "unbound"]
    out.write(f"- **unbound** ({len(rows)}; by status " + ", ".join(f"{k} {v}" for k, v in Counter(x[3].split(' ')[0] for x in rows).most_common()) + "), top 20: "
              + ", ".join(f"`{x[0]}` {x[3].split(' ')[0]} {x[5]} {x[1]}" for x in rows[:20]) + "\n\n")
    out.write(f"Unbound by subsystem ({ph}): " + ", ".join(f"{k} {v}" for k, v in d["unbound_by_subsystem"][ph].items()) + "\n\n")
out.write(f"### Audio init window\n\nStops remaining before `0x0045d460`: **{d['stops_before_45d460']}**; window binding "
          + ", ".join(f"{k} {v}" for k, v in d["window_binding"].items()) + "\n")
out.close()

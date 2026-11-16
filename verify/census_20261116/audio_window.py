# T2-RACEPATH-CENSUS: the stops that remain before 0x0045d460 on the audio init path, as one TSV for the Audio lead.
# Rows = the census_counts.json window (first called after the first enter of 0x005aa3d0, the exe's stop today, and up
# to the first enter of 0x0045d460, plus 0x005aa3d0 itself) whose binding is not bound yet.
# class: plan-stop (plan.json stop), C4-unbound (C4 by match / libid, no binding), no-C4-body (needs a body).
# first_call_order: rank of the first call among the window RVAs, from a run whose --prov-snap listed them all
# (snap seq, detorig.js provSnapInstall); blank when the prov json has no such snaps.
# calls_to_45d460 = calls up to the first enter of 0x0045d460; calls_audio_init = calls in the whole 0x004669b0 call.
#   py -3.12 verify/census_20261116/audio_window.py <prov json> <census_counts.json> <out.tsv>
import json, sys
P = json.load(open(sys.argv[1])); C = json.load(open(sys.argv[2])); out = sys.argv[3]
names = [r["rva"].lower() for r in P["rows"]]; ix = {k: i for i, k in enumerate(names)}
S = P["snaps"]
a, b, m45 = S["0x004669b0:enter"]["counts"], S["0x004669b0:leave"]["counts"], S["0x0045d460:enter"]["counts"]
seq = {k.split(":")[0].lower(): v["seq"] for k, v in S.items() if k.endswith(":enter") and "seq" in v}
# first_call_thread: main = the thread that entered 0x004669b0, other = another thread (snap tid)
main = S["0x004669b0:enter"].get("tid")
thr = {k.split(":")[0].lower(): ("main" if v.get("tid") == main else "other") for k, v in S.items() if k.endswith(":enter") and "seq" in v}
rows = []
for rva, bd, st, nm in C["window_5aa3d0_to_45d460"]:
    if bd == "stop": cl = "plan-stop"
    elif bd == "unbound": cl = "C4-unbound" if st.startswith("C4") else "no-C4-body"
    else: continue
    i = ix[rva]
    rows.append([rva, nm, cl, seq.get(rva, ""), m45[i] - a[i], b[i] - a[i], st, thr.get(rva, "")])
if any(r[3] != "" for r in rows):
    win = sorted((seq[r], r) for r, *_ in C["window_5aa3d0_to_45d460"] if r in seq)
    rank = {r: j + 1 for j, (_, r) in enumerate(win)}
    for r in rows: r[3] = rank.get(r[0], "")
    rows.sort(key=lambda r: (r[3] == "", r[3]))
with open(out, "w", newline="") as f:
    f.write("rva\tname\tclass\tfirst_call_order\tcalls_to_45d460\tcalls_audio_init\tstatus\tfirst_call_thread\n")
    for r in rows: f.write("\t".join(str(x) for x in r) + "\n")
print(len(rows), "rows;", {c: sum(1 for r in rows if r[2] == c) for c in ("plan-stop", "C4-unbound", "no-C4-body")})

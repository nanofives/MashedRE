"""PORT-TOP-RWAUDIO-TABLE-632BA8: append (or rewrite) the ADMITTED.tsv rows of the 12 plain-EQUIV table-path sources.
Inputs (this folder): equiv12.json (equiv.py --json at the tool commit below), dyn12.tsv (rva, xcheck_default,
xcheck_seeded: equiv_dyn.py --runs 16, default and --seeded). Refuses any verdict other than plain EQUIV.
    py -3.12 verify/portorigtop_20261113/rwaudio_632ba8/equiv/gen_admitted13.py <source_commit>"""
import json, pathlib, sys
H = pathlib.Path(__file__).resolve().parent
W = H.parents[3]
ADM = W / "re/match/ADMITTED.tsv"
TOOL = "fea5b5483"   # last commit touching re/tools/match/equiv.py, identical to orch/integration's
RES = "verify/portorigtop_20261113/RESULT_PORT-TOP-RWAUDIO-TABLE-632BA8.md"
src_commit = sys.argv[1]
ev = json.load(open(H / "equiv12.json", encoding="utf-8"))
dyn = {}
for l in open(H / "dyn12.tsv", encoding="utf-8"):
    p = l.rstrip("\n").split("\t")
    if p and p[0] != "rva":
        dyn[p[0]] = (p[1], p[2])
rows = []
for e in ev:
    rva = "%08x" % e["addr"]
    assert e["verdict"] == "EQUIV", (rva, e["verdict"])
    d = dyn.get(rva, ("not run", "not run"))
    rows.append("\t".join([
        "0x" + rva, e["sym"], "re/match/s30/%s.c" % rva, "/O2 /GS", src_commit, TOOL, "EQUIV",
        ",".join(e["classes"]), ",".join(e["rules"]) or "-", "n/a (plain EQUIV)", "-", d[0], d[1],
        "Match/PortTop_%s.cpp (exe, MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT), reached through table 0x00632ba8 "
        "(image code-pointer fix-up)" % rva, RES]))
raw = open(ADM, "rb").read().decode("utf-8")
nl = "\r\n" if "\r\n" in raw else "\n"   # keep the file's line ending (CRLF today)
keep = [l for l in raw.splitlines() if not any(l.startswith("0x%08x\t" % e["addr"]) for e in ev)]
open(ADM, "w", encoding="utf-8", newline="").write(nl.join(keep + rows) + nl)
print("rows", len(rows))

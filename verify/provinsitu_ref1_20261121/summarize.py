# PROV-INSITU-REF1: verdict table from the prov_bisect state files.
#   py -3.12 summarize.py [--tags b1,b2,...] [--rep repstate.jsonl]
# Writes VERDICTS.tsv (one row per sweep row) and prints counts by class. Single-body DIVERGED / CRASH verdicts need a
# confirming run (rep_state.jsonl, same verdict; DIVERGED at the same tick) to count as confirmed.
import argparse, csv, json, collections
from pathlib import Path
H = Path(__file__).resolve().parent
D = Path("D:/MashedWT/PROV-INSITU-REF1_data")
ap = argparse.ArgumentParser()
ap.add_argument("--tags", default="b1,b2,b3")
a = ap.parse_args()

sweep = list(csv.DictReader(open(H / "sweep309.tsv"), delimiter="\t"))
bodies = {x["rva"]: x for x in json.load(open(D / "bodies309.json"))["bodies"]}
cen = {r["rva"].lower(): r for r in json.load(open(D / "census" / "prov.json"))["rows"]}
st = {}
for t in a.tags.split(","):
    f = D / f"{t}_state.jsonl"
    if f.exists():
        for ln in f.read_text().splitlines():
            x = json.loads(ln); st[x["rva"]] = x
rep = collections.defaultdict(list)
f = D / "rep_state.jsonl"
if f.exists():
    for ln in f.read_text().splitlines():
        x = json.loads(ln); rep[x["rva"]].append(x)
xtra = {}
f = H / "extra_verdicts.json"   # crash addresses etc. written by hand from cited run logs
if f.exists():
    xtra = json.loads(f.read_text())


def wleg(b):
    w = b.get("writes")
    if not w:
        return "-"
    if not w["abs_in"] and not w["abs_out"] and not w["indirect"]:
        return "empty"
    if not w["abs_out"] and not w["indirect"]:
        return "covered"
    return "not-covered"


rows = []
for r in sweep:
    rva = r["rva"]; b = bodies.get(rva, {}); s = st.get(rva)
    c = cen.get(rva, {})
    if b.get("status") != "OK":
        v, det = "NOT-WIRED", ",".join(b.get("unresolved", []))[:120]
    elif not s:
        v, det = "UNTESTED", ""
    else:
        v = s["verdict"]
        if v == "IDENTICAL-FOR-N":
            v = f"IDENTICAL-FOR-{s['n']}"
        elif v == "DIVERGED":
            v = f"DIVERGED@{s['n']}"
        det = ""
        if s.get("first"):
            fz = s["first"]
            det = f"tick {fz.get('tick')} {fz.get('segment', '')} blocks {[x[0] for x in fz.get('blocks', [])][:6]}"
        rp = rep.get(rva, [])
        if v.startswith(("DIVERGED", "CRASH")):
            same = [x for x in rp if x["verdict"] == s["verdict"] and (s["verdict"] != "DIVERGED" or x["n"] == s["n"])]
            det = (f"confirmed {1 + len(same)} of {1 + len(rp)}; " if rp else "unconfirmed; ") + det
        if rva in xtra:
            det += "; " + xtra[rva]
    calls = s.get("calls_total") if s else None
    rows.append({"rva": rva, "name": r["name"], "level": r["level"], "class": r["class"], "subsystem": r["subsystem"],
                 "best_pct": r["best_pct"], "link_pct": b.get("pct", ""), "source": b.get("source", ""),
                 "jmp_ok": s.get("jmp_ok") if s else "", "calls_run": calls if calls is not None else "",
                 "calls_census": c.get("calls_total", ""), "group": s.get("group", "") if s else "",
                 "run": s.get("run", "") if s else "", "wleg_static": wleg(b), "insitu_1111": r["insitu_1111"],
                 "verdict": v, "detail": det})
with open(H / "VERDICTS.tsv", "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t"); w.writeheader(); w.writerows(rows)
k = lambda v: v.split("@")[0].split("-FOR-")[0]
tab = collections.defaultdict(collections.Counter)
for x in rows:
    tab[x["class"]][k(x["verdict"])] += 1
cols = ["IDENTICAL", "DIVERGED", "CRASH", "HANG", "NO-TICKS", "NOT-WIRED", "UNTESTED"]
print("class\t" + "\t".join(cols) + "\ttotal")
for c in ("game", "RW-Physics", "RW core"):
    print(c + "\t" + "\t".join(str(tab[c][x]) for x in cols) + f"\t{sum(tab[c].values())}")
tot = collections.Counter()
for c in tab: tot.update(tab[c])
print("all\t" + "\t".join(str(tot[x]) for x in cols) + f"\t{sum(tot.values())}")
others = set(tot) - set(cols)
if others: print("other verdicts:", {o: tot[o] for o in others})
ide = [x for x in rows if x["verdict"].startswith("IDENTICAL")]
print("IDENTICAL by static write-set:", collections.Counter(x["wleg_static"] for x in ide))

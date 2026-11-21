# PROV-INSITU-REF1: PROMOTIONS.tsv (one row per re/promotions/PROV-INSITU-REF1.tsv row: name, class, verdict run, calls,
# write-set leg) and a markdown list for the RESULT on stdout.
import csv
from pathlib import Path
H = Path(__file__).resolve().parent
ROOT = H.parents[1]
V = {r["rva"]: r for r in csv.DictReader(open(H / "VERDICTS.tsv"), delimiter="\t")}
W = {r["rva"]: r for r in csv.DictReader(open(H / "WS_CUT.tsv"), delimiter="\t")}
P = [r["rva"] for r in csv.DictReader(open(ROOT / "re/promotions/PROV-INSITU-REF1.tsv"), delimiter="\t")]
out = []
for rva in P:
    v = V[rva]
    leg = f"static {v['wleg_static']}" if v["wleg_static"] in ("empty", "covered") else \
        f"dynamic covered, {W[rva]['image']} compared-image stores in {W[rva]['calls_stepped']} stepped calls ({W[rva]['ws_run']}, {W[rva]['ws_run_replay']})"
    out.append({"rva": rva, "name": v["name"], "level": v["level"], "class": v["class"], "verdict": v["verdict"], "run": v["run"],
                "calls_run": v["calls_run"], "jmp_ok": v["jmp_ok"], "write_set_leg": leg, "source": v["source"], "link_pct": v["link_pct"]})
with open(H / "PROMOTIONS.tsv", "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(out[0]), delimiter="\t"); w.writeheader(); w.writerows(out)
for c in ("game", "RW-Physics", "RW core"):
    rows = [o for o in out if o["class"] == c]
    print(f"- {c} ({len(rows)}): " + ", ".join(f"`{o['rva']}`" + ("" if o["name"].startswith("FUN_") else f" {o['name']}")
                                                 + (" [dyn]" if o["write_set_leg"].startswith("dynamic") else "") for o in rows))
thin = [o for o in out if o["calls_run"] and int(o["calls_run"]) < 10]
print("\nThin rows (fewer than 10 body calls in the verdict run; the verdict covers only those calls): "
      + ", ".join(f"`{o['rva']}` {o['calls_run']}" for o in thin))

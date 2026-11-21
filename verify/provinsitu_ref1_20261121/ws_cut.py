# PROV-INSITU-REF1 write-set cut (PROV-WRITESET-PROMOTE rule): a body passes the dynamic leg when its stepped calls are
# covered (frame or compared-image stores only) with at least one store into the compared image, AND its write-set run
# itself replays IDENTICAL-FOR-600 against ref1 / ref2 (a diverging stepped run is a different race). Writes WS_CUT.tsv.
import csv, json, sys
from pathlib import Path
H = Path(__file__).resolve().parent
sys.path.insert(0, str(H.parents[1] / "re" / "frida"))
from prov_compare import verdict  # noqa: E402
D = Path("D:/MashedWT/PROV-INSITU-REF1_data"); R = "D:/MashedWT/RO-REF-NAME_data"
grp, gv = {}, {}
for g in sorted(D.glob("ws_g[0-9][0-9]")):
    v = verdict(g, f"{R}/ref1", f"{R}/ref2")
    gv[g.name] = f"{v['verdict']} {v['identical_for']}"
    for x in json.loads((g / "prov_install.json").read_text())["installed"]:
        grp[x["rva"].lower()] = g.name
ws = list(csv.DictReader(open(H / "WRITESET_TABLE.tsv"), delimiter="\t"))
out = []
for r in ws:
    g = grp.get(r["rva"], "?")
    ident = gv.get(g, "").startswith("IDENTICAL")
    cov_img = r["verdict"] == "covered" and int(r["image"]) > 0
    leg = ("pass" if cov_img and ident else "covered+image, run diverged" if cov_img else
           "covered, frame only (L-40 stays)" if r["verdict"] == "covered" else r["verdict"])
    out.append({"rva": r["rva"], "ws_run": g, "ws_run_replay": gv.get(g, ""), "calls_stepped": r["calls_stepped"],
                "stores": r["stores"], "image": r["image"], "dyn_verdict": r["verdict"], "leg": leg})
with open(H / "WS_CUT.tsv", "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(out[0]), delimiter="\t"); w.writeheader(); w.writerows(out)
import collections
print(gv)
print(collections.Counter(o["leg"] for o in out))

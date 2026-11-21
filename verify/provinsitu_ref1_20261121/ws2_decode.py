# PROV-INSITU-REF1-WS2 step 5: decode the write logs of this row's stepped runs with re/tools/prov/prov_writeset.py
# (unchanged decoder and classes), then apply the PROV-INSITU-REF1 cut rule (ws_cut.py): a body passes the dynamic leg
# when its stepped calls are covered (frame or compared-image stores only) with at least one compared-image store AND
# the stepped run itself replays IDENTICAL-FOR-600 against ref1 / ref2. A body that diverges or crashes in its own
# verdict run, or has a standing long-race DIVERGED (the 3 holds), is never a candidate.
#   py -3.12 ws2_decode.py      -> WS2_WRITESET.tsv (one row per body and run), WS2_CUT.tsv (one row per body)
import csv, json, subprocess, sys
from pathlib import Path

H = Path(__file__).resolve().parent
WT = H.parents[1]
D = Path("D:/MashedWT/PROV-INSITU-REF1-WS2_data")
SRC = Path("D:/MashedWT/PROV-INSITU-REF1_data")
R = "D:/MashedWT/RO-REF-NAME_data"
HOLD = {"0x00469aa0", "0x0046e9e0", "0x004709a0"}
state = [json.loads(l) for l in (D / "ws2_state.jsonl").read_text().splitlines() if l.strip()]
ver = {r["rva"]: r for r in csv.DictReader(open(H / "VERDICTS.tsv"), delimiter="\t")}
runs = [x for x in state if x["label"].startswith(("s_", "st_", "ws_g")) and x.get("wlog")]
rows = []
for bodies in sorted({x["bodies"] for x in runs}):
    rs = [x for x in runs if x["bodies"] == bodies]
    rvas = sorted({rv for x in rs for rv in x["rvas"]})
    for x in rs:
        out = D / f"wsdec_{x['label']}.tsv"
        if not out.exists():
            subprocess.check_call(["py", "-3.12", "re/tools/prov/prov_writeset.py", "--bodies", bodies, "--run", str(D / x["label"]),
                                   "--allowed-ref", f"{R}/ref1,{R}/ref2", "--rvas", ",".join(x["rvas"]), "--out", str(out)], cwd=WT)
        for r in csv.DictReader(open(out), delimiter="\t"):
            r.update(run=x["label"], run_replay=f"{x['verdict']} {x.get('identical_for', '')}".strip(), run_bodies=len(x["rvas"]))
            rows.append(r)
hdr = ["rva", "run", "run_bodies", "run_replay", "calls_stepped", "steps", "stores", "frame", "image", "image_masked",
       "caller_stack", "blob", "other", "verdict", "detail"]
with open(H / "WS2_WRITESET.tsv", "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=hdr, delimiter="\t", extrasaction="ignore"); w.writeheader(); w.writerows(rows)
# per body: an escape seen in any stepped run (first-10 or stride sample) makes the body escape; otherwise the best run
# is an IDENTICAL stepped run with a covered verdict and image > 0 (pass), then covered+image in a diverged run, then
# frame only
by = {}
for r in rows:
    by.setdefault(r["rva"], []).append(r)
cut = {}
for rv, rs in by.items():
    own = ver.get(rv, {}).get("verdict", "")
    esc = [r for r in rs if r["verdict"] == "escapes"]
    cov_img = [r for r in rs if r["verdict"] == "covered" and int(r["image"] or 0) > 0]
    ok = [r for r in cov_img if r["run_replay"].startswith("IDENTICAL")]
    if esc:
        leg, r = "escapes", max(esc, key=lambda r: int(r["calls_stepped"] or 0))
    elif ok and own.startswith("IDENTICAL") and rv not in HOLD:
        leg, r = "pass", max(ok, key=lambda r: int(r["calls_stepped"] or 0))
    elif cov_img:
        leg, r = "covered+image, run diverged", cov_img[0]
    elif any(r["verdict"] == "covered" for r in rs):
        leg, r = "covered, frame only (L-40 stays)", max((r for r in rs if r["verdict"] == "covered"), key=lambda r: int(r["calls_stepped"] or 0))
    else:
        leg, r = "not-run", rs[0]
    cut[rv] = {"rva": rv, "name": ver.get(rv, {}).get("name", ""), "class": ver.get(rv, {}).get("class", ""),
               "run": r["run"], "run_replay": r["run_replay"], "calls_stepped": r["calls_stepped"], "stores": r["stores"],
               "frame": r["frame"], "image": r["image"], "verdict": r["verdict"], "leg": leg,
               "runs": ";".join(f"{x['run']}:{x['verdict']}:{x['calls_stepped']}" for x in rs)}
with open(H / "WS2_CUT.tsv", "w", newline="") as f:
    w = csv.DictWriter(f, fieldnames=list(next(iter(cut.values()))), delimiter="\t"); w.writeheader(); w.writerows(sorted(cut.values(), key=lambda c: c["rva"]))
import collections
print(collections.Counter(c["leg"] for c in cut.values()))
print("pass:", [c["rva"] for c in cut.values() if c["leg"] == "pass"])

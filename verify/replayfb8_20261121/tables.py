# REPLAY-FALLBACK-8 / PROV-INSITU-REF1-NOSRC: verdict + write-set table per step (VERDICTS_<step>.tsv), the promotions
# TSVs (re/promotions/<ID>.tsv, rows passing every L-128 leg) and RAW_SHA256.txt over the cited raw files.
#   py -3.12 verify/replayfb8_20261121/tables.py
import csv, hashlib, json, sys
from pathlib import Path
H = Path(__file__).resolve().parent
WT = H.parents[1]
sys.path.insert(0, str(WT / "re" / "frida"))
from prov_compare import verdict  # noqa: E402
D = Path("D:/MashedWT/REPLAY-FALLBACK-8_data")
R = "D:/MashedWT/RO-REF-NAME_data"
S = list(csv.DictReader(open(H / "SOURCES.tsv"), delimiter="\t"))
cen = {r["rva"].lower(): r for r in json.loads((D / "census" / "prov.json").read_text())["rows"]}
IDS = {"fb8": "REPLAY-FALLBACK-8", "nosrc": "PROV-INSITU-REF1-NOSRC"}
RES = {"fb8": "verify/replayfb8_20261121/RESULT_REPLAY-FALLBACK-8.md",
       "nosrc": "verify/replayfb8_20261121/RESULT_PROV-INSITU-REF1-NOSRC.md"}
LVL = {l.split(",")[0].lower(): l.split(",")[3] for l in (WT / "hooks.csv").read_text(encoding="utf-8", errors="replace").splitlines() if l.count(",") > 3}
raw = []   # the no-STUB leg is checked by c4_reclass.py batch (dry run), not here


def sha(p):
    if p.exists():
        raw.append(f"{hashlib.sha256(p.read_bytes()).hexdigest()}  {p.as_posix()}")


for f in ("c0/ticks.bin", "census/prov.json", "census/prov_install.json"):
    sha(D / f)
for step in ("fb8", "nosrc"):
    bodies = {b["rva"]: b for b in json.loads((D / f"bodies_{step}.json").read_text())["bodies"]}
    sha(D / f"bodies_{step}.json")
    st = {}
    sf = D / f"{step}_state.jsonl"
    if sf.exists():
        sha(sf)
        for ln in sf.read_text().splitlines():
            x = json.loads(ln); st[x["rva"]] = x
    ws = {}
    wf = H / f"WRITESET_{step}.tsv"
    if wf.exists():
        ws = {r["rva"].lower(): r for r in csv.DictReader(open(wf), delimiter="\t")}
    rows, prom = [], []
    for s in [x for x in S if x["step"] == step]:
        r = s["rva"].lower(); b = bodies[r]; v = st.get(r); c = cen.get(r, {})
        w = b.get("writes") or {}
        static = ("empty" if not (w.get("abs_in") or w.get("abs_out") or w.get("indirect")) else
                  "covered" if not (w.get("abs_out") or w.get("indirect")) else "not-covered")
        if not c.get("calls_total"):
            verd, run = "NOT-EXECUTED", "census"
        elif not v:
            verd, run = "UNTESTED", ""
        else:
            verd = {"IDENTICAL-FOR-N": f"IDENTICAL-FOR-{v['n']}", "DIVERGED": f"DIVERGED@{v['n']}"}.get(v["verdict"], v["verdict"])
            run = v["run"]
            for f in ("ticks.bin", "prov.json", "prov_install.json", "meta.json"):
                sha(D / run / f)
        dyn, wsrun, wsrep = "", "", ""
        if r in ws:
            L = D / f"ws_{r[2:]}"
            wv = verdict(L, f"{R}/ref1", f"{R}/ref2")
            wsrep = f"{wv['verdict']} {wv['identical_for']}"
            wsrun = L.name
            cov = ws[r]["verdict"] == "covered" and int(ws[r]["image"] or 0) > 0
            dyn = (f"pass ({ws[r]['image']} image stores in {ws[r]['calls_stepped']} stepped calls)" if cov and wsrep.startswith("IDENTICAL")
                   else f"{ws[r]['verdict']} (image {ws[r]['image']}, stores {ws[r]['stores']}, stepped {ws[r]['calls_stepped']}, run {wsrep})")
            for f in ("ticks.bin", "prov.json", "prov_install.json", "wlog.bin"):
                sha(L / f)
        leg = f"static {static}" if static in ("empty", "covered") else (f"dynamic {dyn}" if dyn else "not-covered, no dynamic run")
        legok = static in ("empty", "covered") or dyn.startswith("pass")
        jmp = v.get("jmp_ok") if v else None
        row = {"rva": r, "source": s["source"], "link_pct": b.get("pct"), "neighbour": s["neighbour"],
               "calls_census": c.get("calls_total"), "calls_before_warp": c.get("calls_before_warp"),
               "verdict": verd, "run": run, "calls_run": v.get("calls_total") if v else "", "jmp_ok": jmp,
               "wleg_static": static, "ws_run": wsrun, "ws_run_replay": wsrep, "write_set_leg": leg,
               "promote": bool(verd == "IDENTICAL-FOR-600" and jmp and legok)}
        rows.append(row)
        row["level_now"] = LVL.get(r[2:], "")
        if row["promote"] and row["level_now"] != "C4":   # already C4: no write (c4_reclass SKIP), kept out of the TSV
            prom.append(r)
    with open(H / f"VERDICTS_{step}.tsv", "w", newline="") as f:
        wr = csv.DictWriter(f, fieldnames=list(rows[0]), delimiter="\t"); wr.writeheader(); wr.writerows(rows)
    with open(WT / "re" / "promotions" / f"{IDS[step]}.tsv", "w", newline="") as f:
        f.write("rva\troute\tevidence\tresult\n")
        for r in prom:
            f.write(f"{r}\tfrida-replay\t{RES[step]}\t{RES[step]}\n")
    for x in rows:
        print(step, x["rva"], x["verdict"], x["calls_census"], x["write_set_leg"], "PROMOTE" if x["promote"] else "")
(H / "RAW_SHA256.txt").write_text("\n".join(raw) + "\n")
print(len(raw), "raw files hashed")

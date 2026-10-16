#!/usr/bin/env python3
"""rprops scorer: drawlist_diff 3D mode on the nine d1r1 frames for one standalone arm, plus the
rule-2 collateral review against another arm.

    py -3.12 score_rprops.py <arm_dir_under_log/rprops> [--vs <other arm>] [--out <txt>]

An arm directory holds <track>/d1r2/stand_row_<K>.draws.json for track in training/arctic/city as
written by run_replay3d.py (out dir = log/rprops/<arm>_<track>). Original-side records and cites
come from verify/d1r1_20261016 (read only). Per frame: matched draws, matched triangles, rows,
RED rows, rows by citation id. With --vs: rows present here that the other arm lacks (NEW) and
rows that went away (GONE), keyed like verify/d1r1_20261016/rowdelta.py.
"""
import json
import re
import subprocess
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
D1 = WT / "verify" / "d1r1_20261016"
LOG = WT / "log" / "rprops"
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
FRAMES = {
    "training": (884, 924, 959),
    "arctic": (1051, 1091, 1126),
    "city": (761, 801, 836),
}


def key(r):
    if r["kind"] in ("STATE", "TEX1"):
        return (r["kind"], r["tex"], re.sub(r"^\d+ matched draws", "", r["detail"]))
    if r["kind"] == "FOG":
        return (r["kind"], re.sub(r"\d+ matched draws, ", "", r["detail"]).split(",")[0])
    return (r["kind"], r["tex"])


def run_one(arm, trk, k, cites):
    orig = D1 / "evidence" / f"{trk}_orig_row_{k:05d}.draws.json"
    stand = LOG / f"{arm}_{trk}" / "d1r2" / f"stand_row_{k:05d}.draws.json"
    out = LOG / f"score_{arm}_{trk}_{k}.json"
    if not stand.exists():
        return None
    p = subprocess.run(
        [sys.executable, str(WT / "re" / "tools" / "drawlist_diff.py"), str(orig), str(stand),
         "--trace", str(MAIN / "log" / "d1r1" / f"orig_{trk}" / "trace.csv"),
         "--cite", str(cites), "--json", str(out), "--max-rows", "0"],
        capture_output=True, text=True)
    txt = p.stdout
    m = re.search(r"matched (\d+) draws \((\d+) tri\); rows (\d+) \((\d+) cited\)", txt)
    v = re.search(r"VERDICT: (\w+)", txt)
    d = json.loads(out.read_text())
    return {
        "trk": trk, "k": k, "na": d["na"], "nb": d["nb"], "tri_a": d["tri_a"],
        "matched": int(m.group(1)), "mtri": int(m.group(2)), "rows": int(m.group(3)),
        "cited": int(m.group(4)), "verdict": v.group(1) if v else "?",
        "bycite": _bycite(d["rows"]), "rowset": {key(r): r for r in d["rows"]},
    }


def _bycite(rows):
    c = {}
    for r in rows:
        cid = (r.get("cite") or "RED").split("-")[0]
        c[cid] = c.get(cid, 0) + 1
    return c


def main():
    a = sys.argv[1:]
    arm = a[0]
    vs = a[a.index("--vs") + 1] if "--vs" in a else None
    outp = Path(a[a.index("--out") + 1]) if "--out" in a else None
    cites = D1 / "cites_d1r1.json"
    lines = []
    res = {}
    for trk, ks in FRAMES.items():
        for k in ks:
            r = run_one(arm, trk, k, cites)
            if r:
                res[(trk, k)] = r
    lines.append(f"arm {arm}")
    lines.append(f"{'frame':<14}{'orig':>5}{'match':>6}{'orig tri':>9}{'match tri':>10}{'tri%':>7}"
                 f"{'rows':>6}{'red':>4}  verdict")
    for (trk, k), r in res.items():
        pct = 100.0 * r["mtri"] / r["tri_a"] if r["tri_a"] else 0.0
        lines.append(f"{trk + ' ' + str(k):<14}{r['na']:>5}{r['matched']:>6}{r['tri_a']:>9}"
                     f"{r['mtri']:>10}{pct:>6.1f}%{r['rows']:>6}{r['rows'] - r['cited']:>4}  {r['verdict']}")
    lines.append("rows by citation id (summed per track):")
    for trk in FRAMES:
        tot = {}
        for (t, _k), r in res.items():
            if t == trk:
                for c, n in r["bycite"].items():
                    tot[c] = tot.get(c, 0) + n
        lines.append(f"  {trk:<9}" + " ".join(f"{c}={n}" for c, n in sorted(tot.items())))
    if vs:
        lines.append(f"rule 2 collateral review vs arm {vs} (NEW = row here the other arm lacks):")
        new_total = 0
        worse = 0
        for (trk, k), r in res.items():
            o = run_one(vs, trk, k, cites)
            if not o:
                continue
            new = [x for x in r["rowset"] if x not in o["rowset"]]
            gone = [x for x in o["rowset"] if x not in r["rowset"]]
            new_total += len(new)
            dm = r["matched"] - o["matched"]
            dt = r["mtri"] - o["mtri"]
            if dm < 0 or dt < 0:
                worse += 1
            lines.append(f"  {trk} {k}: rows {o['rows']}->{r['rows']} gone {len(gone)} NEW {len(new)} "
                         f"matched {o['matched']}->{r['matched']} ({dm:+d}) tri {o['mtri']}->{r['mtri']} ({dt:+d})")
            for x in new:
                rr = r["rowset"][x]
                lines.append(f"      NEW {rr['kind']} {rr['tex']} [{rr.get('cite') or 'RED'}]: {rr['detail'][:160]}")
        lines.append(f"  NEW ROWS TOTAL {new_total}; frames with fewer matched draws or triangles: {worse}")
    txt = "\n".join(lines)
    print(txt)
    if outp:
        outp.write_text(txt + "\n")


if __name__ == "__main__":
    main()

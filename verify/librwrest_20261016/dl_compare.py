"""librwrest D-LIST gate (PREREG_librwrest.md section 3): per shot, run re/tools/drawlist_diff.py
(3D mode, d1r1 citations) on the original's draw record against a base and a branch standalone
record, and compare.

    py -3.12 dl_compare.py <out.md> <track> <orig_dir> <base_dir> <post_dir> <trace.csv> K1,K2,...

orig_dir holds orig_row_<K>.draws.json; base_dir / post_dir hold d1r2/stand_row_<K>.draws.json
(run_replay3d.py layout). Reports per shot: rows, RED (uncited), matched draws, per citation
counts, and the original-draw indices that are unmatched in post but matched in base (the
"no previously matched draw stops matching" rule), with their rows.
"""
import json
import subprocess
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent
DIFF = REPO / "re" / "tools" / "drawlist_diff.py"
CITE = REPO / "verify" / "d1r1_20261016" / "cites_d1r1.json"


def run(orig, stand, trace, out_json):
    subprocess.run([sys.executable, str(DIFF), str(orig), str(stand), "--trace", str(trace),
                    "--cite", str(CITE), "--json", str(out_json)],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
    return json.loads(Path(out_json).read_text())


def summarize(j):
    rows = j.get("rows", [])
    red = [r for r in rows if not r.get("cite")]
    cites = Counter(r.get("cite") or "RED" for r in rows)
    kinds = Counter(r["kind"] for r in rows)
    una = set()
    for r in rows:
        una.update(r.get("a_idx") or [])
    return rows, red, cites, kinds, una


def main():
    out_md, track, od, bd, pd, trace, shots = sys.argv[1:8]
    od, bd, pd = Path(od), Path(bd), Path(pd)
    lines = [f"# D-LIST {track}: base vs post (drawlist_diff 3D, cites_d1r1.json)", ""]
    lines.append("| shot | arm | rows | RED | matched / orig draws | kinds |")
    lines.append("|---|---|---:|---:|---|---|")
    detail = []
    for k in [int(s) for s in shots.split(",")]:
        orig = od / f"orig_row_{k:05d}.draws.json"
        res = {}
        for arm, d in (("base", bd), ("post", pd)):
            st = d / "d1r2" / f"stand_row_{k:05d}.draws.json"
            j = run(orig, st, trace, d / f"dl_{k:05d}.json")
            res[arm] = (j,) + summarize(j)
            rows, red, cites, kinds, una = res[arm][1:]
            lines.append(f"| {k} | {arm} | {len(rows)} | {len(red)} | {j.get('matched')} / {j.get('na')} | "
                         + ", ".join(f"{a} {b}" for a, b in sorted(kinds.items())) + " |")
        bc, pc = res["base"][3], res["post"][3]
        ids = sorted(set(bc) | set(pc))
        detail.append(f"## shot {k}: rows per citation (base -> post)")
        detail.append("")
        detail.append("| cite | base | post |")
        detail.append("|---|---:|---:|")
        for c in ids:
            if bc.get(c, 0) != pc.get(c, 0):
                detail.append(f"| **{c}** | {bc.get(c, 0)} | {pc.get(c, 0)} |")
            else:
                detail.append(f"| {c} | {bc.get(c, 0)} | {pc.get(c, 0)} |")
        newly = sorted(res["post"][5] - res["base"][5])
        gone = sorted(res["base"][5] - res["post"][5])
        detail.append("")
        detail.append(f"original draws unmatched in post but matched in base: {len(newly)} {newly[:40]}")
        detail.append(f"original draws matched in post but unmatched in base: {len(gone)} {gone[:40]}")
        for r in res["post"][1]:
            if set(r.get("a_idx") or []) & set(newly):
                detail.append(f"- post row {r['kind']} cite={r.get('cite')} tex={r.get('tex')}: {r.get('detail', '')[:300]}")
        for r in res["post"][2]:
            detail.append(f"- post RED row {r['kind']} tex={r.get('tex')}: {r.get('detail', '')[:300]}")
        detail.append("")
    Path(out_md).write_text("\n".join(lines + [""] + detail) + "\n")
    print("\n".join(lines))


if __name__ == "__main__":
    main()

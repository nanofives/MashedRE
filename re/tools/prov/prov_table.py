"""PROV-INSITU verdict table: one row per blocking function with a match source, from the provlink bodies JSONs, the
census run and the group-test state files (prov_bisect.py). Writes a TSV and prints the counts.

    py -3.12 re/tools/prov/prov_table.py --census D:/.../census --ref D:/.../ref_a --ref2 D:/.../ref_b \
        --bodies a.json,b.json --state t95_state.jsonl,... [--dwords dwords.json] --out PROV_VERDICTS.tsv

Write-set leg (CONFIDENCE.md, L-38 / L-40 / L-54), from the body's own stores (provlink write_set, static):
  empty      no store outside its frame: nothing to compare (L-54)
  covered    every store is to an absolute VA in the per-tick compared image, outside the blocks two unmodified runs
             already disagree on
  not-covered  an indirect store (register base: heap or static, not decided statically; the per-tick hash covers the
             static image only and hashes heap-pointer-valued dwords as 0), an absolute store outside the image, or a
             store into an allowed (masked) block
"""
import argparse, json, pathlib, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(ROOT / "re" / "frida"))
from detorig import DATA_LO  # noqa: E402
from prov_compare import verdict  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--census", required=True)
    ap.add_argument("--ref", required=True)
    ap.add_argument("--ref2", required=True)
    ap.add_argument("--bodies", required=True)
    ap.add_argument("--state", required=True)
    ap.add_argument("--dwords", default="")
    ap.add_argument("--out", required=True)
    a = ap.parse_args()
    rr = verdict(pathlib.Path(a.ref2), pathlib.Path(a.ref), pathlib.Path(a.ref2))
    allowed = set(int(x, 16) for x in rr["allowed_blocks"])
    cen = {r["rva"].lower(): r for r in json.loads((pathlib.Path(a.census) / "prov.json").read_text())["rows"]}
    st = {}
    for f in a.state.split(","):
        p = pathlib.Path(f)
        if p.exists():
            for ln in p.read_text().splitlines():
                x = json.loads(ln)
                st[x["rva"]] = x
    dw = json.loads(pathlib.Path(a.dwords).read_text()) if a.dwords and pathlib.Path(a.dwords).exists() else {}
    rows = []
    for f in a.bodies.split(","):
        for b in json.loads(pathlib.Path(f).read_text())["bodies"]:
            r = b["rva"].lower()
            c = cen.get(r, {})
            w = b.get("writes")
            if w:
                bad_abs = [x for x in w["abs_in"] if (int(x.split("@")[0].split("[")[0], 16) & ~1023) in allowed]
                if not (w["abs_in"] or w["abs_out"] or w["indirect"]):
                    leg = "empty"
                elif w["indirect"] or w["abs_out"] or bad_abs or any("[" in x for x in w["abs_in"]):
                    leg = "not-covered"
                else:
                    leg = "covered"
                wsum = f"frame {w['frame']} abs_in {len(w['abs_in'])} abs_out {len(w['abs_out'])} indirect {len(w['indirect'])}"
            else:
                leg, wsum = "-", "-"
            s = st.get(r)
            calls = c.get("calls_total", 0)
            if b["status"] != "OK":
                v, det = "NOT-WIRED", b["status"] + (" " + ",".join(b.get("unresolved", [])) if b.get("unresolved") else "")
            elif not calls:
                v, det = "NOT-EXECUTED", "census: 0 calls from the warp to the end of the replay"
            elif not s:
                v, det = "UNTESTED", ""
            elif s["verdict"] == "IDENTICAL-FOR-N":
                if s.get("calls_total"):
                    v, det = f"IDENTICAL-FOR-{s['n']}", f"run {s['run']} (group of {s['group']})"
                else:
                    v, det = "NOT-EXECUTED", f"0 calls in run {s['run']} (census {calls})"
            elif s["verdict"] == "DIVERGED":
                fi = s["first"] or {}
                d = dw.get(r)
                det = f"tick {fi.get('tick')}: {fi.get('nblocks')} blocks, first {', '.join(x[0] for x in fi.get('blocks', [])[:4])}"
                if d:
                    det += "; " + "; ".join(f"{x[0]} body {x[1]} orig {x[2]}" for x in d["words"][:4]) + f" (tick {d['tick']})"
                v = f"DIVERGED@{fi.get('tick')}"
            else:
                v, det = s["verdict"], f"run {s['run']} rc {s.get('rc')}"
            inst = "-" if not s else ("ok" if s.get("jmp_ok") else f"FAIL {s.get('jmp_ok')}")
            rows.append([r, b.get("source", ""), f"{b.get('pct', 0):.1f}", b.get("equiv", ""), inst,
                         str(calls), str((s or {}).get("calls_total", "")), wsum, leg, v, det])
    hdr = ["rva", "source", "best_pct", "equiv_v4", "install", "calls_census", "calls_run", "stores", "write_set_leg",
           "verdict", "detail"]
    pathlib.Path(a.out).write_text("\t".join(hdr) + "\n" + "\n".join("\t".join(x) for x in rows) + "\n")
    from collections import Counter
    k = Counter(x[9].split("-FOR-")[0].split("@")[0] for x in rows)
    print(dict(k))
    print("identical + covered/empty:", sum(1 for x in rows if x[9].startswith("IDENTICAL") and x[8] in ("covered", "empty")))


if __name__ == "__main__":
    main()

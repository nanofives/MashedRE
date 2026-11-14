"""EQUIV-5441F0-TIMEOUT: every re/match/ADMITTED.tsv row (plus extra rows given as rva=source) through the HEAD
equiv.py (a frozen copy, --old) and the capped one (re/tools/match/equiv.py), default mode. Prints one
[N/M] line per row and writes a TSV: rva, source, recorded verdict, old line, new line, same (old line == new line),
recorded_ok (the recorded verdict word equals the new verdict word).

    py -3.12 verify/match_20261113/admitted_ab.py --old re/tools/match/equiv_head_tmp.py --out <tsv> [0x004c4a50=re/match/s19/004c4a50.c:EQUIV]
"""
import argparse
import pathlib
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
NEW = ROOT / "re" / "tools" / "match" / "equiv.py"


def run(tool, src, rva):
    try:
        r = subprocess.run([sys.executable, str(tool), str(ROOT / src), "--only", rva], capture_output=True, text=True,
                           timeout=600, cwd=str(ROOT))
    except subprocess.TimeoutExpired:
        return "TIMEOUT 600 s"
    lines = [ln for ln in r.stdout.splitlines() if ln.startswith("0x")]
    return lines[0] if lines else ("NO-OUTPUT " + (r.stdout + r.stderr).strip().splitlines()[-1][:200]
                                   if (r.stdout + r.stderr).strip() else "NO-OUTPUT")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--old", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("extra", nargs="*")
    a = ap.parse_args()
    rows = []
    for ln in (ROOT / "re" / "match" / "ADMITTED.tsv").read_text().splitlines():
        f = ln.split("\t")
        if f and f[0].startswith("0x"):
            rows.append((f[0].lower(), f[2], f[6]))
    for e in a.extra:
        rva, rest = e.split("=", 1)
        src, ver = rest.rsplit(":", 1)
        rows.append((rva.lower(), src, ver))
    out = ["rva\tsource\trecorded\told\tnew\tsame\trecorded_ok"]
    bad = 0
    for i, (rva, src, rec) in enumerate(rows, 1):
        o = run(ROOT / a.old, src, rva)
        n = run(NEW, src, rva)
        same = o == n
        nv = n.split("\t")[1] if "\t" in n else n
        rec_ok = nv.split("(")[0] == rec.split("(")[0]
        bad += not (same and rec_ok)
        print(f"[{i}/{len(rows)}] {rva} {'PASS' if same and rec_ok else 'FAIL'} recorded={rec} new={nv}", flush=True)
        out.append("\t".join([rva, src, rec, o.replace("\t", " "), n.replace("\t", " "), str(same), str(rec_ok)]))
    pathlib.Path(a.out).write_text("\n".join(out) + "\n")
    print(f"{len(rows) - bad}/{len(rows)} identical")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

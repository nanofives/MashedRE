"""MATCH-S03: re-score every re/match/s03/*.c with match.py --bytes and write results.tsv (best per RVA).

    py -3.12 verify/matchs03_20261031/collect.py

Columns: rva, pct, byte_identical, file, sha256 (of the source, CRLF->LF normalized), command.
"""
import hashlib, pathlib, re, subprocess, sys

W = pathlib.Path(__file__).resolve().parents[2]
OUT = W / "verify" / "matchs03_20261031" / "results.tsv"
FLAGS = "/O2 /GS /Ob1"
best = {}
for src in sorted((W / "re" / "match" / "s03").glob("*.c")):
    rel = src.relative_to(W).as_posix()
    cmd = ["py", "-3.12", "re/tools/match/match.py", rel, "--bytes"]
    r = subprocess.run(cmd, cwd=W, capture_output=True, text=True)
    cur = None
    for ln in r.stdout.splitlines():
        m = re.match(r"(0x[0-9a-f]{8}) \S+: ([\d.]+)%", ln)
        if m:
            cur = [m[1], float(m[2]), "n"]
            best.setdefault(m[1], [])
            best[m[1]].append((cur, rel))
        elif cur and "BYTE-IDENTICAL" in ln:
            cur[2] = "y"
    if r.returncode and not any(rel == x[1] for v in best.values() for x in v):
        print(f"{rel}: no score ({(r.stderr or r.stdout)[-200:]!r})", file=sys.stderr)
rows = []
for rva, lst in sorted(best.items()):
    (sc, rel) = max(lst, key=lambda t: (t[0][2] == "y", t[0][1]))
    sha = hashlib.sha256((W / rel).read_bytes().replace(b"\r\n", b"\n")).hexdigest()   # LF-normalized (autocrlf)
    rows.append(f"{rva}\t{sc[1]:.1f}\t{sc[2]}\t{rel}\t{sha}\tpy -3.12 re/tools/match/match.py {rel} --bytes")
OUT.write_text("rva\tpct\tbyte_identical\tfile\tsha256\tcommand\n" + "\n".join(rows) + "\n", encoding="utf-8")
n = sum(1 for r in rows if r.split("\t")[2] == "y")
print(f"{len(rows)} RVAs scored, {n} byte-identical -> {OUT.relative_to(W).as_posix()}  (flags {FLAGS}, ddk cl 13.00.9176 / c2 13.00.9178)")

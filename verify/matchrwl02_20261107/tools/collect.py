"""MATCH-RWL02 results: best score per row over re/match/rwl02/*.c, relocheck on 100% rows.
    py -3.12 collect.py   (from the worktree; writes results.tsv and relocheck.tsv next to it)"""
import csv, hashlib, pathlib, re, subprocess, sys
sys.path.insert(0, "re/tools/match")
import match
OUT = pathlib.Path("verify/matchrwl02_20261107")
D = pathlib.Path("D:/MashedWT/MATCH-RWL02_data")
rows = list(csv.DictReader(open(D / "slice_rows.tsv"), delimiter="\t"))
raw = {r["rva"]: r for r in csv.DictReader(open(D / "g2s" / "results.tsv"), delimiter="\t")}
best = {}
for src in sorted(pathlib.Path("re/match/rwl02").glob("*.c")):
    try: res = match.compare(src, "", quiet=True)
    except SystemExit as e:
        print(f"# {src.name}: {str(e).splitlines()[0][:100]}", file=sys.stderr); continue
    for r in res:
        k = f"0x{r['addr']:08x}"
        how = "insn"
        if r["acc"] < 1 and not r["bytes_bad"] and r["bytes_n"] >= (match.ghidra_size(r["addr"]) or 0):
            r = dict(r, acc=1.0, regs=1.0); how = "bytes"
        if k not in best or (r["acc"], r["regs"]) > (best[k][0], best[k][1]):
            best[k] = (r["acc"], r["regs"], how, src)
rl = []
with open(OUT / "results.tsv", "w", newline="") as f:
    w = csv.writer(f, delimiter="\t")
    w.writerow(["rva", "size", "best%", "regs%", "flags", "by", "sha256", "raw_g2s%", "relocheck"])
    for r in rows:
        k = r["rva"]; b = best.get(k)
        if not b: w.writerow([k, r["size"], "-", "-", "-", "-", "-", "", "-"]); continue
        src = b[3]; txt = src.read_text(encoding="latin-1")
        fl = re.match(r"// FLAGS: (.*)", txt).group(1).strip()
        rc = "-"
        if b[0] >= 1.0:
            p = subprocess.run([sys.executable, "re/tools/match/relocheck.py", str(src)], capture_output=True, text=True).stdout.strip().splitlines()
            rc = p[-1] if p else "?"
            rl.append(rc)
        w.writerow([k, r["size"], f"{100*b[0]:.1f}", f"{100*b[1]:.1f}", fl, b[2], hashlib.sha256(src.read_bytes()).hexdigest()[:16], raw.get(k, {}).get("acc", "") or "compile", "OK" if rc.split("\t")[1:2] == ["OK"] else rc])
open(OUT / "relocheck.tsv", "w").write("\n".join(rl) + "\n")

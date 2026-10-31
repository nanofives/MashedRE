"""Build the MATCH-S01 per-RVA table: every slice RVA, its best matched source, score, sha, swap state.
Writes verify/matchs01_20261031/results.tsv and prints counts. Run from the worktree root."""
import csv, glob, hashlib, os, pathlib, subprocess, sys
W = pathlib.Path("C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/MATCH-S01")
sys.path.insert(0, str(W / "re/tools/match")); sys.path.insert(0, str(W / "scripts"))
import match, rva_body_scan as rbs
SZ = {int(x["entry"], 16): int(x["body_size"]) for x in csv.DictReader(open(W / "re/console/pc_funcs.csv"))}
rvas = [int(l, 16) for l in open("D:/MashedWT/MATCH-S01_data/rvas.txt")]
best = {}
srcs = sorted(glob.glob(str(W / "re/match/s01/*.c"))) + sorted(glob.glob(str(W / "re/match/s01/raw/*.c")))
for f in srcs:
    try:
        rs = match.compare(pathlib.Path(f), "", quiet=True)
    except BaseException as e:
        print("ERR", f, str(e)[:100]); continue
    for r in rs:
        ok = not r["bytes_bad"] and r["bytes_n"] >= SZ.get(r["addr"], 1 << 30)
        key = (ok, r["acc"])
        if r["addr"] not in best or key > best[r["addr"]][0]:
            best[r["addr"]] = (key, f, r)
scan = rbs.scan_all(); exe = rbs.rva_to_tus(scan, "exe"); asi = rbs.rva_to_tus(scan, "asi")
ported = {}
for tu in set(scan["targets"]["exe"]) | set(scan["targets"]["asi"]):
    p = rbs.SRC / tu
    if p.exists():
        for ln in p.read_text(encoding="utf-8", errors="replace").splitlines():
            if '#include "../../../../re/match/s01/' in ln:
                ported.setdefault(ln.split("s01/")[1].rstrip('"'), []).append(tu)
out = [["rva", "size", "exact_pct", "verdict", "source", "source_sha256_12", "swapped_into", "exe_tu", "asi_tu"]]
n = {"BYTES-OK": 0, "partial": 0, "none": 0, "swapped": 0}
for a in rvas:
    if a in best:
        (ok, acc), f, r = best[a]
        rel = os.path.relpath(f, W).replace("\\", "/")
        sha = hashlib.sha256(open(f, "rb").read()).hexdigest()[:12]
        v = "BYTES-OK" if ok else f"partial(bad {len(r['bytes_bad'])}/{r['bytes_n']})"
        n["BYTES-OK" if ok else "partial"] += 1
        sw = ",".join(ported.get(os.path.basename(f), [])) if ok and "/raw/" not in rel else ""
        if sw: n["swapped"] += 1
        out.append([f"0x{a:08x}", SZ[a], f"{100 * acc:.1f}", v, rel, sha, sw, ",".join(exe.get(a, [])), ",".join(asi.get(a, []))])
    else:
        n["none"] += 1
        out.append([f"0x{a:08x}", SZ[a], "", "none", "", "", "", ",".join(exe.get(a, [])), ",".join(asi.get(a, []))])
os.makedirs(W / "verify/matchs01_20261031", exist_ok=True)
with open(W / "verify/matchs01_20261031/results.tsv", "w", newline="") as fh:
    csv.writer(fh, delimiter="\t").writerows(out)
print(n)

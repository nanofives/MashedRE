# REPLAY-FALLBACK-8 / PROV-INSITU-REF1-NOSRC (run 54): link each (rva, source) pair named in SOURCES.tsv with
# re/tools/prov/provlink.py link() and write one bodies JSON per step. The source is taken with `git show <ref>:<path>`
# into D:/MashedWT/REPLAY-FALLBACK-8_data/src/<ref>/<path> (nothing written into re/match), so a static co-resident
# (0x0041b7a0 in 0041c090.c, 0x004969a0 in 00496c10.c, ...) is linked from the file the object build compiles, not
# from the first file provlink's own lookup would pick.
#   py -3.12 verify/replayfb8_20261121/mkbodies.py
import csv, hashlib, json, pathlib, re, subprocess, sys
H = pathlib.Path(__file__).resolve().parent
WT = H.parents[1]
sys.path.insert(0, str(WT / "re" / "tools" / "prov"))
import provlink as P  # noqa: E402
M = P.M
D = pathlib.Path("D:/MashedWT/REPLAY-FALLBACK-8_data")
out = {}
sha = []
for r in csv.DictReader(open(H / "SOURCES.tsv"), delimiter="\t"):
    ref, path, rva = r["ref"], r["source"], r["rva"].lower()
    dst = D / "src" / ref / path
    dst.parent.mkdir(parents=True, exist_ok=True)
    data = subprocess.run(["git", "show", f"{ref}:{path}"], cwd=WT, capture_output=True, check=True).stdout
    dst.write_bytes(data)
    sha.append(f"{hashlib.sha256(data).hexdigest()}  {ref}:{path}")
    res = P.link(dst, int(rva, 16))
    mo = subprocess.run([sys.executable, str(WT / "re/tools/match/match.py"), str(dst), "--only", rva],
                        capture_output=True, text=True).stdout
    pm = re.search(r"([0-9.]+)%", mo)
    res.update({"rva": rva, "source": f"{ref}:{path}", "pct": float(pm.group(1)) if pm else 0.0, "equiv": "-",
                "orig_size": M.ghidra_size(int(rva, 16))})
    out.setdefault(r["step"], []).append(res)
    w = res.get("writes")
    print(f"{r['step']}\t{rva}\t{res['pct']:5.1f}\t{res['status']}\t{res.get('size', '')}\t"
          f"{','.join(res.get('unresolved', []))[:120]}\t"
          + (f"w={len(w['abs_in'])}/{len(w['abs_out'])}/{len(w['indirect'])}" if w else ""), flush=True)
for step, bodies in out.items():
    (D / f"bodies_{step}.json").write_text(json.dumps({"bodies": bodies}, indent=0))
(H / "SOURCES_SHA256.txt").write_text("\n".join(sha) + "\n")

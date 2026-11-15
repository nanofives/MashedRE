"""Map each RVA to the exe wrapper(s) and re/match source(s) the object build compiles for it (static, no build)."""
import csv, pathlib, sys, json
root = pathlib.Path(sys.argv[1]).resolve()
sys.path.insert(0, str(root / "re" / "tools" / "ddkobj"))
import ddkobj as D

rvas = [int(x, 16) for x in sys.argv[2].split(",")]
prov = D.load_provisional()
adm = set()
for l in open(root / "re/match/ADMITTED.tsv", encoding="utf-8"):
    if l.startswith("0x"):
        adm.add(int(l.split("\t")[0], 16))
B = D.load_bindings()
out = {}
for wp, rel, incs in D.exe_wrappers():
    for src, defs in incs:
        info = D.parse_source(src)
        for a, n in info["matches"]:
            if a in rvas:
                out.setdefault(f"0x{a:08x}", []).append({
                    "wrapper": rel, "src": str(src.relative_to(root)).replace("\\", "/"),
                    "name": n, "ported": D.CE.undecorate(n) in info["ported"],
                    "flags": info["flags"]})
res = []
for a in rvas:
    k = f"0x{a:08x}"
    b = B.get(a, {})
    res.append({"rva": k, "admitted": a in adm, "provisional": a in prov,
                "binding": {kk: b.get(kk) for kk in list(b)[:8]} if b else None,
                "units": out.get(k, [])})
print(json.dumps(res, indent=1))

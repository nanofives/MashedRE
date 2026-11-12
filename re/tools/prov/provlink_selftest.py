"""Self-test for provlink.py: link byte-matched (ledger PASS) sources as if the blob sat at the function's own RVA and
compare with MASHED.exe.unpatched. Every byte outside the blob-internal DIR32 sites (our own literal pool) must equal."""
import csv, pathlib, random, struct, sys
HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE)); sys.path.insert(0, str(HERE.parent / "match"))
import provlink as P, match as M
rows = [r for r in csv.DictReader(open(P.ROOT / "re/analysis/match_ledger.tsv"), delimiter="\t") if r["verdict"] == "PASS"]
random.seed(int(sys.argv[1]) if len(sys.argv) > 1 else 1)
ok = bad = skip = 0
for r in random.sample(rows, int(sys.argv[2]) if len(sys.argv) > 2 else 40):
    rva = int(r["rva"], 16)
    res = P.link(P.ROOT / r["source"], rva)
    if res["status"] != "OK" or res["entry"] != 0:
        skip += 1; print("skip", r["rva"], res["status"], res.get("unresolved"), res.get("entry")); continue
    code = bytearray(bytes.fromhex(res["code"]))
    for off, tgt in res["ext"].items():
        off = int(off); struct.pack_into("<i", code, off, tgt - (rva + off + 4))
    n = int(r["bytes_n"]); orig = M.original_bytes(rva, n)
    absset = set()
    for o in res["abs"]:
        absset |= {o, o + 1, o + 2, o + 3}
    d = [i for i in range(min(n, len(code))) if i not in absset and code[i] != orig[i]]
    if d: bad += 1; print("BAD", r["rva"], r["source"], len(d), d[:8])
    else: ok += 1
print(f"selftest ok={ok} bad={bad} skip={skip}")

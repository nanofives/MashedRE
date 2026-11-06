"""MATCH-S19 swap table: one row per RVA that a Match/MatchS19_*.cpp wrapper makes a port body.
    py -3.12 verify/matchs19_20261102/tools/swaps.py D:/MashedWT/MATCH-S19_data/bodies.tsv > verify/matchs19_20261102/swaps.tsv
bodies.tsv is the pre-swap scan (tools/bodies.py on the commit before the swaps): it names each RVA's old body."""
import csv, pathlib, re, subprocess, sys

BS = chr(92)
old = {}
for ln in open(sys.argv[1]):
    r, _m, b, i = ln.rstrip("\n").split("\t")
    old[int(r, 16)] = (b, i)
reloc = {}
for ln in open("verify/matchs19_20261102/relocheck.tsv"):
    f = ln.rstrip("\n").split("\t")
    if len(f) >= 5:
        reloc[(int(f[0], 16), f[4])] = f[1]
rsp = {t: open(f"mashedmod/{t}_sources.rsp").read().replace(BS, "/") for t in ("exe", "asi")}
x87 = open("mashedmod/x87_tus.txt").read().split()
print("rva\twrapper\ttargets\tx87\tport_symbol\tinstall\tsource\tsource_blob_sha\tmatch\trelocheck\told_body")
for w in sorted(pathlib.Path("mashedmod/src/mashed_re/Match").glob("MatchS19_*.cpp")):
    t = w.read_text(encoding="latin-1")
    src = re.search(r'#include "\.\./\.\./\.\./\.\./(re/match/s19/[0-9a-f]+\.c)"', t).group(1)
    s = pathlib.Path(src).read_text(encoding="latin-1")
    blob = subprocess.check_output(["git", "hash-object", src], text=True).strip()
    defs = dict(re.findall(r"^#define (FUN_[0-9a-f]{8}) (\w+)", t, re.M))
    inst = {int(a, 16): n for n, a in re.findall(r"^RH_ScopedInstall\((\w+),\s*0x([0-9a-fA-F]+)\)", t, re.M)}
    tg = "+".join(k for k in ("exe", "asi") if f"Match/{w.name}" in rsp[k])
    for kind, a in re.findall(r"^// (MATCH|HIDDEN): 0x([0-9a-fA-F]+)", s, re.M):
        a = int(a, 16)
        fn = f"FUN_{a:08x}"
        sym = defs.get(fn, fn)
        ob = old.get(a, ("(not a slice row)", ""))[0]
        print("\t".join([f"0x{a:08x}", w.name, tg, "yes" if w.stem in x87 else "no", sym,
                         inst.get(a, "-"), src, blob, "100" if kind == "MATCH" else "hidden(bytes)",
                         reloc.get((a, pathlib.Path(src).name), "-"), ob]))

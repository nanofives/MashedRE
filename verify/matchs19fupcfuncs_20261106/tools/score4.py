"""Score the MATCH-S19 sources of the given RVAs with verify/matchs19_20261102/tools/collect.py's rule, once against
re/console/pc_funcs.csv as committed on <base-ref> and once against the working copy. Run from the worktree:
    py -3.12 verify/matchs19fupcfuncs_20261106/tools/score4.py orch/integration 0x004c07b0 ...
"""
import pathlib
import subprocess
import sys
import tempfile

sys.path.insert(0, "re/tools/match")
import match  # noqa: E402

base, rvas = sys.argv[1], [int(v, 16) for v in sys.argv[2:]]
old = pathlib.Path(tempfile.mkdtemp()) / "pc_funcs_base.csv"
old.write_bytes(subprocess.run(["git", "show", f"{base}:re/console/pc_funcs.csv"], capture_output=True, check=True).stdout)
print("table\trva\tghidra_size\tacc%\tregs%\tbytes_n\tbytes_bad\tcollect_by\tcollect_best%")
for label, path in (("base", old), ("new", match.FUNCS)):
    match.FUNCS, match._FUNCS = path, None
    for a in rvas:
        n = match.ghidra_size(a)
        src = pathlib.Path(f"re/match/s19/{a:08x}.c")
        try:
            res = [r for r in match.compare(src, "", quiet=True) if r["addr"] == a]
        except SystemExit as e:  # collect.py logs and skips these
            print(f"{label}\t0x{a:08x}\t{n}\t-\t-\t-\t-\trefused\t- ({str(e).splitlines()[0][:80]})")
            continue
        r = res[0]
        how = "insn"
        if r["acc"] < 1 and not r["bytes_bad"] and r["bytes_n"] >= (n or 0):
            how = "bytes"
        best = 100.0 if how == "bytes" else 100 * r["acc"]
        print(f"{label}\t0x{a:08x}\t{n}\t{100*r['acc']:.1f}\t{100*r['regs']:.1f}\t{r['bytes_n']}\t"
              f"{len(r['bytes_bad']) if r['bytes_bad'] else 0}\t{how}\t{best:.1f}")

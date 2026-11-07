"""Flag sweep for below-100% s21 files: try FLAGS variants on a scratch copy, report best per RVA (no source edits)."""
import sys, pathlib, re, shutil
sys.path.insert(0, "re/tools/match")
import match
S = pathlib.Path("D:/MashedWT/MATCH-S21_data/scratch/flagsweep"); S.mkdir(parents=True, exist_ok=True)
import os
VAR = os.environ["VARS"].split("|") if os.environ.get("VARS") else ["/O2 /GS /Ob1", "/O2 /GS /Ob1 /G5", "/O2 /Ob1 /G5", "/O2 /Ob1 /G4", "/O2 /Ob1 /G3"]
for va8 in sys.argv[1:]:
    src = pathlib.Path(os.environ.get("SRCDIR","re/match/s21")) / f"{va8}.c"; t = src.read_text()
    L = t.split("\n"); cur = L[0]
    res = []
    for v in VAR:
        L[0] = f"// FLAGS: {v}"
        p = S / f"{va8}.c"; p.write_text("\n".join(L))
        try:
            r = [x for x in match.compare(p, "", quiet=True) if f"{x['addr']:08x}" == va8]
            res.append((r[0]["acc"], r[0]["regs"], v) if r else (0, 0, v))
        except SystemExit as e:
            res.append((0, 0, v + " ERR"))
    best = max(res)
    print(f"0x{va8}\tcur={cur[10:]}\t" + "\t".join(f"{a*100:.1f}:{v}" for a, _, v in res) + f"\tBEST {best[0]*100:.1f} {best[2]}", flush=True)

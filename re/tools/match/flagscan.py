"""Score every MATCH in the given sources under several flag sets (flag inference).
    py -3.12 re/tools/match/flagscan.py <src> [<src> ...] [--sets "/O2|/Ox|/O2 /G7"] [--toolchain ddk]"""
import argparse, pathlib, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match
SETS = ["/O2", "/Ox", "/O1", "/O2 /G5", "/O2 /G6", "/O2 /G7", "/O2 /GB", "/O2 /Oy-", "/O2 /Op", "/O2 /Ob1",
        "/O2 /Ob2", "/O2 /Oi-", "/Ogsy", "/Ogty", "/O2 /QIfist", "/O2 /GX", "/O2 /GS", "/O2 /Gr", "/O2 /Gz"]
ap = argparse.ArgumentParser(); ap.add_argument("srcs", nargs="+"); ap.add_argument("--sets", default="")
ap.add_argument("--toolchain", default=None); a = ap.parse_args()
sets = a.sets.split("|") if a.sets else SETS
rows = {}
for s in sets:
    for src in a.srcs:
        try:
            res = match.compare(pathlib.Path(src), s, quiet=True, toolchain=a.toolchain)
        except SystemExit as e:
            print(f"{s:14} {src}: FAIL {str(e)[:120]}"); continue
        for r in res:
            rows.setdefault(f"0x{r['addr']:08x} {r['sym']}", {})[s] = r["acc"]
print("function".ljust(40) + "".join(s[:9].rjust(10) for s in sets))
for k, v in rows.items():
    print(k[:40].ljust(40) + "".join((f"{100*v[s]:.1f}" if s in v else "-").rjust(10) for s in sets))

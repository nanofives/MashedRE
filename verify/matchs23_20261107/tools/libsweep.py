"""MATCH-S23: compile libpng 1.0.12 + zlib 1.1.3 (official sources) with a flag/define set and pair them with the
slice rows (libmatch.run). Prints identical counts; writes libsweep/<tag>.tsv.
    py -3.12 libsweep.py "<flags>" [-D...] --tag name     (run from the MATCH-S23 worktree)"""
import argparse, pathlib, sys
W = pathlib.Path.cwd()
sys.path.insert(0, str(W / "re/tools/match"))
import coffpe, libmatch, period_cc
S = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\tools\_download\pngzlib\src")
Z = S / "zlib-1.1.3"; P = S / "libpng-1.0.12"
ZS = "adler32 compress crc32 deflate gzio infblock infcodes inffast inflate inftrees infutil trees uncompr zutil".split()
PS = "png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil".split()
D = pathlib.Path("D:/MashedWT/MATCH-S23_data")
ap = argparse.ArgumentParser(); ap.add_argument("flags"); ap.add_argument("--tag", required=True)
ap.add_argument("--lib", default="both"); ap.add_argument("--extra", default="", help="extra flags for libpng only")
a = ap.parse_args()
slice_vas = [int(x, 16) for x in open(D / "slice_rvas.txt")]
names = {}
import csv
for r in csv.DictReader(open(W / "hooks.csv", encoding="utf-8")):
    try: names[int(r["rva"], 16)] = (r["name"], r["subsystem"], r["confidence"])
    except ValueError: pass
lo, hi = 0x00514000, 0x0052e000
rows = sorted((va, *names[va]) for va in names if lo <= va < hi)
img = coffpe.Image(); ext = libmatch.row_extents()
objs = []
for lib, srcs, incs, fl in (("zlib", [Z / f"{n}.c" for n in ZS], [Z], a.flags), ("libpng", [P / f"{n}.c" for n in PS], [P, Z], a.flags + " " + a.extra)):
    if a.lib not in ("both", lib): continue
    out = D / "libsweep" / f"{lib}_{a.tag}"
    errs = period_cc.compile_all(srcs, out, fl.strip(), incs)
    for e in errs[:5]: print("  ", str(e)[:200])
    objs += libmatch.load_objs([out])
res, extra, fns, used = libmatch.run(img, objs, rows, True, ext, pair=True)
ids = {va for va, r in res.items() if r["status"].startswith("identical")}
ins = [va for va in slice_vas if va in ids]
print(f"{a.tag}: rows {len(rows)} identical {len(ids)}; slice identical {len(ins)}/{len(slice_vas)}; extra {len(extra)}")
libmatch.write_tsv(D / "libsweep" / f"{a.tag}.tsv", rows, res, extra)

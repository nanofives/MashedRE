import pathlib, sys, tempfile
W = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\MATCH-LIB")
sys.path.insert(0, str(W / "re/tools/match"))
import coffpe, libmatch, period_cc
S = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\tools\_download\pngzlib\src")
Z = S / "zlib-1.1.3"; P = S / "libpng-1.0.12"
zs = [Z / f"{n}.c" for n in "adler32 compress crc32 deflate gzio infblock infcodes inffast inflate inftrees infutil trees uncompr zutil".split()]
ps = [P / f"{n}.c" for n in "png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil".split()]
img = coffpe.Image()
rows = libmatch.hooks_rows(["third-party-library[libpng"])
ext = libmatch.row_extents()
flags = sys.argv[1:] or ["/O2", "/O2 /G5", "/O2 /Ob1 /G5", "/O2 /GS /Ob1", "/Ox", "/O2 /G6", "/O1"]
out_root = pathlib.Path(sys.argv[0]).parent / "pngzlib_out"
for fl in flags:
    tag = fl.replace("/", "").replace(" ", "_")
    for lib, srcs, incs in (("zlib", zs, [Z]), ("libpng", ps, [P, Z])):
        out = out_root / f"{lib}_{tag}"
        errs = period_cc.compile_all(srcs, out, fl, incs)
        for e in errs: print("  ", e)
        objs = libmatch.load_objs([out])
        res, extra, fns, used = libmatch.run(img, objs, rows if lib == "libpng" else [], True, ext, pair=(lib == "libpng"))
        n_id = sum(1 for r in res.values() if r["status"].startswith("identical"))
        uva = sorted({e[0] for e in extra})
        print(f"{lib:6s} {fl:16s} fns {len(fns)} rows-identical {n_id}/{len(rows) if lib=='libpng' else 0} identical-elsewhere {len(extra)} entries / {len(uva)} addrs")
        libmatch.write_tsv(out_root / f"{lib}_{tag}.tsv", rows if lib == "libpng" else [], res, extra)

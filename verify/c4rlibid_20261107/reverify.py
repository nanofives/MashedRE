"""C4R-LIBID re-verification: rebuild each vendored library with the period compiler and re-run libmatch against
the CURRENT hooks.csv rows of that library (2026-11-07).

  MSYS_NO_PATHCONV=1 py -3.12 verify/c4rlibid_20261107/reverify.py <config> [<config> ...]

Configs: lua, lua_ob2, lua401, qhull, qhull_ob1, crt_libcmt, crt_libc, zlib, libpng, (and *_<variant> below).
Each writes <config>.tsv beside this file (libidentity_functions.tsv layout, plus a `library` column first).
"""
import pathlib, sys, tempfile
HERE = pathlib.Path(__file__).resolve().parent
W = HERE.parents[1]
sys.path.insert(0, str(W / "re/tools/match"))
import coffpe, libmatch, period_cc  # noqa: E402

MAIN = period_cc.MAIN
LUA40 = MAIN / "tools/_download/lua/src_4.0/lua"
LUA401 = MAIN / "tools/_download/lua/src_4.0.1/lua-4.0.1"
QH = W / "mashedmod/deps/qhull-2002.1"
QHS = QH / "src" if (QH / "src").exists() else QH
QSKIP = {"unix", "user_eg", "user_eg2", "qconvex", "qdelaun", "qhalf", "qvoronoi", "rbox"}
PZ = MAIN / "tools/_download/pngzlib/src"
Z, P = PZ / "zlib-1.1.3", PZ / "libpng-1.0.12"
LIBDIR = MAIN / "tools/vc7ddk/lib/wxp/i386"
LUAF = "/Og /Oi /Ot /Oy /Ob1 /GF /G5"
QHF = "/Og /Oi /Ot /Oy /Ob2 /GF /G5"


def lua_srcs(base):
    s = [p for p in sorted((base / "src").glob("*.c")) if p.name != "ltests.c"]
    return s + sorted((base / "src/lib").glob("*.c")), [base / "include", base / "src"]


ls40, li40 = lua_srcs(LUA40)
ls401, li401 = lua_srcs(LUA401)
qs = [p for p in sorted(QHS.glob("*.c")) if p.stem not in QSKIP]
zs = [Z / f"{n}.c" for n in "adler32 compress crc32 deflate gzio infblock infcodes inffast inflate inftrees infutil trees uncompr zutil".split()]
ps = [P / f"{n}.c" for n in "png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil".split()]

LUA_P = ["third-party-library[lua-4.0]"]
QH_P = ["third-party-library[qhull-2002.1"]
CRT_P = ["third-party-library[msvc-crt"]
# name: (library, srcs|None, incs, flags, objs-or-lib, row prefixes)
CONF = {
    "lua": ("lua-4.0", ls40, li40, LUAF, None, LUA_P),
    "lua401": ("lua-4.0.1-source", ls401, li401, LUAF, None, LUA_P),
    "lua_ob2": ("lua-4.0", ls40, li40, "/Og /Oi /Ot /Oy /Ob2 /GF /G5", None, LUA_P),
    "lua_o2g5": ("lua-4.0", ls40, li40, "/O2 /G5", None, LUA_P),
    "lua_o2ob1g5": ("lua-4.0", ls40, li40, "/O2 /Ob1 /G5", None, LUA_P),
    "lua_g6": ("lua-4.0", ls40, li40, "/Og /Oi /Ot /Oy /Ob1 /GF /G6", None, LUA_P),
    "lua_op": ("lua-4.0", ls40, li40, "/Og /Oi /Ot /Oy /Ob1 /GF /G5 /Op", None, LUA_P),
    "qhull": ("qhull-2002.1", qs, [QHS], QHF, None, QH_P),
    "qhull_o2g5": ("qhull-2002.1", qs, [QHS], "/O2 /G5", None, QH_P),
    "qhull_ob1": ("qhull-2002.1", qs, [QHS], "/O2 /Ob1 /G5", None, QH_P),
    "qhull_ox": ("qhull-2002.1", qs, [QHS], "/Ox /G5", None, QH_P),
    "qhull_op": ("qhull-2002.1", qs, [QHS], "/O2 /G5 /Op", None, QH_P),
    "crt_libcmt": ("msvc-crt", None, None, None, LIBDIR / "libcmt.lib", CRT_P),
    "crt_libc": ("msvc-crt", None, None, None, LIBDIR / "libc.lib", CRT_P),
    # objs may be a list of archives; DDK RTM 2600 libcmt is extracted to the session scratchpad (not committed)
    "crt_cpmt": ("msvc-crt", None, None, None, [LIBDIR / "libcmt.lib", LIBDIR / "libcpmt.lib"], CRT_P),
    "crt_rtm": ("msvc-crt", None, None, None, [pathlib.Path(__import__("os").environ.get("DDK_RTM_LIBCMT", "missing"))], CRT_P),
    "dshow_strmbase": ("directshow-baseclasses", None, None, None, LIBDIR / "strmbase.lib", ["third-party-library[directshow-baseclasses]"]),
    "dshow_strmbase_rtm": ("directshow-baseclasses", None, None, None, [pathlib.Path(__import__("os").environ.get("DDK_RTM_STRMBASE", "missing"))], ["third-party-library[directshow-baseclasses]"]),
    "zlib": ("zlib-1.1.3", zs, [Z], "/O2 /G5", None, ["third-party-library[zlib-1.1.3]"]),
    "libpng": ("libpng-1.0.12", ps, [P, Z], "/O2 /G5", None, ["third-party-library[libpng"]),
    "zlib_ox": ("zlib-1.1.3", zs, [Z], "/Ox /G3 /Gs", None, ["third-party-library[zlib-1.1.3]"]),
    "libpng_ox": ("libpng-1.0.12", ps, [P, Z], "/Ox /G3 /Gs", None, ["third-party-library[libpng"]),
}


def go(name, img, ext):
    lib, srcs, incs, flags, objsrc, pref = CONF[name]
    if objsrc is None:
        out = pathlib.Path(tempfile.mkdtemp(prefix=f"c4rlibid_{name}_"))
        for e in period_cc.compile_all(srcs, out, flags, incs):
            print("  ", e)
        objs = libmatch.load_objs([out])
    else:
        objs = libmatch.load_objs(objsrc if isinstance(objsrc, list) else [objsrc])
    rows = libmatch.hooks_rows(pref)
    res, extra, fns, used = libmatch.run(img, objs, rows, True, ext)
    n_id, n_amb, n_df, n_nc = libmatch.counts(res, rows)
    print(f"[{name}] {lib} flags={flags or (objsrc if isinstance(objsrc, list) else [objsrc])[-1].name} rows {len(rows)} identical {n_id} (ambiguous {n_amb}) "
          f"differs {n_df} not-compared {n_nc} obj-fns {len(fns)} identical-elsewhere {len(extra)}", flush=True)
    tmp = HERE / f"{name}.tsv.tmp"
    libmatch.write_tsv(tmp, rows, res, extra)
    lines = tmp.read_text(encoding="utf-8").splitlines()
    with open(HERE / f"{name}.tsv", "w", encoding="utf-8", newline="") as fo:
        fo.write("library\t" + lines[0] + "\n")
        for ln in lines[1:]:
            fo.write(f"{lib}\t{ln}\n")
    tmp.unlink()


if __name__ == "__main__":
    img = coffpe.Image()
    ext = libmatch.row_extents()
    for n in sys.argv[1:]:
        go(n, img, ext)

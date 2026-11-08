"""C4R-DSHOW-BASECLASSES: build the DirectX 9.0b (Summer 2003) SDK DirectShow BaseClasses source with the period
compiler (XP DDK cl 13.00.9176 / c2 13.00.9178) under a small flag grid, and libmatch the hooks.csv
third-party-library[directshow-baseclasses] rows against each build (2026-11-07).

  MSYS_NO_PATHCONV=1 py -3.12 verify/c4rdshowbase_20261107/dshowbase.py <config> [<config> ...] | all

Base config = the SDK's own baseclasses.dsp "Win32 Release" line, less the PCH/IDE switches:
  /Gz /MT /GX /O2, -D NDEBUG WIN32 _MBCS _LIB _WIN32_DCOM WINVER=0x400 (Release Unicode adds UNICODE _UNICODE).
Each config writes <config>.tsv beside this file (libidentity_functions.tsv layout with a `library` column first),
same as verify/c4rdxlib_20261107/dxlib.py. Sources and headers live in the main checkout's gitignored tools/
(L-94). Headers: the BaseClasses dir, stubinc/ (cderr.h, lzexpand.h, winperf.h are not in the DDK SDKINCS cabs),
the SDK Include dir, then the DDK inc/wxp and inc/crt.
"""
import pathlib, sys, tempfile
HERE = pathlib.Path(__file__).resolve().parent
W = HERE.parents[1]
sys.path.insert(0, str(W / "re/tools/match"))
import coffpe, libmatch, period_cc  # noqa: E402

M = period_cc.MAIN
BC = M / "tools/dxsdk9/sum2003/Samples/C++/DirectShow/BaseClasses"
INCS = [BC, HERE / "stubinc", M / "tools/dxsdk9/sum2003/Include", M / "tools/vc7ddk/inc/wxp", M / "tools/vc7ddk/inc/crt"]
REL = ["NDEBUG", "WIN32", "_MBCS", "_LIB", "_WIN32_DCOM", "WINVER=0x400"]
UNI = [d for d in REL if d != "_MBCS"] + ["UNICODE", "_UNICODE"]
DSHOW_P = ["third-party-library[directshow-baseclasses]"]
LIB = "directshow-baseclasses-sum2003"
# name: (flags, defines)
CONF = {
    "rel": ("/Gz /MT /GX /O2", REL),
    "rel_uni": ("/Gz /MT /GX /O2", UNI),
    "rel_o1": ("/Gz /MT /GX /O1", REL),
    "rel_ob1": ("/Gz /MT /GX /O2 /Ob1", REL),
    "rel_g5": ("/Gz /MT /GX /O2 /G5", REL),
    "rel_g6": ("/Gz /MT /GX /O2 /G6", REL),
    "rel_uni_ob1": ("/Gz /MT /GX /O2 /Ob1", UNI),
    "rel_nogz": ("/MT /GX /O2", REL),
}


def go(name, img, ext):
    flags, defs = CONF[name]
    out = pathlib.Path(tempfile.mkdtemp(prefix=f"dsb_{name}_"))
    errs = period_cc.compile_all(sorted(BC.glob("*.cpp")), out, flags, INCS, defs)
    for e in errs:
        print("  ", e[:300])
    objs = libmatch.load_objs([out])
    rows = libmatch.hooks_rows(DSHOW_P, W / "hooks.csv")
    res, extra, fns, used = libmatch.run(img, objs, rows, True, ext)
    n_id, n_amb, n_df, n_nc = libmatch.counts(res, rows)
    print(f"[{name}] {flags} -D {' '.join(defs)}: objs {len(objs)} rows {len(rows)} identical {n_id} "
          f"(ambiguous {n_amb}) differs {n_df} not-compared {n_nc} obj-fns {len(fns)} identical-elsewhere {len(extra)}",
          flush=True)
    tmp = HERE / f"{name}.tsv.tmp"
    libmatch.write_tsv(tmp, rows, res, extra)
    lines = tmp.read_text(encoding="utf-8").splitlines()
    with open(HERE / f"{name}.tsv", "w", encoding="utf-8", newline="") as fo:
        fo.write("library\t" + lines[0] + "\n")
        for ln in lines[1:]:
            fo.write(f"{LIB}\t{ln}\n")
    tmp.unlink()


if __name__ == "__main__":
    img = coffpe.Image()
    ext = libmatch.row_extents()
    names = list(CONF) if sys.argv[1:] == ["all"] else sys.argv[1:]
    for n in names:
        go(n, img, ext)

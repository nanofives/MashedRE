"""C4R-DXLIB: libmatch the hooks.csv d3dx9 rows against the DirectX 9.0 SDK static d3dx9.lib of two releases, and
the DirectShow BaseClasses rows against the XP DDK strmbase.lib (2026-11-07).

  MSYS_NO_PATHCONV=1 py -3.12 verify/c4rdxlib_20261107/dxlib.py <config> [<config> ...]

Configs: d3dx_sum2003 (9.0b, Summer 2003), d3dx_sum2004 (9.0c, Summer 2004), dshow_strmbase (XP DDK SP1 strmbase.lib).
Each writes <config>.tsv beside this file (libidentity_functions.tsv layout plus a `library` column first), same as
verify/c4rlibid_20261107/reverify.py. Inputs live in the main checkout's gitignored tools/dxsdk9 (L-94).
"""
import pathlib, sys
HERE = pathlib.Path(__file__).resolve().parent
W = HERE.parents[1]
sys.path.insert(0, str(W / "re/tools/match"))
import coffpe, libmatch, period_cc  # noqa: E402

DX = period_cc.MAIN / "tools/dxsdk9"
D3DX_P = ["third-party-library[d3dx9"]
DSHOW_P = ["third-party-library[directshow-baseclasses]"]
CONF = {
    "d3dx_sum2003": ("d3dx9-sum2003", [DX / "sum2003/Lib/d3dx9.lib"], D3DX_P),
    "d3dx_sum2004": ("d3dx9-sum2004", [DX / "sum2004/Lib/d3dx9.lib"], D3DX_P),
    "dshow_strmbase": ("directshow-baseclasses-ddk", [period_cc.MAIN / "tools/vc7ddk/lib/wxp/i386/strmbase.lib"], DSHOW_P),
}


def go(name, img, ext):
    lib, objsrc, pref = CONF[name]
    objs = libmatch.load_objs(objsrc)
    rows = libmatch.hooks_rows(pref)
    res, extra, fns, used = libmatch.run(img, objs, rows, True, ext)
    n_id, n_amb, n_df, n_nc = libmatch.counts(res, rows)
    print(f"[{name}] {lib} {objsrc[-1]} objs {len(objs)} rows {len(rows)} identical {n_id} (ambiguous {n_amb}) "
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

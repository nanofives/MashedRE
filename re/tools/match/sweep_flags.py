"""Flag sweep: compile a library with each candidate flag set and count identical hooks.csv rows.

  py -3.12 re/tools/match/sweep_flags.py lua|lua401|qhull [--flags "/O2" "/O2 /G6" ...] [--tsv-dir <dir>] [--search]
"""
import argparse
import pathlib
import sys
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import coffpe  # noqa: E402
import libmatch  # noqa: E402
import period_cc  # noqa: E402

MAIN = period_cc.MAIN
LUA40 = MAIN / "tools" / "_download" / "lua" / "src_4.0" / "lua"
LUA401 = MAIN / "tools" / "_download" / "lua" / "src_4.0.1" / "lua-4.0.1"
QHULL = coffpe.ROOT / "mashedmod" / "deps" / "qhull-2002.1"
QHULL_SKIP = {"unix", "user_eg", "user_eg2", "qconvex", "qdelaun", "qhalf", "qvoronoi", "rbox"}


def lib_def(key):
    if key in ("lua", "lua401"):
        base = LUA40 if key == "lua" else LUA401
        srcs = [p for p in sorted((base / "src").glob("*.c")) if p.name != "ltests.c"]
        srcs += sorted((base / "src" / "lib").glob("*.c"))
        return srcs, [base / "include", base / "src"], ["third-party-library[lua-4.0]"]
    if key == "qhull":
        src = QHULL / "src" if (QHULL / "src").exists() else QHULL
        srcs = [p for p in sorted(src.glob("*.c")) if p.stem not in QHULL_SKIP]
        return srcs, [src], ["third-party-library[qhull-2002.1"]
    raise SystemExit(key)


DEFAULT_FLAGS = ["/O2", "/O2 /G5", "/O2 /G6", "/Ox", "/Ox /G5", "/O1", "/Ogityb2 /G5",
                 "/Og /Oi /Ot /Oy /Ob1 /Gs /GF /G5", "/Og /Oi /Ot /Oy /Ob1 /Gs /GF /G6"]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("lib")
    ap.add_argument("--flags", nargs="+", default=DEFAULT_FLAGS)
    ap.add_argument("--tsv-dir")
    ap.add_argument("--search", action="store_true")
    ap.add_argument("--per-obj", action="store_true")
    ap.add_argument("--no-pair", action="store_true", help="identity only (fast flag sweep)")
    ap.add_argument("-D", action="append", default=[])
    a = ap.parse_args()
    srcs, incs, subs = lib_def(a.lib)
    img = coffpe.Image()
    rows = libmatch.hooks_rows(subs)
    ext = libmatch.row_extents()
    for fl in a.flags:
        out = pathlib.Path(tempfile.mkdtemp(prefix=f"lm_{a.lib}_"))
        errs = period_cc.compile_all(srcs, out, fl, incs, a.D)
        for e in errs:
            print("  ", e)
        objs = libmatch.load_objs([out])
        res, extra, fns, used = libmatch.run(img, objs, rows, do_search=a.search, extents=ext, pair=not a.no_pair)
        n_id, n_amb, n_df, n_nc = libmatch.counts(res, rows)
        cid = {o.comp_id for o in objs}
        print(f"{a.lib}\t{fl}\trows {len(rows)}\tidentical {n_id} (ambiguous {n_amb})\tdiffers {n_df}\t"
              f"not-compared {n_nc}\tobj-fns {len(fns)}\tunlisted-identical {len(extra)}\tcomp.id {sorted(cid)}",
              flush=True)
        if a.per_obj:
            per = {}
            for r in res.values():
                if r["status"].startswith("identical"):
                    per[r["obj"]] = per.get(r["obj"], 0) + 1
            print("   per-obj identical:", " ".join(f"{k.replace('.obj', '')}={v}" for k, v in sorted(per.items())))
        if a.tsv_dir:
            p = pathlib.Path(a.tsv_dir)
            p.mkdir(parents=True, exist_ok=True)
            tag = fl.replace("/", "").replace(" ", "_")
            libmatch.write_tsv(p / f"{a.lib}_{tag}.tsv", rows, res, extra)


if __name__ == "__main__":
    main()

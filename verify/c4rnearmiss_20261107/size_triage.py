"""For each row (rva<TAB>src), score under the file's FLAGS at pc_funcs body_size and at body_end - entry + 1
(contiguous size), report both plus the whole-symbol byte compare (C4R-NEARMISS, 2026-11-07)."""
import pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools" / "match"))
import match  # noqa: E402

rows = {}
for ln in (ROOT / "re/console/pc_funcs.csv").read_text().splitlines()[1:]:
    e, be, sz, _ = ln.split(",")[:4]
    rows[int(e, 16)] = (int(be, 16) - int(e, 16) + 1, int(sz))
for ln in pathlib.Path(sys.argv[1]).read_text().splitlines():
    rva, src = ln.split("\t")[:2]
    a = int(rva, 16)
    contig, gsz = rows.get(a, (None, None))
    p = ROOT / src
    t = p.read_text()
    sym = dict(match.parse_annotations(t)).get(a)
    if sym is None:
        print(f"{rva}\t{src}\tNOANN"); continue
    fl = match.file_flags(t) or match.DEFAULT_FLAGS
    try:
        secs, syms = match.coff(match.compile_obj(p, fl, match.file_lang(t), match.file_toolchain(t)))
        g = match.score_function(a, secs, syms, sym, gsz)[0]
        c = match.score_function(a, secs, syms, sym, contig)[0] if contig != gsz else g
        n, bad = match.byte_compare(a, secs, syms, sym)
    except SystemExit as ex:
        print(f"{rva}\t{src}\tERR {ex}"); continue
    print(f"{rva}\t{src}\t{gsz}\t{contig}\t{100*g:.1f}\t{100*c:.1f}\t{n}\t{len(bad)}\t{fl}", flush=True)

"""C4R-AC80-EXTENT negative checks: the extent-override gate must refuse 0x0040ac80 when (1) the member sizes do not
sum to pc_funcs body_size, (2) a HIDDEN member is not byte-identical (one constant changed in a copy of the source),
(3) a member is missing from the source, (4) the override names a different source. Control: the stock row passes.

    py -3.12 verify/c4rac80extent_20261107/negative_gate.py
"""
import copy, pathlib, sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "re" / "tools"))
import c4_reclass as g  # noqa: E402

RVA = "0040ac80"
SRC = "re/match/s02/0040ac80.c"
BASE = g.load_extents()[RVA]


def run(name, ext, src_rel, expect_pass):
    g.load_extents = lambda: {RVA: ext}
    ev = g.eval_source(str(ROOT / src_rel))
    ok, detail, _ = g.gate_match(RVA, src_rel, ev)
    good = bool(ok) == expect_pass
    print(f"[{name}] {'PASS' if ok else 'REFUSED'} ({'as expected' if good else 'UNEXPECTED'}): {detail}")
    return good


def main():
    res = [run("control", BASE, SRC, True)]
    e = copy.deepcopy(BASE)
    e["members"][1] = (e["members"][1][0], e["members"][1][1], 30)
    res.append(run("sum-mismatch", e, SRC, False))
    tmp = HERE / "_neg_0040ac80.c"
    try:
        text = (ROOT / SRC).read_text()
        tmp.write_text(text.replace("DAT_008a9588 == 0xb", "DAT_008a9588 == 0xc", 1))
        e = copy.deepcopy(BASE)
        e["source"] = tmp.relative_to(ROOT).as_posix()
        res.append(run("hidden-member-byte", e, e["source"], False))
        tmp.write_text(text.replace("// HIDDEN: 0x0040abe0 _FUN_0040abe0 31", "// (marker removed)", 1))
        res.append(run("hidden-marker-missing", e, e["source"], False))
    finally:
        tmp.unlink(missing_ok=True)
    e = copy.deepcopy(BASE)
    e["source"] = "re/match/s02/0040ab40.c"
    res.append(run("wrong-source", e, SRC, False))
    print(f"{sum(res)}/{len(res)} as expected")
    return 0 if all(res) else 1


if __name__ == "__main__":
    sys.exit(main())

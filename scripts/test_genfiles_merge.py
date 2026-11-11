"""Self-test for scripts/genfiles_merge.py (HB-GENFILES-NOCONFLICT). No MASHED launch.

Real pair: base orch/integration 5fc6a43e4, ours ATOMIC-VIEW-2 0fd2195bb, theirs HUDINIT-A d4ee8a938. Plain git
line merge conflicts on BINDINGS.tsv; every one of the three files must merge cleanly and equal an independent
serial oracle:
  BINDINGS.tsv        ours' per-row changes (vs base) applied to base, then theirs', new rows appended in that order
  SEED_CANDIDATES     the address-sorted union of both sides' row changes
  PortStateSeed.inc   re/tools/gen_portstate_seed.py's render of the merged inventory with bytes from the binary
Plus: a one-sided merge returns ours byte for byte, and a real disagreement conflicts (exit 1, markers).
"""
import importlib.util, os, pathlib, subprocess, sys

import pytest

HERE = pathlib.Path(__file__).resolve().parent
REPO = HERE.parent
sys.path.insert(0, str(HERE))
import genfiles_merge as gm  # noqa: E402

BASE, OURS, THEIRS = "5fc6a43e4", "0fd2195bb", "d4ee8a938"
F = {"bindings": "verify/porttop_20261108/BINDINGS.tsv",
     "seedtsv": "verify/portstate_20261108/SEED_CANDIDATES.tsv",
     "seedinc": "mashedmod/src/mashed_re/Compat/PortStateSeed.inc"}


def show(rev, path):
    r = subprocess.run(["git", "-C", str(REPO), "show", "%s:%s" % (rev, path)], capture_output=True)
    if r.returncode:
        pytest.skip("commit %s not in this clone" % rev)
    return r.stdout.decode("utf-8")


def rows(text):
    ls = text.replace("\r\n", "\n").rstrip("\n").split("\n")
    return ls[0], [l for l in ls[1:] if l.strip()]


# ------------------------------------------------------------------------------------------- the real conflicting pair
def test_plain_line_merge_conflicts_on_the_real_pair(tmp_path):
    p = {}
    for tag, rev in (("b", BASE), ("o", OURS), ("t", THEIRS)):
        p[tag] = tmp_path / tag
        p[tag].write_text(show(rev, F["bindings"]), encoding="utf-8")
    r = subprocess.run(["git", "merge-file", "-p", str(p["o"]), str(p["b"]), str(p["t"])], capture_output=True)
    assert r.returncode > 0


def test_bindings_real_pair_equals_rowwise_serial_oracle():
    b, o, t = (show(r, F["bindings"]) for r in (BASE, OURS, THEIRS))
    merged, conflicts = gm.merge_bindings(b, o, t)
    assert conflicts == 0
    # oracle: apply each side's row changes to base in merge order (ours, then theirs); new rows appended
    hb, rb = rows(b)
    key = lambda r: r.split("\t", 1)[0].lower()
    base = {key(r): r for r in rb}
    order = [key(r) for r in rb]
    cur = dict(base)
    for side in (o, t):
        _, rs = rows(side)
        for r in rs:
            k = key(r)
            if base.get(k) == r:
                continue                         # unchanged by this side
            if k in base and cur[k] != base[k] and k in base:
                # both sides changed this row: the merge may concatenate note suffixes; check it separately
                bf, cf, nf = base[k].split("\t"), cur[k].split("\t"), r.split("\t")
                cur[k] = "\t".join(c if n == bb else (n if c == bb else bb + c[len(bb):] + n[len(bb):])
                                   for bb, c, n in zip(bf, cf, nf))
                continue
            if k not in cur:
                order.append(k)
            cur[k] = r
    want = "\n".join([hb] + [cur[k] for k in order]) + "\n"
    assert merged.replace("\r\n", "\n") == want


def test_seed_tsv_real_pair_equals_sorted_union():
    b, o, t = (show(r, F["seedtsv"]) for r in (BASE, OURS, THEIRS))
    merged, conflicts = gm.merge_seed_tsv(b, o, t)
    assert conflicts == 0
    h, rb = rows(b)
    _, ro = rows(o)
    _, rt = rows(t)
    union = {}
    for r in rb + ro + rt:
        union[int(r.split("\t")[0], 16)] = r
    assert set(rows(merged)[1]) == set(union.values())
    assert [int(r.split("\t")[0], 16) for r in rows(merged)[1]] == sorted(union)


def test_seed_inc_real_pair_equals_generator_on_merged_inventory(tmp_path):
    g = gm._gen()
    binp = g.find_unpatched()
    if not binp or not os.path.isfile(binp):
        pytest.skip("no original/MASHED.exe.unpatched")
    b, o, t = (show(r, F["seedtsv"]) for r in (BASE, OURS, THEIRS))
    inv_text, _ = gm.merge_seed_tsv(b, o, t)
    inv = tmp_path / "SEED_CANDIDATES.tsv"
    inv.write_text(inv_text, encoding="utf-8")
    raw = open(binp, "rb").read()
    entries, errs = g.resolve(raw, g.pe_sections(raw), g.read_inventory(str(inv)))
    assert not errs
    want, _ = g.render(entries, os.path.join(g.REPO, F["seedtsv"]), None, g.sha256(binp))
    bi, oi, ti = (show(r, F["seedinc"]) for r in (BASE, OURS, THEIRS))
    merged, conflicts = gm.merge_seed_inc(bi, oi, ti)
    assert conflicts == 0
    assert merged.replace("\r\n", "\n").rstrip("\n") == want.rstrip("\n")


# ------------------------------------------------------------------------------------------- no-op and disagreement
@pytest.mark.parametrize("kind", ["bindings", "seedtsv", "seedinc"])
def test_one_sided_merge_returns_ours_unchanged(kind):
    b, o = show(BASE, F[kind]), show(OURS, F[kind])
    merged, conflicts = gm.MERGERS[kind](b, o, b)
    assert conflicts == 0
    if kind == "seedinc":   # rendered again: the only change allowed is the dropped inventory sha256
        assert merged.replace("\r\n", "\n").split("\n")[4:] == o.replace("\r\n", "\n").split("\n")[4:]
    else:
        assert merged == o


SEED_HDR = "addr\tglob\tsection\tkind\tsize\texe\tn_rows\trows\n"


def test_seed_tsv_same_address_different_value_conflicts():
    base = SEED_HDR + "0x005f9800\tA\t.data\tscalar/array\t4\torigtop\t1\t0x00400000\n"
    ours = base + "0x005f9808\tDAT_005f9808\t.data\tscalar/array\t12\torigtop\t1\t0x004548e0\n"
    theirs = base + "0x005f9808\tDAT_005f9808\t.data\tscalar/array\t16\torigtop\t1\t0x00555360\n"
    merged, conflicts = gm.merge_seed_tsv(base, ours, theirs)
    assert conflicts == 1 and "<<<<<<< ours" in merged and ">>>>>>> theirs" in merged


def test_seed_inc_same_address_different_bytes_conflicts():
    g = gm._gen()
    def inc(ents):
        entries = [(a, d, dict(glob=n, section=".data", rows="")) for a, d, n in ents]
        return g.render(entries, os.path.join(g.REPO, F["seedtsv"]), None, "0" * 64)[0] + "\n"
    base = inc([(0x5f9800, b"\1\2\3\4", "A")])
    ours = inc([(0x5f9800, b"\1\2\3\4", "A"), (0x5f9808, b"\5\5\5\5", "B")])
    theirs = inc([(0x5f9800, b"\1\2\3\4", "A"), (0x5f9808, b"\6\6\6\6", "B")])
    merged, conflicts = gm.merge_seed_inc(base, ours, theirs)
    assert conflicts == 1 and "0x005f9808" in merged.splitlines()[-1]
    clean, c2 = gm.merge_seed_inc(base, ours, inc([(0x5f9800, b"\1\2\3\4", "A"), (0x5f9810, b"\7", "C")]))
    assert c2 == 0 and "0x005f9808" in clean and "0x005f9810" in clean


B_HDR = "rva\tname\tkind\texe_symbol\tnote\tlevel\tcaller\n"


def test_bindings_same_rva_different_kind_conflicts_and_note_suffixes_combine():
    base = B_HDR + "0x00401000\tA\tstand-in\tS\tn0\t1\t0x00400000\n"
    o = B_HDR + "0x00401000\tA\tmatched\tOrig\tn0\t1\t0x00400000\n"
    t = B_HDR + "0x00401000\tA\tbound\tB\tn0\t1\t0x00400000\n"
    merged, conflicts = gm.merge_bindings(base, o, t)
    assert conflicts == 1 and "<<<<<<< ours" in merged
    o2 = B_HDR + "0x00401000\tA\tstand-in\tS\tn0 X:ours\t1\t0x00400000\n"
    t2 = B_HDR + "0x00401000\tA\tstand-in\tS\tn0 Y:theirs\t1\t0x00400000\n"
    merged2, c2 = gm.merge_bindings(base, o2, t2)
    assert c2 == 0 and "n0 X:ours Y:theirs" in merged2


def test_bindings_new_rows_order_ours_then_theirs():
    base = B_HDR + "0x1\ta\tk\ts\tn\t1\t0x0\n"
    o = base + "0x3\tc\tk\ts\tn\t1\t0x0\n"
    t = base + "0x2\tb\tk\ts\tn\t1\t0x0\n"
    merged, conflicts = gm.merge_bindings(base, o, t)
    assert conflicts == 0
    assert [r.split("\t")[0] for r in rows(merged)[1]] == ["0x1", "0x3", "0x2"]


def test_driver_writes_result_and_exit_codes(tmp_path):
    base = SEED_HDR + "0x005f9800\tA\t.data\tscalar/array\t4\torigtop\t1\t0x00400000\n"
    o = base + "0x005f9808\tB\t.data\tscalar/array\t4\torigtop\t1\t-\n"
    t = base + "0x005f9810\tC\t.data\tscalar/array\t4\torigtop\t1\t-\n"
    for n, s in (("O", base), ("A", o), ("B", t)):
        (tmp_path / n).write_text(s, encoding="utf-8")
    assert gm.driver("seedtsv", str(tmp_path / "O"), str(tmp_path / "A"), str(tmp_path / "B")) == 0
    assert "0x005f9810" in (tmp_path / "A").read_text(encoding="utf-8")


def test_bindings_rows_helper_is_idempotent(tmp_path):
    sys.path.insert(0, str(REPO / "verify" / "portorigtop_20261109"))
    import bindings_rows
    p = tmp_path / "B.tsv"
    p.write_text(B_HDR + "0x00401000\tA\tstand-in\tS\tn0\t1\t0x00400000\n", encoding="utf-8")
    fixed = {0x00402000: ("B", "matched", "Orig_B", "ROW: new", 1, 0x00401000)}
    append = {0x00401000: " ROW: note"}
    bindings_rows.apply("ROW: ", fixed, append, tsv=str(p), quiet=True)
    once = p.read_text(encoding="utf-8")
    bindings_rows.apply("ROW: ", fixed, append, tsv=str(p), quiet=True)
    assert p.read_text(encoding="utf-8") == once
    assert "n0 ROW: note" in once and once.rstrip("\n").split("\n")[-1].startswith("0x00402000\tB\tmatched")

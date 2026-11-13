# Per-tick compare of two detorig.py runs made with --clock tick or --tick-hash (REPLAY-ORACLE-DESIGN, 2026-11-11).
#
# Each run's ticks.bin holds, for every tick from the race anchor on, [tick counter, N, one FNV-1a hash per
# block of the writable image 0x005ea000..0x00915008] (heap-range dwords hashed as 0, detorig.is_ptr). Record i
# of run A is compared with record i of run B (the i-th frame-timer entry after the race anchor in each run).
#
#   CONSTANT  a block that differs at every compared tick (a value written before the window and never
#             rewritten, or a counter that runs alike from a different start: see the checkpoint classes of
#             detorig_report.py for which)
#   NEW       a block that is equal at some tick and differs at a later one: a divergence inside the window
#
# Verdict: IDENTICAL-FOR-N = the number of leading ticks with no NEW block (and equal N - N_anchor).
#
#   py -3.12 re/frida/detorig_ticks.py DIR_A DIR_B [--json OUT]
import argparse, json, struct, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from detorig import DATA_LO, is_ptr_pair, region_of


def load(d):
    d = Path(d)
    m = json.loads((d / "meta.json").read_text())
    t = m.get("ticks")
    if not t:
        raise SystemExit(f"{d}: no ticks in meta.json")
    raw = (d / t["file"]).read_bytes()
    recw = t["recw"]
    n = len(raw) // (recw * 4)
    recs = [struct.unpack_from(f"<{recw}I", raw, i * recw * 4) for i in range(n)]
    return m, t, recs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a"); ap.add_argument("b")
    ap.add_argument("--json", default="")
    ap.add_argument("--ignore", default="", help="LO-HI hex VA ranges whose NEW blocks do not end IDENTICAL-FOR-N (named in the report)")
    ap.add_argument("--n-ok", action="store_true", help="do not end IDENTICAL-FOR-N at an N misalignment (per-tick clock-call count; --clock tick runs)")
    ap.add_argument("--masks", default="", help="mask file(s), comma-separated (re/frida/replay_masks.tsv): a NEW block whose 1 KB range is entirely MASK dwords is ignored; --explain also scores with them")
    ap.add_argument("--explain", action="store_true", help="classify the NEW blocks' dwords from the 15 checkpoint dumps")
    a = ap.parse_args()
    ma, ta, ra = load(a.a)
    mb, tb, rb = load(a.b)
    assert ta["recw"] == tb["recw"] and ta["block"] == tb["block"], "different block layout"
    blk = ta["block"]
    na_anchor, nb_anchor = ma["race"]["n"], mb["race"]["n"]
    n = min(len(ra), len(rb))
    nblk = ta["nblk"]
    raw_va = []   # VA of each raw word appended after the block hashes (--tick-raw)
    for lo, hi in ta.get("raw") or []:
        raw_va += list(range(lo, hi, 4))
    raw_first = {}
    diffs = []
    nmis = None
    for i in range(n):
        x, y = ra[i], rb[i]
        if nmis is None and (x[1] - na_anchor) != (y[1] - nb_anchor):
            nmis = i
        diffs.append({j for j in range(2, 2 + nblk) if x[j] != y[j]})
        for j in range(2 + nblk, len(x)):
            if x[j] != y[j] and not is_ptr_pair(x[j], y[j]) and j not in raw_first:
                raw_first[j] = (i, x[j], y[j])
    always = set.intersection(*diffs) if diffs else set()
    seen_equal = set()
    first_new = {}
    for i, s in enumerate(diffs):
        for j in s:
            if j in seen_equal and j not in first_new:
                first_new[j] = i
        seen_equal |= {j for j in range(2, 2 + nblk) if j not in s}
    # a block that is different from tick 0 but becomes equal and differs again is NEW at that later tick (above)
    ign = [(int(x, 16), int(y, 16)) for x, y in (r.split("-") for r in a.ignore.split(",") if r)]
    va0 = lambda j: DATA_LO + (j - 2) * blk
    ignored = {j for j in first_new if any(lo < va0(j) + blk and va0(j) < hi for lo, hi in ign)}
    masked_blocks = []
    if a.masks:
        from detorig import load_norm
        mk = {v for v, c in load_norm(a.masks)["mask"].items() if c.startswith("MASK:")}
        full = {j for j in first_new if all(va0(j) + 4 * w in mk for w in range(blk // 4))}
        masked_blocks = [[hex(va0(j)), first_new[j]] for j in sorted(full - ignored)]
        ignored |= full
    ident = n
    counted = [t for j, t in first_new.items() if j not in ignored]
    if counted:
        ident = min(counted)
    if nmis is not None and not a.n_ok:
        ident = min(ident, nmis)
    va = lambda j: DATA_LO + (j - 2) * blk
    rep = {
        "a": str(a.a), "b": str(a.b), "ticks_compared": n, "block": blk,
        "dn_warp": [ma["race"].get("dn_warp"), mb["race"].get("dn_warp")],
        "identical_for_ticks": ident, "n_misalign_at": nmis, "ignored": a.ignore,
        "masked_blocks": masked_blocks,
        "ignored_new_blocks": [[hex(va0(j)), first_new[j]] for j in sorted(ignored)],
        "constant_blocks": [[hex(va(j)), region_of(va(j))] for j in sorted(always)],
        "new_blocks": [[hex(va(j)), first_new[j], region_of(va(j))] for j in sorted(first_new, key=lambda j: (first_new[j], j))],
        "diff_count_per_tick": [len(s) for s in diffs],
        "ticks_with_only_constant_blocks": sum(1 for s in diffs if s <= always),
        "raw_first_diff": [[hex(raw_va[j - 2 - nblk]), t, f"{x:08x}", f"{y:08x}"] for j, (t, x, y) in sorted(raw_first.items(), key=lambda kv: kv[1][0])],
    }
    print(f"{a.a} vs {a.b}: {n} ticks, block {blk} B, load calls {rep['dn_warp']}")
    print(f"  constant blocks: {len(always)}  NEW blocks: {len(first_new)}  N misaligned at: {nmis}")
    print(f"  IDENTICAL-FOR-N = {ident} of {n}" + ("" if ident < n else "  (every tick: no NEW block)")
          + (f"  ignoring {a.ignore}: {rep['ignored_new_blocks']}" if a.ignore else ""))
    for x in rep["new_blocks"][:20]:
        print(f"    NEW {x[0]} from tick {x[1]}  ({x[2]})")
    if raw_va:
        print(f"  raw words: {len(raw_va)}, differing (non-PTR) at some tick: {len(raw_first)}")
        for x in rep["raw_first_diff"][:30]:
            print(f"    RAW {x[0]} first differs at tick {x[1]}: {x[2]} vs {x[3]}")
    if a.explain:
        # Name each NEW block's dwords with the checkpoint scorer's classes (detorig_report.report over the 15 full
        # dumps): a block whose differing dwords are all in explained classes is EXPLAINED, else it lists them.
        from detorig_report import report
        cls = report(a.a, a.b, 0, a.masks or None)["_cls"]
        expl = []
        for j in sorted(first_new, key=lambda j: (first_new[j], j)):
            lo = va(j)
            ds = {v: c for v, c in cls.items() if lo <= v < lo + blk}
            bad = {hex(v): c for v, c in ds.items() if c == "UNEXPLAINED"}
            seen = sorted({c.split("/")[0] for c in ds.values()})
            expl.append({"block": hex(lo), "tick": first_new[j], "classes": seen, "unexplained": bad,
                         "dwords": {hex(v): c for v, c in sorted(ds.items())[:12]}})
            print(f"    {hex(lo)} tick {first_new[j]}: classes {seen}" + (f"  UNEXPLAINED {bad}" if bad else "")
                  + ("" if ds else "  (no dword differs at any checkpoint)"))
        rep["new_block_explain"] = expl
    if a.json:
        Path(a.json).write_text(json.dumps(rep, indent=1))


if __name__ == "__main__":
    main()

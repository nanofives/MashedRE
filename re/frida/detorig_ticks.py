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
#
# --masks (RO-CTL-OSWORD, 2026-11-16): a block is dropped per dword, by three routes, each named in the report:
#   full  the block's whole 1 KB is MASK dwords (the only route before RO-CTL-OSWORD, kept by --block-masks-only)
#   raw   the block lies inside the --tick-raw ranges, so every tick record holds its dwords: at each tick where its
#         hash differs, the dwords that differ (heap rule of the hash: a heap-range value counts as 0) are checked, and
#         a tick whose differing dwords are all MASK drops the block at that tick only. A tick where the hash differs
#         but no raw dword does keeps the block (raw / hash disagreement, listed under raw_inconsistent).
#   dump  the block is not raw-covered: the full-image checkpoint dumps both runs carry at the same tick (dump at tick u
#         = after record u - 1, re-hashed against that record first) are read. The block is dropped when at every such
#         checkpoint where its hash differs only MASK dwords differ, and at least one of those checkpoints is at or after
#         its NEW tick. Weaker than raw: a dword that differs only between two checkpoints is not seen.
import argparse, json, struct, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from detorig import DATA_LO, is_ptr, is_ptr_pair, region_of


def hz(v):
    """The tick hash's heap rule: a heap-range dword is hashed as 0 (detorig.js CModule, detorig_firstdiv.block_hashes)."""
    return 0 if is_ptr(v) else v


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


def dump_route(da, db, diffs, first_new, ignored, mk, va0, blk, n):
    """Route dump of --masks (header). Returns ({block: [[tick, [dword VAs]], ...]} dropped,
    {block: {tick: [unmasked dword VAs] or 'no dword differs'}} kept, [checkpoint ticks used])."""
    from detorig_firstdiv import load_run, read_dump, verify_dump
    A, B = load_run(da), load_run(db)
    use = []
    for u in sorted(set(A["dumps"]) & set(B["dumps"])):
        if u < 1 or u - 1 >= n:
            continue
        if verify_dump(A, u)[0] == "OK" and verify_dump(B, u)[0] == "OK":
            use.append(u)
    words = {u: (read_dump(A["dumps"][u]), read_dump(B["dumps"][u])) for u in use}
    dropped, kept = {}, {}
    for j in first_new:
        if j in ignored:
            continue
        w0 = (va0(j) - DATA_LO) // 4
        ev, bad = [], {}
        for u in use:
            if j not in diffs[u - 1]:
                continue
            wa, wb = words[u]
            dd = [DATA_LO + 4 * (w0 + w) for w in range(blk // 4)
                  if w0 + w < len(wa) and hz(int(wa[w0 + w])) != hz(int(wb[w0 + w]))]
            um = [hex(v) for v in dd if v not in mk]
            if not dd:
                bad[u] = "no dword differs"
            elif um:
                bad[u] = um[:8]
            else:
                ev.append([u, [hex(v) for v in dd]])
        if bad:
            kept[j] = bad
        elif any(u - 1 >= first_new[j] for u, _ in ev):
            dropped[j] = ev
    return dropped, kept, use


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("a"); ap.add_argument("b")
    ap.add_argument("--json", default="")
    ap.add_argument("--ignore", default="", help="LO-HI hex VA ranges whose NEW blocks do not end IDENTICAL-FOR-N (named in the report)")
    ap.add_argument("--n-ok", action="store_true", help="do not end IDENTICAL-FOR-N at an N misalignment (per-tick clock-call count; --clock tick runs)")
    ap.add_argument("--masks", default="", help="mask file(s), comma-separated (re/frida/warp_normalize.tsv,re/frida/replay_masks.tsv): MASK dwords are dropped per dword (routes full / raw / dump, see the header); --explain also scores with them")
    ap.add_argument("--block-masks-only", action="store_true", help="with --masks, drop only blocks whose whole 1 KB is MASK (the rule before RO-CTL-OSWORD)")
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
    va0 = lambda j: DATA_LO + (j - 2) * blk
    mk = set()
    if a.masks:
        from detorig import load_norm
        # union over the files: load_norm keeps the first file's class per dword (for the class name), so a RULE row in
        # warp_normalize.tsv (0x007e95a0 RULE:WINMSG) would hide the MASK:os row of replay_masks.tsv for the same dword
        mk = {v for f in a.masks.split(",") if f for v, c in load_norm(f)["mask"].items() if c.startswith("MASK:")}
    # route raw: per tick, per dword, on blocks the raw words cover whole
    raw_masked, raw_inconsistent, raw_race = {}, {}, {}
    if mk and not a.block_masks_only and raw_va:
        col = {v: 2 + nblk + k for k, v in enumerate(raw_va)}
        for j in range(2, 2 + nblk):
            lo = va0(j)
            cols = [col.get(lo + 4 * w) for w in range(blk // 4)]
            if None in cols:
                continue
            for i in range(n):
                if j not in diffs[i]:
                    continue
                x, y = ra[i], rb[i]
                dd = [lo + 4 * w for w, c in enumerate(cols) if hz(x[c]) != hz(y[c])]
                if not dd:
                    raw_inconsistent.setdefault(j, []).append(i)
                elif all(v in mk for v in dd):
                    diffs[i].discard(j)
                    raw_masked.setdefault(j, set()).update(dd)
            # A raw / hash disagreement: the hash pass and the raw copy run one after the other in the same tick listener
            # (js/detorig.js onEnter), so a dword written in between by another thread differs in one and not the other.
            # Such a tick is dropped only when every dword of the block that differs at any tick of the pair is MASK.
            if j in raw_inconsistent:
                moving = {lo + 4 * w for w, c in enumerate(cols) for i in range(n) if hz(ra[i][c]) != hz(rb[i][c])}
                if moving and moving <= mk:
                    for i in raw_inconsistent[j]:
                        diffs[i].discard(j)
                    raw_race[j] = (raw_inconsistent.pop(j), sorted(moving))
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
    ignored = {j for j in first_new if any(lo < va0(j) + blk and va0(j) < hi for lo, hi in ign)}
    masked_blocks, dump_masked, dump_unmasked, dump_ticks = [], {}, {}, []
    if mk:
        full = {j for j in first_new if all(va0(j) + 4 * w in mk for w in range(blk // 4))}
        masked_blocks = [[hex(va0(j)), first_new[j], "full"] for j in sorted(full - ignored)]
        ignored |= full
        if not a.block_masks_only:
            dump_masked, dump_unmasked, dump_ticks = dump_route(a.a, a.b, diffs, first_new, ignored, mk, va0, blk, n)
            masked_blocks += [[hex(va0(j)), first_new[j], "dump", ev] for j, ev in sorted(dump_masked.items())]
            ignored |= set(dump_masked)
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
        "raw_masked_blocks": {hex(va0(j)): sorted(hex(v) for v in s) for j, s in sorted(raw_masked.items())},
        "raw_inconsistent": {hex(va0(j)): t[:20] for j, t in sorted(raw_inconsistent.items())},
        "raw_race": {hex(va0(j)): {"ticks": t[:20], "moving": [hex(v) for v in mv]} for j, (t, mv) in sorted(raw_race.items())},
        "dump_ticks": dump_ticks,
        "dump_unmasked": {hex(va0(j)): b for j, b in sorted(dump_unmasked.items())},
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
        print(f"    NEW {x[0]} from tick {x[1]}  ({x[2]})" + (" [masked]" if int(x[0], 16) in {int(m[0], 16) for m in masked_blocks} else ""))
    if mk:
        for blkva, dws in rep["raw_masked_blocks"].items():
            print(f"  mask raw: {blkva} dropped at the ticks where only {dws} differ")
        for m in masked_blocks:
            print(f"  mask {m[2]}: {m[0]} (NEW at tick {m[1]})" + (f" checkpoints {[e[0] for e in m[3]]}, dwords {sorted({d for e in m[3] for d in e[1]})}" if m[2] == "dump" else ""))
        for blkva, t in rep["raw_inconsistent"].items():
            print(f"  raw/hash disagreement in {blkva} at ticks {t} (kept: a non-MASK dword of the block differs at some tick)")
        for blkva, r in rep["raw_race"].items():
            print(f"  raw/hash disagreement in {blkva} at ticks {r['ticks']} dropped: the block's only differing dwords are MASK {r['moving']}")
        if not a.block_masks_only:
            print(f"  dump route checkpoint ticks (both runs, re-hash OK): {dump_ticks}")
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

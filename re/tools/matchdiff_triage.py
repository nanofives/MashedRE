#!/usr/bin/env python3
"""matchdiff_triage.py -- classify matchdiff_sweep FAIL rows against the known artifact classes.

    py -3.12 re/tools/matchdiff_triage.py [--conf C3,C4] [--out re/parity/matchdiff_triage.csv]

The sweep (re/analysis/matchdiff_sweep_20260909.md) leaves FAIL rows that are a mix of
real transcription defects and four compiler/source-shape artifacts. Eyeballing 154 rows
invites confirmation bias, so each class here is decided by a TEST against the binary,
not by pattern-matching the delta string. Rows no test explains are the shortlist.

Classes, and the evidence each one requires:

  INLINED       every orig-only address lies inside the .data footprint of a function
                our compiled code CALLS by absolute address (our `.text` advisory row).
                i.e. the original inlined a helper we call out to. Footprints are
                computed from the original binary, so this is checkable, not assumed.
  UNROLLED      our extra addresses form an arithmetic progression whose base or bound
                the original references (we enumerate; the original walks a base reg).
  INDUCTION     orig-only and our-only addresses pair 1:1 within +/-0x40 (the compiler
                rotated a loop: base-1 with the index shifted by one).
  WINDOW        our extras all sit within 0x200 of an address the original references
                (the original cached a base in a register and used [reg+disp]).
  CANDIDATE     nothing above explains it -> hand review.

A row can satisfy several tests; the first matching class in the order above wins, and
the full test vector is written to the CSV so a reviewer can see what else matched.
"""
import argparse
import collections
import csv
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from matchdiff import (function_bytes_from_obj, disasm, operand_facts,  # noqa: E402
                       parse_coff, load_func_entries, DATA, function_relocs_named)
from matchdiff_sweep import (load_registrations, load_bounds, load_conf,  # noqa: E402
                             OBJ_ASI, BOUNDS, EXE, ROOT)

import pefile  # noqa: E402

FOOTPRINT_CACHE = ROOT / "re" / "console" / "cache" / "data_footprints.json"


def build_footprints(pe, base, bounds):
    """rva -> sorted list of .data addresses the ORIGINAL function references.

    NOTE: capstone's linear sweep stops at the first undecodable byte, so a footprint
    can be partial (memory: capstone-linear-sweep-truncates). Partial footprints can
    only make the INLINED test FAIL to fire, never fire it wrongly.
    """
    if FOOTPRINT_CACHE.exists():
        return {int(k): v for k, v in json.loads(FOOTPRINT_CACHE.read_text()).items()}
    fp = {}
    for rva, size in bounds.items():
        try:
            code = pe.get_data(rva - base, size)
        except Exception:                                    # noqa: BLE001
            continue
        fp[rva] = sorted(set(operand_facts(disasm(code, rva), [], rva)[0]))
    FOOTPRINT_CACHE.parent.mkdir(parents=True, exist_ok=True)
    FOOTPRINT_CACHE.write_text(json.dumps({str(k): v for k, v in fp.items()}))
    return fp


def classify(miss, extra, our_calls, footprints, orig_all, orig_disps):
    """-> (class, tests_that_matched)"""
    tests = []

    # INLINED: orig-only addresses covered by the footprint of something we call
    if miss:
        covered = set()
        for tgt in our_calls:
            covered |= set(footprints.get(tgt, ()))
        if covered and all(m in covered for m in miss):
            tests.append("INLINED")

    # FOLDED-MEMBER: our literal = orig base + a member displacement the ORIGINAL
    # actually uses ([eax + 0x294]), i.e. the compiler folded base+offset into one
    # address. Effective addresses are identical. Verified by hand on 0x0041eda0.
    if miss and extra:
        paired = 0
        for x in extra:
            if any((x - m) in orig_disps or (m - x) in orig_disps for m in miss):
                paired += 1
        if paired == len(extra):
            tests.append("FOLDED-MEMBER")

    # UNROLLED: our extras are an arithmetic progression anchored to an orig address
    if len(extra) >= 2:
        e = sorted(extra)
        d = e[1] - e[0]
        if d and all(e[i + 1] - e[i] == d for i in range(len(e) - 1)):
            anchors = {e[0] - d, e[-1] + d, e[0], e[-1]}
            if anchors & (set(miss) | set(orig_all)):
                tests.append("UNROLLED")

    # INDUCTION: 1:1 pairing within +/-0x40
    if miss and extra and len(miss) == len(extra):
        if all(any(0 < abs(x - m) <= 0x40 for m in miss) for x in extra):
            tests.append("INDUCTION")

    # WINDOW: extras cluster near an address the original does reference
    if extra and orig_all:
        if all(any(abs(x - o) <= 0x200 for o in orig_all) for x in extra):
            tests.append("WINDOW")

    for name in ("INLINED", "FOLDED-MEMBER", "UNROLLED", "INDUCTION", "WINDOW"):
        if name in tests:
            return name, tests
    return "CANDIDATE", tests


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--conf", default="C3,C4")
    ap.add_argument("--out", default=str(ROOT / "re" / "parity" / "matchdiff_triage.csv"))
    a = ap.parse_args()
    want = set(a.conf.split(","))

    regs, _ = load_registrations()
    bounds, conf = load_bounds(), load_conf()
    ents = load_func_entries()
    pe = pefile.PE(str(EXE), fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    print("building .data footprints for %d original functions..." % len(bounds))
    footprints = build_footprints(pe, base, bounds)

    sym_rva = {sname: srva for (sname, srva) in regs}
    print("indexing defined symbols across %d objects..." % len(list(OBJ_ASI.glob('*.obj'))))
    defined_in = {}
    for op in OBJ_ASI.glob('*.obj'):
        try:
            for x in parse_coff(op.read_bytes())[1]:
                if x['sec'] > 0 and x['name'] not in defined_in:
                    defined_in[x['name']] = op
        except Exception:                                    # noqa: BLE001
            continue
    obj_cache = {}
    rows, tally = [], collections.Counter()
    for (sym, rva), cpp in sorted(regs.items(), key=lambda kv: kv[0][1]):
        c = conf.get(rva, ("", "", ""))
        if c[0] not in want or rva not in bounds:
            continue
        objp = OBJ_ASI / (cpp.stem + ".obj")
        if not objp.exists():
            continue
        if objp not in obj_cache:
            obj_cache[objp] = objp.read_bytes()
        try:
            ours, relocs, _ = function_bytes_from_obj(obj_cache[objp], sym)
        except SystemExit:
            continue
        orig = pe.get_data(rva - base, bounds[rva])
        fo = operand_facts(disasm(ours, rva), [r[0] for r in relocs], rva, ents)
        fg = operand_facts(disasm(orig, rva), [], rva, ents)

        # only rows the SWEEP flagged are triaged; a row that already corresponds is
        # not a finding and must not inflate the denominator
        if not (set(fg[0]) - set(fo[0])) and not (set(fo[0]) - set(fg[0])):
            continue

        # WRAPPER: our export may be a naked ABI shim / thin wrapper that delegates
        # to a *_Body symbol in the SAME object (PromptStripTwin -> PromptStripTwinBody,
        # AiControlStep -> ControlStepBody<...>). Its own footprint is then tiny and
        # every original address reads as missing. Follow one level into local callees
        # and use the UNION as our effective footprint. Verified by hand on 4 rows.
        local_defs = {x['name'] for x in parse_coff(obj_cache[objp])[1] if x['sec'] > 0}
        # cross-TU: a shared helper often lives in ANOTHER object (FastSqrt calls
        # RwLutGuard::Resolve, defined in its own TU), so a same-object-only union
        # cannot reach it. defined_in maps every defined symbol -> its object.
        # TRADE-OFF: the wider the union, the weaker the test -- a real defect can hide
        # behind a helper that happens to touch the right address. Depth is capped at 2
        # and every followed symbol is recorded in the CSV's `tests` column.
        wrapped, our_data = [], set(fo[0])
        # Follow local callees to DEPTH 2. One level is not enough: a shared helper may
        # itself delegate (Math/RwLutGuard.h RwLutRoot() reads 0x007d3ff8/0x007d3ffc for
        # FastSqrt & friends), and naked shims sometimes chain shim -> body -> impl.
        seen, frontier, cur_home = {sym}, [sym], {sym: objp}
        for _depth in range(2):
            nxt = []
            for cur in frontier:
                cur_obj = cur_home.get(cur, objp)
                for _off, _t, tname in function_relocs_named(obj_cache[cur_obj], cur):
                    base_name = tname.lstrip('_')
                    home = objp if tname in local_defs else defined_in.get(tname)
                    if home is None or base_name in seen:
                        continue
                    seen.add(base_name)
                    if home not in obj_cache:
                        obj_cache[home] = home.read_bytes()
                    cur_home[base_name] = home
                    try:
                        b, br, _ = function_bytes_from_obj(obj_cache[home], base_name)
                    except SystemExit:
                        continue
                    our_data |= set(operand_facts(disasm(b, rva), [r[0] for r in br], rva, ents)[0])
                    wrapped.append(tname)
                    nxt.append(base_name)
            frontier = nxt

        # ASYMMETRIC, deliberately: the callee union may only EXPLAIN missing addresses
        # ("the original touches X inline; we touch it inside a helper we call"). It must
        # not contribute EXTRAS -- a helper's own globals say nothing about the original,
        # and folding them in made CANDIDATE go UP (57 -> 62) when first tried.
        miss = sorted(set(fg[0]) - our_data)
        extra = sorted(set(fo[0]) - set(fg[0]))
        if not miss and not extra:
            if wrapped:
                tally['WRAPPER'] += 1
                rows.append(dict(rva="%08x" % rva, symbol=sym, conf=c[0], subsystem=c[2],
                                 file=cpp.name, klass='WRAPPER',
                                 tests='resolved-via:' + ','.join(wrapped[:3]),
                                 n_miss=0, n_extra=0, ours_b=len(ours), orig_b=len(orig),
                                 ratio='-', miss='', extra=''))
            continue
        # callees: absolute literals AND relocated calls resolved by symbol name
        our_calls = set(fo[1])
        for _off, _t, tname in function_relocs_named(obj_cache[objp], sym):
            tgt = sym_rva.get(tname.lstrip('_'))
            if tgt:
                our_calls.add(tgt)
        # member displacements the ORIGINAL uses (for the FOLDED-MEMBER test)
        orig_disps = set()
        for ins in disasm(orig, rva):
            for op in ins.operands:
                if op.type == 3 and op.mem.base != 0 and 0 < op.mem.disp < 0x10000:
                    orig_disps.add(op.mem.disp)
        klass, tests = classify(miss, extra, sorted(our_calls), footprints,
                                set(fg[0]), orig_disps)
        tally[klass] += 1
        rows.append(dict(
            rva="%08x" % rva, symbol=sym, conf=c[0], subsystem=c[2], file=cpp.name,
            klass=klass, tests="|".join(tests),
            n_miss=len(miss), n_extra=len(extra),
            ours_b=len(ours), orig_b=len(orig),
            ratio="%.2f" % (len(ours) / len(orig) if orig else 0),
            miss=",".join("%08x" % x for x in miss[:8]),
            extra=",".join("%08x" % x for x in extra[:8])))

    outp = Path(a.out)
    outp.parent.mkdir(parents=True, exist_ok=True)
    with open(outp, "w", encoding="utf-8", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=list(rows[0].keys()))
        w.writeheader()
        w.writerows(rows)

    print("\n%d FAIL rows classified:" % len(rows))
    for k, v in tally.most_common():
        print("  %-10s %3d   (%.0f%%)" % (k, v, 100.0 * v / len(rows)))
    print("wrote %s\n" % outp)
    cands = [r for r in rows if r["klass"] == "CANDIDATE"]
    print("CANDIDATE shortlist (%d) -- no artifact test explains these:" % len(cands))
    for r in sorted(cands, key=lambda r: (r["conf"] != "C4", r["rva"])):
        print("  %s %-28s %-3s %-9s -%2d/+%-2d %5sB/%5sB r=%s %s" %
              (r["rva"], r["symbol"][:28], r["conf"], r["subsystem"][:9],
               r["n_miss"], r["n_extra"], r["ours_b"], r["orig_b"], r["ratio"],
               ("STUB?" if float(r["ratio"]) < 0.5 else "")))
    return 0


if __name__ == "__main__":
    sys.exit(main())

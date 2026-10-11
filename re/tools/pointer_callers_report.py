"""What re/analysis/pointer_callers_<date>.tsv changes for the anti-island gate and for the A/B mutator lane.

Read-only: reads hooks.csv, the TSV, log/refgraph/refs.tsv (RefGraphPC.java) and re/analysis/plans/mutators.txt.
Writes nothing unless -o is given (the gate-candidate list).

1. ANTI-ISLAND (caller half only). The gate's current caller set for a function is Ghidra's getCallingFunctions
   (DecompPC.java `callers`), which is every function containing ANY reference to the entry. It is rebuilt here from
   refs.tsv the same way. A C2 row is HELD when no member qualifies under caller_screen.py (C2+, or a hooks.csv name
   that is not FUN_/LAB_/thunk_/sub_). It GAINS when an installer function from the TSV qualifies and is not the row
   itself. The callee half is not evaluated, so a gain is a candidate for the gate, not a passed gate. Whether an
   installer counts as a caller is a re/CONFIDENCE.md question this script does not answer.

2. MUTATORS. scripts/write_surface.py walk(depth 3) over mutators.txt reproduces b77f6b42's
   "indirect - NOT reachable" set (1,126 on that commit). For each one this reports
   (a) whether the mutator itself gains an installer row, and
   (b) the class of every indirect site in its call tree, since (a) does not touch the blocker: the blocker is the
       mutator's CALLEE side. Classes: iat (call/jmp [abs] into the import address table), idx_table
       (jmp [reg*4 + abs] that write_surface.switch_table() rejects), ptr_known (call/jmp [abs] whose slot the TSV maps to a function: a ptr_call row at that
       site), ptr_abs (other call/jmp [abs]), reg_disp (call [reg + disp]: vtable / struct field), reg (call reg).

    py -3.12 re/tools/pointer_callers_report.py re/analysis/pointer_callers_2026-10-11.tsv [-o gate_candidates.tsv]
"""
import argparse, collections, csv, io, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "re" / "tools"))
sys.path.insert(0, str(ROOT / "scripts"))
import pointer_callers as pc  # noqa: E402

IAT = (0x005cc000, 0x005cc000 + 0x2e8)          # PE data directory 12 of the anchored image
CLAUSE_KINDS = ("vtable", "fn_table")


def callee_half(im, hooks, t, imports):
    """(verdict, basis) for the C2->C3 callee half of `t`, from its own bytes (capstone over Ghidra's ranges).

    satisfied      a direct callee (call rel32, or jmp rel32 out of the body) qualifies under caller_screen.py's
                   rule (C2+, or a hooks.csv name not FUN_/LAB_/thunk_/sub_; CONFIDENCE.md L26), or the function
                   calls an import (IAT slot or a thunk `jmp [IAT]`: a named library API)
    leaf           no callee at all: L24, needs the A/B on a non-trivial input domain
    indirect-only  no direct callee, at least one call/jmp through a register or memory that is not a switch:
                   L27, needs the A/B to exercise the dispatch itself
    blocked        direct callees exist and none qualifies
    """
    import capstone
    from capstone import x86 as cx
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    md.skipdata = True
    own = [(lo, hi) for lo, hi, e in im.ranges if e == t]
    inside = lambda v: any(lo <= v < hi for lo, hi in own)
    direct, imps, indirect = set(), set(), 0
    for lo, hi in own:
        for i in md.disasm(im.img[lo - im.base:hi - im.base], lo):
            if i.id == 0 or i.mnemonic not in ("call", "jmp"):
                continue
            op = i.operands[0]
            if op.type == cx.X86_OP_IMM:
                if i.mnemonic == "call" or not inside(op.imm):
                    if op.imm != t:
                        direct.add(op.imm)
            elif op.type == cx.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 \
                    and IAT[0] <= op.mem.disp < IAT[1]:
                imps.add(imports.get(op.mem.disp, f"IAT 0x{op.mem.disp:08x}"))
            elif op.type == cx.X86_OP_MEM and op.mem.scale == 4 and op.mem.base == 0 \
                    and inside(im.dword(op.mem.disp) or 0):
                continue                                    # switch: case label inside this body
            else:
                indirect += 1
    for c in sorted(direct):                                # a thunk `jmp [IAT]` is the import itself
        o = c - im.base
        if im.img[o:o + 2] == b"\xff\x25":
            slot = int.from_bytes(im.img[o + 2:o + 6], "little")
            if IAT[0] <= slot < IAT[1]:
                imps.add(imports.get(slot, f"IAT 0x{slot:08x}"))
    good = [c for c in sorted(direct) if pc.qualifies(hooks, c)]
    if good:
        c = good[0]
        return "satisfied", f"callee 0x{c:08x} {hooks[c]['name']} {hooks[c]['conf']}" + (
            f" (+{len(good) - 1})" if len(good) > 1 else "")
    if imps:
        return "satisfied", "import " + ",".join(sorted(imps))
    if not direct and not indirect:
        return "leaf", "no callee (L24: A/B on a non-trivial domain)"
    if not direct:
        return "indirect-only", f"{indirect} indirect site(s) (L27: A/B must exercise the dispatch)"
    return "blocked", "anonymous callees " + ";".join(f"0x{c:08x}" for c in sorted(direct))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("tsv")
    ap.add_argument("-o", "--out")
    a = ap.parse_args()
    hooks = pc.load_hooks()
    rows = list(csv.DictReader(io.open(a.tsv, encoding="utf-8"), delimiter="\t"))
    inst = collections.defaultdict(set)
    for r in rows:
        # the installer clause (re/CONFIDENCE.md, 2026-10-11): vtable / function-table rows whose boundary test is
        # 'resolved'; code kinds are already in Ghidra's caller set, so they add nothing to the caller half
        if r["kind"] in CLAUSE_KINDS and r.get("boundary", "resolved") == "resolved":
            inst[int(r["target_rva"], 16)].add(int(r["installer_fn"], 16))

    # ---- 1. anti-island, caller half ----------------------------------------------------------------------
    calling = collections.defaultdict(set)
    for r in csv.DictReader(io.open(pc.refgraph_dir() / "refs.tsv", encoding="utf-8"), delimiter="\t"):
        if r["from_func"] != "-":
            calling[int(r["to"], 16)].add(int(r["from_func"], 16))
    c2 = [rva for rva, h in hooks.items() if h["conf"] == "C2"]
    held = [t for t in c2 if not any(pc.qualifies(hooks, f) for f in calling.get(t, ()))]
    gain, any_inst = [], 0
    for t in held:
        new = {f for f in inst.get(t, ()) if f != t}
        any_inst += bool(new)
        q = sorted(f for f in new if pc.qualifies(hooks, f))
        if q:
            gain.append((t, q))
    no_caller = sum(1 for t in held if not calling.get(t))
    print(f"ANTI-ISLAND (caller half): C2 rows {len(c2)}; held {len(held)} "
          f"({no_caller} with no Ghidra caller at all)")
    print(f"  held rows with any installer from the TSV: {any_inst}")
    print(f"  held rows gaining a qualifying (C2+ or named) installer: {len(gain)}")
    kinds = collections.Counter()
    for t, q in gain:
        ks = {r["kind"] for r in rows if int(r["target_rva"], 16) == t and r["installer_fn"] != "-"
              and int(r["installer_fn"], 16) in q}
        kinds.update(ks)
    print("  by installer kind:", dict(kinds.most_common()))
    # worst case for the unmeasured adjacent-vtable risk: only slot 0 of a table counts
    slot0 = {int(r["target_rva"], 16) for r in rows if r["kind"] in CLAUSE_KINDS
             and " slot 0 " in f" {r['detail']}" and pc.qualifies(hooks, int(r["installer_fn"], 16))}
    print(f"  of which survive a slot-0-only cut (worst case for hidden vtable starts): "
          f"{sum(1 for t, _ in gain if t in slot0)}")

    # ---- callee half for the gains --------------------------------------------------------------------------
    import pefile
    im = pc.Image()
    pe = pefile.PE(str(pc._main_repo() / "original" / "MASHED.exe.unpatched"))
    imports = {i.address: (i.name or b"ord").decode() for e in pe.DIRECTORY_ENTRY_IMPORT for i in e.imports}
    split = {t: callee_half(im, hooks, t, imports) for t, _ in gain}
    v = collections.Counter(s[0] for s in split.values())
    both = sum(1 for s in split.values() if s[0] != "blocked")
    print(f"  callee half over the {len(gain)}: {dict(v.most_common())}")
    print(f"  SPLIT: both halves satisfied {both} | caller satisfied, callee blocking {len(gain) - both}")
    if a.out:
        with io.open(a.out, "w", encoding="utf-8", newline="") as f:
            f.write("target_rva\ttarget_name\tqualifying_installers\tsplit\tcallee_half\tcallee_basis\n")
            for t, q in gain:
                verdict, basis = split[t]
                f.write(f"0x{t:08x}\t{hooks[t]['name']}\t" + ";".join(
                    f"0x{x:08x} {hooks[x]['name']} {hooks[x]['conf']}" for x in q)
                    + f"\t{'callee_blocking' if verdict == 'blocked' else 'both_satisfied'}\t{verdict}\t{basis}\n")
        print(f"  wrote {a.out}")

    # ---- 2. mutators ----------------------------------------------------------------------------------------
    import write_surface as ws
    ws.EXE = pc._main_repo() / "original" / "MASHED.exe.unpatched"
    base, data = ws.load()
    starts = sorted(r for r in hooks if r >= 0x400000)
    muts = [int(l.split("\t")[0], 16) for l in io.open(ROOT / "re/analysis/plans/mutators.txt", encoding="utf-8")
            if l.startswith("0x")]
    ptr_sites = {int(r["installer_site_rva"], 16) for r in rows if r["kind"] == "ptr_call"}
    inst_any = collections.defaultdict(set)
    for r in rows:
        if r["kind"] in pc.INSTALLER_KINDS:
            inst_any[int(r["target_rva"], 16)].add(int(r["installer_fn"], 16))
    blocked, gained, gained_any, cls_mut, all_cls = [], 0, 0, collections.Counter(), collections.Counter()
    only = collections.Counter()
    for m in muts:
        try:
            _, _, ind = ws.walk(m, 3, base, data, starts)
        except Exception:
            continue
        if not ind:
            continue
        blocked.append(m)
        gained += bool({f for f in inst.get(m, ()) if f != m})
        gained_any += bool({f for f in inst_any.get(m, ()) if f != m})
        cs = set()
        for fn, d, at, op in ind:
            mabs = re.fullmatch(r"dword ptr \[(0x[0-9a-f]+)\]", op)
            if mabs and IAT[0] <= int(mabs.group(1), 16) < IAT[1]:
                c = "iat"
            elif re.fullmatch(r"dword ptr \[e[a-z]{2}\*4 \+ 0x[0-9a-f]+\]", op):
                c = "idx_table"     # [reg*4 + tbl] that write_surface.switch_table() did NOT accept as a switch
            elif mabs:
                c = "ptr_known" if at in ptr_sites else "ptr_abs"
            elif "[" in op:
                c = "reg_disp"
            else:
                c = "reg"
            cs.add(c)
            all_cls[c] += 1
        cls_mut.update(cs)
        only[frozenset(cs)] += 1
    n = len(blocked)
    print(f"\nMUTATORS: {len(muts)} in mutators.txt; indirect at depth 3: {n}")
    print(f"  (a) blocked mutators that gain an installer row themselves: {gained_any} (any kind), "
          f"{gained} (installer-clause kinds)")
    print(f"  (b) indirect sites by class: {dict(all_cls.most_common())}")
    print(f"      blocked mutators with >=1 site of class: {dict(cls_mut.most_common())}")
    # ptr_known = target set mapped by the TSV, statically followable once no store_mem replaces the global.
    # iat is not: an import writes through its arguments, so it stays a blocker for a sound snapshot set.
    print(f"      blocked ONLY by ptr_known (followable with this TSV): "
          f"{sum(v for k, v in only.items() if k == {'ptr_known'})}")
    print(f"      blocked ONLY by iat/ptr_known: {sum(v for k, v in only.items() if k <= {'iat', 'ptr_known'})}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

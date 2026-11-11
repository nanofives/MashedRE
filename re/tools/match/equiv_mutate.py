"""Mutation test for equiv.py (EQUIV-CHECK).

For every pair that equiv.py proves (synthetic positives from test_equiv.CASES, plus real EQUIV rows of a
measurement TSV), flip semantically meaningful bits in OUR body only, at instructions the proof executed:

    disp   a non-relocated displacement + 4        imm    a non-relocated immediate + 1
    cc     a conditional jump / setcc negated      reg    one register field of one instruction changed
    swap   two adjacent non-branch instructions swapped, at least one writing memory

Every mutant must come out NOT-PROVEN. A mutant that comes out EQUIV is run through the Unicorn cross-check
(equiv_dyn): DIFFERS = the tool let a real difference through (a soundness bug); SAME = recorded as "survived,
no observable difference in N runs" and listed for manual reading. `swap` mutants the proof accepts are reported
apart: a swap of provably independent instructions is a correct EQUIV, not an escape.

    py -3.12 re/tools/match/equiv_mutate.py [--tsv verify/equivcheck_20261111/measure_v2.tsv] [--real 40]
        [--out verify/equivcheck_20261111/mutation.tsv]
"""
import argparse
import collections
import pathlib
import random
import struct
import sys

import capstone
from capstone import x86 as X

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import importlib, os  # noqa: E402
equiv = importlib.import_module(os.environ.get("EQUIV_MODULE", "equiv"))  # noqa: E402
import equiv_dyn  # noqa: E402
import match  # noqa: E402
import test_equiv  # noqa: E402

ROOT = HERE.parents[2]
MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True


def reloc_bytes(relocs):
    out = set()
    for off in relocs:
        out.update(range(off, off + 4))
    return out


def decode(data, pc):
    try:
        return next(MD.disasm(bytes(data[pc:pc + 16]), pc))
    except StopIteration:
        return None


def mutants(data, pcs, relocs):
    """Yields (kind, pc, new_data) for one section's bytes."""
    rb = reloc_bytes(relocs)
    out = []
    for pc in sorted(pcs):
        c = decode(data, pc)
        if c is None:
            continue
        span = set(range(pc, pc + c.size))
        is_jump = c.group(capstone.CS_GRP_JUMP) or c.group(capstone.CS_GRP_CALL) or c.group(capstone.CS_GRP_RET)
        # disp
        if c.disp_offset and not (set(range(pc + c.disp_offset, pc + c.disp_offset + c.disp_size)) & rb):
            d = bytearray(data)
            o = pc + c.disp_offset
            if c.disp_size == 1:
                v = struct.unpack_from("<b", d, o)[0]
                if -128 <= v + 4 <= 127:
                    struct.pack_into("<b", d, o, v + 4)
                    out.append(("disp", pc, d))
            elif c.disp_size == 4:
                struct.pack_into("<i", d, o, (struct.unpack_from("<i", d, o)[0] + 4 + (1 << 31)) % (1 << 32) - (1 << 31))
                out.append(("disp", pc, d))
        # imm (not jump targets)
        if c.imm_offset and not is_jump and not (set(range(pc + c.imm_offset, pc + c.imm_offset + c.imm_size)) & rb):
            d = bytearray(data)
            d[pc + c.imm_offset] = (d[pc + c.imm_offset] + 1) & 0xFF
            out.append(("imm", pc, d))
        # cc
        b0 = data[pc]
        if (0x70 <= b0 <= 0x7F) or (b0 == 0x0F and pc + 1 < len(data) and (0x80 <= data[pc + 1] <= 0x9F)):
            d = bytearray(data)
            if b0 == 0x0F:
                d[pc + 1] ^= 1
            else:
                d[pc] ^= 1
            out.append(("cc", pc, d))
        # reg: the ModRM reg field of an instruction with a register operand
        if not is_jump and not (span & rb) and any(op.type == X.X86_OP_REG for op in c.operands) and \
                not c.mnemonic.startswith("f") and c.mnemonic not in ("push", "pop", "nop", "lea") or \
                (c.mnemonic == "lea" and not (span & rb)):
            np = sum(1 for x in c.prefix if x)
            mo = pc + np + (2 if data[pc + np] == 0x0F else 1)
            if mo < pc + c.size and data[mo] == c.modrm:
                regf = (data[mo] >> 3) & 7
                for new in (regf ^ 1, regf ^ 2, regf ^ 3):
                    if new in (4,):
                        continue
                    d = bytearray(data)
                    d[mo] = (d[mo] & ~0x38) | (new << 3)
                    c2 = decode(d, pc)
                    if c2 is not None and c2.size == c.size and c2.mnemonic == c.mnemonic and c2.op_str != c.op_str:
                        out.append(("reg", pc, d))
                        break
    # swap adjacent non-branch instructions, one of them writing memory
    pl = sorted(pcs)
    for p1 in pl:
        c1 = decode(data, p1)
        if c1 is None:
            continue
        p2 = p1 + c1.size
        if p2 not in pcs:
            continue
        c2 = decode(data, p2)
        if c2 is None:
            continue
        bad = any(c.group(capstone.CS_GRP_JUMP) or c.group(capstone.CS_GRP_CALL) or c.group(capstone.CS_GRP_RET)
                  or c.mnemonic.startswith("f") for c in (c1, c2))
        if bad or (set(range(p1, p2 + c2.size)) & rb):
            continue
        wm = any(op.type == X.X86_OP_MEM and (op.access & capstone.CS_AC_WRITE) for c in (c1, c2) for op in c.operands)
        if not wm or bytes(c1.bytes) == bytes(c2.bytes):
            continue
        d = bytearray(data)
        d[p1:p2 + c2.size] = bytes(c2.bytes) + bytes(c1.bytes)
        out.append(("swap", p1, d))
    return out


def prove(secs, syms, text, symA, addr, B_kind, B_arg):
    equiv.reset_terms()
    res = equiv.Resolver(text, secs, syms, addr)
    A = equiv.ObjSide("ours", secs, syms, symA, addr, res)
    if B_kind == "image":
        B = equiv.ImageSide("orig", addr, B_arg)
        try:
            equiv.verify_odata(text, A, B, res)
        except Exception:
            pass
    else:
        B = equiv.ObjSide("orig", secs, syms, B_arg, addr, res)
    try:
        r = equiv.check_sides(A, B, res, addr)
    except equiv.NotProven as e:
        r = {"verdict": "NOT-PROVEN", "kind": e.kind}
    return r, A, B, res


def dyn(secs, syms, res, A, B, addr, ign):
    layout = equiv_dyn.Layout(secs, syms, res, A.secidx)
    bA = equiv_dyn.Body(equiv_dyn.OURS_BASE + A.start, equiv_dyn.OURS_BASE + A.start, equiv_dyn.OURS_BASE + A.end)
    if isinstance(B, equiv.ImageSide):
        t = match.function_table()
        later = [e for e in t if e > addr]
        hi = min(later) if later else B.end
        bB = equiv_dyn.Body(addr, addr, max(hi, B.end))
    else:
        bB = equiv_dyn.Body(equiv_dyn.OURS_BASE + B.start, equiv_dyn.OURS_BASE + B.start, equiv_dyn.OURS_BASE + B.end)
    return equiv_dyn.check(layout, bA, bB, runs=24, ignore=ign)


def run_pair(label, secs, syms, text, symA, addr, B_kind, B_arg, classes, rows, kills):
    r0, A, B, res = prove(secs, syms, text, symA, addr, B_kind, B_arg)
    if r0["verdict"] == "NOT-PROVEN":
        return
    ign = tuple(r0.get("regs", ()))
    pcs = set(equiv.LAST_PCS.get("A", set()))
    sec = secs[A.secidx - 1]
    for kind, pc, data in mutants(sec["data"], pcs, [o for o, _, _ in sec["relocs"]]):
        ms = [dict(x) for x in secs]
        ms[A.secidx - 1] = dict(sec, data=bytes(data))
        r, A2, B2, res2 = prove(ms, syms, text, symA, addr, B_kind, B_arg)
        if r["verdict"] == "NOT-PROVEN":
            status = "killed"
            for c in classes or ["(none)"]:
                kills[c]["killed"] += 1
        elif r["verdict"] != r0["verdict"] or tuple(r.get("regs", ())) != ign:
            # the mutant changed a return register: the tool reports it (EQUIV -> EQUIV-IF-CALLERS-IGNORE(r))
            tally, first = dyn(ms, syms, res2, A2, B2, addr, tuple(r.get("regs", ())))
            status = "ESCAPED" if tally["differs"] else "flagged-return-register"
            for c in classes or ["(none)"]:
                kills[c][status] += 1
        else:
            tally, first = dyn(ms, syms, res2, A2, B2, addr, ign)
            if tally["differs"]:
                status = "ESCAPED"
            elif kind == "swap":
                status = "swap-proven-independent"
            else:
                status = "survived-no-dynamic-difference"
            for c in classes or ["(none)"]:
                kills[c][status] += 1
        rows.append((label, kind, f"+0x{pc - A.start:x}", status, r["verdict"],
                     r.get("kind", "") or ",".join(r.get("regs", ()))))
        print(f"[{len(rows)}] {label} {kind} +0x{pc - A.start:x} {status}", flush=True)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tsv", default="")
    ap.add_argument("--real", type=int, default=40)
    ap.add_argument("--out", default="")
    ap.add_argument("--seed", type=int, default=7)
    a = ap.parse_args()
    rows = []
    kills = collections.defaultdict(collections.Counter)
    secs, syms = test_equiv.cases_obj()
    for name, (want, extra) in test_equiv.CASES.items():
        if want == "NOT-PROVEN":
            continue
        r = test_equiv.run_pair(name)
        run_pair(name, secs, syms, "", name + "_a", test_equiv.FAKE, "obj", name + "_b", r.get("classes", []), rows, kills)
    if a.tsv:
        real = []
        for ln in pathlib.Path(a.tsv).read_text().splitlines()[1:]:
            f = ln.split("\t")
            if f[7] in ("EQUIV", "EQUIV-IF-CALLERS-IGNORE"):
                real.append((f[1], int(f[0], 16), [c for c in f[8].split(",") if c]))
        random.Random(a.seed).shuffle(real)
        for src, rva, classes in real[:a.real]:
            p = ROOT / src
            text = p.read_text()
            sym = dict(match.parse_annotations(text))[rva]
            s2, y2 = equiv.coff_full(match.compile_obj(p, match.file_flags(text) or match.DEFAULT_FLAGS,
                                                       match.file_lang(text), match.file_toolchain(text)))
            run_pair(f"0x{rva:08x}", s2, y2, text, sym, rva, "image", match.ghidra_size(rva), classes, rows, kills)
    print("\nper class (a mutant counts once for every class its pair used):")
    for c, k in sorted(kills.items()):
        print(f"  {c}: " + ", ".join(f"{x}={n}" for x, n in sorted(k.items())))
    tot = collections.Counter(r[3] for r in rows)
    print("TOTAL", dict(tot))
    if a.out:
        lines = ["pair\tkind\tsite\tstatus\tverdict\tkind_detail"] + ["\t".join(r) for r in rows]
        lines.append("")
        lines.append("# per class: " + "; ".join(f"{c}: " + ", ".join(f"{x}={n}" for x, n in sorted(k.items()))
                                         for c, k in sorted(kills.items())))
        pathlib.Path(a.out).write_text("\n".join(lines) + "\n")


if __name__ == "__main__":
    main()

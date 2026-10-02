#!/usr/bin/env python3
"""Register-relative displacement sweep for a vehicle-record offset, x87 stores INCLUDED.

Why this exists (D2 attempt 19, gate WS). Two instruments already sweep the image and
NEITHER answers "who writes [reg + OFF]":

  * re/tools/fold_sweep.py matches the FOLDED absolute 0x008815a0 + i*0xd04 + OFF, so it
    never sees `fst dword ptr [edi + 0x9e0]` -- the displacement there is 0x9e0, not a
    folded address. Running it on 0x9e0 returns 7 reads and 0 writes, which is a true
    statement about folded encodings and a false one about the field.
  * re/tools/findoffset.py --writes matches the displacement but is blind to x87 stores
    (attempt 18 recorded it missing `0x00467673 fstp [esi+0x9e4]`, a real write).

This tool matches any memory operand whose displacement equals OFF, and classifies the
access by MNEMONIC, with the x87 store set named explicitly.

KNOWN ANSWER (print it with --known): sweeping 0x9e4 must report BOTH
`0x00467673 fstp` and `0x004686cc fstp` as WRITES. Both are published in
verify/d2_budget_20261002/RESULT_STEP2.md section 3 as the only pre-clamp writers of
+0x9e4, and both are x87 stores, so they exercise exactly the blindness above.

Usage:  py -3.12 re/tools/dispsweep.py 0x9e0 [0x9e4 ...] [--known]
"""
import struct
import sys

import capstone

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"

# x87 stores write memory; x87 loads/compares read it. capstone's regs_write does not
# model the x87 stack, so the mnemonic set is the classifier.
X87_WRITE = {"fst", "fstp", "fist", "fistp", "fisttp", "fbstp", "fnstsw", "fnstcw",
             "fstsw", "fstcw", "fnsave", "fsave", "fxsave", "fnstenv", "fstenv"}
X87_READ = {"fld", "fild", "fbld", "fadd", "fsub", "fsubr", "fmul", "fdiv", "fdivr",
            "fcom", "fcomp", "fcomi", "fcomip", "fucom", "fucomp", "fucomi", "fucomip",
            "ficom", "ficomp", "fiadd", "fisub", "fisubr", "fimul", "fidiv", "fidivr",
            "frstor", "fldcw", "fldenv", "fxrstor"}
# integer instructions whose FIRST operand is the destination
INT_WRITE_DST0 = {"mov", "add", "sub", "and", "or", "xor", "inc", "dec", "neg", "not",
                  "shl", "shr", "sar", "rol", "ror", "adc", "sbb", "imul", "movzx",
                  "movsx", "xchg", "lea"}


def sweep_text():
    d = open(EXE, "rb").read()
    pe = struct.unpack_from("<I", d, 0x3C)[0]
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    optsz = struct.unpack_from("<H", d, pe + 20)[0]
    ib = struct.unpack_from("<I", d, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        s = pe + 24 + optsz + i * 40
        name = d[s:s + 8].rstrip(b"\0").decode(errors="replace")
        vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", d, s + 8)
        secs.append((name, vaddr, vsz, roff, rsz))
    _, vaddr, _, roff, rsz = next(s for s in secs if s[0] == ".text")
    span = d[roff:roff + rsz]
    va0 = ib + vaddr
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    off, n = 0, len(span)
    total = 0
    while off < n:
        last = off
        for ins in md.disasm(span[off:], va0 + off):
            total += 1
            yield ins
            last = off + (ins.address - (va0 + off)) + ins.size
        # memory: a capstone sweep stops at the first bad byte; restart past it
        off = max(last, off + 1)
    print(f"swept {total} instructions over .text 0x{va0:08x}..0x{va0 + rsz:08x}",
          file=sys.stderr)


def classify(ins, op_index):
    m = ins.mnemonic
    if m in X87_WRITE:
        return "W"
    if m in X87_READ:
        return "r"
    if m in INT_WRITE_DST0 and op_index == 0:
        return "W" if m != "lea" else "a"     # lea takes an address, not the memory
    if m in ("cmp", "test", "push"):
        return "r"
    return "?"


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    known = "--known" in sys.argv
    offs = [int(a, 16) for a in args]
    if known:
        offs = sorted(set(offs + [0x9E4]))
    hits = {o: [] for o in offs}
    for ins in sweep_text():
        if not ins.operands:
            continue
        for i, op in enumerate(ins.operands):
            if op.type != capstone.x86.X86_OP_MEM:
                continue
            disp = op.mem.disp
            if op.mem.base == 0 and op.mem.index == 0:
                continue              # absolute -> fold_sweep's job, not this one
            if disp in hits:
                hits[disp].append((ins.address, classify(ins, i),
                                   f"{ins.mnemonic} {ins.op_str}"))
    for o in offs:
        rows = hits[o]
        w = [r for r in rows if r[1] == "W"]
        print(f"=== +0x{o:x} register-relative : {len(rows)} hit(s), {len(w)} WRITE(s) ===")
        for addr, cls, text in rows:
            print(f"  {cls} 0x{addr:08x}  {text}")
    if known:
        w = {a for a, c, _ in hits[0x9E4] if c == "W"}
        ok = {0x00467673, 0x004686CC} <= w
        print(f"\nKNOWN ANSWER +0x9e4 writers include 0x00467673 and 0x004686cc: "
              f"{'PASS' if ok else 'FAIL'}  (writes found: "
              f"{', '.join('0x%08x' % a for a in sorted(w))})")


if __name__ == "__main__":
    main()

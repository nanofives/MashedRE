#!/usr/bin/env python3
"""Absolute/folded-operand sweep for a vehicle-record offset.

Record base DAT_008815a0, stride 0xd04 (attempt 16 self-check: rec == 0x8815a0).
A folded encoding of &veh[i]+OFF is the absolute 0x008815a0 + i*0xd04 + OFF.
Matches: absolute mem operands, base/index+disp operands whose disp is the folded
address, lea/push/cmp immediates of it, and the dword-index immediate OFF/4.

Usage:  fold_sweep.py 0xb0c [0xbf8 ...]
Known-answer: 0xbf8 MUST report the attempt-15 hits at 0x00882198.
"""
import struct, sys
import capstone

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
d = open(EXE, 'rb').read()
pe = struct.unpack_from("<I", d, 0x3C)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]
optsz = struct.unpack_from("<H", d, pe + 20)[0]
ib = struct.unpack_from("<I", d, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    s = pe + 24 + optsz + i * 40
    name = d[s:s+8].rstrip(b"\0").decode(errors="replace")
    vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", d, s + 8)
    secs.append((name, vaddr, vsz, roff, rsz))
_, vaddr, _, roff, rsz = next(s for s in secs if s[0] == ".text")
span = d[roff:roff+rsz]
VA0 = ib + vaddr

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True

def sweep():
    off, n = 0, len(span)
    while off < n:
        last = off
        for ins in md.disasm(span[off:], VA0 + off):
            yield ins
            last = off + (ins.address - (VA0 + off)) + ins.size
        off = max(last, off + 1)

OFFS = [int(x, 16) for x in sys.argv[1:]] or [0xb0c]
BASES = (0x008815a0, 0x008815a4)
TARGETS = {}
for off in OFFS:
    for b in BASES:
        for i in range(12):
            TARGETS[b + i*0xd04 + off] = (off, f"base 0x{b:08x} slot {i}")
IMMIDX = {off // 4: off for off in OFFS}

n_ins = 0
res = {off: [] for off in OFFS}
for ins in sweep():
    n_ins += 1
    for op in ins.operands:
        if op.type == capstone.x86.X86_OP_MEM:
            dsp = op.mem.disp & 0xffffffff
            if dsp in TARGETS:
                off, tag = TARGETS[dsp]
                kind = "W" if (op.access & capstone.CS_AC_WRITE) else "r"
                form = "ABS" if (op.mem.base == 0 and op.mem.index == 0) else "FOLDED"
                res[off].append((ins.address, kind, form, tag,
                                 f"{ins.mnemonic} {ins.op_str}"))
        elif op.type == capstone.x86.X86_OP_IMM:
            imm = op.imm & 0xffffffff
            if imm in TARGETS:
                off, tag = TARGETS[imm]
                res[off].append((ins.address, "A", "IMM-ADDR", tag,
                                 f"{ins.mnemonic} {ins.op_str}"))
            elif imm in IMMIDX and ins.mnemonic in ("push","mov","cmp","add","sub","lea"):
                off = IMMIDX[imm]
                res[off].append((ins.address, "?", "IMM-DWIDX", f"imm 0x{imm:x} == 0x{off:x}/4",
                                 f"{ins.mnemonic} {ins.op_str}"))

print(f"swept {n_ins} instructions over .text 0x{VA0:08x}..0x{VA0+len(span):08x}")
for off in OFFS:
    hits = sorted(set(res[off]))
    print(f"=== +0x{off:x} folded/absolute : {len(hits)} hit(s) ===")
    for a, k, form, tag, txt in hits:
        print(f"  {k} {form:9s} 0x{a:08x}  {txt}    <- {tag}")

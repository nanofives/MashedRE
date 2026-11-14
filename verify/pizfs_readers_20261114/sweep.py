"""Byte-anchored absolute-address operand sweep of MASHED.exe for given VA ranges.

Usage: py -3.12 sweep.py <exe> <inventory.csv> <lo:hi> [<lo:hi> ...]
Every 4-byte little-endian value in [lo, hi) found at any byte offset of any section is a hit.
In .text each hit is decoded: every start s in hit-12..hit-1 is tried with capstone, and a decode
is kept when the instruction spans the 4 hit bytes and its disp or imm equals the value.
A decode is marked SYNC when a linear disassembly from the containing inventory function start
(or, without one, from any of 64 prior offsets) lands exactly on s.
"""
import sys, csv, struct, bisect
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_MEM, X86_OP_IMM

exe, inv = sys.argv[1], sys.argv[2]
ranges = [tuple(int(x, 16) for x in a.split(':')) for a in sys.argv[3:]]
pe = pefile.PE(exe, fast_load=True)
base = pe.OPTIONAL_HEADER.ImageBase
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True

funcs = []
with open(inv) as f:
    for r in csv.DictReader(f):
        funcs.append((int(r['rva'], 16), r['name'], int(r['body_size'] or 0)))
funcs.sort(); fstarts = [x[0] for x in funcs]

def func_of(va):
    i = bisect.bisect_right(fstarts, va) - 1
    if i < 0: return None
    s, n, sz = funcs[i]
    return (s, n, sz, va < s + sz)

def inr(v):
    return any(lo <= v < hi for lo, hi in ranges)

text = None
for sec in pe.sections:
    name = sec.Name.rstrip(b'\0').decode()
    data = sec.get_data()[:sec.Misc_VirtualSize]
    sva = base + sec.VirtualAddress
    if name == '.text': text = (sva, data)
    for off in range(len(data) - 3):
        v = struct.unpack_from('<I', data, off)[0]
        if not inr(v): continue
        hva = sva + off
        if name != '.text':
            print(f'DATA\t{name}\t{hva:08x}\tvalue={v:08x}')
            continue
        decs = []
        for s in range(off - 12, off):
            if s < 0: continue
            ins = next(md.disasm(data[s:s + 16], sva + s, 1), None)
            if ins is None or not (s < off and s + ins.size >= off + 4): continue
            match = False
            for op in ins.operands:
                if op.type == X86_OP_MEM and (op.mem.disp & 0xffffffff) == v and ins.disp_offset == off - s: match = True
                if op.type == X86_OP_IMM and (op.imm & 0xffffffff) == v and ins.imm_offset == off - s: match = True
            if match: decs.append((s, ins))
        for s, ins in decs:
            iva = sva + s
            fo = func_of(iva)
            sync = False
            if fo and fo[3]:
                p = fo[0] - sva
                for i2 in md.disasm(data[p:s + 16], fo[0]):
                    if i2.address == iva: sync = True; break
                    if i2.address > iva: break
            else:
                for back in range(1, 65):
                    p = s - back
                    if p < 0: break
                    for i2 in md.disasm(data[p:s + 16], sva + p):
                        if i2.address >= iva:
                            if i2.address == iva: sync = True
                            break
                    if sync: break
            fs = f'{fo[1]}{"" if fo[3] else "(outside body)"}' if fo else '?'
            print(f'CODE\t{iva:08x}\t{ins.mnemonic} {ins.op_str}\tvalue={v:08x}\t{fs}\t{"SYNC" if sync else "NOSYNC"}')
        if not decs:
            print(f'RAW\t.text\t{hva:08x}\tvalue={v:08x}\tno operand decode')

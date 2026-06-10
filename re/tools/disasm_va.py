#!/usr/bin/env python3
"""Disassemble a VA range from original/MASHED.exe (i386). Usage:
   py -3.12 re/tools/disasm_va.py 0x45d4d0 0x40
   py -3.12 re/tools/disasm_va.py 0x45d4d0 0x40 <other.exe>
"""
import sys, struct
import capstone

def load(path):
    d = open(path, 'rb').read()
    e = struct.unpack_from('<I', d, 0x3c)[0]
    coff = e + 4
    nsec, = struct.unpack_from('<H', d, coff + 2)
    opt = coff + 20
    sect_off = opt + struct.unpack_from('<H', d, coff + 16)[0]
    ib, = struct.unpack_from('<I', d, opt + 28)
    secs = []
    for i in range(nsec):
        so = sect_off + i * 40
        vsz, vrva, rsz, rptr = struct.unpack_from('<IIII', d, so + 8)
        secs.append((vrva, vsz, rptr, rsz))
    return d, ib, secs

def va2off(va, ib, secs):
    rva = va - ib
    for vrva, vsz, rptr, rsz in secs:
        if vrva <= rva < vrva + max(vsz, rsz):
            return rptr + (rva - vrva)
    return None

def main():
    va = int(sys.argv[1], 0)
    n = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0x40
    path = sys.argv[3] if len(sys.argv) > 3 else r'original\MASHED.exe'
    d, ib, secs = load(path)
    off = va2off(va, ib, secs)
    if off is None:
        print("VA not in any section"); return
    code = d[off:off + n]
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    for ins in md.disasm(code, va):
        b = ' '.join(f'{x:02x}' for x in ins.bytes)
        print(f"{ins.address:#010x}  {b:<24}  {ins.mnemonic} {ins.op_str}")

if __name__ == '__main__':
    main()

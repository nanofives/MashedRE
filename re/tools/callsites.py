# Find every direct CALL to a target VA in the anchored MASHED.exe.unpatched, and
# CONFIRM each hit by disassembling it rather than trusting the byte pattern.
#
# Why the confirm step is not optional: a raw `E8 <rel32>` search happily matches
# bytes that are an immediate, a displacement or the tail of another instruction
# (memory `capstone-sweep-stops-at-bad-byte`: "raw byte searches invent operands").
# A hit is reported only when capstone, started at that exact address, decodes a
# 5-byte `call` whose operand equals the target.
#
# Usage: py -3.12 re/tools/callsites.py 0x00469aa0
import struct
import sys
from pathlib import Path

import capstone

ROOT = Path(__file__).resolve().parent.parent.parent
EXE = ROOT / "original" / "MASHED.exe.unpatched"


def parse_pe(data):
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt_size = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    sec0 = pe + 24 + opt_size
    secs = []
    for i in range(nsec):
        s = sec0 + i * 40
        name = data[s:s + 8].rstrip(b"\0").decode()
        vsz, vaddr, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
        secs.append((name, vaddr, vsz, roff, rsz))
    return base, secs


def main():
    target = int(sys.argv[1], 16)
    data = EXE.read_bytes()
    base, secs = parse_pe(data)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    hits, rejected = [], 0
    for name, vaddr, vsz, roff, rsz in secs:
        if name not in ('.text', 'CODE'):
            continue
        n = min(vsz, rsz) if rsz else vsz
        blob = data[roff:roff + n]
        for i in range(len(blob) - 5):
            if blob[i] != 0xE8:
                continue
            rel, = struct.unpack_from('<i', blob, i + 1)
            va = base + vaddr + i
            if va + 5 + rel != target:
                continue
            ins = next(md.disasm(blob[i:i + 8], va, 1), None)
            if (ins and ins.mnemonic == 'call' and ins.size == 5
                    and ins.op_str == f'0x{target:x}'):
                hits.append((va, ins.bytes.hex()))
            else:
                rejected += 1
    print(f'target 0x{target:08x}  section .text  confirmed call sites {len(hits)}'
          f'  byte-pattern hits rejected by the disassembler {rejected}')
    for va, b in hits:
        print(f'  0x{va:08x}  {b}')


if __name__ == '__main__':
    main()

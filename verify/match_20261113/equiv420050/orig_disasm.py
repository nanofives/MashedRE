"""Disassemble MASHED.exe.unpatched at 0x00420050 (474 B) with addresses, for the RESULT of MATCH-EQUIV-420050-ORDER.
Usage: py -3.12 orig_disasm.py <path to MASHED.exe.unpatched> [start_hex] [count]"""
import struct
import sys

import capstone

path = sys.argv[1]
start = int(sys.argv[2], 16) if len(sys.argv) > 2 else 0x00420050
size = int(sys.argv[3]) if len(sys.argv) > 3 else 474
data = open(path, 'rb').read()
pe = struct.unpack_from('<I', data, 0x3c)[0]
nsec = struct.unpack_from('<H', data, pe + 6)[0]
optsz = struct.unpack_from('<H', data, pe + 20)[0]
base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
sec = pe + 24 + optsz
off = None
for i in range(nsec):
    s = sec + 40 * i
    vsz, va, rsz, rptr = struct.unpack_from('<IIII', data, s + 8)
    if base + va <= start < base + va + max(vsz, rsz):
        off = rptr + (start - base - va)
        break
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for ins in md.disasm(data[off:off + size], start):
    print('0x%08x  %-24s %s %s' % (ins.address, ins.bytes.hex(), ins.mnemonic, ins.op_str))

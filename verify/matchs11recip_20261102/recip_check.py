import pefile, struct, sys
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
exe = sys.argv[1]
pe = pefile.PE(exe); base = pe.OPTIONAL_HEADER.ImageBase
def rd(va, n):
    return pe.get_data(va - base, n)
def f32(va):
    b = rd(va, 4); return struct.unpack('<I', b)[0], struct.unpack('<f', b)[0]
md = Cs(CS_ARCH_X86, CS_MODE_32)
import numpy as np
r640 = np.float32(np.float64(1.0)/640.0); r480 = np.float32(np.float64(1.0)/480.0)
print("(float)(1.0/640) = %s bits %08x" % (repr(float(r640)), struct.unpack('<I', struct.pack('<f', r640))[0]))
print("(float)(1.0/480) = %s bits %08x" % (repr(float(r480)), struct.unpack('<I', struct.pack('<f', r480))[0]))
funcs = [(0x00472c60,350),(0x00472dc0,400),(0x00472f40,400),(0x004730b0,400),(0x00473540,380),(0x004736c0,380)]
for rva, n in funcs:
    print("\n== %08x ==" % rva)
    code = rd(rva, n)
    for i in md.disasm(code, rva):
        if i.mnemonic in ('fmul','fdiv','fdivr','fmulp','fdivp') or i.mnemonic == 'call':
            note = ''
            if '0x5c' in i.op_str and '[' in i.op_str:
                a = int(i.op_str.split('0x')[-1].rstrip(']'),16)
                bits, val = f32(a); note = '  ; [%08x]=%08x %r' % (a, bits, val)
            print("%08x  %s %s%s" % (i.address, i.mnemonic, i.op_str, note))
        if i.mnemonic == 'ret' and i.address > rva + 20: break

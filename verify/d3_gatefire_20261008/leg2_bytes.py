"""Leg 2 byte reads (capstone + constants) over original/MASHED.exe.unpatched. Read-only."""
import pefile, capstone, struct, sys
pe = pefile.PE(r"original/MASHED.exe.unpatched"); ib = pe.OPTIONAL_HEADER.ImageBase
def rd(va, n):
    rva = va - ib
    for s in pe.sections:
        if s.VirtualAddress <= rva < s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData):
            off = rva - s.VirtualAddress
            if off >= s.SizeOfRawData: return None
            return pe.__data__[s.PointerToRawData + off:s.PointerToRawData + off + n]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def dis(a, b):
    for i in md.disasm(rd(a, b - a), a):
        print("  %08x  %-6s %s" % (i.address, i.mnemonic, i.op_str))
def f32(a):
    b = rd(a, 4); return None if b is None else (struct.unpack('<f', b)[0], b[::-1].hex())
if __name__ == "__main__":
    which = sys.argv[1]
    if which == "dis": dis(int(sys.argv[2], 16), int(sys.argv[3], 16))
    if which == "f32":
        for a in sys.argv[2:]: print(a, f32(int(a, 16)))

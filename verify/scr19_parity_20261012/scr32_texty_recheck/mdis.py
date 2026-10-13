# dis.py <va_hex> [max_insns] [--stop-ret]  -- linear disasm of MASHED.exe.unpatched, annotating
# absolute [0x00xxxxxx] memory operands that land in initialized data with their dword / float value.
import sys, struct, re
import pefile, capstone
EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
pe = pefile.PE(EXE, fast_load=True); IB = pe.OPTIONAL_HEADER.ImageBase
def rd(va, n):
    rva = va - IB
    for s in pe.sections:
        if s.VirtualAddress <= rva < s.VirtualAddress + s.Misc_VirtualSize:
            off = rva - s.VirtualAddress
            if off + n > s.SizeOfRawData: return None   # BSS
            return pe.__data__[s.PointerToRawData + off: s.PointerToRawData + off + n]
    return None
va = int(sys.argv[1], 16); mx = int(sys.argv[2]) if len(sys.argv) > 2 else 200
stop = "--stop-ret" in sys.argv
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
code = rd(va, 0x2000)
n = 0
for i in md.disasm(code, va):
    ann = ""
    for m in re.finditer(r"0x([0-9a-f]{6,8})", i.op_str):
        a = int(m.group(1), 16)
        if IB <= a < IB + 0x600000 and ("[" in i.op_str):
            b = rd(a, 8)
            if b is None: ann += f"  ; [{a:#010x}] BSS"
            else:
                dw = struct.unpack("<I", b[:4])[0]; f = struct.unpack("<f", b[:4])[0]; d = struct.unpack("<d", b)[0]
                ann += f"  ; [{a:#010x}] dw {dw:#010x} f {f:.9g} d {d:.9g}"
    print(f"{i.address:#010x}: {i.mnemonic:7s} {i.op_str}{ann}")
    n += 1
    if n >= mx or (stop and i.mnemonic.startswith("ret")): break

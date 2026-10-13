import sys, struct, pefile, capstone
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
base = pe.OPTIONAL_HEADER.ImageBase
def rd(va, n):
    return pe.get_data(va-base, n)
start, end = int(sys.argv[1],16), int(sys.argv[2],16)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
code = rd(start, end-start)
import re
for i in md.disasm(code, start):
    s = f"{i.address:08x}  {i.mnemonic} {i.op_str}"
    for m in re.findall(r"0x5c[0-9a-f]{4}", i.op_str):
        v = int(m,16)
        f = struct.unpack('<f', rd(v,4))[0]; d = struct.unpack('<d', rd(v,8))[0]
        s += f"   ; [{m}] f={f!r} d={d!r}"
    print(s)

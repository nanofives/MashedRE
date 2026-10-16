"""Every E8 rel32 call to a target in MASHED.exe (.unpatched anchor) .text, with the 12 instructions before it.
Usage: callsites.py <target> [n_before]"""
import sys, struct
from pathlib import Path
import capstone, pefile
EXE = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
tgt = int(sys.argv[1], 16); nb = int(sys.argv[2]) if len(sys.argv) > 2 else 12
pe = pefile.PE(str(EXE)); base = pe.OPTIONAL_HEADER.ImageBase; img = pe.get_memory_mapped_image()
t = [s for s in pe.sections if s.Name.startswith(b'.text')][0]
tlo = base + t.VirtualAddress; code = bytes(img[t.VirtualAddress:t.VirtualAddress + t.Misc_VirtualSize])
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for i in range(len(code) - 5):
    if code[i] == 0xE8 and (tlo + i + 5 + struct.unpack_from('<i', code, i + 1)[0]) & 0xffffffff == tgt:
        at = tlo + i
        # disassemble a window before; pick the alignment that lands exactly on the call
        for back in range(nb * 7, 0, -1):
            ins = list(md.disasm(code[i - back:i + 5], at - back))
            if ins and ins[-1].address == at:
                print(f"--- call at 0x{at:08x}")
                for x in ins[-(nb + 1):]:
                    print(f"  0x{x.address:08x}  {x.mnemonic} {x.op_str}")
                break

# Static check for the group-H C4 controls: disassemble named exports of one or more .asi files.
# Usage: py -3.12 re/frida/c4_hud_asi_disasm.py <asi> [<asi>...] -- <export> [<export>...]
import sys, pefile, capstone
a = sys.argv[1:]; i = a.index("--"); asis, exps = a[:i], a[i+1:]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for p in asis:
    pe = pefile.PE(p, fast_load=False)
    ex = {e.name.decode(): e.address for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name}
    for n in exps:
        rva = ex[n]; data = pe.get_data(rva, 0x400); base = pe.OPTIONAL_HEADER.ImageBase
        print(f"== {p} :: {n} rva=0x{rva:x}")
        for ins in md.disasm(data, base + rva):
            print(f"  +0x{ins.address-base:06x} {ins.mnemonic} {ins.op_str}")
            if ins.mnemonic == "ret" or (ins.mnemonic == "jmp" and ins.op_str.startswith("0x") and False): break

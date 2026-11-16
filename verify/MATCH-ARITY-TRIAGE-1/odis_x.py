"""py -3.12 dis.py <start_hex> <end_hex> [<start> <end> ...]  disassemble MASHED.exe.unpatched ranges (end inclusive)"""
import sys, pefile, capstone
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
a = sys.argv[1:]
for k in range(0, len(a), 2):
    s, e = int(a[k], 16), int(a[k + 1], 16)
    for i in md.disasm(pe.get_data(s - 0x400000, e - s + 1), s):
        print(f"{i.address:#010x} {i.mnemonic} {i.op_str}")
    print("--")

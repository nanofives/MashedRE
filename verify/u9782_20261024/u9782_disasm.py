import sys, pefile, capstone
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
base = pe.OPTIONAL_HEADER.ImageBase
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for rng in sys.argv[1:]:
    a, b = (int(x, 16) for x in rng.split("-"))
    data = pe.get_memory_mapped_image()[a - base:b - base]
    print(f"--- {a:#010x}..{b:#010x}")
    for ins in md.disasm(data, a):
        print(f"{ins.address:#010x}  {ins.bytes.hex():<16} {ins.mnemonic} {ins.op_str}")

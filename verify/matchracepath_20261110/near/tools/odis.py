import sys, pefile, capstone
pe = pefile.PE(r"C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched", fast_load=True)
data = pe.get_memory_mapped_image()
va = int(sys.argv[1], 16); n = int(sys.argv[2], 0)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for i in md.disasm(data[va-0x400000: va-0x400000+n], va):
    print(f"{i.address:08x}  {i.bytes.hex():<16} {i.mnemonic} {i.op_str}")

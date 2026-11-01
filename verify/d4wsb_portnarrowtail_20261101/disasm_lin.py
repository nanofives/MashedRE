import sys, pefile, capstone
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
text = next(s for s in pe.sections if s.Name.startswith(b".text"))
base = 0x400000 + text.VirtualAddress; code = text.get_data()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
a=int(sys.argv[1],16); e=int(sys.argv[2],16)
for i in md.disasm(code[a-base:e-base], a):
    print(f"{i.address:08x}  {i.mnemonic} {i.op_str}")

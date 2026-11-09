import sys, pefile
from capstone import *
pe=pefile.PE(r'C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched'); img=pe.get_memory_mapped_image()
md=Cs(CS_ARCH_X86,CS_MODE_32)
va=int(sys.argv[1],16); n=int(sys.argv[2]) if len(sys.argv)>2 else 40
for k,i in enumerate(md.disasm(bytes(img[va-0x400000:va-0x400000+0x800]),va)):
    if k>=n: break
    print(hex(i.address),i.mnemonic,i.op_str)

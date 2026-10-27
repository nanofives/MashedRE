import sys, pefile, capstone
pe = pefile.PE(sys.argv[1]); base = pe.OPTIONAL_HEADER.ImageBase
a, b = int(sys.argv[2], 16), int(sys.argv[3], 16)
data = pe.get_memory_mapped_image()[a - base:b - base]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for i in md.disasm(data, a): print('0x%08x  %-28s %s %s' % (i.address, i.bytes.hex(), i.mnemonic, i.op_str))

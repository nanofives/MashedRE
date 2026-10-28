"""D4-WSB-C4-BROAD-TAIL: capstone listing of 0x0055ae50 and its tail callee 0x00565550 on the anchored image (MASHED.exe.unpatched of the main checkout; prints the SHA-256 prefix first). py -3.12 -I verify/wsbbroadtail_20261027/tail_disasm.py > tail_disasm.txt"""
import sys, hashlib, pefile, capstone
p = r"C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched"
b = open(p,'rb').read(); print(hashlib.sha256(b).hexdigest().upper()[:16])
pe = pefile.PE(data=b)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for a, n in [(0x0055ae50, 0x20), (0x00565550, 0x30)]:
    off = pe.get_offset_from_rva(a - 0x400000)
    for i in md.disasm(b[off:off+n], a):
        print(hex(i.address), i.bytes.hex(), i.mnemonic, i.op_str)
        if i.mnemonic in ('ret','jmp'): break
    print()

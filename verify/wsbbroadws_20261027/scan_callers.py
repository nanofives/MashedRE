# D4-WSB-C4-BROAD-WS: direct callers (E8/E9 rel32) and absolute dword references (any section) of each
# target in the ORIGINAL MASHED.exe.unpatched. Each E8/E9 hit is re-decoded with capstone to confirm it
# is a call/jmp to that target. usage: py -3.12 scan_callers.py <rva> [<rva> ...]
import sys, pefile, capstone
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
IB = pe.OPTIONAL_HEADER.ImageBase
for t in [int(a, 16) for a in sys.argv[1:]]:
    print("target 0x%08x" % t)
    for s in pe.sections:
        d = s.get_data(); va0 = IB + s.VirtualAddress; nm = s.Name.rstrip(b"\0").decode()
        for o in range(len(d) - 4):
            v = int.from_bytes(d[o:o + 4], "little")
            if v == t:
                print("  absref %s 0x%08x" % (nm, va0 + o))
            if o and d[o - 1] in (0xE8, 0xE9) and ((va0 + o + 4 + v) & 0xffffffff) == t:
                ins = next(md.disasm(d[o - 1:o + 4], va0 + o - 1), None)
                if ins and ins.mnemonic in ("call", "jmp"):
                    print("  %s %s 0x%08x" % (ins.mnemonic, nm, va0 + o - 1))

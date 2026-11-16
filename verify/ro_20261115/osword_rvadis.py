import sys, struct, capstone
p = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
b = open(p, "rb").read()
pe = struct.unpack_from("<I", b, 0x3c)[0]
nsec = struct.unpack_from("<H", b, pe + 6)[0]
opt = struct.unpack_from("<H", b, pe + 20)[0]
ib = struct.unpack_from("<I", b, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    o = pe + 24 + opt + 40 * i
    vs, va, rs, ro = struct.unpack_from("<IIII", b, o + 8)
    secs.append((va, vs, ro, rs))
def off(v):
    r = v - ib
    for va, vs, ro, rs in secs:
        if va <= r < va + max(vs, rs):
            return ro + r - va
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for arg in sys.argv[1:]:
    lo, hi = (int(x, 16) for x in arg.split("-"))
    o = off(lo)
    print(f"--- {lo:#010x}..{hi:#010x}")
    for ins in md.disasm(b[o:o + (hi - lo)], lo):
        print(f"{ins.address:#010x}  {ins.bytes.hex():<20} {ins.mnemonic} {ins.op_str}")

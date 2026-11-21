import sys, struct, capstone
p = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
d = open(p, "rb").read()
pe = struct.unpack_from("<I", d, 0x3c)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]
opt = struct.unpack_from("<H", d, pe + 20)[0]
base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
secs = []
o = pe + 24 + opt
for i in range(nsec):
    vs, va, rs, ra = struct.unpack_from("<IIII", d, o + 8)
    secs.append((va, vs, ra))
    o += 40
def off(v):
    r = v - base
    for va, vs, ra in secs:
        if va <= r < va + vs:
            return ra + r - va
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for a in sys.argv[1:]:
    va = int(a, 16); n = 0x90
    print("==", a)
    for ins in md.disasm(d[off(va):off(va) + n], va):
        print(f"  {ins.address:08x} {ins.mnemonic} {ins.op_str}")
        if ins.mnemonic in ("ret",): break

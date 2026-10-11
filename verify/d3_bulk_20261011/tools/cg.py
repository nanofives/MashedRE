# Direct call graph from MASHED.exe.unpatched via capstone linear sweep over function_inventory bodies.
# Edges: call imm32 and jmp imm32 that land on a known function entry outside the body (tail call).
import csv, struct, sys, json, capstone
ROOT = r"C:\Users\maria\Desktop\Proyectos\Mashed"
data = open(ROOT + r"\original\MASHED.exe.unpatched", "rb").read()
pe = struct.unpack_from("<I", data, 0x3C)[0]
nsec = struct.unpack_from("<H", data, pe + 6)[0]
opt = struct.unpack_from("<H", data, pe + 20)[0]
base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
secs = []
for i in range(nsec):
    s = pe + 24 + opt + i * 40
    vsz, va, rsz, roff = struct.unpack_from("<IIII", data, s + 8)
    secs.append((base + va, vsz, roff, rsz))
def read(va, n):
    for sva, vsz, roff, rsz in secs:
        if sva <= va < sva + rsz:
            o = roff + (va - sva)
            return data[o:o + min(n, sva + rsz - va)]
    return b""
funcs = {}
with open(ROOT + r"\re\analysis\function_inventory.csv") as f:
    for r in csv.DictReader(f):
        funcs[int(r["rva"], 16)] = int(r["body_size"])
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
edges = {}
for fva, size in funcs.items():
    code = read(fva, size)
    out = set()
    off = 0
    while off < len(code):
        got = False
        for ins in md.disasm(code[off:], fva + off):
            got = True
            off = ins.address - fva + ins.size
            if ins.mnemonic in ("call", "jmp") and ins.op_str.startswith("0x"):
                t = int(ins.op_str, 16)
                if ins.mnemonic == "call" or (t in funcs and not (fva <= t < fva + size)):
                    out.add(t)
        if not got:
            off += 1
        elif off < len(code):
            off += 1
    edges[fva] = sorted(out)
json.dump({"%08x" % k: ["%08x" % t for t in v] for k, v in edges.items()}, open(sys.argv[1], "w"))
print("funcs", len(funcs), "edges", sum(len(v) for v in edges.values()))

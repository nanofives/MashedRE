import struct, sys
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\re\tools")
from disasm_va import load
d, ib, secs = load(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
def off2va(o):
    for vrva, vsz, rptr, rsz in secs:
        if rptr <= o < rptr + rsz:
            return ib + vrva + (o - rptr)
    return None
for tgt in [int(x, 16) for x in sys.argv[1:]]:
    calls, ptrs = [], []
    for o in range(len(d) - 5):
        b = d[o]
        if b in (0xE8, 0xE9):
            va = off2va(o)
            if va is None:
                continue
            rel, = struct.unpack_from("<i", d, o + 1)
            if va + 5 + rel == tgt:
                calls.append(("call" if b == 0xE8 else "jmp", hex(va)))
    p = struct.pack("<I", tgt)
    i = d.find(p)
    while i >= 0:
        ptrs.append(hex(off2va(i) or 0))
        i = d.find(p, i + 1)
    print(hex(tgt), "calls", calls, "ptrs", ptrs)

import sys, struct
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\re\tools")
from disasm_va import load, va2off
d, ib, secs = load(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
for a in sys.argv[1:]:
    n = 1
    if ':' in a:
        a, n = a.split(':'); n = int(n)
    va = int(a, 16)
    vals = []
    for k in range(n):
        o = va2off(va + 4*k, ib, secs)
        if o is None or o + 4 > len(d):
            vals.append('BSS'); continue
        u, = struct.unpack_from('<I', d, o); f, = struct.unpack_from('<f', d, o); i, = struct.unpack_from('<i', d, o)
        vals.append(f"{u:08x}={f!r}/{i}")
    print(f"0x{va:08x}: " + "  ".join(vals))

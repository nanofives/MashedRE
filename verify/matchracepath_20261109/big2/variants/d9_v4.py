import re
src = open("re/match/s11/00469df0.c").read()
FN0 = src[src.index("static __inline float Side_00469df0"):src.index("// MATCH: 0x00469df0 FUN_00469df0")]
pat = re.compile(r"        if \(Side_00469df0\(x, y, z, &P0, &n0\) < DAT_005d757c &&\n.*?\n.*?\n.*?\) \{\n(.*?)        \}\n", re.S)
def nested(dot):
    def rep(m):
        body = m.group(1)
        out = ""; ind = "        "
        for i, (P, N) in enumerate((("P0","n0"),("P1","n1"),("P3","n2"),("P2","n3"))):
            out += f"{ind}dx = x - {P}.x;\n{ind}dy = y - {P}.y;\n{ind}dz = z - {P}.z;\n{ind}if ({dot % {'n': N}} < DAT_005d757c) {{\n"
            ind += "    "
        out += "".join(ind[4:] + "    " + l.lstrip() + "\n" for l in body.rstrip("\n").split("\n"))
        for i in range(4):
            ind = ind[:-4]
            out += f"{ind}}}\n"
        return out
    t = pat.sub(rep, src)
    t = t.replace(FN0, "").replace("    float nx, ny, nz, ex, ey, ez;\n", "    float nx, ny, nz, ex, ey, ez, dx, dy, dz;\n")
    return t
V = []
for name, dot in (("n1", "dx * %(n)s.x + dy * %(n)s.y + dz * %(n)s.z"), ("n2", "dz * %(n)s.z + dy * %(n)s.y + dx * %(n)s.x"), ("n3", "dx * %(n)s.x + dy * %(n)s.y + %(n)s.z * dz")):
    V.append((name, [(src, nested(dot))]))

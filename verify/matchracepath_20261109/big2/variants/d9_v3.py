import re
src = open("re/match/s11/00469df0.c").read()
FN0 = src[src.index("static __inline float Side_00469df0"):src.index("// MATCH: 0x00469df0 FUN_00469df0")]
def mk(ret, sig="float dx, float dy, float dz, const V3_00469df0 *n"):
    fn = "static __inline float Dot_00469df0(" + sig + ")\n{\n    return " + ret + ";\n}\n\n"
    reps = [(FN0, fn)]
    for P, N in (("P0","n0"),("P1","n1"),("P3","n2"),("P2","n3")):
        reps.append((f"Side_00469df0(x, y, z, &{P}, &{N})", f"Dot_00469df0(x - {P}.x, y - {P}.y, z - {P}.z, &{N})"))
    return reps
def expand(reps):
    # each Side call appears 4 times: replace all occurrences
    out = []
    for o, n in reps:
        out.extend([(o, n)] * (src.count(o)))
    return out
V = [("d1", expand(mk("dx * n->x + dy * n->y + dz * n->z"))),
     ("d2", expand(mk("n->z * dz + n->y * dy + n->x * dx"))),
     ("d3", expand(mk("dx * n->x + dy * n->y + n->z * dz"))),
     ("d4", expand(mk("n.x * dx + n.y * dy + n.z * dz", "float dx, float dy, float dz, V3_00469df0 n")) )]
V[3] = ("d4", [(a, b.replace(", &n", ", n")) for a, b in V[3][1]])

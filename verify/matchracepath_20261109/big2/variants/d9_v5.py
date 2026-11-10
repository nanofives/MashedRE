src = open("re/match/s11/00469df0.c").read()
FN0 = src[src.index("static __inline float Side_00469df0"):src.index("// MATCH: 0x00469df0 FUN_00469df0")]
SUB = "static __inline V3_00469df0 Sub_00469df0(float x, float y, float z, const V3_00469df0 *p)\n{\n    V3_00469df0 r;\n    r.x = x - p->x;\n    r.y = y - p->y;\n    r.z = z - p->z;\n    return r;\n}\n"
def mk(ret, byval=True):
    if byval:
        fn = SUB + "static __inline float Dot_00469df0(const V3_00469df0 *n, V3_00469df0 d)\n{\n    return " + ret + ";\n}\n\n"
    else:
        fn = "static __inline float Dot_00469df0(const V3_00469df0 *n, const V3_00469df0 *d)\n{\n    return " + ret.replace("d.", "d->") + ";\n}\n\n"
    reps = [(FN0, fn)]
    for P, N in (("P0","n0"),("P1","n1"),("P3","n2"),("P2","n3")):
        o = f"Side_00469df0(x, y, z, &{P}, &{N})"
        n = f"Dot_00469df0(&{N}, Sub_00469df0(x, y, z, &{P}))" if byval else f"(d.x = x - {P}.x, d.y = y - {P}.y, d.z = z - {P}.z, Dot_00469df0(&{N}, &d))"
        reps += [(o, n)] * src.count(o)
    if not byval:
        reps.append(("    V3_00469df0 n3;\n", "    V3_00469df0 n3, d;\n"))
    return reps
V = [("s1", mk("n->z * d.z + d.y * n->y + d.x * n->x")), ("s2", mk("d.x * n->x + d.y * n->y + n->z * d.z")), ("s3", mk("n->z * d.z + d.y * n->y + d.x * n->x", False))]

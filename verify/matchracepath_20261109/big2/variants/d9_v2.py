O = open("verify/matchracepath_20261109/big2/variants/loop_old.txt").read()
N = open("verify/matchracepath_20261109/big2/variants/loop_inl.txt").read()
DECL = [("    float n3x, n3y, n3z;\n", "    V3_00469df0 n3;\n    float nx, ny, nz, ex, ey, ez;\n")]
def inl(ret):
    return [("// MATCH: 0x00469df0 FUN_00469df0\n", "static __inline float Side_00469df0(float x, float y, float z, const V3_00469df0 *p, const V3_00469df0 *n)\n{\n    float dx = x - p->x;\n    float dy = y - p->y;\n    float dz = z - p->z;\n    return " + ret + ";\n}\n\n// MATCH: 0x00469df0 FUN_00469df0\n")]
B = [(O, N)] + DECL
V = [("i1", B + inl("dx * n->x + dy * n->y + n->z * dz")),
     ("i2", B + inl("n->z * dz + dy * n->y + dx * n->x")),
     ("i3", B + inl("dx * n->x + (n->z * dz + dy * n->y)")),
     ("i4", B + inl("dx * n->x + (dy * n->y + n->z * dz)"))]

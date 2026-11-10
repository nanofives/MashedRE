import re
src = open("re/match/s11/00469df0.c").read()
def pt(name, decl=True):
    t = src
    for i in range(4):
        t = t.replace(f"        x = o->q[{i}].x;\n        y = o->q[{i}].y;\n        z = o->q[{i}].z;\n", f"        {name}.x = o->q[{i}].x;\n        {name}.y = o->q[{i}].y;\n        {name}.z = o->q[{i}].z;\n")
    t = t.replace("Sub_00469df0(x, y, z, &", f"Sub_00469df0(&{name}, &")
    t = t.replace("Sub_00469df0(float x, float y, float z, const V3_00469df0 *p)", "Sub_00469df0(const V3_00469df0 *a, const V3_00469df0 *p)")
    t = t.replace("    r.x = x - p->x;\n    r.y = y - p->y;\n    r.z = z - p->z;", "    r.x = a->x - p->x;\n    r.y = a->y - p->y;\n    r.z = a->z - p->z;")
    t = t.replace("            sx = sx + x;\n            cnt++;\n            sy = y + sy;", f"            sx = sx + {name}.x;\n            cnt++;\n            sy = {name}.y + sy;")
    t = t.replace("            sz = z + sz;", f"            sz = {name}.z + sz;")
    if decl:
        t = t.replace("    float x, y, z;\n", f"    V3_00469df0 {name};\n")
    else:
        t = t.replace("    float x, y, z;\n", "")
    return t
V = [("pt", [(src, pt("pt"))]), ("ptda", [(src, pt("da", False))]), ("ptdb", [(src, pt("db", False))])]

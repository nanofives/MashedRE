B1 = "(0, m->up.x) * v.x + v.y * m->up.y + v.z * m->up.z"
B3 = "(0, v.x * m->right.x + v.y * m->right.y) + (0, m->right.z) * v.z"
B2 = "v.x * zaxis.x + (0, v.y * zaxis.y) + v.z * zaxis.z"
f1 = {
 "sw": "m->up.x * v.x + v.y * m->up.y + v.z * m->up.z",
 "orig": "v.x * m->up.x + v.y * m->up.y + v.z * m->up.z",
 "cz": "(0, m->up.x * v.x) + v.y * m->up.y + v.z * m->up.z",
 "dbl": "(float)((double)m->up.x * v.x) + v.y * m->up.y + v.z * m->up.z",
 "cc": "(0, (0, m->up.x) * v.x + v.y * m->up.y) + v.z * m->up.z",
 "cv": "(0, m->up.x) * (0, v.x) + v.y * m->up.y + v.z * m->up.z",
}
V = [("b1" + k, [(B1, s)]) for k, s in f1.items()]
V += [("cmp_c", [("if (!(v.y < 0.0f)) d = -d;", "if (!((0, v.y) < 0.0f)) d = -d;")])]
V += [("dvar", [("d = (FUN_004a3384(" + B1 + ") * 57.29578f);", "d = FUN_004a3384(" + B1 + ");\n  d = d * 57.29578f;")])]

S1 = "  v.y = m->up.y;\n  v.z = m->up.z;\n  v.x = 0.0f;\n  FUN_004c39b0(&v, &v);\n"
V = [
 ("cy", [(S1, "  v.y = (0, m->up.y);\n  v.z = m->up.z;\n  v.x = 0.0f;\n  FUN_004c39b0(&v, &v);\n")]),
 ("cz", [(S1, "  v.y = m->up.y;\n  v.z = (0, m->up.z);\n  v.x = 0.0f;\n  FUN_004c39b0(&v, &v);\n")]),
 ("cyz", [(S1, "  v.y = (0, m->up.y);\n  v.z = (0, m->up.z);\n  v.x = 0.0f;\n  FUN_004c39b0(&v, &v);\n")]),
 ("argx", [(S1, "  v.y = m->up.y;\n  v.z = m->up.z;\n  FUN_004c39b0(&v, (v.x = 0.0f, &v));\n")]),
 ("argx1", [(S1, "  v.y = m->up.y;\n  v.z = m->up.z;\n  FUN_004c39b0((v.x = 0.0f, &v), &v);\n")]),
 ("argz", [(S1, "  v.y = m->up.y;\n  v.x = 0.0f;\n  FUN_004c39b0(&v, (v.z = m->up.z, &v));\n")]),
 ("argxz", [(S1, "  v.y = m->up.y;\n  FUN_004c39b0(&v, (v.x = 0.0f, v.z = m->up.z, &v));\n")]),
 ("argxz1", [(S1, "  v.y = m->up.y;\n  FUN_004c39b0((v.x = 0.0f, v.z = m->up.z, &v), &v);\n")]),
 ("tz", [(S1, "  { float t = m->up.z;\n  v.y = m->up.y;\n  v.x = 0.0f;\n  v.z = t; }\n  FUN_004c39b0(&v, &v);\n")]),
 ("pv", [(S1, "  { RwV3d *pv = &v;\n  pv->y = m->up.y;\n  pv->z = m->up.z;\n  pv->x = 0.0f;\n  FUN_004c39b0(pv, pv); }\n")]),
]

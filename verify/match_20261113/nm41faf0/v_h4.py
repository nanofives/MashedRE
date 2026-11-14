CALL = "FUN_004c4d20(&b, &axis, 90.0f, 1);"
DX = "d.x = a.pos.x - b.pos.x;"
INIT = "  axis.x = 1.0f;\n  axis.y = 0.0f;\n  axis.z = 0.0f;\n"
VARIANTS = [
 ("call-dbl90", [(CALL, "FUN_004c4d20(&b, &axis, (float)90.0, 1);")]),
 ("ax-after-d", [(INIT, ""), (CALL, "axis.x = 1.0f; axis.y = 0.0f; axis.z = 0.0f;\n    " + CALL)]),
 ("dx-paren", [(DX, "d.x = (a.pos.x - b.pos.x);")]),
 ("d-fn-order", [("    d.x = a.pos.x - b.pos.x;\n", ""), ("FUN_004c52f0(&b, FUN_004c0ed0(*(char **)(r->objB + 4)), 0);\n", "FUN_004c52f0(&b, FUN_004c0ed0(*(char **)(r->objB + 4)), 0);\n    d.x = a.pos.x - b.pos.x;\n")]),
 ("dx-dblb", [(DX, "d.x = (float)(a.pos.x - (double)b.pos.x);")]),
 ("dx-ldbl", [(DX, "d.x = (float)((long double)a.pos.x - b.pos.x);")]),
 ("dy-ldbl", [("d.y = a.pos.y - b.pos.y;", "d.y = (float)((long double)a.pos.y - (long double)b.pos.y);")]),
 ("dy-cast-f", [("d.y = a.pos.y - b.pos.y;", "d.y = (float)(a.pos.y - b.pos.y);")]),
 ("dz-plain-dy-dbl", [("d.z = (float)((double)a.pos.z - b.pos.z);", "d.z = a.pos.z - b.pos.z;"), ("d.y = a.pos.y - b.pos.y;", "d.y = (float)((double)a.pos.y - b.pos.y);")]),
 ("ret-ign", [(CALL, "(void)" + CALL)]),
]

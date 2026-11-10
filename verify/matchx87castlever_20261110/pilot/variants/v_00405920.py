C = "    if (d.z * d.z + (d.x * d.x + d.y * d.y) > 1e-05f)"
V = [
 ("comma_x", [(C, "    if (d.z * d.z + ((0, d.x) * d.x + d.y * d.y) > 1e-05f)")]),
 ("namedf", [(C, "    { float sq = d.z * d.z + (d.x * d.x + d.y * d.y); if (sq > 1e-05f)"), ("        FUN_004c39b0(&d, &d);\n", "        FUN_004c39b0(&d, &d); }\n")]),
 ("dblsum", [(C, "    if ((float)(d.z * d.z + (double)(d.x * d.x + d.y * d.y)) > 1e-05f)")]),
 ("namedd", [(C, "    { double sq = d.z * d.z + (d.x * d.x + d.y * d.y); if (sq > 1e-05f)"), ("        FUN_004c39b0(&d, &d);\n", "        FUN_004c39b0(&d, &d); }\n")]),
 ("lenlocal", [("    len = FUN_004c3ac0(&d);\n", "    len = FUN_004c3ac0(&d);\n    { float dx = d.x;\n"), (C, "    if (d.z * d.z + (dx * dx + d.y * d.y) > 1e-05f)"), ("        FUN_004c39b0(&d, &d);\n", "        FUN_004c39b0(&d, &d); }\n")]),
 ("ptr", [(C, "    if (d.z * d.z + ((&d)->x * (&d)->x + d.y * d.y) > 1e-05f)")]),
]

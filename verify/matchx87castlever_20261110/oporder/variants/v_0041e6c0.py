A = "    scale.x *= 1.5f;\n    scale.y *= 1.5f;\n    scale.z *= 1.5f;\n"
B = "    scale.x *= 1.125f;\n    scale.y *= 1.125f;\n    scale.z *= 1.125f;\n"
F = "  FUN_004c1480(DAT_0063d610.frame, &m, 0);\n"
V = [
 ("assign", [(A, A.replace("scale.x *= ", "scale.x = scale.x * ").replace("scale.y *= ", "scale.y = scale.y * ").replace("scale.z *= ", "scale.z = scale.z * ")),
             (B, B.replace("scale.x *= ", "scale.x = scale.x * ").replace("scale.y *= ", "scale.y = scale.y * ").replace("scale.z *= ", "scale.z = scale.z * "))]),
 ("commaZ", [(A, A.replace("scale.z *= 1.5f;", "scale.z = (0, scale.z) * 1.5f;")), (B, B.replace("scale.z *= 1.125f;", "scale.z = (0, scale.z) * 1.125f;"))]),
 ("wZ", [(A, A.replace("scale.z *= 1.5f;", "{ double t = scale.z * 1.5f; scale.z = (float)t; }")), (B, B.replace("scale.z *= 1.125f;", "{ double t = scale.z * 1.125f; scale.z = (float)t; }"))]),
 ("ptrframe", [(F, "  { void *fr = DAT_0063d610.frame; FUN_004c1480(fr, &m, 0); }\n")]),
 ("scale_ptr", [(A, "    { RwV3d *s = &scale; s->x *= 1.5f; s->y *= 1.5f; s->z *= 1.5f; }\n"), (B, "    { RwV3d *s = &scale; s->x *= 1.125f; s->y *= 1.125f; s->z *= 1.125f; }\n")]),
 ("k_local", [("  RwMatrix m;\n", "  RwMatrix m;\n  float k;\n"), (A, "    k = 1.5f;\n"), (B, "    k = 1.125f;\n"), (F, "  scale.x *= k;\n  scale.y *= k;\n  scale.z *= k;\n" + F)]),
]

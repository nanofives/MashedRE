B = "  s.x = s.x * 1.25f;\n  s.y = s.y * 1.25f;\n  s.z = s.z * 1.25f;\n"
V = [
 ("compound", [(B, "  s.x *= 1.25f;\n  s.y *= 1.25f;\n  s.z *= 1.25f;\n")]),
 ("constfirst", [(B, "  s.x = 1.25f * s.x;\n  s.y = 1.25f * s.y;\n  s.z = 1.25f * s.z;\n")]),
 ("comma_y", [(B, "  s.x = s.x * 1.25f;\n  s.y = (0, s.y) * 1.25f;\n  s.z = s.z * 1.25f;\n")]),
 ("dbl_x", [(B, "  s.x = (float)(s.x * (double)1.25f);\n  s.y = s.y * 1.25f;\n  s.z = s.z * 1.25f;\n")]),
 ("ptr", [(B, "  { MatchV3d0041c180 *p = &s; p->x = p->x * 1.25f; p->y = p->y * 1.25f; p->z = p->z * 1.25f; }\n")]),
 ("glob_late", [("  FUN_004c13e0(DAT_0063cdb8, &s, 1);", "  FUN_004c13e0((0, DAT_0063cdb8), &s, 1);")]),
 ("local_k", [(B, "  { float k = 1.25f; s.x = s.x * k; s.y = s.y * k; s.z = s.z * k; }\n")]),
]

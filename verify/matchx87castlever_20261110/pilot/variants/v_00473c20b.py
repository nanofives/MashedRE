AB = "            RwV2d a, b;\n"
V = [
 ("ba", [(AB, "            RwV2d b, a;\n")]),
 ("sep", [(AB, "            RwV2d a;\n            RwV2d b;\n")]),
 ("sepba", [(AB, "            RwV2d b;\n            RwV2d a;\n")]),
 ("arr", [(AB, "            RwV2d ab[2];\n"), ("FUN_00493f80(&a, &b);", "FUN_00493f80(&ab[0], &ab[1]);"), ("uu = a.x / b.x;", "uu = ab[0].x / ab[1].x;"), ("vv = a.y / b.y;", "vv = ab[0].y / ab[1].y;")]),
 ("vvfirst", [("            uu = a.x / b.x;\n            vv = a.y / b.y;\n", "            vv = a.y / b.y;\n            uu = a.x / b.x;\n")]),
 ("y2first", [("        x2 = p.x + s.x;\n        y2 = p.y + s.y;\n", "        y2 = p.y + s.y;\n        x2 = p.x + s.x;\n")]),
]

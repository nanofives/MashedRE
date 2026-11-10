K = ("    t.x += FUN_00472650(-0.2f, 0.2f);\n", "    t.x = (0, t.x) + (FUN_00472650(-0.2f, 0.2f));\n")
D = ("    w.y = (FUN_004173a0() * (1.0f/14.0f) + 0.05f) * w.y;\n", "    w.y = (float)((FUN_004173a0() * (1.0f/14.0f) + 0.05f) * (double)(w.y));\n")
L1 = ("    if (w.y < 0.3f && w.y > DAT_005d757c)\n", "    if (w.y < 0.3f && w.y > 0.0f)\n")
L2 = ("    else if (w.y > -0.3f && w.y < DAT_005d757c)\n", "    else if (w.y > -0.3f && w.y < 0.0f)\n")
V = [
 ("K", [K]), ("D", [D]), ("KD", [K, D]), ("lit", [L1, L2]), ("lit_KD", [L1, L2, K, D]),
 ("dy_named", [("        d.y = d.y * 0.023f;\n", "        { float dy = d.y; d.y = dy * 0.023f; }\n")]),
 ("dy_comma", [("        d.y = d.y * 0.023f;\n", "        d.y = (0, d.y) * 0.023f;\n")]),
 ("dy_W", [("        d.y = d.y * 0.023f;\n", "        { double t = d.y * 0.023f; d.y = (float)t; }\n")]),
]
W = ("        d.y = d.y * 0.023f;\n", "        { double t = d.y * 0.023f; d.y = (float)t; }\n")
V = [("KDW", [K, D, W])]

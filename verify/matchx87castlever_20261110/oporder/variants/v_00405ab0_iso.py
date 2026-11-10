A = "        { double t = d.y * 0.023f; d.y = (float)t; }\n"
B = "    t.x = (0, t.x) + (FUN_00472650(-0.2f, 0.2f));\n"
C = "    w.y = (float)((FUN_004173a0() * (1.0f/14.0f) + 0.05f) * (double)(w.y));\n"
V = [
 ("base", []),
 ("cmtA", [(A, A[:-1] + "  /* c */\n")]),
 ("parB", [(B, "    t.x = (0, t.x) + FUN_00472650(-0.2f, 0.2f);\n")]),
 ("parC", [(C, "    w.y = (float)((FUN_004173a0() * (1.0f/14.0f) + 0.05f) * (double)w.y);\n")]),
 ("cmtB", [(B, B[:-1] + "  /* c */\n")]),
]

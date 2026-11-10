OLD = """  DAT_0068f640[i].obj->freq = 22050.0 - sin(DAT_0069059c) * 500.0;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, DAT_0068f640[i].obj->freq);
"""
G = "MGLOBAL(float, DAT_0069059c, 0x0069059c);"
GS = "static float DAT_0069059c;"
F = "22050.0 - sin(DAT_0069059c) * 500.0"
O = "DAT_0068f640[i].obj"
V = [
 ("x1s", [(G, GS), (OLD, f"  if (({O}->freq = {F}, {O}->h) != 0)\n    FUN_005a6dc0({O}->h, 4, 1, {F});\n")]),
 ("x2s", [(G, GS), (OLD, f"  {O}->freq = {F}, {O}->h != 0 ? FUN_005a6dc0({O}->h, 4, 1, {F}) : (void)0;\n")]),
 ("x3s", [(G, GS), (OLD, f"  {O}->freq = {F};\n  {O}->h != 0 ? FUN_005a6dc0({O}->h, 4, 1, {F}) : (void)0;\n")]),
 ("x2l", [(OLD, f"  {{ float f; {O}->freq = f = {F}, {O}->h != 0 ? FUN_005a6dc0({O}->h, 4, 1, f) : (void)0; }}\n")]),
 ("x4l", [(OLD, f"  {{ float f; {O}->freq = f = {F}; {O}->h != 0 && (FUN_005a6dc0({O}->h, 4, 1, f), 1); }}\n")]),
 ("x5l", [(OLD, f"  {{ float f; if ({O}->freq = f = {F}, {O}->h != 0) FUN_005a6dc0({O}->h, 4, 1, f); }}\n")]),
]

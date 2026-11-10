OLD = """  DAT_0068f640[i].obj->freq = 22050.0 - sin(DAT_0069059c) * 500.0;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, DAT_0068f640[i].obj->freq);
"""
G = "MGLOBAL(float, DAT_0069059c, 0x0069059c);"
GS = "static float DAT_0069059c;"
V = [
 ("st_mac", [(G, GS), (OLD, "  DAT_0068f640[i].obj->freq = 22050.0 - sin(DAT_0069059c) * 500.0;\n  if (DAT_0068f640[i].obj->h != 0)\n    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, 22050.0 - sin(DAT_0069059c) * 500.0);\n")]),
 ("st_mac_f", [(G, GS), (OLD, "  DAT_0068f640[i].obj->freq = 22050.0f - sin(DAT_0069059c) * 500.0f;\n  if (DAT_0068f640[i].obj->h != 0)\n    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, 22050.0f - sin(DAT_0069059c) * 500.0f);\n")]),
 ("st_mac_ff", [(G, GS), (OLD, "  DAT_0068f640[i].obj->freq = (float)(22050.0 - sin(DAT_0069059c) * 500.0);\n  if (DAT_0068f640[i].obj->h != 0)\n    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, (float)(22050.0 - sin(DAT_0069059c) * 500.0));\n")]),
 ("ptr_mac", [(OLD, "  { Obj_45efe0 *o = DAT_0068f640[i].obj;\n  o->freq = 22050.0 - sin(DAT_0069059c) * 500.0;\n  if (DAT_0068f640[i].obj->h != 0)\n    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, 22050.0 - sin(DAT_0069059c) * 500.0); }\n")]),
 ("cond", [(OLD, "  DAT_0068f640[i].obj->freq = 22050.0 - sin(DAT_0069059c) * 500.0;\n  if (DAT_0068f640[i].obj->h != 0)\n    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, 22050.0 - sin(DAT_0069059c) * 500.0);\n")]),
]

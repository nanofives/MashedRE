T = "    t = (float)(FUN_004a3384(FUN_004726f0(DAT_00614708, &o->dir)) * 57.29577791868205);\n    if (FUN_004726f0(DAT_006146f0, &o->dir) < 0.0f)\n        t = t * -1.0f;\n    b->f48 = t;\n"
V = [
 ("w", [(T, "    { double tw = FUN_004a3384(FUN_004726f0(DAT_00614708, &o->dir)) * 57.29577791868205; t = (float)tw; }\n    if (FUN_004726f0(DAT_006146f0, &o->dir) < 0.0f)\n        t = t * -1.0f;\n    b->f48 = t;\n")]),
 ("neg", [(T, T.replace("t = t * -1.0f;", "t = -t;"))]),
 ("store_b", [(T, "    b->f48 = (float)(FUN_004a3384(FUN_004726f0(DAT_00614708, &o->dir)) * 57.29577791868205);\n    if (FUN_004726f0(DAT_006146f0, &o->dir) < 0.0f)\n        b->f48 = b->f48 * -1.0f;\n")]),
 ("ifelse", [(T, "    t = (float)(FUN_004a3384(FUN_004726f0(DAT_00614708, &o->dir)) * 57.29577791868205);\n    if (FUN_004726f0(DAT_006146f0, &o->dir) < 0.0f)\n        b->f48 = t * -1.0f;\n    else\n        b->f48 = t;\n")]),
 ("cmp_first", [(T, "    { float c = FUN_004726f0(DAT_006146f0, &o->dir); t = (float)(FUN_004a3384(FUN_004726f0(DAT_00614708, &o->dir)) * 57.29577791868205);\n    if (c < 0.0f)\n        t = t * -1.0f; }\n    b->f48 = t;\n")]),
 ("cmp_rev", [(T, T.replace("FUN_004726f0(DAT_006146f0, &o->dir) < 0.0f", "0.0f > FUN_004726f0(DAT_006146f0, &o->dir)"))]),
]

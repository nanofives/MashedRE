Y = "  param_1->d1.y = param_3->y - param_2->y;\n"
IF = "  if (param_1->d0.x == DAT_005d757c && param_1->d0.y == DAT_005d757c &&\n      param_1->d1.x == DAT_005d757c && param_1->d1.y == DAT_005d757c) {"
IFL = "  if (param_1->d0.x == 0.0f && param_1->d0.y == 0.0f &&\n      param_1->d1.x == 0.0f && param_1->d1.y == 0.0f) {"
V = [
 ("lit", [(IF, IFL)]),
 ("tmpy", [(Y, "  { float t = param_3->y - param_2->y; param_1->d1.y = t; }\n")]),
 ("dbly", [(Y, "  param_1->d1.y = (float)(param_3->y - (double)param_2->y);\n")]),
 ("wy", [(Y, "  { double t = param_3->y - param_2->y; param_1->d1.y = (float)t; }\n")]),
 ("comma_d0x", [(IF, IF.replace("if (param_1->d0.x ==", "if ((0, param_1->d0.x) ==", 1))]),
 ("lit_wy", [(IF, IFL), (Y, "  { double t = param_3->y - param_2->y; param_1->d1.y = (float)t; }\n")]),
 ("lit_tmpy", [(IF, IFL), (Y, "  { float t = param_3->y - param_2->y; param_1->d1.y = t; }\n")]),
]

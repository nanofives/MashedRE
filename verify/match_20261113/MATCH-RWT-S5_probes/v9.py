import pathlib, re
LV1 = "int FUN_004dcbb0" + (pathlib.Path(__file__).resolve().parent / "lv1.c").read_text().split("int FUN_004dcbb0", 1)[1]
LA = "        uint a = *(volatile uint *)&param_1;\n"
FIRST = "    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;"
DECL = "  uint uVar7;\n"


def rep(t, pairs):
    for a, b in pairs:
        assert a in t, a
        t = t.replace(a, b)
    return t


VARIANTS = {
    "lv1": LV1,
    "x1": rep(LV1, [(LA, "        a = *(volatile uint *)&param_1;\n"), (DECL, DECL + "  uint a;\n")]),
    "x2": rep(LV1, [(LA, "        a = *(volatile uint *)&param_1;\n"), (DECL, DECL + "  uint a;\n"),
                    (FIRST, "    a = param_1;\n    uVar6 = ((DAT_007d70cc[idx] + a - 1) / a) * a;")]),
    "x3": rep(LV1, [(LA, "        register uint a = *(volatile uint *)&param_1;\n")]),
    "x4": rep(LV1, [(LA, "        a = *(volatile uint *)&param_1;\n"), (DECL, "  uint a;\n" + DECL),
                    ("  idx = DAT_007d70c8;\n  uVar7 = param_1 * param_2;", "  a = param_1;\n  idx = DAT_007d70c8;\n  uVar7 = a * param_2;"),
                    (FIRST, "    uVar6 = ((DAT_007d70cc[idx] + a - 1) / a) * a;")]),
    "x5": rep(LV1, [("*param_5 = uVar6 / *(volatile uint *)&param_1;", "*param_5 = uVar6 / (0, *(volatile uint *)&param_1);")]),
    "x6": rep(LV1, [(LA + "        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;",
                     "        uint t = DAT_007d70cc[uVar4];\n        uint a = *(volatile uint *)&param_1;\n        uVar6 = ((t + a - 1) / a) * a;\n")]),
    "x7": rep(LV1, [(LA + "        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;",
                     "        uint a = *(volatile uint *)&param_1;\n        uVar6 = ((a + DAT_007d70cc[uVar4] - 1) / a) * a;\n")]),
    "x8": rep(LV1, [(LA + "        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;",
                     "        uint a = *(volatile uint *)&param_1;\n        uVar6 = a * ((DAT_007d70cc[uVar4] + a - 1) / a);\n")]),
}

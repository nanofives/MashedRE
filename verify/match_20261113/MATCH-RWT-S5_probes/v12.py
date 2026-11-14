import pathlib, re
LV1 = "int FUN_004dcbb0" + (pathlib.Path(__file__).resolve().parent / "lv1.c").read_text().split("int FUN_004dcbb0", 1)[1]


def rep(t, pairs):
    for a, b in pairs:
        assert a in t, a
        t = t.replace(a, b)
    return t


TOP = "  idx = DAT_007d70c8;\n  uVar7 = param_1 * param_2;\n"
VARIANTS = {
    "z1": rep(LV1, [(TOP, "  uVar5 = 0;\n" + TOP)]),
    "z2": rep(LV1, [(TOP, "  uVar7 = param_1 * param_2;\n  idx = DAT_007d70c8;\n")]),
    "z3": rep(LV1, [(TOP, TOP + "  uVar5 = uVar7;\n"), ("  uVar6 = 0;\n  uVar5 = uVar7;\nLAB", "  uVar6 = 0;\nLAB")]),
    "z4": rep(LV1, [(TOP, TOP + "  uVar5 = uVar7;\n  uVar6 = 0;\n"), ("  uVar6 = 0;\n  uVar5 = uVar7;\nLAB", "LAB")]),
    "z5": rep(LV1, [(TOP, TOP + "  uVar6 = 0;\n  uVar5 = uVar7;\n"), ("  uVar6 = 0;\n  uVar5 = uVar7;\nLAB", "LAB")]),
}

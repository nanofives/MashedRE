import pathlib, re
LV1 = "int FUN_004dcbb0" + (pathlib.Path(__file__).resolve().parent / "lv1.c").read_text().split("int FUN_004dcbb0", 1)[1]


def rep(t, pairs):
    for a, b in pairs:
        assert a in t, a
        t = t.replace(a, b)
    return t


F1 = ("    uVar5 = uVar6 + uVar7;\n    if (uVar5 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;",
      "    if (uVar6 + uVar7 <= DAT_007d70dc[idx]) { uVar5 = uVar6 + uVar7; goto LAB_004dcc4c; }")
L1 = ("        uVar5 = uVar6 + uVar7;\n        if (uVar5 <= DAT_007d70dc[uVar4]) {",
      "        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {\n          uVar5 = uVar6 + uVar7;")
NF = ("  uVar6 = 0;\n  uVar5 = uVar7;\nLAB_004dcc4c:", "  uVar6 = 0;\n  uVar5 = uVar6 + uVar7;\nLAB_004dcc4c:")
NF2 = ("  uVar6 = 0;\n  uVar5 = uVar7;\nLAB_004dcc4c:", "  uVar6 = 0;\n  uVar5 = uVar7 + uVar6;\nLAB_004dcc4c:")
E2 = ("    uVar5 = uVar6 + uVar7;\n    if", "    uVar5 = uVar7 + uVar6;\n    if")
VARIANTS = {
    "y1": rep(LV1, [L1]),
    "y2": rep(LV1, [F1]),
    "y3": rep(LV1, [F1, L1]),
    "y4": rep(LV1, [NF]),
    "y5": rep(LV1, [NF2]),
    "y6": rep(LV1, [("uVar5 = uVar6 + uVar7;", "uVar5 = uVar7 + uVar6;")]),
}

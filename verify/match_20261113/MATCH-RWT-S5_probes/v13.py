import pathlib, re
LV1 = "int FUN_004dcbb0" + (pathlib.Path(__file__).resolve().parent / "lv1.c").read_text().split("int FUN_004dcbb0", 1)[1]


def rep(t, pairs):
    for a, b in pairs:
        assert a in t, a
        t = t.replace(a, b)
    return t


LOOPB = """        uint a = *(volatile uint *)&param_1;
        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;
        uVar5 = uVar6 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
          idx = uVar4;"""
DEC = "  uint uVar7;\n"
K = {
    "k1": """        uint a = *(volatile uint *)&param_1;
        o2 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;
        e2 = o2 + uVar7;
        if (e2 <= DAT_007d70dc[uVar4]) {
          uVar6 = o2;
          uVar5 = e2;
          idx = uVar4;""",
    "k2": """        uint a = *(volatile uint *)&param_1;
        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;
        e2 = uVar6 + uVar7;
        if (e2 <= DAT_007d70dc[uVar4]) {
          uVar5 = e2;
          idx = uVar4;""",
    "k3": """        uint a = *(volatile uint *)&param_1;
        o2 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;
        uVar5 = o2 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
          uVar6 = o2;
          idx = uVar4;""",
}
VARIANTS = {k: rep(LV1, [(LOOPB, v), (DEC, DEC + "  uint o2;\n  uint e2;\n")]) for k, v in K.items()}
FIRST = """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;"""
VARIANTS["k4"] = rep(LV1, [(FIRST, """    o2 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    e2 = o2 + uVar7;
    uVar6 = o2;
    uVar5 = e2;
    if (e2 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;"""), (DEC, DEC + "  uint o2;\n  uint e2;\n")])
VARIANTS["k5"] = rep(LV1, [(FIRST, """    e2 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1 + uVar7;
    uVar5 = e2;
    uVar6 = e2 - uVar7;
    if (e2 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;"""), (DEC, DEC + "  uint o2;\n  uint e2;\n")])

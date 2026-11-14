import pathlib, re
LV1 = "int FUN_004dcbb0" + (pathlib.Path(__file__).resolve().parent / "lv1.c").read_text().split("int FUN_004dcbb0", 1)[1]


def rep(t, pairs):
    for a, b in pairs:
        assert a in t, a
        t = t.replace(a, b)
    return t


L1 = ("        uVar5 = uVar6 + uVar7;\n        if (uVar5 <= DAT_007d70dc[uVar4]) {",
      "        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {\n          uVar5 = uVar6 + uVar7;")
L1b = ("        uVar5 = uVar6 + uVar7;\n        if (uVar5 <= DAT_007d70dc[uVar4]) {\n          idx = uVar4;\n          DAT_007d70c8 = idx;",
       "        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {\n          idx = uVar4;\n          DAT_007d70c8 = idx;\n          uVar5 = uVar6 + uVar7;")
INNER = """    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == DAT_007d70ec[idx]) {"""
INNER2 = """    piVar2 = DAT_007d70ec[idx];
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == piVar2) {"""
INNER3 = """    pn = DAT_007d70c0;
    piVar2 = DAT_007d70ec[idx];
    for (; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == piVar2) {"""
VARIANTS = {
    "y1": rep(LV1, [L1]),
    "y1b": rep(LV1, [L1b]),
    "y1i2": rep(LV1, [L1, (INNER, INNER2)]),
    "y1i3": rep(LV1, [L1, (INNER, INNER3)]),
    "y1bi2": rep(LV1, [L1b, (INNER, INNER2)]),
    "i2": rep(LV1, [(INNER, INNER2)]),
}

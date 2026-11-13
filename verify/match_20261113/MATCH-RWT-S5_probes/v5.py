import itertools, pathlib
_src = (pathlib.Path(__file__).resolve().parent / "s3_004dcbb0.c").read_text()
BASE = "int FUN_004dcbb0" + _src.split("int FUN_004dcbb0", 1)[1]
D = "*param_5 = uVar6 / param_1;"
TV = (D, "*param_5 = uVar6 / *(volatile uint *)&param_1;")
LOOP = "        uVar6 = ((DAT_007d70cc[uVar4] + param_1 - 1) / param_1) * param_1;"
LV1 = (LOOP, "        uint a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;")
DECLS = "  int *piVar2;\n  int iVar3;\n  int *pn;\n  uint idx;\n  uint uVar4;\n  uint uVar5;\n  uint uVar6;\n  uint uVar7;\n"
INNER = """    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == DAT_007d70ec[idx]) {"""
INNER2 = """    piVar2 = DAT_007d70ec[idx];
    for (pn = DAT_007d70c0; pn != 0; pn = (int *)pn[4]) {
      if ((int *)pn[2] == piVar2) {"""
FOUND = """          idx = uVar4;
          DAT_007d70c8 = idx;
          if (uVar4 < 4) goto LAB_004dcc4c;"""
FOUND2 = """          idx = uVar4;
          DAT_007d70c8 = idx;
          if (idx < 4) goto LAB_004dcc4c;"""
R = {"lv1": [TV, LV1]}
names = ["uVar5", "uVar6", "uVar7"]
for perm in itertools.permutations(["uVar4", "uVar5", "uVar6", "uVar7"]):
    d = "  int *piVar2;\n  int iVar3;\n  int *pn;\n  uint idx;\n" + "".join(f"  uint {n};\n" for n in perm)
    R["d_" + "".join(p[-1] for p in perm)] = [TV, LV1, (DECLS, d)]
R["inner2"] = [TV, LV1, (INNER, INNER2)]
R["found2"] = [TV, LV1, (FOUND, FOUND2)]
R["swapmul"] = [TV, LV1, ("uVar7 = param_1 * param_2;", "uVar7 = param_2 * param_1;")]
VARIANTS = {}
for k, reps in R.items():
    t = BASE
    for a, b in reps:
        assert a in t, (k, a)
        t = t.replace(a, b)
    VARIANTS[k] = t

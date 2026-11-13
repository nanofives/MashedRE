import pathlib
_src = (pathlib.Path(__file__).resolve().parent / "s3_004dcbb0.c").read_text()
BASE = "int FUN_004dcbb0" + _src.split("int FUN_004dcbb0", 1)[1]
D = "*param_5 = uVar6 / param_1;"
TV = (D, "*param_5 = uVar6 / *(volatile uint *)&param_1;")
LOOP = "        uVar6 = ((DAT_007d70cc[uVar4] + param_1 - 1) / param_1) * param_1;"
LV1 = (LOOP, "        uint a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;")
NF = "  uVar6 = 0;\n  uVar5 = uVar7;\nLAB_004dcc4c:"
R = {
    "lv1": [TV, LV1],
    "e_then": [TV, LV1, ("DAT_007d70cc[DAT_007d70c8] = uVar7;", "DAT_007d70cc[DAT_007d70c8] = uVar5;")],
    "nf_swap": [TV, LV1, (NF, "  uVar5 = uVar7;\n  uVar6 = 0;\nLAB_004dcc4c:")],
    "e_cmp": [TV, LV1, ("if (uVar7 > DAT_007d70dc[idx])", "if (uVar5 - uVar6 > DAT_007d70dc[idx])")],
    "push5": [TV, LV1, ("(piVar2,uVar6,uVar7,param_4,0x1800)", "(piVar2,uVar6,uVar5 - uVar6,param_4,0x1800)")],
    "firstloop": [TV, LV1, ("    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;", "    { uint a = *(volatile uint *)&param_1; uVar6 = ((DAT_007d70cc[idx] + a - 1) / a) * a; }")],
}
VARIANTS = {}
for k, reps in R.items():
    t = BASE
    for a, b in reps:
        assert a in t, (k, a)
        t = t.replace(a, b)
    VARIANTS[k] = t

import pathlib
_src = (pathlib.Path(__file__).resolve().parent / "s3_004dcbb0.c").read_text()
BASE = "int FUN_004dcbb0" + _src.split("int FUN_004dcbb0", 1)[1]
D = "*param_5 = uVar6 / param_1;"
TV = (D, "*param_5 = uVar6 / *(volatile uint *)&param_1;")
LOOP = "        uVar6 = ((DAT_007d70cc[uVar4] + param_1 - 1) / param_1) * param_1;"
R = {
    "tvol": [TV],
    "lv1": [TV, (LOOP, "        uint a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;")],
    "lv2": [TV, ("      if (uVar4 != idx) {\n", "      if (uVar4 != idx) {\n        uint a;\n"),
            (LOOP, "        a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;")],
    "lv3": [TV, (LOOP, "        uint t = DAT_007d70cc[uVar4];\n        uint a = *(volatile uint *)&param_1;\n        uVar6 = ((t + a - 1) / a) * a;")],
    "lv4": [TV, (LOOP, "        uVar6 = ((DAT_007d70cc[uVar4] + *(volatile uint *)&param_1 - 1) / param_1) * param_1;")],
}
VARIANTS = {}
for k, reps in R.items():
    t = BASE
    for a, b in reps:
        assert a in t, (k, a)
        t = t.replace(a, b)
    VARIANTS[k] = t

import pathlib
_src = (pathlib.Path(__file__).resolve().parent / "s3_004dcbb0.c").read_text()
BASE = "int FUN_004dcbb0" + _src.split("int FUN_004dcbb0", 1)[1]
D = "*param_5 = uVar6 / param_1;"
R = {
    "tvol": [(D, "*param_5 = uVar6 / *(volatile uint *)&param_1;")],
    "tptr": [(D, "{ uint *pa = &param_1; *param_5 = uVar6 / *pa; }")],
    "tptr2": [("int *pn;", "int *pn;\n  uint *pa;"), ("  idx = DAT_007d70c8;\n", "  pa = &param_1;\n  idx = DAT_007d70c8;\n"), (D, "*param_5 = uVar6 / *pa;")],
    "tvolall": [("param_1 - 1) / param_1) * param_1", "*(volatile uint *)&param_1 - 1) / param_1) * param_1")],
}
VARIANTS = {}
for k, reps in R.items():
    t = BASE
    for a, b in reps:
        assert a in t, (k, a)
        t = t.replace(a, b)
    VARIANTS[k] = t

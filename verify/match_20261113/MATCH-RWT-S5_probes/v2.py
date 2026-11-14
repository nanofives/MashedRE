import pathlib
_src = (pathlib.Path(__file__).resolve().parent / "s3_004dcbb0.c").read_text()
BASE = "int FUN_004dcbb0" + _src.split("int FUN_004dcbb0", 1)[1]

R = {
    "base": [],
    "nodiv": [("*param_5 = uVar6 / param_1;", "*param_5 = uVar6;")],
    "nofirst": [("    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;\n    uVar5 = uVar6 + uVar7;\n    if (uVar5 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;\n", "")],
    "loopglob": [("if (uVar4 != idx)", "if (uVar4 != DAT_007d70c8)")],
    "noloopP": [("uVar6 = ((DAT_007d70cc[uVar4] + param_1 - 1) / param_1) * param_1;", "uVar6 = DAT_007d70cc[uVar4];")],
    "nosize_mul": [("uVar7 = param_1 * param_2;", "uVar7 = param_2;")],
    "tailend2": [("DAT_007d70cc[DAT_007d70c8] = uVar7;\n    *param_5 = 0;", "DAT_007d70cc[DAT_007d70c8] = uVar5;\n    *param_5 = 0;")],
}
VARIANTS = {}
for k, reps in R.items():
    t = BASE
    for a, b in reps:
        assert a in t, (k, a)
        t = t.replace(a, b)
    VARIANTS[k] = t

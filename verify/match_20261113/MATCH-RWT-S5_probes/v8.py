import pathlib, re
_root = pathlib.Path(__file__).resolve().parents[3]
_src = (pathlib.Path(__file__).resolve().parent / "s3_004dcbb0.c").read_text()
BASE = "int FUN_004dcbb0" + _src.split("int FUN_004dcbb0", 1)[1]
LV1 = "int FUN_004dcbb0" + (pathlib.Path(__file__).resolve().parent / "lv1.c").read_text().split("int FUN_004dcbb0", 1)[1]


def p2(t, typ="int"):
    t = t.replace("uVar7 = param_1 * param_2;", "param_2 = param_1 * param_2;")
    t = t.replace("  uint uVar7;\n", "")
    t = re.sub(r"\buVar7\b", "param_2", t)
    if typ != "int":
        t = t.replace("int param_2,", typ + " param_2,")
    return t


VARIANTS = {
    "p2_base": p2(BASE),
    "p2_base_u": p2(BASE, "uint"),
    "p2_lv1": p2(LV1),
    "p2_lv1_u": p2(LV1, "uint"),
}

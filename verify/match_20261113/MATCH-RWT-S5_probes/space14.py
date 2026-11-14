"""Space 14: add refs to size (uVar7) that cost no code: recomputed param_1 * param_2 (CSE), comma forms."""
import itertools, pathlib
BASE = (pathlib.Path(__file__).resolve().parent / "best832.c").read_text()
BASE = "int FUN_004dcbb0" + BASE.split("int FUN_004dcbb0", 1)[1]
T = "  *param_3 = DAT_007d70ec[DAT_007d70c8];\n"
VARIANTS = {"sz1": BASE.replace(T, T + "  DAT_007d70dc[0] = uVar7;\n")}
SITES = {
    "then_call": ("(piVar2,0,uVar7,param_4,0x2800)", "(piVar2,0,param_1 * param_2,param_4,0x2800)"),
    "then_st": ("DAT_007d70cc[DAT_007d70c8] = uVar7;", "DAT_007d70cc[DAT_007d70c8] = param_1 * param_2;"),
    "else_call": ("(piVar2,uVar6,uVar7,param_4,0x1800)", "(piVar2,uVar6,param_1 * param_2,param_4,0x1800)"),
    "lab": ("if (uVar7 > DAT_007d70dc[idx])", "if (param_1 * param_2 > DAT_007d70dc[idx])"),
    "dcst": ("DAT_007d70dc[idx] = uVar7;", "DAT_007d70dc[idx] = param_1 * param_2;"),
    "fcall": ("FUN_004dcaa0(uVar7,", "FUN_004dcaa0(param_1 * param_2,"),
    "nf": ("  uVar5 = uVar7;\nLAB", "  uVar5 = param_1 * param_2;\nLAB"),
    "first": ("    uVar5 = uVar6 + uVar7;\n    if (uVar5 <= DAT_007d70dc[DAT", "    uVar5 = uVar6 + param_1 * param_2;\n    if (uVar5 <= DAT_007d70dc[DAT"),
}
for k, (x, y) in SITES.items():
    assert x in BASE, x
    VARIANTS["cse_" + k] = BASE.replace(x, y)
for (k1, (x1, y1)), (k2, (x2, y2)) in itertools.combinations(SITES.items(), 2):
    VARIANTS[f"cse2_{k1}_{k2}"] = BASE.replace(x1, y1).replace(x2, y2)

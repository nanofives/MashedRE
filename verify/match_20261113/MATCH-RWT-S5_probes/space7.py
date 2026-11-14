"""Space 7: lower the reload temp's priority (merge it with other variables / widen its span). Base: space6 dopatdfg."""
import pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space6.py").read_text(), ns)
BASE = ns["VARIANTS"]["dopatdfg"].replace("\n      uVar6 = ((DAT_007d70cc[uVar4] + a", "\n        uVar6 = ((DAT_007d70cc[uVar4] + a")


def rep(t, pairs):
    for x, y in pairs:
        assert x in t, x
        t = t.replace(x, y)
    return t


VARIANTS = {"base": BASE}
# A1: reload temp merged with the Lock result (one uint variable)
A1 = rep(BASE, [("  int iVar3;\n", ""), ("iVar3 = (*", "a = (*"), ("return iVar3 >= 0;", "return (int)a >= 0;")])
VARIANTS["A1"] = A1
# A3: a also carries param_1 through the first check
VARIANTS["A3"] = rep(BASE, [("    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;",
                             "    a = param_1;\n    uVar6 = ((DAT_007d70cc[idx] + a - 1) / a) * a;")])
VARIANTS["A3b"] = rep(BASE, [("  idx = DAT_007d70c8;\n  uVar7 = param_1 * param_2;",
                              "  a = param_1;\n  idx = DAT_007d70c8;\n  uVar7 = a * param_2;"),
                             ("    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;",
                              "    uVar6 = ((DAT_007d70cc[idx] + a - 1) / a) * a;")])
# A5: reload temp merged with key (pointer-sized) via a union-free cast: key holds the reload as int*
VARIANTS["A5"] = rep(BASE, [("        a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;",
                             "        key = (int *)*(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + (uint)key - 1) / (uint)key) * (uint)key;")])
# A6: reload temp merged with the tail divisor
VARIANTS["A6"] = rep(BASE, [("*param_5 = uVar6 / param_1;", "a = param_1;\n    *param_5 = uVar6 / a;")])
# A7: a merged with piVar2 (vtable object pointer in the tail)
VARIANTS["A7"] = rep(BASE, [("        a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;",
                             "        piVar2 = (int *)*(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + (uint)piVar2 - 1) / (uint)piVar2) * (uint)piVar2;")])

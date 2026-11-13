import pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space2.py").read_text().rsplit("VARIANTS = {}", 1)[0], ns)
BEST = (ns["TOPS"]["p0"] + ns["FIRST"]["fg2"] + ns["LOOPHEAD"] + ns["LOOPEND"]["l0"].replace("FOUND", ns["FOUND"]["d0"])
        + ns["MIDS"]["m0"] + ns["NF"]["n0"] + ns["INNER"]["i1"] + ns["TAIL"]["t0"] + ns["END"])
A = "        uint a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;\n"
T = "*param_5 = uVar6 / *(volatile uint *)&param_1;"
assert A in BEST and T in BEST


def rep(t, pairs):
    for x, y in pairs:
        assert x in t, x
        t = t.replace(x, y)
    return t


P2 = "(&param_2)[-1]"
VARIANTS = {
    "best": BEST,
    "q1": rep(BEST, [(A, f"        uVar6 = ((DAT_007d70cc[uVar4] + {P2} - 1) / {P2}) * {P2};\n"), (T, f"*param_5 = uVar6 / {P2};")]),
    "q2": rep(BEST, [(A, f"        uint a = {P2};\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;\n"), (T, f"*param_5 = uVar6 / {P2};")]),
    "q3": rep(BEST, [(A, f"        uVar6 = ((DAT_007d70cc[uVar4] + {P2} - 1) / {P2}) * {P2};\n")]),
    "q4": rep(BEST, [(A, f"        uVar6 = ((DAT_007d70cc[uVar4] + {P2} - 1) / {P2}) * {P2};\n"), (T, "*param_5 = uVar6 / param_1;")]),
}

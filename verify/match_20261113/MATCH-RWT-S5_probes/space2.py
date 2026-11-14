import itertools, pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space1.py").read_text().split("VARIANTS = {}")[0], ns)
PRO, LOOPHEAD, LOOPEND, FOUND, MID, NF, INNER, TAIL, END = (ns[k] for k in "PRO LOOPHEAD LOOPEND FOUND MID NF INNER TAIL END".split())
FIRST = {
    "fi": """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[idx]) goto LAB_004dcc4c;
""",
    "fg": """    uVar6 = ((DAT_007d70cc[DAT_007d70c8] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[DAT_007d70c8]) goto LAB_004dcc4c;
""",
    "fg2": """    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;
    uVar5 = uVar6 + uVar7;
    if (uVar5 <= DAT_007d70dc[DAT_007d70c8]) goto LAB_004dcc4c;
""",
}
MIDS = {
    "m0": MID,
    "m1": MID.replace("""    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = idx;
    }""", """    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = 0;
    }"""),
    "m2": MID.replace("""    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = idx;
    }""", """    DAT_007d70c8 = ++idx;
    if (idx >= 4) {
      DAT_007d70c8 = idx = 0;
    }"""),
}
TOPS = {
    "p0": PRO,
    "p1": PRO.replace("  idx = DAT_007d70c8;\n  uVar7 = param_1 * param_2;\n", "  uVar7 = param_1 * param_2;\n  idx = DAT_007d70c8;\n"),
}
VARIANTS = {}
for p, f, l, m, i in itertools.product(TOPS, FIRST, ["l0", "l1"], MIDS, ["i0", "i1"]):
    body = TOPS[p] + FIRST[f] + LOOPHEAD + LOOPEND[l].replace("FOUND", FOUND["d0"]) + MIDS[m] + NF["n0"] + INNER[i] + TAIL["t0"] + END
    VARIANTS[f"{p}{f}{l}{m}{i}"] = body

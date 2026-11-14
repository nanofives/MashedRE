"""Space 4: intermediate end-reference forms in the loop and first check (on top of space3 fg/ra/t0)."""
import itertools, pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space3.py").read_text().rsplit("VARIANTS = {}", 1)[0], ns)
PRO, FIRST, MID, TAIL, END = ns["PRO"], ns["FIRST"], ns["MID"], ns["TAIL"], ns["END"]
HEAD = "    uVar4 = 0;\n    do {\n      if (uVar4 != idx) {\n        a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;\n"
FOUNDT = """          idx = uVar4;
          DAT_007d70c8 = idx;
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
"""
LOOP = {
    "l0": "        uVar5 = uVar6 + uVar7;\n        if (uVar5 <= DAT_007d70dc[uVar4]) {\n",
    "l3": "        uVar5 = uVar6 + uVar7;\n        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {\n",
    "l4": "        if ((uVar5 = uVar6 + uVar7) <= DAT_007d70dc[uVar4]) {\n",
    "l5": "        uVar5 = uVar7 + uVar6;\n        if (uVar5 <= DAT_007d70dc[uVar4]) {\n",
    "l6": "        uVar5 = uVar6 + uVar7;\n        if (DAT_007d70dc[uVar4] >= uVar5) {\n",
    "l7": "        uVar5 = uVar6;\n        uVar5 += uVar7;\n        if (uVar5 <= DAT_007d70dc[uVar4]) {\n",
    "l8": "        uVar5 = uVar6 + uVar7;\n        if (!(uVar5 > DAT_007d70dc[uVar4])) {\n",
}
FIRST2 = dict(FIRST)
FIRST2["f3"] = FIRST["fg"].replace("    uVar5 = uVar6 + uVar7;\n    if (uVar5 <=", "    uVar5 = uVar6 + uVar7;\n    if (uVar6 + uVar7 <=")
FIRST2["f4"] = FIRST["fg"].replace("    uVar5 = uVar6 + uVar7;\n    if (uVar5 <=", "    if ((uVar5 = uVar6 + uVar7) <=")
MIDS = {
    "m0": MID,
    "m1": MID.replace("  if (uVar7 > DAT_007d70dc[idx]) {", "  if (DAT_007d70dc[idx] < uVar7) {"),
    "m2": MID.replace("  uVar6 = 0;\n  uVar5 = uVar7;\n", "  uVar5 = uVar7;\n  uVar6 = 0;\n"),
}
VARIANTS = {}
for f, l, m in itertools.product(["fg", "fy", "f3", "f4"], LOOP, MIDS):
    VARIANTS[f"{f}{l}{m}"] = PRO + FIRST2[f] + HEAD + LOOP[l] + FOUNDT + MIDS[m] + TAIL["t0"] + END

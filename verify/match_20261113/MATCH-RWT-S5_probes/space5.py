"""Space 5: found-path store forms and reload placement, on the space3 fg/ra/l0/t0 base."""
import itertools, pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space3.py").read_text().rsplit("VARIANTS = {}", 1)[0], ns)
PRO, FIRST, MID, TAIL, END = ns["PRO"], ns["FIRST"], ns["MID"], ns["TAIL"], ns["END"]
HEADS = {
    "h0": "        a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;\n",
    "h1": "        uVar6 = DAT_007d70cc[uVar4];\n        a = *(volatile uint *)&param_1;\n        uVar6 = ((uVar6 + a - 1) / a) * a;\n",
    "h2": "        uVar5 = DAT_007d70cc[uVar4];\n        a = *(volatile uint *)&param_1;\n        uVar6 = ((uVar5 + a - 1) / a) * a;\n",
    "h3": "        uVar6 = (0, DAT_007d70cc[uVar4]) + (a = *(volatile uint *)&param_1) - 1;\n        uVar6 = (uVar6 / a) * a;\n",
    "h4": "        uVar6 = (DAT_007d70cc[uVar4] + (a = *(volatile uint *)&param_1) - 1) / a * a;\n",
}
FOUNDS = {
    "d0": "          idx = uVar4;\n          DAT_007d70c8 = idx;\n          if (uVar4 < 4) goto LAB_004dcc4c;\n",
    "d1": "          DAT_007d70c8 = uVar4;\n          idx = DAT_007d70c8;\n          if (uVar4 < 4) goto LAB_004dcc4c;\n",
    "d2": "          idx = DAT_007d70c8 = uVar4;\n          if (uVar4 < 4) goto LAB_004dcc4c;\n",
    "d3": "          DAT_007d70c8 = idx = uVar4;\n          if (idx < 4) goto LAB_004dcc4c;\n",
    "d4": "          idx = uVar4;\n          DAT_007d70c8 = idx;\n          if (idx < 4) goto LAB_004dcc4c;\n",
    "d5": "          idx = uVar4;\n          DAT_007d70c8 = uVar4;\n          if (idx < 4) goto LAB_004dcc4c;\n",
}
TMPL = """    uVar4 = 0;
    do {
      if (uVar4 != idx) {
HEAD        uVar5 = uVar6 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
FOUND          break;
        }
"""
VARIANTS = {}
for h, d in itertools.product(HEADS, FOUNDS):
    VARIANTS[f"{h}{d}"] = PRO + FIRST["fg"] + TMPL.replace("HEAD", HEADS[h]).replace("FOUND", FOUNDS[d]) + MID + TAIL["t0"] + END

"""Space 6: loop built from goto (no front-end loop) x param_1 forms."""
import itertools, pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space3.py").read_text().rsplit("VARIANTS = {}", 1)[0], ns)
PRO, FIRST, MID, TAIL, END = ns["PRO"], ns["FIRST"], ns["MID"], ns["TAIL"], ns["END"]
MID_TAIL = MID.split("  }\n  uVar6 = 0;", 1)[1]
INC = """    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = idx;
    }
  }
  uVar6 = 0;"""
LOOPS = {
    "goto1": """    uVar4 = 0;
next:
    if (uVar4 != idx) {
      uVar6 = ((DAT_007d70cc[uVar4] + P - 1) / P) * P;
      uVar5 = uVar6 + uVar7;
      if (uVar5 <= DAT_007d70dc[uVar4]) {
        idx = uVar4;
        DAT_007d70c8 = idx;
        if (uVar4 < 4) goto LAB_004dcc4c;
        goto inc;
      }
    }
    uVar4 = uVar4 + 1;
    if (uVar4 < 4) goto next;
inc:
""",
    "do": """    uVar4 = 0;
    do {
      if (uVar4 != idx) {
        uVar6 = ((DAT_007d70cc[uVar4] + P - 1) / P) * P;
        uVar5 = uVar6 + uVar7;
        if (uVar5 <= DAT_007d70dc[uVar4]) {
          idx = uVar4;
          DAT_007d70c8 = idx;
          if (uVar4 < 4) goto LAB_004dcc4c;
          break;
        }
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < 4);
""",
}
PS = {"pp": "param_1", "pa": "a"}
TAILD = {"td": "*param_5 = uVar6 / param_1;", "tv": "*param_5 = uVar6 / *(volatile uint *)&param_1;"}
VARIANTS = {}
for lp, p, td, f in itertools.product(LOOPS, PS, TAILD, ["fi", "fg"]):
    loop = LOOPS[lp].replace("P", PS[p])
    if p == "pa":
        loop = loop.replace("      uVar6 = ((DAT", "      a = *(volatile uint *)&param_1;\n      uVar6 = ((DAT", 1).replace("        uVar6 = ((DAT", "        a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT", 1)
    end = END.replace("*param_5 = uVar6 / *(volatile uint *)&param_1;", TAILD[td])
    VARIANTS[f"{lp}{p}{td}{f}"] = PRO + FIRST[f] + loop.replace("DAT_007d70cc[uVar4] + a", "DAT_007d70cc[uVar4] + a") + INC + MID_TAIL + TAIL["t0"] + end

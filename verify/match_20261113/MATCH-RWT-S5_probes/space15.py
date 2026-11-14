"""Space 15: loop end form y1 (size weight up) x idx-ref reductions (global reads where c2 forwards them)."""
import itertools, pathlib
BASE = (pathlib.Path(__file__).resolve().parent / "best832.c").read_text()
BASE = "int FUN_004dcbb0" + BASE.split("int FUN_004dcbb0", 1)[1]
L0 = "        uVar5 = uVar6 + uVar7;\n        if (uVar5 <= DAT_007d70dc[uVar4]) {\n"
LOOPS = {
    "l0": L0,
    "l1": "        if (uVar6 + uVar7 <= DAT_007d70dc[uVar4]) {\n          uVar5 = uVar6 + uVar7;\n",
}
DIMS = {
    "F": [("", ""), ("    uVar6 = ((DAT_007d70cc[idx] +", "    uVar6 = ((DAT_007d70cc[DAT_007d70c8] +")],
    "K": [("", ""), ("    key = DAT_007d70ec[idx];", "    key = DAT_007d70ec[DAT_007d70c8];")],
    "D": [("", ""), ("    DAT_007d70dc[idx] = uVar7;\n    FUN_004dcaa0(uVar7,&DAT_007d70ec[idx]);",
                     "    DAT_007d70dc[idx] = uVar7;\n    FUN_004dcaa0(uVar7,&DAT_007d70ec[DAT_007d70c8]);")],
    "C": [("", ""), ("      if (uVar4 != idx) {", "      if (uVar4 != DAT_007d70c8) {")],
    "I": [("", ""), ("""    idx = idx + 1;
    DAT_007d70c8 = idx;
    if (idx >= 4) {
      idx = 0;
      DAT_007d70c8 = idx;
    }""", """    DAT_007d70c8 = DAT_007d70c8 + 1;
    idx = DAT_007d70c8;
    if (idx >= 4) {
      DAT_007d70c8 = 0;
      idx = 0;
    }""")],
}
VARIANTS = {}
for l, *ch in itertools.product(LOOPS, *[range(len(v)) for v in DIMS.values()]):
    t = BASE.replace(L0, LOOPS[l])
    name = l
    for (dk, opts), c in zip(DIMS.items(), ch):
        x, y = opts[c]
        if x:
            assert x in t, x
            t = t.replace(x, y)
        name += f"{dk}{c}"
    VARIANTS[name] = t

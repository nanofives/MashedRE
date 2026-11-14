"""MATCH-RWT-S5 attempt 3, step 2: base e1 t_lp_dp (size in edi, param_1 spilled by c2). Remaining: idx and end swap
ebx/ebp. Sweep idx vs DAT_007d70c8 at every site where both hold the same value (no store in between)."""
import itertools, pathlib
_ns = {}
exec(pathlib.Path(__file__).with_name("e1.py").read_text(), _ns)
BASE = _ns["VARIANTS"]["t_lp_dp"]
SITES = [
    ("r", "uVar6 = ((DAT_007d70cc[idx] + a0 - 1)", "uVar6 = ((DAT_007d70cc[DAT_007d70c8] + a0 - 1)"),
    ("f", "if (uVar5 <= DAT_007d70dc[DAT_007d70c8]) goto", "if (uVar5 <= DAT_007d70dc[idx]) goto"),
    ("l", "if (uVar7 > DAT_007d70dc[idx]) {", "if (uVar7 > DAT_007d70dc[DAT_007d70c8]) {"),
    ("k", "key = DAT_007d70ec[idx];", "key = DAT_007d70ec[DAT_007d70c8];"),
    ("s", "DAT_007d70dc[idx] = uVar7;", "DAT_007d70dc[DAT_007d70c8] = uVar7;"),
    ("c", "FUN_004dcaa0(uVar7,&DAT_007d70ec[idx]);", "FUN_004dcaa0(uVar7,&DAT_007d70ec[DAT_007d70c8]);"),
    ("p", "piVar2 = DAT_007d70ec[idx];", "piVar2 = DAT_007d70ec[DAT_007d70c8];"),
]
VARIANTS = {}
for bits in itertools.product([0, 1], repeat=len(SITES)):
    b = BASE
    name = ""
    for on, (n, x, y) in zip(bits, SITES):
        if on:
            assert x in b, n
            b = b.replace(x, y)
            name += n
    VARIANTS[name or "base"] = b

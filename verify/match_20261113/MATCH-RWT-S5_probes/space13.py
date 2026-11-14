"""Space 13 (diagnostic only, changes code): does extra size weight ever move size into edi with the reload local present?"""
import pathlib
BASE = (pathlib.Path(__file__).resolve().parent / "best832.c").read_text()
BASE = "int FUN_004dcbb0" + BASE.split("int FUN_004dcbb0", 1)[1]
T = "  *param_3 = DAT_007d70ec[DAT_007d70c8];\n"
assert T in BASE
VARIANTS = {}
for n in (0, 2, 4, 8, 16):
    VARIANTS[f"sz{n}"] = BASE.replace(T, T + "".join(f"  DAT_007d70dc[{i % 4}] = uVar7;\n" for i in range(n)))
LOOPX = "        uVar5 = uVar6 + uVar7;\n"
for n in (1, 2, 4):
    VARIANTS[f"szl{n}"] = BASE.replace(LOOPX, LOOPX + "".join("        DAT_007d70ec[3] = (int *)uVar7;\n" for i in range(n)))

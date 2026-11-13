"""Space 11: parameter / local signedness (int vs uint) on the plain-param_1 and volatile-reload bases."""
import itertools, pathlib, re
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space6.py").read_text(), ns)
BASES = {"pp": ns["VARIANTS"]["dopptdfg"], "pa": ns["VARIANTS"]["dopatdfg"]}
VARIANTS = {}
for (bk, b), p1, p2, loc in itertools.product(BASES.items(), ["uint", "int"], ["int", "uint"], ["u", "i5", "i7", "i6", "i57"]):
    t = b.replace("int FUN_004dcbb0(uint param_1,int param_2,", f"int FUN_004dcbb0({p1} param_1,{p2} param_2,")
    if p1 == "int":
        t = t.replace("*(volatile uint *)&param_1", "*(volatile int *)&param_1")
    for v in {"i5": ["uVar5"], "i7": ["uVar7"], "i6": ["uVar6"], "i57": ["uVar5", "uVar7"], "u": []}[loc]:
        t = t.replace(f"  uint {v};\n", f"  int {v};\n")
    VARIANTS[f"{bk}_{p1}_{p2}_{loc}"] = t

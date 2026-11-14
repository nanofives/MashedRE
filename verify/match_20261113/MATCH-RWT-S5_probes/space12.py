"""Space 12: block boundaries around the reload temp (labels, gotos) to change its span."""
import pathlib
BASE = (pathlib.Path(__file__).resolve().parent / "best832.c").read_text()
BASE = "int FUN_004dcbb0" + BASE.split("int FUN_004dcbb0", 1)[1]
R = "        a = *(volatile uint *)&param_1;\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;\n"
assert R in BASE, BASE
FORMS = {
    "base": R,
    "lab1": "        a = *(volatile uint *)&param_1;\nL1:\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;\n",
    "lab2": "        uVar6 = DAT_007d70cc[uVar4];\n        a = *(volatile uint *)&param_1;\nL1:\n        uVar6 = ((uVar6 + a - 1) / a) * a;\n",
    "lab3": "        uVar6 = DAT_007d70cc[uVar4];\nL1:\n        a = *(volatile uint *)&param_1;\n        uVar6 = ((uVar6 + a - 1) / a) * a;\n",
    "div2": "        a = *(volatile uint *)&param_1;\n        uVar6 = (DAT_007d70cc[uVar4] + a - 1) / a;\nL1:\n        uVar6 = uVar6 * a;\n",
    "lab1g": "        a = *(volatile uint *)&param_1;\nL1:\n        uVar6 = ((DAT_007d70cc[uVar4] + a - 1) / a) * a;\n        if (uVar4 == 9) goto L1;\n",
}
VARIANTS = {k: BASE.replace(R, v) for k, v in FORMS.items()}

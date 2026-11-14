"""Space 8: param_1 plain everywhere + address-taken forms that emit no code. Base: space6 dopptdfg (plain param_1)."""
import pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space6.py").read_text(), ns)
BASE = ns["VARIANTS"]["dopptdfg"]
TOP = "  idx = DAT_007d70c8;\n"
assert TOP in BASE
FORMS = {
    "plain": "",
    "ifnull": "  if (&param_1 == 0) return 0;\n",
    "void": "  (void)&param_1;\n",
    "cmpaddr": "  if ((uint *)&param_1 == (uint *)&param_2) return 0;\n",
    "deadptr": "  { uint *pp = &param_1; (void)pp; }\n",
    "deadstore": "  { uint *pp = &param_1; *pp = *pp; }\n",
    "selfassign": "  *(uint *)&param_1 = param_1;\n",
    "selfvol": "  *(volatile uint *)&param_1 = param_1;\n",
    "sizeof": "  (void)sizeof(&param_1);\n",
    "cast": "  (void)(uint)&param_1;\n",
}
VARIANTS = {k: BASE.replace(TOP, v + TOP, 1) for k, v in FORMS.items()}

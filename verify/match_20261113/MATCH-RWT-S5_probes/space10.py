"""Space 10: param_1 as a 4-byte struct / union passed by value (ABI identical), plain uses everywhere."""
import pathlib, re
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space6.py").read_text(), ns)
BASE = ns["VARIANTS"]["dopptdfg"]
SIG = "int FUN_004dcbb0(uint param_1,"
assert SIG in BASE
TYPES = {
    "st": "typedef struct { uint v; } P1T;\n",
    "un": "typedef union { uint v; int i; } P1T;\n",
    "arr": "typedef struct { uint v[1]; } P1T;\n",
}
VARIANTS = {}
for k, td in TYPES.items():
    acc = "param_1.v[0]" if k == "arr" else "param_1.v"
    body = BASE.replace(SIG, "int FUN_004dcbb0(P1T param_1,")
    head, rest = body.split("{", 1)
    rest = re.sub(r"\bparam_1\b", acc, rest)
    VARIANTS[k] = td + head + "{" + rest

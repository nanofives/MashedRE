import pathlib
_ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "v1.py").read_text(), _ns)
G1, G2 = _ns["G1"], _ns["G2"]
LOOP = "offset = ((DAT_007d70cc[n] + param_1 - 1) / param_1) * param_1;"
LOOPV = "uint a = *(volatile uint *)&param_1;\n        offset = ((DAT_007d70cc[n] + a - 1) / a) * a;"
TAILD = "*param_5 = offset / param_1;"
TAILV = "*param_5 = offset / *(volatile uint *)&param_1;"


def v(t):
    assert LOOP in t and TAILD in t
    return t.replace(LOOP, LOOPV).replace(TAILD, TAILV)


VARIANTS = {"g1v": v(G1), "g2v": v(G2)}

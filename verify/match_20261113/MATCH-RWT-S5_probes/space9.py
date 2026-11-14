"""Space 9: inline round-up helpers on the fg+key structure (plain param_1)."""
import pathlib
ns = {"__file__": __file__}
exec((pathlib.Path(__file__).resolve().parent / "space6.py").read_text(), ns)
BASE = ns["VARIANTS"]["dopptdfg"]
F = "    uVar6 = ((DAT_007d70cc[idx] + param_1 - 1) / param_1) * param_1;"
L = "        uVar6 = ((DAT_007d70cc[uVar4] + param_1 - 1) / param_1) * param_1;"
assert F in BASE and L in BASE
HELP = {
    "inl": "static __inline uint RoundUp(uint x, uint a) { return ((x + a - 1) / a) * a; }\n",
    "finl": "static __forceinline uint RoundUp(uint x, uint a) { return ((x + a - 1) / a) * a; }\n",
    "inl2": "static __inline uint RoundUp(uint x, uint a) { uint q = (x + a - 1) / a; return q * a; }\n",
    "inl3": "static __inline uint RoundUp(uint a, uint x) { return ((x + a - 1) / a) * a; }\n",
}
VARIANTS = {}
for h, txt in HELP.items():
    call_f = "    uVar6 = RoundUp(DAT_007d70cc[idx], param_1);" if h != "inl3" else "    uVar6 = RoundUp(param_1, DAT_007d70cc[idx]);"
    call_l = "        uVar6 = RoundUp(DAT_007d70cc[uVar4], param_1);" if h != "inl3" else "        uVar6 = RoundUp(param_1, DAT_007d70cc[uVar4]);"
    b = txt + BASE
    VARIANTS[h + "_both"] = b.replace(F, call_f).replace(L, call_l)
    VARIANTS[h + "_loop"] = b.replace(L, call_l)

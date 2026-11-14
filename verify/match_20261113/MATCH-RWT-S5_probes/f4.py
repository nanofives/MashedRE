"""MATCH-RWT-S5 Fable attempt, step 4: f3.py's g tail (100%) with the entry copy of param_1 varied.
t = volatile read (the 100% form), c = plain copy, n = param_1 everywhere, k* = key forms with t."""
import pathlib
_ns = {}
exec(pathlib.Path(__file__).with_name("f3.py").read_text().replace("__file__", "_f"), {"_f": __file__, **_ns}, _ns)
T = _ns["VARIANTS"]["g"]
VOL = "  a0 = *(volatile uint *)&param_1;"
assert VOL in T
G = "DAT_007d70ec[DAT_007d70c8]"
KEY = "    vb = " + G + ";\n"
assert KEY in T
VARIANTS = {
    "t": T,
    "c": T.replace(VOL, "  a0 = param_1;"),
    "n": T.replace(VOL, "").replace("a0 * param_2", "param_1 * param_2")
          .replace("+ a0 - 1) / a0) * a0", "+ param_1 - 1) / param_1) * param_1"),
    # volatile copy feeds only the size product, the first round-up reads param_1
    "ts": T.replace("+ a0 - 1) / a0) * a0", "+ param_1 - 1) / param_1) * param_1"),
    # volatile copy feeds only the first round-up
    "tr": T.replace("a0 * param_2", "param_1 * param_2"),
    # key inline in the pn loop (no local)
    "tki": T.replace(KEY, "").replace("(int *)pn[2] == vb", "(int *)pn[2] == " + G),
}

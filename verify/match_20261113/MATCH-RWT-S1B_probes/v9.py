# S1B attempt 2: per-function #pragma optimize forms (the frame comes from the pragma, not the command line).
exec(open("v1.py").read().split("COMBOS =")[0])
BASE = {k: VARIANTS[k] for k in ("dn", "idx", "idxr", "dn2")}
PRAGMAS = {
    "y_off": '#pragma optimize("y", off)\n',
    "gy_off_on": '#pragma optimize("", off)\n#pragma optimize("", on)\n#pragma optimize("y", off)\n',
    "t_off": '#pragma optimize("t", off)\n',
    "y_off_t_off": '#pragma optimize("ty", off)\n',
    "y_off_s_on": '#pragma optimize("y", off)\n#pragma optimize("s", on)\n',
    "y_off_p_on": '#pragma optimize("y", off)\n#pragma optimize("p", on)\n',
    "y_off_a_on": '#pragma optimize("y", off)\n#pragma optimize("a", on)\n',
    "y_off_w_on": '#pragma optimize("y", off)\n#pragma optimize("w", on)\n',
    "y_off_i_off": '#pragma optimize("iy", off)\n',
}
VARIANTS = {}
for pn, pt in PRAGMAS.items():
    for bn, bt in BASE.items():
        VARIANTS[f"{bn}_{pn}"] = pt + "// MATCH: 0x004cb4c0 FUN_004cb4c0\n" + bt if False else bt
        VARIANTS[f"{bn}_{pn}"] = (pt, bt)
# gen.py expects text bodies; wrap: the pragma goes in the body text before the function.
VARIANTS = {k: v[0] + v[1] for k, v in VARIANTS.items()}
COMBOS = [(tc, fl) for tc in ("ddk", "rw9044") for fl in ("/O2 /Ob1 /G5", "/O2 /Ob1 /G5 /Oy-")]

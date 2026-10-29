r"""Negative controls for check_glow.py (A4): the same dump against three mutated formulas; each must FAIL.

    py -3.12 verify\iconcube_20261028\negctl_check_glow.py <H_GLOWD.puglow.tsv>

M1 colour index off by one (table[type + 1]); M2 state-1 position from the PRE frame (before FUN_00458a40);
M3 state-1 d2 summed (dx dx + dy dy) + dz dz and the size in double then rounded (a rounding-level change).
"""
import importlib.util
import io
import sys
from contextlib import redirect_stdout
from pathlib import Path

import numpy as np

HERE = Path(__file__).resolve().parent


def load():
    spec = importlib.util.spec_from_file_location("cg", HERE / "check_glow.py")
    m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
    return m


def run(m, dump):
    sys.argv = ["check_glow.py", dump, "--max-print", "0"]
    buf = io.StringIO()
    with redirect_stdout(buf):
        rc = m.main()
    return rc, buf.getvalue().strip().splitlines()[0]


def main():
    dump = sys.argv[1]
    res = []
    m = load()                                     # M1
    m.COL = m.COL[1:] + [0]
    res.append(("M1 colour index +1",) + run(m, dump))
    m = load()                                     # M2
    orig = m.expected
    def exp2(pre, post):
        return orig(pre, {i: pre[i] for i in pre})
    m.expected = exp2
    res.append(("M2 state-1 pos from PRE",) + run(m, dump))
    m = load()                                     # M3
    F = m.F
    def exp3(pre, post):
        out = orig3(pre, post)
        for e in out:
            if e["st"] != 1:
                continue
            st, ty, a04, r18, t1c, a, p, fr = pre[e["i"]]
            dx, dy, dz = (float(F(a[k] - p[k])) for k in range(3))
            d2 = (dx * dx + dy * dy) + dz * dz
            s = 1.25 if d2 <= 0 else 1.25 * min(max(1.0 - d2, 0.0), 1.0)
            b = m.bits(F(s)); e["size"] = (b, b)
        return out
    orig3 = m.expected
    m.expected = exp3
    res.append(("M3 state-1 size in double",) + run(m, dump))
    bad = 0
    for name, rc, line in res:
        print(f"{name}: rc={rc}  {line}")
        bad += rc == 0
    print("NEGCTL", "PASS (every mutant fails)" if bad == 0 else f"FAIL ({bad} mutant(s) passed)")
    return 0 if bad == 0 else 1


if __name__ == "__main__":
    sys.exit(main())

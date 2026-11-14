"""Trace which instructions make equiv.py's post-proof attribution add each class.

Read-only wrapper: loads re/tools/match/equiv.py, rewrites the text of attribute() so every
classes.add(X) also prints X with the local residue it came from, then runs equiv.main() with argv.
The proof itself is not touched. Usage:
  py -3.12 verify/match_20261113/admit420050/attr_trace.py re/match/s05/00420050.c --only 0x0041fcc0
"""
import inspect
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "re", "tools", "match"))
import equiv  # noqa: E402

src = inspect.getsource(equiv.attribute)


def repl(m):
    cls = m.group(1)
    ind = m.group(0)[: len(m.group(0)) - len(m.group(0).lstrip())]
    return (f'{ind}print("[attr] {cls}", {{k: v for k, v in locals().items() '
            f'if k in ("xa", "xb", "tag", "i1", "i2", "j1", "j2", "pa", "pb", "ba", "bb", "seq")}}, '
            f'(ca.address, cb.address, ca.mnemonic, ca.op_str, cb.mnemonic, cb.op_str) if "ca" in locals() else "", '
            f'"sigma", sigma if "sigma" in locals() else "", file=sys.stderr)\n'
            f'{m.group(0)}')


src2 = re.sub(r'^[ \t]*classes\.add\("([A-Z-]+)"\)', repl, src, flags=re.M)
ns = equiv.__dict__
ns["sys"] = sys
exec(compile(src2, "attribute_traced", "exec"), ns)
sys.argv = ["equiv.py"] + sys.argv[1:]
equiv.main()

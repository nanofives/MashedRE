"""PROV-CRASH-12: callee prototypes declared in each provisional source (PROV_VERDICTS.tsv) whose parameter count
differs from the callee's own match source (the `// MATCH: 0x<rva>` definition anywhere under re/match).

usage: py -3.12 re/tools/prov/prov_arity_scan.py [--verdicts verify/provinsitu_20261111/PROV_VERDICTS.tsv] > ARITY_SCAN.tsv

A cdecl callee declared with fewer arguments than it reads gets the caller's frame contents for the missing ones
(0x005a9f10 -> FUN_005ae3a0). A defining source is itself a reconstruction: a mismatch is a lead to check against the
callee's disassembly, not a verdict.
"""
import argparse
import pathlib
import re

ap = argparse.ArgumentParser()
ap.add_argument("--verdicts", default="verify/provinsitu_20261111/PROV_VERDICTS.tsv")
a = ap.parse_args()


def nargs(s):
    s = s.strip()
    return 0 if s in ("", "void") else s.count(",") + 1


defs = {}
for f in pathlib.Path("re/match").rglob("*.c"):
    t = f.read_text(errors="replace")
    for m in re.finditer(r"//\s*MATCH:\s*(?:MASHED\.exe\s+)?0x([0-9a-fA-F]{8})[^\n]*\n[^\n{;]*?\bFUN_\1\s*\(([^)]*)\)\s*\{",
                         t, re.I):
        defs.setdefault(m.group(1).lower(), set()).add((nargs(m.group(2)), f.as_posix()))

print("rva\tsource\tprovinsitu_verdict\tcallee\tdeclared_args\tdefined_args\tdefining_source")
for ln in pathlib.Path(a.verdicts).read_text().splitlines()[1:]:
    f = ln.split("\t")
    rva, src, ver = f[0], f[1], f[9]
    p = pathlib.Path(src)
    if not p.exists():
        continue
    t = p.read_text(errors="replace")
    for m in re.finditer(r"^[\w \*]*?\bFUN_([0-9a-fA-F]{8})\s*\(([^)]*)\)\s*;", t, re.M):
        if "..." in m.group(2):
            continue
        c, n = m.group(1).lower(), nargs(m.group(2))
        for dn, ds in sorted(defs.get(c, ())):
            if dn != n:
                print(f"{rva}\t{src}\t{ver}\t0x{c}\t{n}\t{dn}\t{ds}")

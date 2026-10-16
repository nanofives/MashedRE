"""Summarise the path2_<key>.txt reports: install proof (opcode E9 + rel32 to the survivor's export) is separated
from the call-through vectors (interceptor entries / per-input results), because several registry entries carry
path2_tests in the early-window shape that the path2 template cannot drive ('expected an unsigned integer')."""
import pathlib
import re
import sys

d = pathlib.Path(sys.argv[1])
rows = []
for f in sorted(d.glob("path2_*.txt")):
    key = f.stem[len("path2_"):]
    t = f.read_text(encoding="utf-8", errors="replace")
    op = re.search(r"opcode:\s+(\S+)\s+expected 0xe9\s+->\s+(\w+)", t)
    rel = re.search(r"rel32:\s+(\S+)\s+expected (\S+)\s+->\s+(\w+)", t)
    ent = re.search(r"interceptor entries \(reimpl\):\s+(\d+) / (\d+)", t)
    errs = sorted(set(re.findall(r"err=([^\n]+)", t)))
    exp = re.search(r"export: (\S+)", t)
    inst = "INSTALL-OK" if (op and op.group(2) == "OK" and rel and rel.group(3) == "OK") else "INSTALL-FAIL"
    tail = t.split("\nFAIL:", 1)[-1] if "\nFAIL:" in t else ""
    calls = "calls-OK" if ("(none)" in tail.split("report written")[0] and "[FAIL]" not in t) else "calls-FAIL"
    rows.append((key, exp.group(1) if exp else "?", inst, f"entries {ent.group(1)}/{ent.group(2)}" if ent else "entries ?",
                 calls, "; ".join(errs)[:90]))
w = max(len(r[0]) for r in rows)
for r in rows:
    print(f"{r[0]:{w}s}  {r[1]:26s}  {r[2]:13s}  {r[3]:14s}  {r[4]:11s}  {r[5]}")
print(f"-- {len(rows)} reports, install-ok {sum(1 for r in rows if r[2]=='INSTALL-OK')}, calls-ok {sum(1 for r in rows if r[4]=='calls-OK')}")

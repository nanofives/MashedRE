"""Summarise a MASHED_HOOK_MANIFEST (idx \\t rva \\t installed \\t name) for the DUP-INSTALL RVAs.

    py -3.12 manifest_summary.py <manifest> <rvas27.txt> [<out.txt>]

For every RVA: every registry entry with that target RVA, its installed flag, and whether the RVA ended with
exactly one installed hook. A pair with 2 entries and 1 installed means the second one was refused
(HookSystem "DUAL-INSTALL REFUSED"), i.e. dead code in that run.
"""
import pathlib
import sys

man = pathlib.Path(sys.argv[1])
rvas = [int(x, 16) for x in pathlib.Path(sys.argv[2]).read_text().split()]
out = pathlib.Path(sys.argv[3]) if len(sys.argv) > 3 else None
rows = []
for ln in man.read_text(encoding="utf-8", errors="replace").splitlines():
    p = ln.split("\t")
    if len(p) >= 4:
        rows.append((int(p[0]), int(p[1], 16), p[2] == "1", p[3]))
lines = []
bad = 0
for r in rvas:
    ent = [(i, ins, n) for i, rv, ins, n in rows if rv == r]
    inst = [n for i, ins, n in ent if ins]
    status = "OK-single" if len(ent) == 1 and len(inst) == 1 else ("DUAL(1 refused)" if len(ent) >= 2 and len(inst) == 1 else
                                                                  ("NO-ENTRY" if not ent else f"CHECK entries={len(ent)} installed={len(inst)}"))
    if not status.startswith("OK"):
        bad += 1
    lines.append(f"0x{r:08X}  {status:18s}  " + "  ".join(f"[{i}]{'+' if ins else '-'}{n}" for i, ins, n in ent))
lines.append(f"-- {len(rvas)} rvas, {len(rvas) - bad} single-install, {bad} other")
txt = "\n".join(lines)
print(txt)
if out:
    out.write_text(txt + "\n", encoding="utf-8")

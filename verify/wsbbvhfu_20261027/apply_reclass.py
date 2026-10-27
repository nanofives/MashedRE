# D4-WSB-BVH-FU re-classify transaction (C3 -> C4, 1 row: 0x00565200). Edits only that hooks.csv row
# (csv round-trip of the line, every other line byte-identical) and inserts one CHANGELOG line right
# below <!-- ENTRIES -->. Refuses if the row is not C3 or the marker is missing. Run once from the worktree root.
import csv, io, sys
from pathlib import Path

R = "verify/wsbbvhfu_20261027/RESULT_D4-WSB-BVH-FU.md"
K = "00565200"
EV = ("census racediff C4-CLEAN, tool v7 (OFF boot --hooks __none__: asi loaded, hook byte 8b; ON boot jmp e9), "
      "track0 mode10 cars4, A4 calls 1-3000, 6/6 calls, 1 sig each side (verify/wsbbvhfu_20261027/racediff_summary.tsv); "
      "write-set RunRegion 0x1c at param_3 CLEAN 48/48, PATCHBYTE A/B-IS-REAL, hook installed alone "
      "(verify/wsbbvhfu_20261027/shadow_summary.tsv), region covers all 6 stores of the body "
      "(verify/wsbbvh_20261027/RESULT_D4-WSB-BVH-WRITESET.md static item 4). Both legs on the exe/asi with the "
      "D4-WSB-NARROW-FLIP default (merge fc09a9e04), gates unchanged")

hp = Path("hooks.csv")
lines = hp.read_bytes().decode("utf-8").split("\r\n")
name = None
for i, ln in enumerate(lines):
    if ln.split(",", 1)[0] != K:
        continue
    r = next(csv.reader([ln]))
    if r[3] != "C3":
        sys.exit(f"refuse: {K} is {r[3]}, not C3")
    r[3], r[4], r[6], r[7] = "C4", "verified", "c4_racediff_bvh:census-track0-mode10-cars4-f1-3000+shadow_region:track0-mode10-cars4", "verify/wsbbvhfu_20261027/racediff_summary.tsv"
    r[8] += f" || C4 2026-10-27 (D4-WSB-BVH-FU): {EV}. Both legs with the inline JMP live. No callee, no stub in the body. {R}"
    b = io.StringIO(); csv.writer(b, lineterminator="").writerow(r)
    lines[i] = b.getvalue(); name = r[1]
if name is None:
    sys.exit(f"refuse: row {K} missing")
hp.write_bytes("\r\n".join(lines).encode("utf-8"))

cp = Path("re/analysis/CHANGELOG.md")
t = cp.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in t else "\n"
m = "<!-- ENTRIES -->" + nl
if m not in t:
    sys.exit("refuse: no ENTRIES marker")
add = (f"2026-10-27  {K}  {name}  C3->C4  D4-WSB-BVH-FU: census racediff C4-CLEAN (tool v7, hook installed alone) "
       f"+ RunRegion write-set CLEAN 48/48 (A/B-IS-REAL); {R}{nl}")
cp.write_bytes(t.replace(m, m + add, 1).encode("utf-8"))
print("promoted:", K, name)

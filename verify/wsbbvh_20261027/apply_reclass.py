# D4-WSB-BVH-WRITESET re-classify transaction (C3 -> C4, 5 rows). Edits only the 5 hooks.csv rows
# (csv round-trip of those lines, every other line byte-identical) and inserts CHANGELOG lines right
# below <!-- ENTRIES -->. Refuses if a row is not C3 or the marker is missing. Run once from the worktree root.
import csv, io, sys
from pathlib import Path

R = "verify/wsbbvh_20261027/RESULT_D4-WSB-BVH-WRITESET.md"
WS = ("write-set shadow CLEAN (ShadowAB::RunTracked, hook installed alone, PATCHBYTE A/B-IS-REAL, "
      "census track0 mode10 cars4, 24/24 samples, 0 divergent, page-level writes + caller 4 KB stack window compared)")
ROWS = {
    "0055fea0": ("verify/wsbbvh_20261026/racediff_summary.tsv", "Quick Battle racediff C4-CLEAN 2026-10-26 (6/6 calls, 1 sig each side)"),
    "0055ff90": ("verify/wsbbvh_20261026/racediff_summary.tsv", "Quick Battle racediff C4-CLEAN 2026-10-26 (6/6 calls, 1 sig each side)"),
    "00561040": ("verify/wsbbvh_20261026/racediff_summary.tsv", "Quick Battle racediff C4-CLEAN 2026-10-26 (6/6 calls, 1 sig each side)"),
    "00564040": ("verify/wsbbvh_20261026/racediff_summary.tsv", "Quick Battle racediff C4-CLEAN 2026-10-26 (6/6 calls, 1 sig each side)"),
    "00565260": ("verify/wsbbvh_20261027/racediff_summary.tsv", "Quick Battle racediff C4-CLEAN 2026-10-27 tool v6 (6/6 calls, 4 sigs each side), return not compared under PREREG 3 (EAX dead at all 3 call sites, high half of 0x00564c80 param_2 never read, argwidth.txt)"),
}
WSN = {"00565260": WS.replace("24/24 samples", "20/20 samples")}

hp = Path("hooks.csv")
lines = hp.read_bytes().decode("utf-8").split("\r\n")
done = {}
for i, ln in enumerate(lines):
    k = ln.split(",", 1)[0]
    if k not in ROWS:
        continue
    r = next(csv.reader([ln]))
    if r[3] != "C3":
        sys.exit(f"refuse: {k} is {r[3]}, not C3")
    fd, rd = ROWS[k]
    r[3], r[4], r[6], r[7] = "C4", "verified", "c4_racediff_bvh:QuickBattle-f300-1200+shadow_tracked:track0-mode10-cars4", fd
    r[8] += (f" || C4 2026-10-27 (D4-WSB-BVH-WRITESET): {rd}; {WSN.get(k, WS)}. Both legs with the inline JMP live. "
             f"No stub in the body. {R}")
    b = io.StringIO(); csv.writer(b, lineterminator="").writerow(r)
    lines[i] = b.getvalue(); done[k] = r[1]
if set(done) != set(ROWS):
    sys.exit(f"refuse: rows missing {set(ROWS) - set(done)}")
hp.write_bytes("\r\n".join(lines).encode("utf-8"))

cp = Path("re/analysis/CHANGELOG.md")
t = cp.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in t else "\n"
m = "<!-- ENTRIES -->" + nl
if m not in t:
    sys.exit("refuse: no ENTRIES marker")
add = "".join(f"2026-10-27  {k}  {done[k]}  C3->C4  D4-WSB-BVH-WRITESET: racediff C4-CLEAN (hook installed alone) + write-set shadow CLEAN (RunTracked, A/B-IS-REAL); {R}{nl}"
              for k in sorted(done))
cp.write_bytes(t.replace(m, m + add, 1).encode("utf-8"))
print("promoted:", ", ".join(sorted(done)))

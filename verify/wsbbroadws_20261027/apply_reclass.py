# D4-WSB-C4-BROAD-WS round 2 re-classify transaction (run once from the worktree root, after leg 1 held).
# C3 -> C4: 0055a1f0, 0055ab30, 0055c0f0 (heap-tracked write-set leg HEAP-CLEAN + canonical racediff C4-CLEAN).
# Stay C3 with a dated note: 0055bae0, 0055c000 (heap leg compared 0 pages; their writes land in the caller's stack).
# Edits only those 5 hooks.csv rows (csv round-trip of the line, every other line byte-identical) and inserts one
# CHANGELOG line per row right below <!-- ENTRIES -->. Refuses if a row is not C3 or is missing.
import csv, io, sys
from pathlib import Path

R = "verify/wsbbroadws_20261027/RESULT_D4-WSB-C4-BROAD-WS.md"
S = "verify/wsbbroadws_20261027/racediff_r2_summary.tsv"
D = "2026-10-27"
HEAP = ("heap-tracked write-set shadow HEAP-CLEAN (ShadowAB::RunTracked, MASHED_SHADOW_PRIVATE=1 + STW, hook installed "
        "alone, PATCHBYTE A/B-IS-REAL, census track0 mode10 cars4, 24/24 samples, 0 divergent, {h} heap pages compared "
        "in total ({p} per sample), caller 4 KB stack window and return compared)")
PROMOTE = {
    "0055a1f0": ("shadow_tracked_heap:track0-mode10-cars4+c4_racediff:quickbattle-a4-300-1200-argw",
                 HEAP.format(h=104, p="4..12") + ". Quick Battle racediff C4-CLEAN, tool v10, C4_DEREF, declared-width "
                 "mask 0xffff on arg 1 of the 0x00564040 call-out (proof argwidth_00564040.txt): 6/6 calls, 1 sig each "
                 "side, ON byte e9. Round 1's sample-0 divergence at 0x00624048 did not recur under STW"),
    "0055ab30": ("shadow_tracked_heap:track0-mode10-cars4+c4_racediff:quickbattle-a4-300-1200-argw",
                 HEAP.format(h=150, p="6..8") + ". Quick Battle racediff C4-CLEAN, tool v10, C4_DEREF, declared-width "
                 "masks 0xffff on arg 1 of the 0x00565260 call-out (proof argwidth_00565260.txt) and of the 0x00564c80 "
                 "call-out (verify/wsbbvh_20261027/argwidth.txt): 6/6 calls, 4 sigs each side, ON byte e9"),
    "0055c0f0": ("shadow_tracked_heap:track0-mode10-cars4+c4_racediff:census-track0-mode10-cars4-hold400-a4-300-30000",
                 HEAP.format(h=24, p="1") + ". Census racediff C4-CLEAN, tool v10, C4_DEREF, C4_HOLD=400, A4 calls "
                 "300-30000: 6/6 calls, 1 sig each side ('eax=-', the body has no call-outs, so its output is covered "
                 "by the heap leg), ON byte e9"),
}
STAY = {
    "0055bae0": ("stays C3. Heap-tracked write-set leg CLEAN 24/24 but 0 heap pages compared; the NEED_PAGES arm "
                 "discarded 256 calls, every return 0x001afc2c (caller stack): its only writes land in the caller's "
                 "4 KB stack window, which is compared (stack=0) but is not a heap page. Census racediff (v10, "
                 "C4_HOLD=400, A4 300-30000) C4-CLEAN 6/6, 1 sig. Promotion needs a ruling on whether the stack-window "
                 "compare counts as the write-set leg"),
    "0055c000": ("stays C3. Heap-tracked write-set leg CLEAN 24/24 but 0 heap pages compared; the NEED_PAGES arm "
                 "discarded 256 calls, every return a caller-stack address (0x001af78c..0x001af7f8 there, 0x001af6fc..0x001af810 in the 24 leg-2 samples): its only writes "
                 "land in the caller's 4 KB stack window (compared, stack=0). Census racediff (v10, C4_HOLD=400, A4 "
                 "300-30000) not observed on either side in 2 attempts, though the 60 s census shadow leg sampled it "
                 "24 times. No scenario"),
}
want = set(PROMOTE) | set(STAY)
hp = Path("hooks.csv")
lines = hp.read_bytes().decode("utf-8").split("\r\n")
names = {}
for i, ln in enumerate(lines):
    k = ln.split(",", 1)[0]
    if k not in want:
        continue
    r = next(csv.reader([ln]))
    if r[3] != "C3":
        sys.exit(f"refuse: {k} is {r[3]}, not C3")
    if k in PROMOTE:
        sc, ev = PROMOTE[k]
        r[3], r[4], r[6], r[7] = "C4", "verified", sc, S
        r[8] += (f" || C4 {D} (D4-WSB-C4-BROAD-WS round 2): {ev}. Hook installed alone, inline JMP live. "
                 f"No stub in the body. {R}")
    else:
        r[8] += f" || {D} (D4-WSB-C4-BROAD-WS round 2): {STAY[k]}. {R}"
    b = io.StringIO(); csv.writer(b, lineterminator="").writerow(r)
    lines[i] = b.getvalue(); names[k] = r[1]
missing = sorted(want - set(names))
if missing:
    sys.exit(f"refuse: rows missing {missing}")
hp.write_bytes("\r\n".join(lines).encode("utf-8"))

cp = Path("re/analysis/CHANGELOG.md")
raw = cp.read_bytes().decode("utf-8")
nl = "\r\n" if "\r\n" in raw else "\n"
marker = "<!-- ENTRIES -->" + nl
if raw.count(marker) != 1:
    sys.exit("refuse: CHANGELOG marker not found exactly once")
add = "".join(f"{D}  {k}  {names[k]}  C3->C4  D4-WSB-C4-BROAD-WS r2: heap-tracked write-set HEAP-CLEAN + canonical "
              f"racediff C4-CLEAN (hook installed alone); {R}{nl}" for k in sorted(PROMOTE))
add += "".join(f"{D}  {k}  {names[k]}  C3 (no change)  D4-WSB-C4-BROAD-WS r2: heap-tracked leg compared 0 pages "
               f"(writes only to the caller's stack window); {R}{nl}" for k in sorted(STAY))
cp.write_bytes(raw.replace(marker, marker + add, 1).encode("utf-8"))
print("applied", sorted(names))

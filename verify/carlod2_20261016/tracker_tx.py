"""carlod2 tracker transaction (re-classify skill). Run once from the worktree root:

    py -3.12 verify/carlod2_20261016/tracker_tx.py

- hooks.csv (cp1252): 004b5240 C2 -> C3 (path1 early_window GREEN on the non-degenerate vector or_4b5240_set, negative control
  RED, path2 install + entry PASS with the shared-.asi identity check OK).
- UNCERTAINTIES.md: U-9706 (b) and (e) narrowed.
- re/analysis/CHANGELOG.md: entries below the exact `<!-- ENTRIES -->` line.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DATE = "2026-10-16"
R = "verify/carlod2_20261016/RESULT_carlod2.md"

# ---- hooks.csv ---------------------------------------------------------------------------
hp = ROOT / "hooks.csv"
h = open(hp, encoding="cp1252", newline="").read()
m = re.search(r"(?m)^(004b5240,[^\r\n]*)", h)
assert m
row = m.group(1)
assert ",C2,impl," in row and "stale-r115-no-clear-arm" in row, row[:200]
nrow = row.replace(",C2,impl,", ",C3,impl,", 1).replace("stale-r115-no-clear-arm",
                                                      "log/diff_or_4b5240_set.csv", 1)
add = (" | carlod2 " + DATE + " C2->C3: vector or_4b5240_set (seed 0xFF, flag 0 then 1; the r115 vector's seed 0xF9 has bit 2 clear "
       "so flag=0 changed nothing on either side) early_window GREEN 2/2 non-degenerate (flag=0: 0xFFFFFFFF -> 0xFFFBFFFF on both "
       "sides, the and cl,0xfb arm at 0x004b5256); negative control = the pre-fix body (no clear arm) RED on flag=0; path2 install "
       "+ entry PASS (p2bufs shape) in a backup/no-foreign-MASHED/restore deploy window, asi identity OK (rva 0x03ae40). " + R)
nrow = (nrow[:-2] + add + '",') if nrow.endswith('",') else ((nrow[:-1] + add + '"') if nrow.endswith('"') else (nrow + add))
h = h.replace(row, nrow, 1)
open(hp, "w", encoding="cp1252", newline="").write(h)

# ---- UNCERTAINTIES.md ----------------------------------------------------------------------
up = ROOT / "UNCERTAINTIES.md"
u = open(up, encoding="utf-8", newline="").read()
m = re.search(r"(?m)^\| U-9706 \|[^\r\n]*", u)
assert m
r6 = m.group(0)
old_b = "(b) F & 0x800 is the controller's brake byte"
assert r6.count(old_b) == 1
r6n = r6.replace(old_b,
                 "(b) [carlod2 " + DATE + ": WIRED, opt-in `MASHED_CAR_BRAKE=1`: slot 0 from the player's `io.input[5]`, slots 1..3 from "
                 "the ported AI tick's ctrl block byte [5], one frame delayed (AI tick `0x0040fc6c` before `FUN_0040da50` "
                 "`0x0040fc75`; the log shows byte 255 at row 1090 and the plate at 1091 on Arctic slot 3, matching the original). "
                 "Held OFF because the standalone AI's brake output differs from the original's at the cited rows: it brakes at "
                 "Arctic 1090 slot 3 (match), not at Arctic 1125 slot 1 or City 835 slots 1/2 (original does), and at TRAINING 923 "
                 "slot 3 (original does not), PREREG_carlod2 rules 1-2 fail; a cause for the AI to diverge there is not mapped. "
                 "Also measured: supplying the bit removes rfx's Headlight MISSING/EXTRA rows on the three plate frames (U-9711). "
                 "`" + R + "`] F & 0x800 is the controller's brake byte", 1)
old_e = "(e) Car"
if old_e in r6n:
    r6n = r6n.replace(old_e, "(e) [carlod2: DONE, each livery model's own part-0x13 origin is used] Car", 1)
u = u.replace(r6, r6n, 1)
open(up, "w", encoding="utf-8", newline="").write(u)

# ---- CHANGELOG -----------------------------------------------------------------------------
cp = ROOT / "re" / "analysis" / "CHANGELOG.md"
c = open(cp, encoding="utf-8", newline="").read()
nl = "\r\n" if "\r\n" in c else "\n"
lines = c.split(nl)
k = [i for i, ln in enumerate(lines) if ln.strip() == "<!-- ENTRIES -->"]
assert len(k) == 1
entries = [
    f"{DATE}  U-9706  NARROWED  (b) brake byte wired behind flag 0x800, opt-in MASHED_CAR_BRAKE=1 (standalone AI brake output differs at 3 of 4 cited events); (e) per-livery part-0x13 origin done  {R}",
    f"{DATE}  004b5240  C2->C3  or_4b5240_set (seed 0xFF) early_window GREEN non-degenerate, pre-fix body RED, path2 PASS in a deploy window with asi identity OK  {R}",
]
lines[k[0] + 1:k[0] + 1] = entries
open(cp, "w", encoding="utf-8", newline="").write(nl.join(lines))
print("tracker transaction applied")

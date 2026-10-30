# RESULT D5-KNOBS-INV-FIX2 (2026-10-30)

Text-only correction of the default column of 49 non-class-d rows in `verify/d5knobsinv_20261030/RESULT_D5-KNOBS-INVENTORY.md` that still read OFF / value / n/a but are seeded by `kFaith` (`mashedmod/src/mashed_re/Core/FaithDefaults.cpp`). List source: `verify/d5knobsinvfix_20261030/RESULT_D5-KNOBS-INVENTORY-FIX.md` section "Outside class d". No source, script, tracker or build input touched, no build, no game run, no -Full pass.

**Verdict: GREEN. 49 of 49 names re-checked against kFaith (name present, kFaith line equals the listed line, inventory row name matches, row not already corrected); 49 default cells edited, old value kept in the cell.**

## Method

- Parsed `kFaith` from FaithDefaults.cpp as it stands on the base (orch/integration 4b04c3841). Opt-out: `MASHED_NO_<name>=1` or `MASHED_NO_FAITH=1`.
- For each listed inventory line: asserted the row's name, asserted the name minus `MASHED_` is in kFaith on the listed line, then replaced only the default cell with `ON (seeded "<value>") [corrected D5-KNOBS-INV-FIX2; was <old>]`. The four class x rows (no env read in the cited source) read `n/a (no env read here; kFaith seeds "1") [corrected ...; was n/a]`.
- `MASHED_AI_LUT` is seeded "7" (a value, not a flag). `MASHED_INTRO_MOVIES` is seeded "1" (FaithDefaults.cpp:77) and also opts out with `MASHED_INTRO_MOVIES=0`.
- `git diff --stat`: one file, 49 insertions, 49 deletions.

## Corrected rows

| inventory line | knob | class | old default | new default | kFaith line | seed |
|---|---|---|---|---|---|---|
| 33 | MASHED_A5_RUBBER_LIVE | b | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:48 | "1" |
| 34 | MASHED_A6A_LC0 | b | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:33 | "1" |
| 35 | MASHED_AI_LUT | b | value (parsed) | ON (seeded) | FaithDefaults.cpp:34 | "7" |
| 140 | MASHED_PARTMORPH | b | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:72 | "1" |
| 145 | MASHED_WCS_COMBINE_PC24 | b | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:46 | "1" |
| 146 | MASHED_WCS_TRINORMAL | b | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:44 | "1" |
| 147 | MASHED_A4_DOT_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:62 | "1" |
| 148 | MASHED_A4_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:37 | "1" |
| 149 | MASHED_A5_AIR_F32 | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:62 | "1" |
| 150 | MASHED_A5_CONTACT_INT | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:62 | "1" |
| 151 | MASHED_A5_DOT_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:61 | "1" |
| 152 | MASHED_A5_FD0_LIVE | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:37 | "1" |
| 153 | MASHED_A5_RING_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:61 | "1" |
| 154 | MASHED_A5_SPIN_FILD | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:62 | "1" |
| 155 | MASHED_A6A_DOT_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:58 | "1" |
| 157 | MASHED_A6A_GEAR_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:37 | "1" |
| 158 | MASHED_A6A_SPIN_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:41 | "1" |
| 164 | MASHED_AI_FWD_ATROW | c | OFF (set, not "0") | ON (seeded) | FaithDefaults.cpp:35 | "1" |
| 177 | MASHED_FIXUP_F32 | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:41 | "1" |
| 178 | MASHED_FIXUP_SLIDE_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:42 | "1" |
| 179 | MASHED_FIXUP_SUM_ORDER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:41 | "1" |
| 180 | MASHED_FIX_5CEA44 | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:38 | "1" |
| 186 | MASHED_HEADING_FWD_LIVE | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:34 | "1" |
| 189 | MASHED_INTRO_MOVIES | c | value (parsed) | ON (seeded) | FaithDefaults.cpp:77 | "1" |
| 194 | MASHED_MODE7 | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:36 | "1" |
| 195 | MASHED_MODE_LIVE | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:51 | "1" |
| 197 | MASHED_P3_RESET_PAIR | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:66 | "1" |
| 199 | MASHED_PATHPROG | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:29 | "1" |
| 200 | MASHED_PICKUPS_ORIG | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:35 | "1" |
| 208 | MASHED_REASSERT_KEEP_NORMAL | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:31 | "1" |
| 209 | MASHED_REGISTRY | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:35 | "1" |
| 210 | MASHED_REG_CALLBACKS | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:36 | "1" |
| 211 | MASHED_REG_CARS | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:36 | "1" |
| 212 | MASHED_REG_LATE | c | OFF (set, not "0") | ON (seeded) | FaithDefaults.cpp:37 | "1" |
| 215 | MASHED_ROTATE_LUT | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:33 | "1" |
| 216 | MASHED_ROTATE_PC24 | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:34 | "1" |
| 218 | MASHED_ROUND_GRID | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:29 | "1" |
| 227 | MASHED_SLOT_TYPE_LIVE | c | OFF (set, not "0") | ON (seeded) | FaithDefaults.cpp:51 | "1" |
| 231 | MASHED_STEER_SCALE_RDATA | c | OFF (set, not "0") | ON (seeded) | FaithDefaults.cpp:35 | "1" |
| 232 | MASHED_SUBSTEP_OUTER | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:54 | "1" |
| 233 | MASHED_TER_SPIN_F32 | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:41 | "1" |
| 234 | MASHED_TGT3 | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:30 | "1" |
| 238 | MASHED_UP_TRINORMAL | c | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:32 | "1" |
| 297 | MASHED_CAR_STATE0 | a | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:72 | "1" |
| 466 | MASHED_SUBSTATE | a | OFF (unset -> off) | ON (seeded) | FaithDefaults.cpp:66 | "1" |
| 633 | MASHED_FIX_5CEA60 | x | n/a | n/a (kFaith seeds) | FaithDefaults.cpp:32 | "1" |
| 635 | MASHED_FWD_AT | x | n/a | n/a (kFaith seeds) | FaithDefaults.cpp:33 | "1" |
| 637 | MASHED_GRID_DIR | x | n/a | n/a (kFaith seeds) | FaithDefaults.cpp:30 | "1" |
| 679 | MASHED_RW_SSE_POINTS | x | n/a | n/a (kFaith seeds) | FaithDefaults.cpp:32 | "1" |

## Raw data

None committed. Script lived in the session temp dir.

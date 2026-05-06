# Leaderboard / Time Attack Records — Session JJJJJ
Session: leaderboard-20260506-JJJJJ | Slot: Mashed_pool3 (read-only) | Date: 2026-05-06
SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Anchor outcome

Strategies 1+2 (direct string search for "Leaderboard", "HighScore", "BestTime", "scores.bin",
etc.) returned **zero hits**. Found via Strategy 1 on "time trial" and "record" variants:

| String | Address | References |
|--------|---------|------------|
| `"setting time attack record too %d:%d:%f\n"` | 0x005ccf98 | 0x00411ec8 → FUN_00411d90 |
| `"Time trial recording error time %d\n"` | 0x005ccedc | 0x00411625 → FUN_00411600 |
| `"time trial time is %f\n"` | 0x005cd6ac | 0x0042941a → FUN_00429310 |

String `RACELEADER` (0x00603ad0) has zero cross-references — dead/asset string, irrelevant.

LEADERBOARD_FN: **FUN_00411d90** (Replay::CreateOrLoad) — already in hooks.csv (row 641,
replay_record-20260503 session). Strategy: "time trial" string search (Strategy 1 variant).

## Leaderboard system summary

Mashed has **no global leaderboard table with player names**. The "leaderboard" for Time Trial
mode is a per-track single best-time record stored in three parallel arrays, indexed by the
current track index (FUN_00430790 → DAT_0067f17c):

| Array | Address | Content |
|-------|---------|---------|
| minutes | DAT_007f0db4 | integer minutes of best lap |
| seconds | DAT_007f0de8 | integer seconds (0–59) |
| fractional | DAT_007f0e1c | fractional seconds (float) |

Written by FUN_004299d0 (called from Replay::CreateOrLoad at 0x00411ee4 when a prior
best-time replay exists) and by FUN_00429a30 (hud_frontend_d3 session, row 998).

The replay-side record system (per-frame ghost, .rep file I/O, lap comparison) is fully
documented in `re/analysis/replay_record/notes.md` (replay_record-20260503).

## New functions mapped this session

| ID | RVA | Name | Size | Status |
|----|-----|------|------|--------|
| U-2227 | 0x004299d0 | TimeRecord::WriteTrackBest | 83b | C2 |
| U-2228 | 0x00429310 | TimeTrial::Tick | 643b | C1 |
| U-2229 | 0x0040d270 | Course::Finish | 440b | C1 |

FUN_00430790 promoted from S-1605 (passthrough stub) — see notes in 0x00430790.md.

## Functions covered but already in hooks.csv (replay_record-20260503)

FUN_00411350 (Replay::TimeFormat), FUN_00411530 (Replay::GetTimeAtIdx),
FUN_00411580 (Replay::GetBestTime), FUN_004115c0 (Replay::GetCurrentTime),
FUN_00411600 (Replay::RecordFrame), FUN_00411870 (Replay::LapFinish),
FUN_00411d90 (Replay::CreateOrLoad), FUN_00482930 (Replay::New) — all C1/vehicle.

## Deferred (D=6580..6589)

| D | RVA | Reason |
|---|-----|--------|
| D-6580 | 0x0040e560 | FUN_0040e560 — called from TimeTrial::Tick before Replay::LapFinish; not confirmed |
| D-6581 | 0x0040fc00 | FUN_0040fc00 — race main-tick (calls TimeTrial::Tick); 620b; 30+ callees |
| D-6582 | 0x004115c0 | Replay::GetCurrentTime already mapped; skip |

Bucket for continuation: leaderboard-cont1.

## Stubs introduced this session

S-2220: TimeTrial::Tick (FUN_00429310) depends on FUN_0040e350 (race state getter, identity unknown)
S-2221: TimeTrial::Tick (FUN_00429310) depends on FUN_0042b8c0 (joypad/speed value, identity unknown)
S-2222: Course::Finish (FUN_0040d270) depends on FUN_00426ba0 (unknown predicate)

## Uncertainty notes

U-2227: FUN_004299d0 also writes param_1/2/3 into DAT_0067d9a0/d998/d990 before writing the
  arrays — purpose of these intermediate globals unclear. They are not read back within the
  function; possible side-effect for HUD display.
U-2228: TimeTrial::Tick: DAT_008991b4 accumulates a value from FUN_0042b8c0() — possibly
  a track-sector progress float, but meaning unconfirmed.
U-2229: Course::Finish: FUN_0040d270 is a large course-transition handler (440b). Only the
  tail (calls Replay::CreateOrLoad then sets DAT_0063ba8c=1) is clearly leaderboard-related.
  The bulk of the function handles car-collision detection and lap routing.

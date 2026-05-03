# Profile / Career / Unlocks — Subsystem Overview

**Session:** profile_career-20260503  
**Slot:** Mashed_pool9  
**Subsystem bucket:** save (career data persists via gamesave.bin)

---

## Strategy outcome

All three anchor strategies (string search, file-path search, call-graph from session P) converged on the career system. String search strategy-1 found no user-visible career/unlock/progress/trophy strings in the binary — this is because Mashed's career UI strings live in `.piz` asset archives, not embedded in the PE. Strategy-3 (call-graph from SAVE_LOAD_FN) located the track progression table and championship handlers.

---

## gamesave.bin layout (corrects session P calculation)

Session P calculated the buffer size as 150,432 bytes — this is a decimal arithmetic error. The correct value is:
`0x24fa0 = 2×65536 + 4×4096 + 15×256 + 10×16 + 0 = **151,456 bytes**`.

| Offset | Size | Source global | Content |
|--------|------|---------------|---------|
| 0x00000 | 4 B | — | Magic `0xDEADBEEF` (written by serializer FUN_00404ee0) |
| 0x00004 | 150,076 B | `DAT_008a94a8` (ptr) | Replay / ghost data buffer |
| 0x24A40 | 1,312 B | `DAT_007f0a40` | Track/player progression table (see below) |
| 0x24F60 | 64 B | — | Unused / padding |

The serializer `FUN_00404ee0` also packs 12 bytes from the sparse feature unlock table (`DAT_007f105c`, stride 0x13 ints = 76 bytes) into save-block positions `[0x24A40+1300]..[0x24A40+1311]` before the block copy. These 12 bytes correspond to 12 feature slots in the unlock table.

---

## Career system

### Game modes (DAT_0067e9fc)

| Value | Mode | Cup |
|-------|------|-----|
| 2 | TimeTrial | — |
| 3 | Championship | BronzeCup [UNCERTAIN U-1547] |
| 4 | Championship | SilverCup |
| 5 | Championship | GoldCup |
| 6–9 | Unknown | [UNCERTAIN U-1528 from game_mode session] |
| 10 | QuickRace | — |
| 11 | Set by FUN_0043df00 | [UNCERTAIN U-1529 from game_mode session] |

### Track progression table (DAT_007f0a40, 13 × 48 bytes)

13 tracks × 12 dword fields. Championship tracks 0–9 (10 tracks); bonus tracks 10–12. Full field map in `re/analysis/profile_career/00430290.md`.

Completion states: `0` = unavailable, `1` = available/next, `2` = done.

Key fields:
- field_1 (`DAT_007f0a44` + track×48): BronzeCup completion per track
- field_2 (`DAT_007f0a48` + track×48): SilverCup completion per track
- field_5 (`DAT_007f0a54` + track×48): GoldCup completion per track

All-done flags (set when all 10 tracks completed):
- `DAT_007f0c54` — SilverCup all-done (set to 1 → 2)
- `DAT_007f0c84` — GoldCup all-done (set to 1 → 2)
- `DAT_007f0f2c` — Global completion flag (GoldCup all 10 done; also set by FUN_004924f0's button-sequence tracker per game_state_d2 session)

### Core career flow

```
Race ends → FUN_00411170 (already in vehicle/replay_record session, RVA 0x00411170)
  ↓ calls FUN_00410510() — race-end evaluator (returns winning car idx or 0)
  ↓ if non-zero: FUN_00430290() — championship completion handler
      ↓ marks track done in DAT_007f0a40 table
      ↓ counts completed tracks
      ↓ if milestone: FUN_0042a920(event_id) — trophy event poster
      ↓ FUN_004099a0() — autosave trigger ("starting autosave")
```

### Replay data

`DAT_008a94a8` is a **pointer to the replay buffer** (not career data). Functions `FUN_004117b0` and `FUN_00411d90` use this pointer for replay save/load, and for time-attack ghost comparison. The save serializer stores the replay buffer wholesale in gamesave.bin at offset 4.

### Feature unlock table (DAT_007f105c)

Identified by session hud_frontend_d2 (`re/analysis/hud_frontend_d2/0x0042fe90.md`) as "feature unlock table (stride 0x13 ints)". Used to render Y/Z availability indicators per vehicle feature in the vehicle select screen. 12 entries saved to gamesave.bin. All 12 initialized to `1` (unlocked) by `FUN_004924f0`. Not a traditional career unlock — all content starts unlocked.

### Unlock codes

Four 10-character alphabetic codes at `0x005cd92c`:
- `PSOLPEBCYB` (at 0x005cd92c)
- `MDXKHDIGNX` (at 0x005cd938)
- `LFDGUYVDEU` (at 0x005cd944)
- `TKGFWNCIKR` (at 0x005cd950)

Each padded to 12 bytes (null-terminated + 1 extra null). Referenced by DATA READ from `0x004343b8` within `FUN_00448220` (a 17KB frontend/career-menu function at entry `0x00448220`, body `0x00434360`–`0x004486f2`). [UNCERTAIN U-1553: role of these codes — player-facing unlock passwords vs internal identifiers]

---

## Functions catalogued this session

| RVA | Role | File |
|-----|------|------|
| 0x00430290 | CHAMPIONSHIP_COMPLETE_FN | re/analysis/profile_career/00430290.md |
| 0x004099a0 | AUTOSAVE_TRIGGER | re/analysis/profile_career/004099a0.md |
| 0x0042a920 | TROPHY_EVENT_POST | re/analysis/profile_career/0042a920.md |

## Functions DEFERRED

| RVA | Reason | DEFERRED ID |
|-----|--------|-------------|
| 0x0040dd60 | Guard predicate in FUN_00430290; semantics unknown | D-4540 |
| 0x00448220 | 17KB frontend/career-menu; reads unlock codes at 0x005cd92c | D-4541 |
| 0x00410510 | Race-end evaluator; 800 bytes; many callees | D-4542 |

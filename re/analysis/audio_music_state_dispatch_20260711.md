---
rva: 0x00466b50
name: FUN_00466b50 (top-level audio-tick dispatcher)
subsystem_observed: audio/util
session_date: 2026-07-11
opened_in_slot: Mashed_pool14
---

# WS-J M1 slice — music-state transition harvest

## Scope note

This is a **read-only harvest** of the original's game-mode-conditioned audio
dispatch, done to satisfy RE_MASTER_PLAN_2026-07.md §7 item 6's second half
("which game events push which music state, citing call site RVAs"). It does
NOT promote any of the cited functions — they keep their existing hooks.csv
confidence (already C1/C2/C3/C4 from prior sessions; see table below). No new
hook was authored for this half of the slice.

## What the original actually does (NOT a discrete SetState(enum))

Unlike the standalone's `MusicSetState(MusicState)` (a 4-value enum switched
instantly), the original drives audio via a **per-frame envelope dispatcher**,
`FUN_00466b50` (0x00466b50..0x00466f84, hooks.csv: util/C2), called every
frame (top-level tick, gated on `DAT_006904e8 != 0`). It reads the current
game-mode word `DAT_00771968` via `FUN_00492d10` (hooks.csv: `Global771968Get`,
C3) and `switch`es on it (8 cases, 0..7), calling different **fade-envelope
step functions** with different direction codes per case — RAMP UP / RAMP DOWN
/ SNAP-TO-0 / FAST-DECAY, not a hard cut:

- `FUN_0045db50(dir)` (0x0045db50, hooks.csv: util/C2) steps float
  `DAT_0068fc8c` toward 0 or 1: `dir=0` decrement by `DAT_005cc9c0`, `dir=1`
  increment by `DAT_005cc9c0`, `dir=2` snap to 0, `dir=3` decrement by
  `DAT_005cd0ec` (a larger/"fast" step constant) — clamped to `[0,1]` against
  thresholds `DAT_005cc320`/`DAT_005d757c`.
- `FUN_0045dbe0(dir)` (0x0045dbe0, hooks.csv: util/C2) steps a second float
  `DAT_0068fcbc` the same way (`dir=0/1/2`, same step constant
  `DAT_005cc9c0`), then — gated on `FUN_00493f70(0)` (returns `DAT_00771a04`,
  hooks.csv: `VideoStateFlagGet`, **C4**) — calls
  `FUN_004943f0(DAT_006900d4 * DAT_0068fcbc)` (0x004943f0, hooks.csv:
  audio/C2, but tagged "input/DInput-adjacent" from a prior sweep — see
  Uncertainties below).
- `FUN_0045dfc0()` / `FUN_0045df70(target)` (0x0045dfc0/0x0045df70, hooks.csv:
  util/C2 and C3 respectively) step a THIRD float `DAT_006036c0` toward a
  target (0 or `1.0f`) by step `DAT_005cc9a4`; this value is read back as the
  master multiplier for every active virtual voice in `FUN_0045dd60`'s
  per-frame ducking loop (`fVar1 = DAT_006036c0; ... local_8 = fVar3*fVar1*...`).

## Per-mode dispatch table (RVA-cited, mode value = `DAT_00771968`)

| mode | call site (approx.) | envelope calls | mechanical read |
|------|---------------------|-----------------|------------------|
| 0 | 0x00466bda-0x00466be6 | `FUN_0045dbe0(2)` (snap `0068fcbc`->0) + `FUN_0045db50(1)` (`0068fc8c` ramp up) | one envelope snapped off, the other ramping up |
| 1 | 0x00466bf0-0x00466ca0 (largest branch: track-load bookkeeping via `FUN_0042b900` etc.) | conditionally `FUN_0045dbe0(1)`+`FUN_0045db50(0)` OR `FUN_0045db50(cond)`+`FUN_0045dbe0(0)`, then `FUN_0045df70(0)` | loading-phase envelope crossfade |
| 2 | 0x00466ca6-0x00466cd0 | `FUN_0045dbe0(0)` + conditionally `FUN_0045db50(1)` | ramp-down one envelope |
| 3, 7 | 0x00466cd6-0x00466f28 (by far the largest branch: calls `FUN_00462dd0` CreateStream, the 4-slot per-car opponent-AI loop `FUN_00463640`/`FUN_004645c0`/`FUN_004644a0`, and a nested dispatch switch on a track/mode sub-value) | `FUN_0045dfc0()` at entry, `FUN_0045db50(3)` (fast-decay) at exit | heaviest per-frame body; matches the standalone's InRace tick by sheer callee shape (CreateStream + AI loop) |
| 4 | 0x00466f30-0x00466f5c | `FUN_0045dbe0(0 or 1)` + `FUN_0045df70(0)` + `FUN_0045db50(0)` | |
| 5 | 0x00466f5e-0x00466ce0-ish (stream-drain: counts 4 stream states, stops CreateStream via `FUN_00462ec0` once all 4 streams report state 3 for >=3 consecutive frames) | `FUN_0045dbe0(2)` + `FUN_0045df70(0)` + `FUN_0045db50(0)` | matches a post-race "let the last voices drain, then cut" shape |
| 6 | 0x00466f7a-0x00466f7e | `FUN_0045dbe0(2)` + `FUN_0042b8f0()` | snap off |

## Cross-reference to the standalone's existing wiring

`mashedmod/src/mashed_re/Race/GameFlow.cpp` already calls
`Audio::MusicSetState(...)` at 3 boundaries (predates this session):
`GameFlow_RequestExit` -> `Menu`, the `LoadingRace`->`InRace` edge -> `Race`,
`GameFlow_RequestResults` -> `Results`. These are a reasonable behavioral
match to the busiest/most audio-distinct mode transitions found above (mode
3/7's CreateStream+AI-loop body is the strongest race-loop signal; modes
0/5/6 all snap `DAT_0068fcbc` to 0, consistent with "duck/silence" at
menu/results/shutdown boundaries) but this is NOT a byte-verified 1:1
mapping — see Uncertainties.

## Uncertainties

- `DAT_00771968`'s exact mode-value semantics (0..7) are **pre-existing and
  still open**: `[UNCERTAIN U-0686]` (filed prior session, cited in
  `re/analysis/game_mode_cont2/REPORT.md`). Not re-litigated here.
- U-9017 (new, filed in UNCERTAINTIES.md): whether the original gives
  "final round" or "win/lose" outcomes any DISTINCT music/audio treatment
  beyond the generic Results-mode envelope snap found above. No call site
  distinguishing win from lose was found in `FUN_00466b50`'s dispatch (mode
  5's body only counts stream-drain state, not race outcome). Non-blocking:
  the standalone does not attempt a win/lose audio distinction either, so
  there is no regression, only an unconfirmed absence.
- `FUN_004943f0`'s target object `DAT_00771a20` is tagged "input/DInput-
  adjacent" by a prior sweep (hooks.csv notes) rather than confirmed as a
  music voice; my fresh read of its body (two COM-vtable calls at offsets
  +0x20/+0x1c matching a GetVolume/SetVolume shape) is consistent with a
  DirectSound buffer but this was not independently confirmed against
  `DAT_00771a20`'s allocation site this session.

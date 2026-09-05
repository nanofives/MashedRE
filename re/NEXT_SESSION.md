# Next session — kickoff prompt

Written at the end of the 2026-09-04 **setup-screen faithfulness + team scoring +
text-source audit** lane (branch `race/first-frame-parity`). Paste the block below.

---

Resume the Mashed frontend/UI lane. Branch `race/first-frame-parity`, tree clean
apart from an untracked `videocfg.bin` (a standalone-run byproduct — do NOT
commit it), no children running, no worktrees or pool slots held.

Read `re/analysis/race_hud_capture_20260902.md` **Findings 29–36** (the newest
eight, all from this lane). Do NOT read the file top to bottom — later Findings
overturn earlier ones in the SAME file. In particular **Finding 33 corrects
Finding 32** (there is no `+1` message-id offset; it was the wrong FILE), and
**Finding 34's claim about `FUN_00427e00` is itself corrected by 33** — read
both as written.

## What this lane established (do not re-derive)

The two player-setup screens (15 Ability Select, 16 Team Select) are now **fully
ported and fully sourced** — no invented renderer, no invented input, no
invented text.

| thing | state |
|---|---|
| `0x0042f7b0` ability input step | IDENTIFIED = Ability Select's per-frame input (sibling of `0x0042fa00`), already C3 as `FrontendCursorUpdate`; wired into screen 15. New registry entry `frontend_cursor_update_abil` (the old GREEN was guard-only). path1 8/8, path2 PASS. Finding 29 |
| `0x0043a610` / `0x0043aa30` renderers | **C2 → C3**, `Frontend/SetupScreenRenderers.cpp` (ASI-only TU). `0x0042f8d0` takes its **alpha in AL** — a register arg no decomp shows, found by a hook-on/off draw-stream A/B. No path1 (device-null AV + racy VBUF); accepted evidence is the draw-stream A/B with an off-vs-off control. Finding 30 |
| team scoring | the `DAT_0067ea64` arms of `0x0040eee0` ported exe-side in `TrackRenderer` (`ScoreOnEliminationTeams`); all three arms observed (`0,1,0,1` reaches the 2-alive one). `MASHED_TEAM_PLAY`/`MASHED_TEAMS`. The victim still gets `+delta` in team play — faithful, recorded. Findings 31, +2-alive |
| rejection modal | the original does NOT annotate Team Select; it posts a modal (`0x0042bf30` → `0x00433f40`) with body id `0xd6..0xd9` and refuses to advance. Verdict line now pulls the real string, draws nothing on a legal split. Finding 32 |
| **U-9083** | RESOLVED — the port's `g_menu_str` read the loose `FONT/English.dat` (449 ids); the game reads the `Font36.piz` copy (677 ids), shifted from id `0x16`. Fixed. Finding 33. Memory `[[two-copies-of-english-dat]]` |
| **U-4259** | RESOLVED — `DAT_0067e850` is stride-12 single-dword; `+4`/`+8` have no reader/writer (XrefRange, 34 refs all at offset 0). Finding 30 |
| text audit | main draw path was always correct (`GetMenuMessage` on `Font36.piz`); only `g_menu_str` was wrong. Setup headers, options values (insults `0x59`/`0x1b2`/`0x1b1`, autosave `0x59+flag`) and the Challenge panel now draw by id. Findings 34, 35, 36 |
| Challenge Select panel | was an INVENTED "Bronze Challenge / Locked" caption; is actually a per-track mode checklist (`0x22`/`0x140` heading + `0x56`/`0x24b`/`0x141`) with per-flag Lock icons. Flag-dependence proven both by poke AND through the real save restore. Finding 36 |
| save-span mirror | **DEFECT FIXED**: `Nav_GameStateLoadSave` only filled a private `g_save_span`, so consumers reading live `0x007f0a40..` saw zeroed `.bss`. Now mirrors to the live range. `MASHED_SAVE=<path>` overrides the save file without swapping the shared reference. Finding 36 |

## Traps this lane paid for — carry these forward

- **A wrong reference TABLE gives confident, self-consistent, wrong readings.**
  U-9083 fit a `+1` across five independent pairings that did not exist. Reading
  the LOADER settled it, not comparing more strings. `[[two-copies-of-english-dat]]`
- **Decomp is silent about register arguments.** `0x0042f8d0` takes its alpha in
  AL; the tell is a `mov al, …` at the call site with no matching stack push, and
  the symptom in a draw-stream diff is matching geometry with colour-only
  mismatches. `[[decomp-is-silent-about-register-args]]`
- **The standalone commits original globals ZEROED**, so a field whose absent
  value is `-1` (team id `0x007f1a18`) reads as a valid `0`. Seed the producer's
  input and let the original derive; never poke the derived field.
  `[[zeroed-granule-vs-minus-one-sentinel]]`
- **An inherited GREEN can be guard-only.** `0x0042f7b0`'s 10/10 seeded the
  early-out guard in every vector and never ran the loop. Read the vectors before
  trusting a C3. `[[inherited-green-may-be-guard-only]]`
- **Arm-entry counters armed BEFORE the first run.** An arm that never fires and
  an arm that fires and awards nothing are the same silence in the score column.
- **A capture at the default state is often degenerate** (both option arms read
  "Off"; a fresh save locks every challenge row). Drive to a non-default state or
  the frame proves nothing.
- **Do NOT swap `original/gamesave.bin`** (or any file under `original/`) to test
  a save — it is the shared diffing reference. Use `MASHED_SAVE=<path>` with a
  scratch copy; verify the reference SHA is unchanged after.

## New verification knobs (display-only, not set in normal play)

`MASHED_ABIL_KEYS` / `MASHED_TEAM_KEYS` (per-profile taps), `MASHED_TEAM_PLAY=1`
(now written at BOOT, so the frontend sees it), `MASHED_TEAMS="0,0,1,1"`,
`MASHED_PLAYERS=2..4`, `MASHED_MSG_IDS="0xd6,…"` (dump ids through the port's
decoder), `MASHED_CHAL_UNLOCK="a,b,c"` (poke the three challenge flags),
`MASHED_SAVE=<path>` (override the restored save file).

## Candidate next slices

1. **`0x0042f8d0` / `0x0042bf30` / `0x00433f40` to C3 proper.** The plate drawer
   (with its AL arg now understood), the modal poster and the modal renderer are
   all read end-to-end this lane but only the first is installed anywhere. The
   modal pair would make the rejection flow a real hook rather than a port-side
   verdict line.
2. **Sournce the remaining literals.** Still hardcoded: `GameFlow.cpp` `kAreas`
   (a real defect flagged 2026-08-27 — "Arctic" occurs 0 times in the exe; not
   currently drawn but wired into `g_cup.tracks[].name`), and the `A`/`B`/`-`
   team marker (`[SCAFFOLD]`; the original's marker is the sprite slide, Finding
   27 — the letter has no id and should probably just be dropped).
3. **The `"check"` sprite question.** Finding 36 left it `[UNCERTAIN]` whether
   `FUN_0040bb50("check", …)` resolves to anything (INTERFACE.TXD has Lock, Star,
   Tick; no "check"). A Frida read of the dictionary `DAT_0063b8fc` at screen 6
   would settle whether an unlocked row draws Tick or nothing.
4. **Leave the frontend lane.** R7 has other subsystems; the standings/setup/team
   chain is done and sourced.

## Open risks / residuals (all recorded, none blocking)

- The **2-alive same-team COLLAPSE** on a `0,0,1,1` split needs both eliminations
  from one team, which ~12 rounds did not produce; reached instead via `0,1,0,1`.
  Implemented and now observed, but the `0,0,1,1` path itself is unforced.
- The **insults/autosave scale**: the original passes `0.7` for both rows, the
  port uses `0.72`/`0.6` (a June eyeball). Left alone — the two scale laws are not
  the same quantity — but flagged before anyone re-fits that row.
- **Save WRITE is not claimed.** Only that the restore carries the challenge
  flags into live memory; whether the port emits a save the original would accept
  is untouched, as is every other span field.
- The `videocfg.bin` byproduct at the repo root — do not commit it.

# Next session — kickoff prompt

Written at the end of the 2026-09-05 **Challenge-Select icon dictionary + literals**
lane (branch `race/first-frame-parity`, commit `f6d873c8`). Paste the block below.

---

Resume the Mashed frontend/UI lane. Branch `race/first-frame-parity` @ `f6d873c8`.

**FIRST: `git status`.** Another session's **modal-pair slice is still uncommitted**
here — `Frontend/MenuModal.cpp` (untracked) plus edits to `hooks.csv`,
`UNCERTAINTIES.md` (U-9084), `re/analysis/CHANGELOG.md`,
`re/analysis/race_hud_capture_20260902.md` (Finding 37), `re/frida/hooks_registry.py`,
`re/frida/menu_draw_burst.py`, `MenuMenusB.cpp`, `PromoLoop_round72.cpp`,
`asi_sources.rsp`. That work is complete and correct — do **not** commit it, edit it,
or revert it. Untracked `videocfg.bin` is a byproduct; never commit it.

Read `re/analysis/chalsel_icon_dictionary_20260905.md` (short, self-contained). It is
the successor to `race_hud_capture_20260902.md` Finding 36 and **overturns Finding 36's
icon paragraph** — read it after 36, and do not re-derive it. It was filed as its own
note rather than appended to the findings file only because that file was dirty.

## What this lane established (do not re-derive)

| thing | state |
|---|---|
| two sprite dictionaries | `0x0040bb50`→`DAT_0063b8fc`=`BADGES.TXD` (23 tex) and `0x0040bb90`→`DAT_0063b904`=`INTERFACE.TXD` (30 tex), each with its own slot gate (`0x0042ee00` lock/dot/check 16x16; `0x004391b0` Lock/Star/tick 32x32). A third head is `DAT_0063b8f8` behind `0x0040bb30`. Memory `[[two-sprite-dictionaries-badges-vs-interface]]` |
| the checklist icons | **RESOLVED.** `FUN_00439210` rows call the BADGES forwarder: `0x004395c6` tests the flag, `0x004395d9` pushes `"check"`, `0x004395e6` pushes `"lock"`. The original draws an icon on EVERY row. Port fixed: unlocked rows now draw `check`; locked rows now draw BADGES' 16x16 `lock` instead of INTERFACE's 32x32 `Lock` |
| `"Tick"` | never a candidate — wrong dictionary, has its own user as lowercase `"tick"` (`0x005cda3c`), and `FUN_004c5c00` is not prefix-tolerant (`0x004c5c5a` needs both strings to end together) |
| handle collision | `kSlotLock` 61 collided with `kSlotVehPrev0` (61..68) and `kHandleLock` 52 with `kHandleVehPrev0` (52..59). Moved to 93/94 and 84/85; bridge census comment corrected (~85 of 96, headroom ~11) |
| screen-6 Star | **unchanged, deliberately.** `0x00435f82`/`0x00435fdd` do push `"Star"` into `bb50`, but they are inside `FUN_00434720` (screen 5). The screen-6 star goes through the `bb90` gate, so the port's INTERFACE Star stands |
| A/B/- team marker | **DROPPED.** `[SCAFFOLD]`, no counterpart in `FUN_0043aa30`. The original's marker is the sprite slide + roster stack, both already ported — the letter was inventing output on top of faithful output |
| `kAreas[].name` / `Cup::name` | **DROPPED.** "Arctic" occurs 0 times in the exe; the names fed `Cup::tracks[].name`, which nothing read. Row labels come from the message table (id `0x49 + row`). The `piz` column keeps the identity |

## Left open, with reasons

1. **No screenshot of the fixed panel.** The standalone **exits on focus loss**, and a
   non-interactive session has no foreground desktop, so neither `MASHED_NAV_DEMO` nor
   `MASHED_PARITY` reaches frontend asset loading — no `walk_*.bmp`, no `parity/re_s6.bmp`,
   and `LoadMenuBadgeSprite`'s new `F38:` log line is **unverified at runtime**. The port
   change rests on the static branch read plus the live dictionary walk, which is the
   load-bearing evidence, but the render itself is unconfirmed. **First thing to do next
   session, from an interactive desktop:**
   `MASHED_NAV_DEMO=1 MASHED_CHAL_UNLOCK="1,0,1" MASHED_WIN_POS=left-bl mashedmod\build\mashed_re.exe`
   then check `verify/walk_06_challengeselect.bmp` shows a check on rows 0 and 2 and a
   padlock on row 1, and `log/mashed_re.log` for two `F38: badges.txd ... upload OK` lines.
   Compare against `verify/chalsel_panel_unlocked.bmp` (the pre-fix shot, same flags).
2. **No tracker rows.** `hooks.csv`, `UNCERTAINTIES.md` and `CHANGELOG.md` are dirty with
   the other session's transaction and are `re-classify`-only. Once that lands, file:
   **U-9085** (below), and a note on the `0x00439210` row that its icon sourcing is now
   pinned.
3. **U-9085 — a C3 body that does not match its RVA.** `LinkedListStringSearch`
   (`0x004c5c00`, `Frontend/SpriteCluster.cpp`) has the sentinel as `*(head+8)` when the
   binary makes it the ADDRESS `head+8` (`0x004c5c05 add eax,8`, loop test `0x004c5c68`),
   and reads the name as a pointer at `node+8` when it is an INLINE array
   (`0x004c5c1c lea ecx,[eax+0x10]`, `0x004c5c27 mov cl,[esi]`). Return value and case
   folding are right. Its `RH_ScopedInstall` has been commented out since 2026-05-24, so
   nothing has mis-executed — same shape as `0x0042f8d0` in Finding 37. **Decide the
   demotion deliberately; do not fix it in passing.** Memory
   `[[stale-c3-body-behind-disabled-install]]`.
4. The probe observed **zero natural `FUN_0040bb50` calls** — a synthetic
   `FUN_0043d2a0` push does not satisfy the panel guard (`FUN_00430760()==0 &&
   DAT_0067e9fc==6`), so the checklist block never ran. Stated, not read as agreement.

## Traps carried forward

- **A wrong reference TABLE gives confident, self-consistent, wrong readings.** Third
  time this lane paid for it (U-9083 the wrong `English.dat`; now the wrong TXD). Both
  times the LOADER settled it, not more string comparisons.
- **Case folding can make a wrong-dictionary guess look right.** `"lock"` resolves in
  both dictionaries; only `"check"` and the live head value separate them. When a lookup
  folds case, an offline file dump is not sufficient — read the runtime head.
- **A MASS-DISABLED body is unverified**, whatever `hooks.csv` says.
- Structure-level agreement with field-level disagreement (right node COUNT, garbage
  names) means the traversal is right and an OFFSET is wrong.
- Everything from the 2026-09-04 list still stands: register args, zeroed granules vs
  `-1` sentinels, guard-only GREENs, degenerate default states, never swap anything
  under `original/`.
- **Don't `Remove-Item` under `verify/`** to make room for a capture — `verify/parity/*.bmp`
  are tracked reference shots and are not bit-reproducible. (Deleted `re_s6.bmp` here and
  restored it from git; it was force-added precisely because `*.bmp` is gitignored.)

## Knobs

`MASHED_CHAL_UNLOCK="a,b,c"`, `MASHED_TEAM_PLAY=1` (BOOT), `MASHED_TEAMS`,
`MASHED_PLAYERS`, `MASHED_ABIL_KEYS`/`MASHED_TEAM_KEYS`, `MASHED_MSG_IDS`,
`MASHED_SAVE=<path>`, `MASHED_NAV_DEMO=1`, `MASHED_PARITY=1`,
`MASHED_DBG_BBDUMP=<frame>` + `MASHED_DBG_BBDUMP_OUT=<path>`, `MASHED_WIN_POS=left-bl`.
New probe: `py -3.12 re/frida/chal_icon_probe.py [--screen 6]`.

## Candidate next slices

1. **Close out this one properly** — the screen-6 capture in (1) above, then the tracker
   rows in (2) and the U-9085 decision in (3). Small, and it is the honest finish.
2. **Audit the other `FUN_0040bb50` / `FUN_0040bb90` call sites the same way.** The
   dictionary split was mis-assumed once; the port has ~16 texture registrations and
   several were named off whichever TXD was open at the time. `0x00434720`'s badges
   `Star` on screen 5 is a known, unexamined instance.
3. **Leave the frontend lane.** R7 has other subsystems; the standings/setup/team/
   challenge chain is now ported and sourced.

# Next session — kickoff prompt

Written at the end of the 2026-09-05/06 **Challenge-Select icon dictionary +
literals + sprite-forwarder audit + row-icon trace** lane (branch
`race/first-frame-parity`, commit `d30fafaf`). Paste the block below.

---

Resume the Mashed frontend/UI lane. Branch `race/first-frame-parity` @ `d30fafaf`.

**Tree is clean** apart from an untracked `videocfg.bin` (a run byproduct — never
commit it). The other session's modal-pair transaction and this lane's tracker rows
are all committed now (its accidental whole-file CRLF strip of `CHANGELOG.md` was
re-normalized to CRLF when landed, so the history diff is the real entries only).

Read `re/analysis/chalsel_icon_dictionary_20260905.md` (short, self-contained). It is
the successor to `race_hud_capture_20260902.md` Finding 36 and **overturns Finding 36's
icon paragraph** — read it after 36, and do not re-derive it.

## What this lane established (do not re-derive)

| thing | state |
|---|---|
| **four** sprite dictionaries | read out of the loader `FUN_0040bbb0`: `bb30`→`0x0063b8f8`=`FX.TXD`, `bb50`→`0x0063b8fc`=`BADGES.TXD` (23 tex), `bb70`→`0x0063b900`=`TrackImages.txd`, `bb90`→`0x0063b904`=`Interface.txd` (30 tex). Fifth head `0x0068b9ac` = powerups. Three arg-rewriting gates feed them: `0x0042ee00`→bb50 (lock/dot/check 16x16), `0x004391b0`→bb90 (Lock/Star/tick 32x32), `0x0042fab0`→bb90 (10 NFL* colours). Map any call site with `py -3.12 re/tools/sprite_forwarder_map.py`. Memory `[[two-sprite-dictionaries-badges-vs-interface]]` |
| the checklist icons | **RESOLVED.** `FUN_00439210` rows call the BADGES forwarder: `0x004395c6` tests the flag, `0x004395d9` pushes `"check"`, `0x004395e6` pushes `"lock"`. The original draws an icon on EVERY row. Port fixed: unlocked rows now draw `check`; locked rows now draw BADGES' 16x16 `lock` instead of INTERFACE's 32x32 `Lock` |
| `"Tick"` | never a candidate — wrong dictionary, has its own user as lowercase `"tick"` (`0x005cda3c`), and `FUN_004c5c00` is not prefix-tolerant (`0x004c5c5a` needs both strings to end together) |
| handle collision | `kSlotLock` 61 collided with `kSlotVehPrev0` (61..68) and `kHandleLock` 52 with `kHandleVehPrev0` (52..59). Moved to 93/94 and 84/85; bridge census comment corrected (~85 of 96, headroom ~11) |
| screen-6 row icon | **the port's per-row pulsing Star is WRONG** (superseded the earlier "Star settled" note). Geometry measured: non-selected rows draw a faint black status glyph at x=220 w=22 argb=0x3f000000 via the BADGES gate; the selected row draws a `MultiPlayer` category sprite (x~208 via `0x0042ee40`) and no small badge. No Star on any row; nothing at x~36. See slice 2 — the fix is scoped but needs a desktop screenshot. (Separately, the two badges-`Star` sites `0x00435f87`/`0x00435fe2` are in `FUN_00434720` = screen 5, a different screen.) |
| forwarder audit | **all 107 call sites swept; no further texture-source defects.** Button/Arrow/NFL*/vs/Star/lock/check and the 24 previews all already came from the right dictionary. Negative result recorded so nobody re-runs it — see the Addendum in `re/analysis/chalsel_icon_dictionary_20260905.md` |
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
2. **Tracker rows: FILED** (commit `d30fafaf`). U-9085, the `0x00439210` identity/icon
   note, and two CHANGELOG entries are in. Nothing left here.
3. **U-9085: RESOLVED + hook VERIFIED** (commits `f59e60ad`, `8754e74e`). `0x004c5c00`
   has TWO reimpls: the naked `Search4c5c00` (`Util/PromoLoop_sessionB.cpp`,
   RH_ScopedInstall ACTIVE, byte-faithful — the installed copy) and the C
   `LinkedListStringSearch` (`Frontend/SpriteCluster.cpp`, standalone-only via
   `SpriteLookupC`). The C copy had the traversal bug and was re-transcribed; it stays
   uninstalled so it can't double-install with `Search4c5c00` (U-9065). Do NOT re-enable
   it. The live hook was verified: path1 `early_window_leaf_diff` GREEN 5/5, path2 install
   confirmed (0xE9+rel32; call-through hit the known 0-arg harness gap). Stays C3.
   Also fixed a CSV-quote defect this lane introduced (two rows had unterminated notes
   fields, merging 8 rows on parse — see `8754e74e`). Memory
   `[[duplicate-rva-implementations-drift]]`. Nothing left here.
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

1. **Close out this one properly** — only the screen-6 capture (Left-open #1) remains;
   it needs a desktop (the standalone exits on focus loss here). Tracker rows filed and
   U-9085 resolved.
2. **Implement the Challenge-Select ROW state-icon (geometry now MEASURED)** — the
   full trace/geometry is done (commits `7d027314`/`df2a7124`, Addenda 2+3 of
   `re/analysis/chalsel_icon_dictionary_20260905.md`); this is now a scoped
   implement-and-**screenshot-verify**, held back only because this headless session
   could produce no capture. MEASURED screen-6 model (numbers already in the port's
   `N*kVScale` space):
   - **non-selected rows**: state icon at **x=220, w=h=22, argb=0x3f000000** (faint
     black), texture via BADGES gate on cup-table **column 3**
     (`*(u32*)(0x007f0a40 + row*0x30 + 0xc)`): `0→lock 1→dot 2→check 3→none`;
   - **selected row**: a category sprite (`MultiPlayer` on screen 6, via the
     screen-dispatch gate `0x0042ee40`) at **x~208 w~45 white**, and NO small badge —
     its INTERFACE tick is suppressed by `FUN_00430760()` on this state;
   - **no Star on any row.** The port's pulsing white Star at x~36 matches nothing.
   Faithful change = a multi-texture composition rewrite: load the category sprites
   (`MultiPlayer`/`QuickRace`/… from INTERFACE.TXD) + `dot` (BADGES, alongside the
   `lock`/`check` already loaded), draw category-on-selected / black-status-glyph-on-
   others at the measured geometry, remove the star, model `FUN_00430760`'s
   suppression. **Do it from a desktop** and confirm with
   `MASHED_NAV_DEMO=1 MASHED_CHAL_UNLOCK=... mashedmod\build\mashed_re.exe` →
   `verify/walk_06_challengeselect.bmp`. Drive non-uniform states with
   `MASHED_SAVE=<scratch>` / the probe's `--poke v0,v1,v2,v3` (the fresh save is the
   degenerate all-col3=2 trap).
3. **Leave the frontend lane.** R7 has other subsystems; the standings/setup/team/
   challenge chain is now ported and sourced.

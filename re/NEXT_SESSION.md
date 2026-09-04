# Next session — kickoff prompt

Written at the end of the 2026-09-03/04 **in-race UI faithfulness + player-setup** lane
(branch `race/first-frame-parity`). Paste the block below.

---

Resume the Mashed frontend/UI lane. Branch `race/first-frame-parity`, tree clean, no
children running, no worktrees or pool slots held.

Read `re/analysis/race_hud_capture_20260902.md` **Findings 21–28** (the newest eight, all
from this lane). Do NOT read the file top to bottom — early Findings are overturned by
later ones in the SAME file. In particular **Finding 23 corrects its own first draft**, and
**U-9082 is WITHDRAWN as never-valid**; read both as written, not as summarised elsewhere.

## What this lane established (do not re-derive)

**The in-race UI is fully ported and contains no invented elements.**

| thing | state |
|---|---|
| driving HUD (lap/pos, countdown, winner banner, power-up text) | INVENTED; the original draws no 2D HUD while driving. Now behind `MASHED_DEV_HUD=1`, off by default |
| post-race RESULTS screen | the original has none — the end-of-match screen IS the standings screen. Results now renders the ported overlay |
| `0x0041c410` standings row update | **C2 → C3**, `HUD/HudStandingsRowUpdate.cpp`, path1 GREEN 8/8, path2 install-verified, 45 s race smoke |
| `DAT_0067ea64` | **U-9078 RESOLVED** = Team Play (msg `0x140` "Team Play" vs `0x13e` "Standard Play"). Consequence: target 8 / crown 7; standard 4-player 12 / 10 |
| team scoring | decoded (Finding 24): `+delta` to every player slot on the winning team, `-delta` to the rest; same-team tie-break on a 0..100 progress scale, wrap period 100.0, bands 80.0 / 20.0 |
| player setup | `0x0042fa00` ported as an exe-side twin; teams assignable; legal splits reachable (1v1 and 2v2 verified) |
| setup screens 15/16 | both renderers read out of the binary (`FUN_0043a610` / `FUN_0043aa30`): **one row per assigned slot, profile order**, exact geometry, column slide, per-player car sprite |

## Traps this lane paid for — carry these forward

- **A string near a write describes the write's EFFECT; it does not name the variable.**
  Reading the caption `"First to 12 points wins."` as `DAT_0067ea64`'s definition produced a
  confident, self-consistent, WRONG conclusion that fit both consumers. The witness that
  settled it was a named menu entry that *writes* the flag. Prefer the witness that
  constrains identity over the one that merely correlates.
- **Check `hooks.csv` AND `grep -rn "RH_ScopedInstall(..., 0x<rva>)"` BEFORE writing an
  implementation.** `0x0042fa00` already had one; I shipped a second install on one RVA and
  path2 confirmed *my* export had displaced the established one. One RVA, one install.
- **But a second copy is sometimes legitimate**: some TUs are in `asi_sources.rsp` and NOT
  in `build.bat`'s exe list, so the standalone cannot reach them. That is a real reason for
  an exe-side twin — which must register nothing.
- **RVA tunnels are latent until called.** `CarSlotAssign` was in the exe build list for
  months and AV'd (`0xC0000005`) the first time the exe actually called it, because it jumps
  to `FUN_0040e480` in unmapped `.text`. Guard at the single point with
  `#ifdef MASHED_STANDALONE`.
- **A capture is a file on disk.** An all-red frame was misdiagnosed as a `SlotColour` bug;
  instrumenting showed the state was correct all along and the BMP predated the rebuild.
  Instrument rather than infer from pixels.
- **Adjacency is not relatedness.** The flag next to the team panel's belongs to a settings
  strip, not to Ability Select.
- `build_config` in `run_diff.py` is a **whitelist** — a new arg_type's CONFIG keys are
  silently dropped unless forwarded there, and the handler can go GREEN with all of them
  ignored. Prove a forwarded key by toggling it.
- OS key injection (`keybd_event` via `sa_capture`) does **not** reach this exe's
  DirectInput. Use `MASHED_TEAM_KEYS` (per-profile taps through the real active/processed
  protocol) for unattended verification.
- `MASHED_RES=800x600` is mandatory for standalone captures, and `sa_capture`'s PrintWindow
  path ignores it — use the game's own `MASHED_DBG_BBDUMP` / `MASHED_DBG_BBDUMP_OUT`.
- Ghidra MCP is hard-blocked on this account. `analyzeHeadless` + `DecompPC.java` /
  `XrefRange.java` / `CallersPC.java` against a pool slot works and IS Ghidra on the same
  binary. `XrefRange` misses computed writes (`base + i*stride + field`).

## New verification knobs (display-only, not set in normal play)

`MASHED_DEV_HUD=1`, `MASHED_TEAM_PLAY=1`, `MASHED_PLAYERS=2..4`,
`MASHED_TEAM_KEYS="0d,1dd"` (per-profile UP/DOWN taps).

## Candidate next slices

1. **Screen 15's input handler.** The only piece of the two setup screens still missing:
   ability values sit at the entry default of 1 because no ability input is ported (and
   none was invented). Find it the way `FUN_0042fa00` was found — it will be gated on that
   panel's state global in `FUN_0043c000`.
2. **`FUN_0043a610` / `FUN_0043aa30` to C2/C3 proper.** Both are read end-to-end now and
   currently sit DEFERRED in `Frontend/HudFrontendDispatchers_t4.cpp`'s catalogue. The
   geometry is transcribed into the port but the functions themselves are not hooked.
3. **Team scoring in the standalone.** Finding 24 has the rule; `TrackRenderer` scores
   per-car. Needs the race layer, not the frontend.
4. **Leave the lane.** R7 has other subsystems; the standings/setup chain is done.

## Open risks

- The per-row `A`/`B`/`-` letter and the "teams ok" verdict line are still `[SCAFFOLD]`
  presentation. The state behind them is faithful; how the original surfaces a rejected
  split (`MenuTeamBalance`'s 1/2/3 codes exist to be shown) is unmeasured.
- Screen 15's row-plate height is `[UNCERTAIN]` — taken as 28.0 by analogy because the
  4th argument decompiles as a clobbered register.
- An untracked `verify/`-style `videocfg.bin` may appear at the repo root from standalone
  runs. It is a byproduct; do not commit it.

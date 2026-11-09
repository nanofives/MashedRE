# RESULT PORT-COL-0x00418860: why the port column re-derives `body`

Branch `orch/PORT-COL-0x00418860` off orch/integration `bba25cb03`. No code changed. Verdict **DECISION**:
- The derivation changed. The call path did not.
- The committed `called` is the right value.
- The fix that keeps it right on re-derivation exposes a hidden dual copy, so it is a decision.

## 1. Which commit changed the derivation

0x00418860 has exactly one exe-side binding in `scripts/backfill_port.py`, **channel B**:
- The scanner (channel A) finds no exe body for the RVA. It sees only the asi-only `Ai/AiController.cpp:267`.
- Channel B reads `exe_side_copy` = `Ai/AiStandalone.cpp:1682` from `re/analysis/dual_copy_audit_2026-09-29.csv`
  (row 38).
- It takes the name of a definition that starts within 3 lines after that line, else the audit name `FUN_00418860`.
- `exe_bodies()` in `scripts/backfill_port.py` is the code: the `after[0] - line <= 3` test.

Name channel B reads at each first-parent commit (the same test, run on the file at that commit):

| commit (orch/integration first parent) | definition after line 1682 | channel-B name | derived port |
|---|---|---|---|
| `0674b170b` (PORT-TOP-04F merge) | 1684 `AiPreTickRubberBand` | AiPreTickRubberBand | called |
| `8a313b7d2^1` | 1684 `AiPreTickRubberBand` | AiPreTickRubberBand | called |
| **`8a313b7d2`** Merge orch/PORT-AISPLINE-DUALCOPY | 1830 `Ai_ResetVehicleStates` (148 lines away) | `FUN_00418860` (fallback, no map symbol) | **body** (`unresolved`) |
| `bba25cb03` (today) | 1830 | `FUN_00418860` | body |

What happened:
- `8a313b7d2` is the only first-parent merge since 04F that touches `Ai/AiStandalone.cpp`
  (`git log --first-parent 0674b170b^..orch/integration -- mashedmod/src/mashed_re/Ai/AiStandalone.cpp`).
- Its commit `d6ed23348` removes 7 lines above line 1682 (the compL arm).
- No build is needed for this: the map only matters once a name is chosen, and the fallback name is in no map.

**The old `called` was right by accident.** `AiPreTickRubberBand` is a different function: FUN_004177b0, called from
the tick at `Ai/AiStandalone.cpp:2132` (`AiPreTickRubberBand();   // FUN_004177b0`). It was reached only because the
tick that calls it is reached.

The exe body of 0x00418860 is **`Ai::Ai_Standalone_Tick`** (`Ai/AiStandalone.cpp:2106`):
- It is headed `// FUN_00418860 — per-frame tick` above its tracer helper.
- Its guard is `I32(kSplineRaceCnt) <= 3`, the `cmp [0x801ca0],4` that `Ai_CountdownAlivePoke` cites for 0x00418860.
- It is called from four sites in `D3d9Render/TrackRenderer.cpp` (5310, 5572, 5668 `// FUN_00418860`, 5830).

## 2. Does the default exe still run it?

**Statically, yes.** In the linked exe (`e871d3da`, the PORT-BATCH-TOOL build at `25a6fe1c5`), the backfill_port graph
reaches `?Ai_Standalone_Tick@Ai@@YAXXZ` (0x7d700, AiStandalone.obj) by
`_WinMain@16 -> RenderFrame -> TrackRenderer::UpdateCar -> Ai_Standalone_Tick`.
`git diff 25a6fe1c5 bba25cb03 -- Ai/ D3d9Render/TrackRenderer.cpp` is empty, so the same holds for today's tree.

**At run time, yes.** One boot under `test-slot` + `mashed_lock.py run`, with the default exe (switch OFF) and the
G-CTRL scenario environment:
- environment: TRAINING, `MASHED_GAME_MODE=10`, `MASHED_DETERMINISTIC=1`, `MASHED_DET_FRAMES=1200`, plus the existing
  tracer `MASHED_AI_TICKTRACE` (`Ai/AiStandalone.cpp`, `TickTrace`)
- result: exit 0 after 14 s

| tracer stage | lines |
|---|---|
| enter | 1196 |
| splines-ok | 1196 |
| pre-rubberband / post-rubberband | 1196 / 1196 |
| pre-slotstate | 1196 |
| pre-vehiclestep / post-vehiclestep | 3386 / 3386 |
| leave | 1196 |

The tick runs once per frame, all the way through, on 1196 of the 1200 deterministic frames.

**No landed swap broke a call path.** Only the derivation changed.

## 3. The correction

The committed column is correct: orch/integration's hooks.csv says `called`, and nobody has committed the `body`
re-derivation. PORT-BATCH-TOOL kept the committed value. What needs fixing is the derivation, so the next
`backfill_port.py` run does not write `body`.

re-classify cannot do that by hand: its SKILL.md line 47 says `exe_file` and `port` "are derived: never hand-edit
them". Options:

- **(a) Anchor the exe body.** Put `// 0x00418860 Ai_Standalone_Tick` directly above its definition, so channel A binds
  the real body.
  - The lint then sees what the 2026-09-29 audit already recorded and the lint never saw: exe-only `Ai_Standalone_Tick`
    against asi-only `Ai/AiController.cpp:267`. Audit row 38: **DIFFERS-BEHAVIOUR**, "missing DAT_007f0fd0==7
    force-step of vehicle 0 ...". That pair is a new CROSS-TARGET finding.
  - So (a) needs either an allowlist line (adding a line is a decision) or consolidating the pair into one body (D4
    dual-copy work, a behaviour change behind the gates).
- **(b) Point the audit cell at today's line** (`Ai/AiStandalone.cpp:2106`). One cell of a dated audit CSV. It drifts
  again on the next edit above it, and the dual copy stays hidden.
- **(c) Make channel B robust in `backfill_port.py`.** When the line misses, look for the definition whose header
  comment names the RVA. It is a script change, not a column edit. It keeps the dual copy hidden, and the comment
  heuristic is the class that misbinds call-throughs (memory scanner-second-pass-binds-call-throughs).

**Recommendation: (a) with an allowlist line now**, citing audit row 38 and this RESULT. The burn-down entry is "fold
AiController.cpp:267 into Ai_Standalone_Tick, or the reverse, under the gates". The column then derives `called` from
the real body, and the 0x00418860 dual copy is on the burn-down list where the lint can count it. I did not apply it.
The lead's instruction was no code change, and the allowlist line is a decision.

## Evidence (raw data in `D:/MashedWT/PORT-COL-0x00418860_data`, not committed)

| file | SHA-256 |
|---|---|
| ticktrace.txt (13,948 lines) | b21a4d92f11147f518c80e298c16a633d028347cc425ec95e7c040e7eb121a4b |
| tick_gctrl.log (mashed_re.log of the boot) | ba25d2ca833ecf148df7d5d3f5a35c9f36e8531433034e27e88a552e25426420 |

Exe used: `D:`-independent worktree build `.worktrees/PORT-BATCH-TOOL/mashedmod/build/mashed_re.exe`, SHA-256
`e871d3da307381f33e6991e48f30b5a347c181e5fb78146b813a410dd80305cb`, MASHED_BATCH_HUD unset. Its AI and TrackRenderer
sources are identical to `bba25cb03`. Root `D:/MashedWT/PORT-BATCH-TOOL_root`.

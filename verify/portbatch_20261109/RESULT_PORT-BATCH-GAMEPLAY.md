# PORT-BATCH-GAMEPLAY: HANDOFF (nothing swapped yet)

State: branch `orch/PORT-BATCH-GAMEPLAY` = `orch/PORT-BATCH-TOOL` `5da842022` (enqueued, not yet merged) +
`git merge orch/integration` (`bba25cb03`). Ledger row claimed by cmw9nrf9f3b7dp415piou1zs1 (parent
cmw7xu1960f37p41557tefpnd), now `handoff`. No code changed on this branch. The previous worker's context got heavy
after PORT-BATCH-TOOL and PORT-COL-0x00418860, so the batch series starts in a fresh session from here.

## Brief (L-107 (a), Mariano 2026-11-09)

- Swap the subsystem's default-reached closed set in one commit, **no knob**. This is an explicit exception to the
  default-OFF rule.
- In the same commit, delete the hand bodies the set replaces, so `scripts/lint_rva_bodies.py` stays NEW=0. Both
  targets must build.
- Gates: G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3, and the 12 nav-demo captures (04E list). Use the
  off/on/off/on repeat rule for capture flakes. "off" = the base build, "on" = the batch build.
- On any move: bisect by rebuilding with halves (build mode, see below), drop the responsible rows, and re-gate.
  Nothing that moves a hash or a capture lands. List the dropped rows with what they move.
- Order: GAMEPLAY (25), HUD (23), FRONTEND (19), AI (16), RENDER (16). One row and one commit per batch. Each batch
  branches from the previous batch branch while that one is unmerged.
- Each batch ends with `RESULT_PORT-BATCH-<SUBSYS>.md` (rows, port-column change, bisect outcome) and a REPORT to the
  lead. A REPORT before the turn ends goes through `mcp__happy__send_to_session` to cmw7xu1960f37p41557tefpnd.
- Also binding: L-106 (b) keeps MASHED_BATCH_HUD OFF. The PORT-COL anchor + allowlist for 0x00418860 waits for
  Mariano, so apply neither.

## The gameplay set

Command (exe map from the PORT-BATCH-TOOL build, `25a6fe1c5` + hud set; rebuild this branch first and pass its own
build dir for the real run):
`py -3.12 re/tools/port_closed_sets.py --sub gameplay --all`. Output kept in
`D:/MashedWT/PORT-BATCH-GAMEPLAY_data/gameplay_set.txt`.

| rva | name | port | shape | blocker | matched source |
|---|---|---|---|---|---|
| 0x00407b00 | FUN_00407b00 | called | inplace | DUP | s01/00407b00.c |
| 0x00407b70 | FUN_00407b70 | called | inplace | DUP | s01/00407b70.c |
| 0x00407d70 | RangeTableCountGroup | called | inplace | DUP | s01/00407d70.c |
| 0x00407db0 | RangeTableGroupOffset | called | inplace | DUP | s01/00407db0.c |
| 0x00409a80 | FUN_00409a80 | none | - | none | s02/00409a80.c |
| 0x0040b890 | FUN_0040b890 | called | inplace | DUP | s02/0040b890.c |
| 0x0040b8e0 | FUN_0040b8e0 | called | inplace | DUP | s02/0040b8e0.c |
| 0x0040ba60 | Active4Slots40ba60 | none | - | CROSS | s02/0040ba60.c |
| 0x0040e4b0 | Race::GetSoleFinishedPlayer | none | - | none | s02/0040e4b0.c |
| 0x00414220 | FUN_00414220 | none | - | none | s03/h2_00414220.c |
| 0x00417750 | FUN_00417750 | none | - | none | s03/h3_00417750.c |
| 0x0041f360 | GatedSwitch636ad0 | (malformed row) | inplace | DUP | s04/0041f360.c |
| 0x0042c1a0 | StateAdvance2to3 | none | - | CROSS | s07/0042c1a0.c |
| 0x00451820 | FUN_00451820 | called | - | DUP | s09/00451820.c |
| 0x0045bff0 | Pred45bff0 | none | rebind | CROSS | s10/0045bff0.c (forwarder in Match/MatchS10_0045c330.cpp) |
| 0x0045c110 | FUN_0045c110 | called | inplace | DUP | s10/0045c110.c |
| 0x0045cab0 | SlotAliveState45cab0 | none | - | CROSS | s10/0045cab0.c |
| 0x0045d0e0 | FUN_0045d0e0 | none | - | SEED | s10/0045d0e0.c |
| 0x0045dff0 | FUN_0045dff0 | none | - | none | s10/0045dff0.c |
| 0x0046be10 | Write46be10 | none | - | none | s11/0046be10.c |
| 0x0046c7b0 | VehicleSlotGetter | called | inplace | DUP | s11/0046c7b0.c |
| 0x0046cbb0 | VehicleCarStateRead | called | inplace | DUP | s11/0046cbb0.c |
| 0x0046d4a0 | PtrCompute881ec8 | called | inplace | DUP | s11/0046d4a0.c |
| 0x00472550 | Table691508Get | none | - | CROSS | s11/00472550.c |
| 0x005c9d00 | GetRaceEndTrigger | called | inplace | DUP | gffu/005c9d00.c |

## Read before swapping

1. **VEHBASE rows.** PLAN.md section 5 keeps the MATCH-S11-FU-VEHBASE rows (vehicle-record leaves, rebind decision)
   out of every batch. Check `0x0046c7b0`, `0x0046cbb0` and `0x0046d4a0` against that ledger row first. PtrCompute881ec8
   is the rebased record pointer, which the NOT-SAFE rule of PLAN.md section 4 step 1 names. If the rows are in
   VEHBASE, drop them from the batch, with their in-set callers if those need them. The tool does not know VEHBASE:
   add it as a blocker, or pass an exclude list.
2. **In-place swap (DUP rows).**
   - The wrapper must define the matched function under the **hand body's exported name and calling convention**,
     because exe callers keep calling that name. Use the PortSave01 / MATCH-S01 wrapper shape: `#define FUN_<rva>
     <HandName>`, `RH_ScopedInstall` names unchanged.
   - Then delete the hand body (and any asi copy).
   - `extern "C"` hides signature mismatches at link time, so compare the matched prototype with the hand
     declaration and every exe caller by hand.
   - `re/tools/port_batch_gen.py` today emits `PortBatch_<rva>` names and wires only `void(void) {}` forwarders. It
     needs an in-place mode: `--inplace` uses the hand name from `closed_members.tsv` `exe_body` + rva_body_scan,
     writes the wrapper into both targets when the hand body was in a shared TU, and lists the hand bodies to delete.
3. **CROSS rows.** The other body is in an asi-only TU, an old hook harness. The lint fails on an exe-only matched
   body next to it. Either compile the wrapper in **both** targets and delete the asi-only body (the asi then runs the
   matched body where it hooked: check the hook's `RH_ScopedInstall` still installs), or drop the row.
4. **SEED row 0x0045d0e0.** It reads a nonzero file-backed address the L-99 seed lacks. Either extend
   `verify/portstate_20261108/SEED_CANDIDATES.tsv` and regenerate with `re/tools/gen_portstate_seed.py` (the seed is
   default ON since L-101), or drop the row.
5. **0x0041f360** has a malformed hooks.csv row (11 fields, an unquoted comma). backfill leaves it untouched. Fix it
   through re-classify before the port-column backfill, or report it.
6. **Port column.** Run `scripts/backfill_exe_file.py` then `scripts/backfill_port.py`. Known drift: `0x00418860`
   re-derives `body` on this tree (PORT-COL-0x00418860, decision pending). Keep its committed `called` and say so.
7. **Bisect build mode** is not written yet. `re/tools/port_batch_bisect.py` today toggles
   `MASHED_BATCH_<SUB>_ROWS` at run time.
   - Add `--build-mode`: per pass, regenerate the batch for a head subset, then build with `MASHED_NO_DEPLOY=1`
     (honour the lock rule: no build during a PRIORITY-HOLD or SOLO). Gate that exe with `A4_EXE` / the knob_boot
     exe, using the DEF stack on both sides.
   - Keep the OFF/ON/OFF/ON repeat: off = base exe, on = batch exe. Copy each build's exe out before the next build
     (e.g. `D:/MashedWT/<step>_data/exe_<label>.exe`). run_arms reads `A4_EXE`. knob_boot.py's EXE is fixed to the
     worktree build, so add an override.
8. **Capture flake.** Under machine load the deterministic frontend.mpg backdrop freezes on a different frame
   (`D3d9Render/MpegVideoTexture.cpp:231-264`): PORT-BATCH-TOOL run 1 had 1/12 on an OFF pass. That is what the repeat
   rule is for. A side that does not repeat is UNSTABLE: re-run it, do not bisect it.
9. **Gates may be blind.** The hud batch never ran in any gate scenario (no `[PORT-BATCH]` log line). For in-place
   rows, check the hand bodies the gates actually execute (they are `called`), and state in the RESULT which swapped
   rows the gate scenarios cover.

## Kickoff for the next session

```
You are an L2 port worker for Mashed (lead cmw7xu1960f37p41557tefpnd). Read
.worktrees/orch-integration/re/orchestrator/WORKER_RULES.md (binding), then
.worktrees/PORT-BATCH-GAMEPLAY/verify/portbatch_20261109/RESULT_PORT-BATCH-GAMEPLAY.md (this handoff) and
verify/portbatch_20261109/RESULT_PORT-BATCH-TOOL.md. Take over PORT-BATCH-GAMEPLAY in worktree
.worktrees/PORT-BATCH-GAMEPLAY (claim it again with your session id, -Parent cmw7xu1960f37p41557tefpnd). Do
"Read before swapping" 1-7, swap the gameplay set per L-107 (a), gate, bisect if needed, write the RESULT, set review,
REPORT to the lead. Then PORT-BATCH-HUD, FRONTEND, AI, RENDER in that order, one branch, row and commit each. Hand off
again if context gets heavy.
```

# PREREG MATCH-PORTSTATE-GAMEFLOW-FU (2026-11-02)

Follow-up of MATCH-PORTSTATE-GAMEFLOW (merged, `verify/portstategameflow_20261102/RESULT_MATCH-PORTSTATE-GAMEFLOW.md`,
handoff item 1: exe bodies for the callees of the game-mode machine 0x004929d0 that had none). Base: `orch/integration`
@ 78be531e1. Source commit: `e86684a29`. Committed alone, before any arm of this step runs.

## Change

Exe built from `e86684a29` with `MASHED_NO_DEPLOY=1`: `=== Build OK ===` for both targets, `[rva-lint] 2 finding(s)
... allowlisted=2 NEW=0`, `[asm-lint] OK`. Exe SHA-256 `bda9abcb99957da93e4bf1cdb718c12aeec2504f97def111f11bfacb013e35e2`.

1. **Matched sources** (`re/match/gffu/`, XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, `re/tools/match/match.py`):
   0x004927c0 100%, 0x00417740 100%, 0x00497450 100%, 0x005c9d00 100%, the machine 0x004929d0 95.7% (residual: the
   position of the esi save, 10 push/pop instructions, see the file header). The existing matched sources of the other
   callees re-scored 100% on this tree: 0x0042c2f0, 0x0040d440, 0x0042c220, 0x0042c280, 0x00432080, 0x0040e450,
   0x0040b6e0, 0x0040b540, 0x0040b700, 0x0040e460.
2. **Exe bodies** for the callees that had none in the exe (one body per RVA, both targets unless noted):
   - moved from asi-only TUs to `Match/` TUs that compile the matched source, install kept: 0x0040e450 / 0x0040e460
     (from `Util/PromoLoop_round26.cpp`), 0x0040b6e0 and 0x00497450 (from `Util/PromoLoop_sessionB.cpp`), 0x0040b540
     (from `Gameplay/ScoreMasks_ah3.cpp`), 0x00496900 (thunk, from `Util/PromoLoop_round24.cpp`, now forwards to the
     0x00497450 body instead of calling the original by address);
   - new bodies, no install (the .asi keeps the original): 0x0040b700, 0x004927c0, and the callee 0x00417740 of
     0x0040b540;
   - existing Match TUs added to the exe list: 0x0042c2f0, 0x0042c280, 0x00432080, 0x0042c220;
   - 0x005c9d00 binds to the existing exe body `GetRaceEndTrigger` (`Util/GameStateGetters.cpp`, xor eax,eax; ret).
   Not ported: 0x0040d440 and its callee 0x0040d270 (the original course loader: piz, COURSE.LUA, 0x0040cea0,
   0x0040d110, 0x00426e10, about 24 callees). The exe's course load is TrackRenderer port code.
3. **Knob `MASHED_PORTSTATE_GAMEFLOW_FU`** (default OFF, read once, exe only, `Race/GameModeMachine.cpp`), called
   once per main-loop frame after `GameFlow_Update` (`exe_main.cpp`). Phase word `0x00771968` in {3, 7}: the matched
   machine runs (0x004927c0, then case 3 or case 7). Other phases: 0x004927c0 only (the machine's entry call). Case 2
   binds 0x0040d440 to a logging stub that the tick cannot reach (neither case 3 nor case 7 writes 2). Meant with
   `MASHED_PORTSTATE_GAMEFLOW` (which writes the phase word) and COURSE + RECORDS: stack `PSGF` in `run_arms.py`.

## Static read of what the knob writes (exe readers, `exe_sources.rsp` TUs and their included matched sources)

- 0x0042c1d0 zeroes `0x0067ea10[12]` every case-3 frame. Exe readers: none on the race path (`exe_main.cpp:325 /
  8787 / 9263` list the address in a memory-region table and a diagnostic).
- 0x0042c220 sets `0x0067ea10[id] = 1` for a slot with 0x0040e470 == 1 whose 0x00497450 predicate reads 0
  (`0x007e96fc + id * 0x200`, no exe writer). Then 0x0042c280 -> 0x0042bf30 sets `0x0067eab0 = 1`, and 0x0042c1c0
  returns it, so case 3 writes phase 7 and case 7 holds while it stays 1. Exe readers of `0x0067eab0`: the frontend
  only (`Frontend/MenuHelpers.cpp:109`, matched 0x0042aeb0 / 0x0042af50).
- Exe readers of the phase word `0x00771968` besides this step's code: `Collision/RwpIntegrator.cpp:59` (`b5cInRace`,
  phase == 3), which gates the B5c self-test A/B only, and that path returns first on the exe (`b5cSelfTestEnabled()`,
  comment at `:58`). `Boot/FrameDispatch.cpp:135` names the address as a constant.
- 0x00432080 reads the input bytes `0x007f1041 + i * 0x4c` / `0x007f1501 + i * 0x4c`. The exe writes them from the
  keyboard (`WriteMashedInputGlobalsFromKeyboard`). An arm sends no keys.
- 0x0040e450 reads `0x0063b908`, which has no exe writer, so the score block of case 3 (0x004331a0, 0x0040b6e0,
  0x0040b540, 0x0040b430, 0x0040b700, 0x00448700, 0x0040e460) does not run.
- 0x004927c0 reads input bytes `0x007f1044..47 / 0x007f10dc..df / 0x007f1504..07 / 0x007f159c..9f`. With no key its
  step word `0x00771980` stays 0 and the fill of `0x007f0a40` (step 12) does not run.

## Predictions

Gate runner: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSGF B_PSGF_K0 B_PSGF_K1
B_PSGF_K26`, private root `D:/MashedWT/MATCH-PORTSTATE-GAMEFLOW-FU_root`, wrapped in `test-slot` and `mashed_lock.py run`.

| id | prediction |
|---|---|
| P1 | knob OFF (default): H_DEF 8284be46 / ba407a16 / 31fe043e, B_DEF_K0/K1/K26 1d5bf4f9 x3 (`merge_train.json` refs). The default build differs only by unreferenced code (layout) |
| P2 | H_PSGF: step 8284be46, ptrace 31fe043e, gates.csv 67931834 (the H_PSG value of MATCH-PORTSTATE-GAMEFLOW, COURSE's abs columns); B_PSGF_K0/K1/K26 1d5bf4f9 x3 |
| P3 | H_PSGF witness: `[portstate-gameflow-fu] knob ON`, then summary lines with `machine` > 0 once the phase is 3; no `0x0040d440 reached` line; no `step 0x00771980` line; `e450=0`; `c080=0` |
| P4 | measured, not predicted [UNCERTAIN: the values 0x0040e470 returns on the gate scenario's four slots were not read statically]: whether 0x0042c220 returns non-zero, and with it whether the witness shows `phase 3 -> 7`. Either outcome is consistent with P2, since no race-path exe reader consumes `0x0067ea10` / `0x0067eab0` / `0x00771968` |

A gate move with the knob ON is a finding for the RESULT, not a flip: the knob stays default OFF. Its flip is a
decision row and needs an original-side run off Training (row-ledger note).

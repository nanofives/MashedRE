# PREREG G-AILEAF Phase 2 (written 2026-10-11, before any knob-ON run)

Registered before `MASHED_AILEAF` is switched on in a race. Phase 1 evidence: `RESULT_AILEAF.md`.

## 1. Arms (`verify/d3_gatefire_20261008/run_h4.ps1`)

| arm | knobs | role |
|---|---|---|
| `AL_CTRL` | none | G-CTRL, already run: `ae67b13a / c8317ce2 / 31db9384` |
| `AL_PP` | `MASHED_PATHPROG` | the base arm the knob stacks on |
| `AL_ON` | `MASHED_PATHPROG`, `MASHED_AILEAF`, trace `AL_ON.aileaf.tsv` | the knob |
| `AL_ON2` (to add) | same as `AL_ON` | same-arm repeat: the collateral floor |

## 2. Prediction (the decision, not the diagnosis)

**What happens on Training, per leaf**
- **0x00414570** returns 0 on every call. The original returns 0 on 6,696 / 6,696
  (`verify/d3_modes37_20261009/PREREG_P3.md:44`). With PATHPROG the progress inputs exist.
  - [UNCERTAIN] Is the exe's PATHPROG progress close enough to the original's that the "ahead within 2.0"
    test fails as it does there? The progress is non-monotone (memory gate-ordinal-progress-is-non-monotone).
- **0x00415880** returns 0 on every call. The exe has no writer for 0x007f1008 or 0x0089a508, and its
  first gate reads `table[10]` = 0 in regime A.
  - In regime C (0x0089a368 = 2, from the exe's private regime) the exe value at the VA is 0. So the
    branch taken is the regime-A one, `table[10]` = 0, return 0.

**What changes as a result**
- `mode` is never set to 1. No mode-2 accept is counted.
- The step / gates / ptrace hashes of `AL_ON` equal `AL_PP`'s.
- The two leaves' x87 work runs inside a `PcScope` that restores the PC bits. The bodies write only:
  `cand` (a stack local of the AI step), and the VA words 0x0089a508 / 4dc / 4e0, which are not read by
  any exe code (grep, 2026-10-11).

## 3. Gates

**G-AILEAF-INERT**
- `AL_ON` step / gates / ptrace sha8 == `AL_PP`'s.
- `AL_ON.aileaf.tsv` shows ret = 1 on 0 rows for both leaves.
- Calls > 0 for both leaves (coverage). A trace with no calls is VOID, not a pass.

**G-AILEAF-PC**
- Run `AL_ON` again with `MASHED_AILEAF_PC=53`: same hashes.

**If ret = 1 appears**
- Report it with the frame and the inputs.
- Do NOT keep the knob path.
- The first suspect is the PATHPROG progress (above), then the AI factor 0x0089a4c0 (no exe writer).

**Report only**
- (b) per regime (`ai_ctrl_window.py --check-regime re/tools/b_envelopes.json`) on `AL_PP` vs `AL_ON`.
- `collateral.py --mode paired`, with the floor = `AL_ON` vs `AL_ON2`.
- A D2 watch-list hit is a D2 REOPEN CANDIDATE row.

## 4. What this cannot show

- An inert pass shows the bodies are wired and harmless on Training. It does not exercise their
  return-1 arms in a race. Those arms are covered only by the synthetic replay (6,000/6,000).
- No default flip is proposed. That is a user decision.

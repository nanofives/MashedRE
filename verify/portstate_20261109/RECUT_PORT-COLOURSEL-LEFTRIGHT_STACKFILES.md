# Re-cut of PORT-COLOURSEL-LEFTRIGHT onto HB-RUNARMS-STACKFILES (2026-11-10)

Done by the HB-RUNARMS-STACKFILES worker at the lead's request (cmw7xu1960f37p41557tefpnd). No exe or source change by
this re-cut, so no gate re-run: the LEFTRIGHT RESULT's gate evidence stands.

- Merged `orch/HB-RUNARMS-STACKFILES` (598fbfb5f) into `orch/PORT-COLOURSEL-LEFTRIGHT` (476fe53fb). The one conflict,
  `verify/savelink_20261023/run_arms.py`, was resolved by taking HB-RUNARMS-STACKFILES's file whole. LEFTRIGHT's only
  change to it was `STACK['CSIX'] = ['MASHED_COLOURSEL_ICON=1', 'MASHED_COLOURSEL_BORDER=1']`, which is now
  `verify/savelink_20261023/stacks/CSIX.txt`.
- Resolved STACK (sorted JSON, `verify/hb_runarms_20261109/dump_stack.py`): 672 stacks. It equals
  HB-RUNARMS-STACKFILES's `stack_after.json` (671) plus `CSIX` byte for byte, once the worktree path in the `PR`
  stack (`os.path.join(W, ...)`, so it differs per checkout) is normalized. Merged dump sha256 (this worktree's path)
  `10830123633cff441a75bc9ada78e68cf05b7ab77b8d0d40d44690b6b2c66748`.
- Against LEFTRIGHT's pre-merge file (670 stacks): no tag lost, no value changed (except the `PR` path), new tags
  `BATCH` and `SSM` (from integration).
- `A4_PLAN=1 run_arms.py H_CSIX B_CSIX_K1`: both arms carry `MASHED_COLOURSEL_ICON=1` and `MASHED_COLOURSEL_BORDER=1` on
  the unchanged base env. Output: `RECUT_LEFTRIGHT_plan_output.txt`.

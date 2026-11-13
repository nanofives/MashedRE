# HANDOFF RO-BIND-TIMER (reboot pause, 2026-11-12 ~11:55)

Worker session cmwdfm88660husc1b6yzdc34c (claude3), parent lead cmwddbai85n02sc1bwkkpmtb7. Ledger row: `working`.
Worktree `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-BIND-TIMER`, branch `orch/RO-BIND-TIMER` @ 684d9a73b.

## Done
- f54dbbbb1: exe-only `Match/PortTop_00493390.cpp` (matched 00493390.c, callee 0x004950b0 -> QpcTimeScaledTo3Mhz).
  `MatchS15_00493480.cpp` binds FUN_00493390 to it in the exe. `MatchS15_004926c0.cpp` binds 0x004950b0 / 0x00496930
  to exe bodies, and 0x0045d3f0 / 0x00466b50 / 0x0045d430 to named stops. `MatchS16_00496930.cpp` added to
  exe_sources.rsp. Dispatchers + per-call self-test in `Match/PortTop_00492370.cpp`. BINDINGS.tsv rows updated.
- Run 1 (f54dbbbb1, both builds Build OK, ddkobj check IDENTICAL 716/798, all 5 of this row's units IDENTICAL):
  vs_off / vs_on_knoboff / ddk_on_knoboff rc 0, 600 frames. vs_on and ddk_on with MASHED_OT_TIMER=1 stopped on
  frame 0 at 0x0045D3F0: DAT_006147b8 is file-initialised to 1 (.data), so the original runs the audio branch of
  0x004926c0 every frame, and its callees have no exe body. Logs: D:/MashedWT/RO-BIND-TIMER_data/*.log, arms.out.
- 684d9a73b: 0x004926c0 moved to its own sub-knob MASHED_OT_AUDIOTICK (default OFF). MASHED_OT_TIMER now binds
  0x00493480 / 0x00493390 only. arms script gained a vs_audio arm (TIMER + AUDIOTICK, expected STOP 0x0045d3f0).

## Next
1. Rebuild both targets if the background build (started ~11:52) did not finish: logs
   D:/MashedWT/RO-BIND-TIMER_build_vs2.log, _build_ddk2.log. Check BYTECHECK rows 00493390/00493480/004926c0/
   00496930/004950b0 IDENTICAL.
2. When `mashed_lock.py status` shows no PRIORITY-HOLD/SOLO: `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/
   mashed_lock.py run --label RO-BIND-TIMER -- bash verify/robindtimer_20261112/timer_arms.sh'` (root
   D:/MashedWT/RO-BIND-TIMER_root, junction to PORT-TOP-04J_rootK/original). Expect vs_on / ddk_on 600 frames and
   `[OT_TIMER] self-test PASS (0 of N checks failed)`.
3. Write verify/robindtimer_20261112/RESULT_RO-BIND-TIMER.md (self-test line, builds, ddkobj, the 0x004926c0 finding:
   needs bodies for 0x0045d3f0 (callees 0x00550be0 / 0x005a60b0), 0x00466b50 (57 callees), 0x0045d430 (0x005a60e0)),
   commit, `roadmap.ps1 set RO-BIND-TIMER review -Evidence ...`, final REPORT block.
Note: the first object build ran while a merge-train PRIORITY-HOLD appeared (check at its start missed it). Say so in the RESULT.

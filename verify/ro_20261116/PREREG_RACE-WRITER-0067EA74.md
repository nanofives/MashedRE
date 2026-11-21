# PREREG RACE-WRITER-0067EA74 (run 53, 2026-11-21)

Branch `orch/RACE-WRITER-0067EA74` from `orch/integration` `a9e5b4c8a`. Committed alone, before any arm.

## Question
At tick 299 (from boot) `0x0067ea74` is 1 on ref1 and 0 on the ORIG_TOP exe arm (RESULT_PORT-OT-RACE.md section 7.2), so the
matched 0x004264d0 returns after 0x0042fe30 on the exe. Which instruction writes `0x0067ea74` on the original, from which
return chain, and at which tick?

## Arm W1 (original, ref1 recipe + a write watch)
`py -3.12 verify/ro_20261116/writer_arms.py w1 -- --watch 0x0067ea74 --watch-from install --watch-stack 16`
(ref1 argv of `REFERENCE_RO-REF-NAME.json`, root `D:\MashedWT\RACE-WRITER-0067EA74_root` = robocopy of
`RO-REF-FIXED-LOAD_root`, MASHED.exe `b9977dab...fe87`, data `D:/MashedWT/RACE-WRITER-0067EA74_data/w1`). Under test-slot
(outside) and mashed_lock (inside). A piz-collision crash (`+0x1509b4`, `+0xe66db`) is a re-run.

## Predictions (from reading code, [UNCERTAIN] until W1)
1. At least one hit before tick 299 whose value after the write is 1.
2. The writing pc (the instruction after the store) is inside 0x00431d00..0x00431d61 (`re/analysis/boot_app_init_d2/00431d00.md`
   line 13: writes 1 to `DAT_0067ea74`). hooks.csv lists its callers as 0x0043dfd0 and 0x004924f0.
3. W1's ckpt 299 holds `0x0067ea74` = 1 (as ref1).

## Pass / stop
- If prediction 2 holds and 0x00431d00 plus its caller are matched sources the race_owned list allows: bind on ORIG_TOP under
  `MASHED_OT_RACELOAD=1` (default OFF), then both builds, `ddkobj.py check`, knob boot, self-test, replay oracle on the object
  build. Report load length against ref1's 108 and the first divergence record.
- If the writer is unmatched, C2, cpp-unclean, or owned by another row (audio_owned.txt, bulk_cp1.tsv, the RACE rows named in the
  task): no work-around, the RESULT names its RVA and the exact first stop.
- No default flips, no G-CTRL, no B, no 12x4 captures (top-down row).

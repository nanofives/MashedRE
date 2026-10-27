# PREREG D4-WSB-BVH-FU (committed before any arm)

Base: `orch/integration` @ `cb1ba12de` (contains `orch/D4-WSB-BVH-WRITESET` @ `130929d9d`). Before the build and
any arm, `orch/integration` is merged in again once it contains the D4-WSB-NARROW-FLIP merge commit
(`git merge-base --is-ancestor` checked and recorded), so every arm runs on the new default
(`MASHED_NARROW_DOT_ORDER`, `MASHED_NARROW_PAIR_INT`, `MASHED_LEAVES1_X87` ON). Prior evidence:
`verify/wsbbvh_20261027/RESULT_D4-WSB-BVH-WRITESET.md`. Rows: `0x00564c80`, `0x00561390`, `0x00563e70`, `0x00565200`
(all C3).

## Changes under test (harness only, no knob, no default)

1. `Core/ShadowTrack.h`: a divergent `RunTracked` sample also logs, for the first differing page, the count of differing
   dwords (`ndw=`) and the first 4 of them as `off:pre/orig/port` (`v=`). `pre` = the value before either run. Only the
   armed path (`MASHED_SHADOW_AB=1`) reaches it. `Core/ShadowAB.h`: the log line buffer goes 256 -> 512 bytes.
2. `c4_racediff_v7.py` (hand copy of v6, 4 asserted replacements): the census OFF boot passes `--hooks __none__`
   instead of no `--hooks`. Reason (static, `mashedmod/src/dinput8_shim/dinput8_shim.cpp` `LoadDevAsi`): with no
   `--hooks`, `scenario_launch.py:5036` sets `MASHED_RE_NO_AUTO_HOOK=1`, and the shim then returns before
   `LoadLibraryW(mashed_re_dev.asi)`. So the v6 OFF boot never had the `.asi`, `asiloaded()` stayed false for the
   whole race (the kept `launcher_off.txt` shows a full 58 s race), and the rpc died with the process. The old PREREG 4
   text already asked for ".asi loaded, nothing installed": `__none__` gives exactly that (`MASHED_HOOK_ONLY` matches no
   token, `HookSystem.cpp:209`), the same OFF env the Quick Battle driver uses (`c4_racediff_v6.py:208`). The poll also
   stops when the launcher exits, and an OFF/ON boot with no `.asi` reports no capture.

## Arms and verdict rules (fixed now)

1. **Gates** on this branch's exe: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`. Must equal
   `merge_train.json` `gctrl.ref` / `b.ref` (G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664`). Any
   hash off = stop, no promotion, report DECISION.
2. **V (diagnostic, `0x00564c80`).** `run_shadow2.py 00564c80`, census scenario, no STW. Records the values at
   `0x007dc8c8` per divergent sample. Not a verdict leg: `0x00564c80` stays C3 in this step whatever V shows. The RESULT
   reports `pre`, `orig - pre`, `port - pre` per sample, and names the next step from them only.
3. **S (verdict, `0x00561390`).** `MASHED_SHADOW_STW=1 SHADOW_TAG=stw run_shadow2.py 00561390`, census scenario, every
   other thread suspended during each A/B window (heap locks held). CLEAN = n >= 20 samples, 0 divergent, `PATCHBYTE
   installed=e9 A/B-IS-REAL`. Any divergent sample = stays C3. Basis: a write-set compare measures the function's own
   writes. Another thread's write inside the window is not one. STW removes that thread from the window and changes
   nothing else in the compare. One re-run only if the boot gives n < 20 or no log.
   S2 (diagnostic, not a verdict): the same row without STW (`SHADOW_TAG=nostw`), to record the values of any
   `0x007dce08` divergence.
4. **R (Quick Battle racediff, `0x00561390`)** on this exe, `run_bvh3.py 00561390`, tool v7 (Quick Battle path is v6
   unchanged). C4-CLEAN rule unchanged from the v5/v6 tool.
5. **W (write-set re-run, `0x00563e70`, `0x00565200`)** on this exe, `run_shadow2.py 00563e70 00565200`. Rule = old
   PREREG 2 (CLEAN = n >= 1, 0 divergent, `A/B-IS-REAL`).
6. **C (census racediff, `0x00563e70`, `0x00565200`)**: `C4_SCENARIO=census C4_WINDOW="1 3000" run_bvh3.py 00563e70
   00565200`. ON `--hooks 0x<rva>`, OFF `--hooks __none__`, both `MASHED_NO_SELFTEST=1`. C4-CLEAN rule unchanged from
   the v5 tool. Counts only if both sides logged `armed: asi=True` and captured >= 1 call of the row. Not observed on
   both sides after one re-run = stays C3.

A row goes C3 -> C4 only with a C4-CLEAN racediff on this exe AND a CLEAN write-set leg on this exe:
`0x00561390` needs S + R, `0x00563e70` / `0x00565200` need W + C. `0x00564c80` stays C3. Promotions through the
re-classify skill only. Every boot under the test slot and `mashed_lock.py run --label D4-WSB-BVH-FU`, private root
`D:/MashedWT/D4-WSB-BVH-FU_root`, raw data in `D:/MashedWT/D4-WSB-BVH-FU_data`.

# RESULT D5-MENUANIMSPEED (GREEN)

Deleted the MASHED_MENU_ANIM_SPEED switch (L-35 opt-in). `MenuAnimSpeed()` is gone from
`Frontend/MenuAnimTick.h` and `MenuAnimTickTwin.cpp`; `base` is now `(in.phase >= 4) ? 2.0f : 1.0f`
(the old `x * 1.0f` was exact, so default behaviour is bit-identical). Comment in `MenuNavSM.cpp` updated.
MASHED_NO_MENU_CLOCK_FAITHFUL (exe_main.cpp:5165) untouched and still works.
Historic scripts under verify/menuspeed_20261027 still name the removed env var (frozen evidence, not edited).

Branch contains D4-WSB-NARROW-FLIP (fc09a9e04 is an ancestor of HEAD, merged orch/integration in before gating).

## Menu timing (verify/menuanimspeed_20261027/run_exe_timing.py, output timing_run.txt)
- F0 default: item settle call 16 (ticks 16), item freeze 41, back settle 40, back freeze 90.
- F1 MASHED_NO_MENU_CLOCK_FAITHFUL=1: item settle 29, freeze 70, back settle 68, freeze 150 (opt-out still distinct).

## Gates (default exe, savelink_20261023/run_arms.py, data in D:/MashedWT/D5-MENUANIMSPEED_data/arms)
G-CTRL 5943c5f1/364350ae/52052890, B 3a03670c/e85ea6fb/53778664: all equal the merge_train.json refs.
Raw files stay in the data dir, log gate_run.txt there.

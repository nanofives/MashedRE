"""re/frida/early_window_leaf_diff.py pointed at the private root: same harness and registry (negctl fixtures
included), only ORIG_DIR / EXE move to D:\\MashedWT\\D4-DC-RFX-CORE_root\\original and ASI is this worktree's
mashedmod/build/mashed_re_dev.asi (its own MASHED_EW_ASI override, set here). The lane the ten handler-less
registry arg_types (deref_p1field_glob_set, arg_default_memcpy_abs, copy_arg_to_globals, multi_deref_global_set,
multi_array_scatter) were earned on. Usage: py -3.12 run_ew_private.py <hook>; prints VERDICT: GREEN|RED."""
import os
import sys

WT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ROOT_ORIG = r"D:\MashedWT\D4-DC-RFX-CORE_root\original"
sys.path.insert(0, os.path.join(WT, 're', 'frida'))
import early_window_leaf_diff as EW  # noqa: E402

EW.ORIG_DIR = ROOT_ORIG
EW.EXE = os.path.join(ROOT_ORIG, 'MASHED.exe')
EW.ASI = os.path.join(WT, 'mashedmod', 'build', 'mashed_re_dev.asi')

if __name__ == '__main__':
    n = sys.argv[1]
    print(f"hook: {n}  (early-window, private root {ROOT_ORIG}, asi {EW.ASI})")
    ok = EW.run(n)
    print("VERDICT:", "GREEN" if ok else ("RED" if ok is False else "ERROR/REFUSED"))
    sys.exit(0 if ok else 1)

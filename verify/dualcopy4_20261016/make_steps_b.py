"""steps for window B (the final tree of this lane): steps_b1.json = path1 only (run_seq.py, no shared-asi deploy: run_diff.py loads
THIS worktree's mashedmod/build/mashed_re_dev.asi itself and spawns the original with auto-hook off), steps_b2.json = what needs the
shared asi (path2 call-through, early-window leaf diff, nav boot, install manifest), run through window.py.

Keys cover every RVA this lane touched: Timer x4 (+ the two path2 vectors repaired), 0x00431b70, 0x004c5c00 (early-window lane),
0x00420de0, 0x00405890, 0x0042f6b0, 0x0042ad90, 0x0042fa00.
"""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
PY = ["py", "-3.12"]
D2 = "verify/dupinstall2_20261016"
P1 = ["timer_bit_field_set", "timer_slot_data_copy", "timer_array_clear_d40", "timer_init_loop", "menu_flag_dat_007f0f10_get",
      "accum_420de0", "pred_405890", "menu_mode_sync", "find_42ad90", "menu_team_select_tick"]
P2 = P1 + ["search_4c5c00"]          # search_4c5c00 has no run_diff handler: path1 below
s1 = [{"name": f"path1_{k}", "timeout": 300, "cmd": PY + ["re/frida/run_diff.py", k]} for k in P1]
s2 = [{"name": "path1_ew_search_4c5c00", "timeout": 300, "cmd": PY + ["re/frida/early_window_leaf_diff.py", "search_4c5c00"],
       "env": {"MASHED_EW_ASI": "{ASI}"}}]
s2 += [{"name": f"path2_{k}", "timeout": 300, "cmd": PY + ["re/frida/run_verify_hook.py", k], "env": {"MASHED_VERIFY_ASI": "{ASI}"}} for k in P2]
s2 += [{"name": "nav_probe_full_hookset", "timeout": 240, "cmd": PY + [f"{D2}/nav_probe.py", "fix_full"]},
       {"name": "install_observe_all", "timeout": 200, "cmd": PY + [f"{D2}/install_observe.py", "ALL", "--seconds", "45"],
        "env": {"DUPINSTALL_OUT": "{OUT}/install_all"}}]
(HERE / "steps_b1.json").write_text(json.dumps(s1, indent=1), encoding="utf-8")
(HERE / "steps_b2.json").write_text(json.dumps(s2, indent=1), encoding="utf-8")
print(len(s1), "path1 steps,", len(s2), "window steps")

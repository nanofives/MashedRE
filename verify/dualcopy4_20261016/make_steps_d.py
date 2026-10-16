"""steps for the FINAL tree (image d): steps_d1.json = path1 via run_diff.py (no shared-asi deploy), steps_ew_d.json = the early-window lane with
MASHED_EW_ASI = images/d.asi, steps_d2.json = what needs the shared asi (path2 call-through, nav boot, install manifest) for window.py.
Every key whose RVA this lane touched, plus menu_entry_array_init (0x0042d3e0, rewritten)."""
import json, pathlib

HERE = pathlib.Path(__file__).resolve().parent
PY = ["py", "-3.12"]
D2 = "verify/dupinstall2_20261016"
asi = str((HERE / "images" / "d.asi").resolve())
P1 = ["timer_bit_field_set", "timer_slot_data_copy", "timer_array_clear_d40", "timer_init_loop", "menu_flag_dat_007f0f10_get",
      "menu_mode_sync", "menu_team_select_tick", "menu_entry_array_init"]
EW = ["accum_420de0", "pred_405890", "find_42ad90", "search_4c5c00"]
P2 = P1 + ["accum_420de0", "pred_405890", "search_4c5c00"]
s1 = [{"name": f"path1_{k}", "timeout": 300, "cmd": PY + ["re/frida/run_diff.py", k]} for k in P1]
sew = [{"name": f"ew_{k}", "timeout": 300, "cmd": PY + ["re/frida/early_window_leaf_diff.py", k], "env": {"MASHED_EW_ASI": asi}} for k in EW]
s2 = [{"name": f"path2_{k}", "timeout": 300, "cmd": PY + ["re/frida/run_verify_hook.py", k], "env": {"MASHED_VERIFY_ASI": "{ASI}"}} for k in P2]
s2 += [{"name": "nav_probe_full_hookset", "timeout": 240, "cmd": PY + [f"{D2}/nav_probe.py", "fix_full"]},
       {"name": "install_observe_all", "timeout": 200, "cmd": PY + [f"{D2}/install_observe.py", "ALL", "--seconds", "45"],
        "env": {"DUPINSTALL_OUT": "{OUT}/install_all"}}]
for n, s in (("steps_d1.json", s1), ("steps_ew_d.json", sew), ("steps_d2.json", s2)):
    (HERE / n).write_text(json.dumps(s, indent=1), encoding="utf-8")
print(len(s1), "path1,", len(sew), "early-window,", len(s2), "window steps; asi =", asi)

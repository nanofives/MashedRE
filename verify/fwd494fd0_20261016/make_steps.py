"""steps_window1.json: re-earn window for 0x00495080 with the FULL hook set (no MASHED_HOOK_SKIP anywhere).
Reuses verify/dupinstall2_20261016/window.py (backup + no-foreign-MASHED poll + restore in a finally)."""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
PY = ["py", "-3.12"]
D2 = "verify/dupinstall2_20261016"
steps = [
    # nav boot to a race with every hook installed: must reach the race and stay alive (was AV EIP 0x1)
    {"name": "nav_probe_full_hookset", "timeout": 240, "cmd": PY + [f"{D2}/nav_probe.py", "fix_full"]},
    # the row itself
    {"name": "path2_input_axis_scale_bias_apply", "timeout": 300,
     "cmd": PY + ["re/frida/run_verify_hook.py", "input_axis_scale_bias_apply"], "env": {"MASHED_VERIFY_ASI": "{ASI}"}},
    # the three race-scenario rows that needed the SKIP workaround, now without it
    {"name": "path2_table_773030_get", "timeout": 300,
     "cmd": PY + ["re/frida/run_verify_hook.py", "table_773030_get"], "env": {"MASHED_VERIFY_ASI": "{ASI}"}},
    {"name": "path2_plugin_data_dword_a", "timeout": 300,
     "cmd": PY + ["re/frida/run_verify_hook.py", "plugin_data_dword_a"], "env": {"MASHED_VERIFY_ASI": "{ASI}"}},
    {"name": "path2_plugin_data_dword_b", "timeout": 300,
     "cmd": PY + ["re/frida/run_verify_hook.py", "plugin_data_dword_b"], "env": {"MASHED_VERIFY_ASI": "{ASI}"}},
    # full hook set, boot + 45 s survival, with crash handler, manifest names the installs
    {"name": "install_observe_all", "timeout": 200,
     "cmd": PY + [f"{D2}/install_observe.py", "ALL", "--seconds", "45"], "env": {"DUPINSTALL_OUT": "{OUT}/install_all"}},
]
(HERE / "steps_window1.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "steps")

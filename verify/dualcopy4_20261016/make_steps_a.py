"""steps_a.json: window A of the dualcopy4 lane, the asi with the group-(a) twin merges.

  path1 / path2   the KEPT registry key of every merged RVA (Timer x4, 0x00431b70, 0x004c5c00)
  nav boot        full auto-hook set reaches a race alive
  install_observe ALL, 45 s (manifest names every kept install once)
py -3.12 make_steps_a.py  -> steps_a.json
"""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
PY = ["py", "-3.12"]
D2 = "verify/dupinstall2_20261016"
KEYS = ["timer_bit_field_set", "timer_slot_data_copy", "timer_array_clear_d40", "timer_init_loop",
        "menu_flag_dat_007f0f10_get", "search_4c5c00"]
steps = []
for k in KEYS:
    steps.append({"name": f"path1_{k}", "timeout": 300, "cmd": PY + ["re/frida/run_diff.py", k]})
    steps.append({"name": f"path2_{k}", "timeout": 300, "cmd": PY + ["re/frida/run_verify_hook.py", k],
                  "env": {"MASHED_VERIFY_ASI": "{ASI}"}})
steps.append({"name": "nav_probe_full_hookset", "timeout": 240, "cmd": PY + [f"{D2}/nav_probe.py", "fix_full"]})
steps.append({"name": "install_observe_all", "timeout": 200,
              "cmd": PY + [f"{D2}/install_observe.py", "ALL", "--seconds", "45"], "env": {"DUPINSTALL_OUT": "{OUT}/install_all"}})
(HERE / "steps_a.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "steps")

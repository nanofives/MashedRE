"""Write steps_window1.json for window.py (one deploy window: install observation, path2 for the survivors,
the strict C4 tools for the five flagged rows)."""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
rvas = ",".join((HERE / "rvas27.txt").read_text().split())
menu_keys = ["copter_find_639dc4", "float_idx_639de0", "get_ai_lap_counter", "ret_63a5f0", "bool0_out_8a94e0",
             "maxscoreflags_40b9a0", "boolmin_out_8a94e0", "active4slots_40ba60", "slot_index_to_ptr",
             "clear_67d99c_x4", "menu_readiness_check_c", "hud_rect_emitter5", "frontend_page_id_dispatch",
             "dinput_init_predicate", "display_active_flag_get", "set_dat_00912a20", "get_dat_00912a20",
             "audio_list_init"]
race_keys = ["table_773030_get", "plugin_data_dword_a", "plugin_data_dword_b", "vehicle_car_state_read"]
PY = ["py", "-3.12"]
steps = [{"name": "install_after", "timeout": 120,
          "cmd": PY + ["verify/dupinstall2_20261016/install_observe.py", rvas, "--seconds", "30"],
          "env": {"DUPINSTALL_OUT": "{OUT}/install_after"}}]
for k in menu_keys + race_keys:
    steps.append({"name": "path2_" + k, "timeout": 300,
                  "cmd": PY + ["re/frida/run_verify_hook.py", k], "env": {"MASHED_VERIFY_ASI": "{ASI}"}})
steps.append({"name": "c4_verify", "timeout": 300,
              "cmd": PY + ["verify/dupinstall2_20261016/c4_verify.py", "0x00498bf0,0x004098a0,0x0040b9a0,0x0040ba60,0x00407a20",
                           "--seconds", "25"],
              "env": {"DUPINSTALL_OUT": "{OUT}/c4_verify"}})
steps.append({"name": "c4_racediff", "timeout": 900,
              "cmd": PY + ["verify/dupinstall2_20261016/c4_racediff.py", "--rvas",
                           "0x00407a20,0x004098a0,0x0040b9a0,0x0040ba60,0x0046cbb0"],
              "env": {"DUPINSTALL_OUT": "{OUT}/c4_racediff"}})
(HERE / "steps_window1.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "steps")

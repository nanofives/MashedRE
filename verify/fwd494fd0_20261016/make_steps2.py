"""steps_window2.json: re-earn window for the two consolidations of Part C, on ONE build:

  0x0040e480 CarSlotStateSet -> Ai/CarSlotState.cpp        (path1 + path2 + live)
  0x00417180 AiBankSwitch    -> Ai/AiBankSwitch.cpp        (the in-process A/B driver AbAiBank in a live race, twice)

Reuses verify/dupinstall2_20261016/window.py (backup + no-foreign-MASHED poll + restore in a finally).

  path1   run_diff.py car_slot_state_set          hook BYPASSED, ptr_ptr_entity_set on the worktree export
  path2   run_verify_hook.py car_slot_state_set   the E9 + rel32 lands on the new TU's export, 2/2 entries
  ab      ab_run.py AbAiBank (track 0, track 3)   MINE (the shared body) vs ORIG, whole write surface, live race
  nav     nav_probe.py fix_full                   FULL hook set, drive to a race, process stays alive
  nav     nav_probe.py --only <hook>              ONLY that hook live, drive to a race (the original's AI tick calls the
                                                  new body for real: FUN_00418860 -> FUN_0040e480 / FUN_00418560 -> FUN_00417180)
  boot    install_observe.py ALL                  full hook set, 45 s, manifest names the installs
"""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
PY = ["py", "-3.12"]
D2 = "verify/dupinstall2_20261016"
AB = "verify/fwd494fd0_20261016/ab_run.py"
steps = [
    {"name": "path1_car_slot_state_set", "timeout": 300, "cmd": PY + ["re/frida/run_diff.py", "car_slot_state_set"]},
    {"name": "path2_car_slot_state_set", "timeout": 300,
     "cmd": PY + ["re/frida/run_verify_hook.py", "car_slot_state_set"], "env": {"MASHED_VERIFY_ASI": "{ASI}"}},
    {"name": "ab_AbAiBank_track0", "timeout": 700,
     "cmd": PY + [AB, "{OUT}/ab", "track0", "AbAiBank", "ai_ab_bank.log", "--track", "0", "--cars", "4", "--hold", "120"]},
    {"name": "ab_AbAiBank_track3", "timeout": 700,
     "cmd": PY + [AB, "{OUT}/ab", "track3", "AbAiBank", "ai_ab_bank.log", "--track", "3", "--cars", "4", "--hold", "120"]},
    {"name": "nav_probe_full_hookset", "timeout": 240, "cmd": PY + [f"{D2}/nav_probe.py", "fix_full"]},
    {"name": "nav_probe_only_CarSlotStateSet", "timeout": 240,
     "cmd": PY + [f"{D2}/nav_probe.py", "--only", "CarSlotStateSet", "only_slotstate"]},
    {"name": "nav_probe_only_AiBankSwitch", "timeout": 240,
     "cmd": PY + [f"{D2}/nav_probe.py", "--only", "AiBankSwitch", "only_bank"]},
    {"name": "install_observe_all", "timeout": 200,
     "cmd": PY + [f"{D2}/install_observe.py", "ALL", "--seconds", "45"], "env": {"DUPINSTALL_OUT": "{OUT}/install_all"}},
]
(HERE / "steps_window2.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "steps")

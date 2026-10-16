"""steps_w1.json: window 1 of the dualcopy3 lane, the c12 asi (0x00417640 shared body + FtolVerbatim).

  path1 / path2   ai_post_step_powerup_brake (registry key added by this lane; cache_setter_observe over the gate globals)
  AbAiPost        in-process A/B driver, live race. scenario_launch --track is the ENGINE track index (0 = Training, 11 = Warzone), NOT
                  the Course_Id: Warzone is Course_Id 33 = 0x21 (Race/GameFlow.cpp kAreas), where the gate should pass NATURALLY
                  (the driver counts gate1pass), and Training (Course_Id 30), where only the driver's seeded forced passes reach the
                  body. MINE = the shared body, ORIG = the original through the prologue-re-exec trampoline
  nav boots       full hook set, and ONLY AiPostStepPowerupBrake live
  boot            install_observe ALL (manifest names the install)
"""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
PY = ["py", "-3.12"]
D2 = "verify/dupinstall2_20261016"
AB = "verify/dualcopy3_20261016/ab_run.py"
steps = [
    {"name": "path1_ai_post_step_powerup_brake", "timeout": 300, "cmd": PY + ["re/frida/run_diff.py", "ai_post_step_powerup_brake"]},
    {"name": "path2_ai_post_step_powerup_brake", "timeout": 300,
     "cmd": PY + ["re/frida/run_verify_hook.py", "ai_post_step_powerup_brake"], "env": {"MASHED_VERIFY_ASI": "{ASI}"}},
    {"name": "ab_AbAiPost_warzone", "timeout": 700,
     "cmd": PY + [AB, "{OUT}/ab", "warzone", "AbAiPost", "ai_ab_poststep.log", "--track", "11", "--cars", "4", "--hold", "120"]},
    {"name": "ab_AbAiPost_track0", "timeout": 700,
     "cmd": PY + [AB, "{OUT}/ab", "track0", "AbAiPost", "ai_ab_poststep.log", "--track", "0", "--cars", "4", "--hold", "120"]},
    {"name": "nav_probe_full_hookset", "timeout": 240, "cmd": PY + [f"{D2}/nav_probe.py", "fix_full"]},
    {"name": "nav_probe_only_AiPostStepPowerupBrake", "timeout": 240,
     "cmd": PY + [f"{D2}/nav_probe.py", "--only", "AiPostStepPowerupBrake", "only_post"]},
    {"name": "install_observe_all", "timeout": 200,
     "cmd": PY + [f"{D2}/install_observe.py", "ALL", "--seconds", "45"], "env": {"DUPINSTALL_OUT": "{OUT}/install_all"}},
]
(HERE / "steps_w1.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "steps")

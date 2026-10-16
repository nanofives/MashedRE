"""steps_window2.json: the race-scenario path2 rows with the miscompiled hook skipped, plus the nav probe with and
without it on the worktree build (window.py deploys the worktree .asi for the window)."""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
PY = ["py", "-3.12"]
SKIP = {"MASHED_HOOK_SKIP": "Fwd494fd0_DoublePlusHalf"}
steps = [
    {"name": "nav_probe_full_hookset", "timeout": 200,
     "cmd": PY + ["verify/dupinstall2_20261016/nav_probe.py", "w2_full"]},
    {"name": "nav_probe_skip_fwd494fd0", "timeout": 200,
     "cmd": PY + ["verify/dupinstall2_20261016/nav_probe.py", "--env", "MASHED_HOOK_SKIP=Fwd494fd0_DoublePlusHalf", "w2_skip"]},
]
for k in ("table_773030_get", "plugin_data_dword_a", "plugin_data_dword_b"):
    steps.append({"name": "path2_" + k, "timeout": 300, "cmd": PY + ["re/frida/run_verify_hook.py", k],
                  "env": dict(SKIP, MASHED_VERIFY_ASI="{ASI}")})
(HERE / "steps_window2.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "steps")

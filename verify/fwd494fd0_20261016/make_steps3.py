"""steps_window3.json: path2 for car_slot_state_set again, after the registry's path2_tests were put in the {'scalars': [...]}
shape the verify template applies (window 2's path2 installed fine, E9 + rel32 OK, but the call-through died 'bad argument count'
because a bare [0, 1] fell through to fn(input))."""
import json
import pathlib

HERE = pathlib.Path(__file__).resolve().parent
steps = [
    {"name": "path2_car_slot_state_set", "timeout": 300,
     "cmd": ["py", "-3.12", "re/frida/run_verify_hook.py", "car_slot_state_set"], "env": {"MASHED_VERIFY_ASI": "{ASI}"}},
]
(HERE / "steps_window3.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "step")

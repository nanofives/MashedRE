"""steps_ew.json: the early-window leaf lane (re/frida/early_window_leaf_diff.py) for the four keys whose arg_type run_diff.py has no handler
for (accum_420de0 fastcall_float_clamp, pred_405890 two_global_predicate, find_42ad90 edx_ebx_edi_find, search_4c5c00 circular_str_search_ci).
The lane spawns the original with MASHED_RE_NO_AUTO_HOOK=1 and LoadLibraryW's the .asi itself, from MASHED_EW_ASI, so the shared .asi is NOT touched.
    py -3.12 make_steps_ew.py <abs path of the .asi under test>   ->  steps_ew.json
"""
import json, pathlib, sys

HERE = pathlib.Path(__file__).resolve().parent
asi = str(pathlib.Path(sys.argv[1]).resolve())
steps = [{"name": f"ew_{k}", "timeout": 300, "cmd": ["py", "-3.12", "re/frida/early_window_leaf_diff.py", k], "env": {"MASHED_EW_ASI": asi}}
         for k in ("accum_420de0", "pred_405890", "find_42ad90", "search_4c5c00")]
(HERE / "steps_ew.json").write_text(json.dumps(steps, indent=1), encoding="utf-8")
print(len(steps), "steps, asi =", asi)

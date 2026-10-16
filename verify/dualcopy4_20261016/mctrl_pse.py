"""mctrl_pse.py : the twin control for 0x0042fa00. Runs run_diff's path1 for a TEMPORARY registry key 'mctrl_pse' = the current menu_team_select_tick entry
(9 vectors, incl. the two out-of-range ones v7/v8) but with export 'PlayerSlotEdgeAdjust', i.e. the asi twin this lane deleted. Requires the asi in
mashedmod/build/ to be one that still exports it (images/base.asi). The key is NOT written to the registry file.
    py -3.12 mctrl_pse.py
Expected: RED on exactly the vectors where the original and the clamped twin differ (v7, v8)."""
import runpy, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "re" / "frida"))
import hooks_registry as HR  # noqa: E402

e = dict(HR.HOOKS["menu_team_select_tick"])
e["export"] = "PlayerSlotEdgeAdjust"
e.pop("path2_tests", None)
HR.HOOKS["mctrl_pse"] = e
sys.argv = [str(ROOT / "re" / "frida" / "run_diff.py"), "mctrl_pse"]
runpy.run_path(sys.argv[0], run_name="__main__")

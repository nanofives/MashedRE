# RESULT RC-D11075-LAUNCHER (text only)

DEFERRED D-11075 named the launcher `dadacde6`. L-79 A (answered 2026-11-01) retired
`scripts/mashed_launch.ps1`, the two `Play Mashed*.bat` and `build_qol_asi.bat`
(merged as `d3f81a09d`, D5-SHIM-STEP3-LAUNCHER). Verified: none of those files exist in the tree.
The row stays Active (the QoL asi source `mashedmod/src/qol_asi/mashed_qol.cpp` and its re-pickup
condition are unchanged), so it was rewritten, not closed: the Why cell now records the retirement.
The title cell still carries the old launcher words (the tool rewrites why/when/tag only).
Tool: `re/tools/deferred_close.py rewrite` (re-classify skill). CHANGELOG line added by the tool.
No builds, no game runs.

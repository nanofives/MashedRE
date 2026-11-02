# RESULT D5-SHIM-STEP3-LAUNCHER (text-only, no game boot)

Decision L-79 = (A) (Mariano): retire the original-with-QoL launcher. Shipping product is mashed_re.exe.

## Removed (git rm)
- `Play Mashed.bat` (repo root)
- `Play Mashed (TV).bat` (repo root)
- `scripts/mashed_launch.ps1`
- `mashedmod/build_qol_asi.bat` (the QoL asi build+deploy step; its only caller was the launcher's missing-file hint at mashed_launch.ps1:61)

## Callers found (git grep -i `mashed_launch|Play Mashed`, all tracked files)
| Hit | Kind | Action |
|---|---|---|
| `Play Mashed.bat:4-5`, `Play Mashed (TV).bat:4-5` | callers of mashed_launch.ps1 | deleted with it |
| `scripts/mashed_launch.ps1` (self refs, usage lines 17-22; 61 names build_qol_asi.bat) | the launcher | deleted |
| `scripts/lan_release/Play Mashed.bat`, `README.txt:7`, `mashed_update.ps1:164,185`, `stage_lan_release.py:14,48` | DIFFERENT file: shipping client launcher that starts mashed_re.exe (CLIENT_FILES) | kept, not a caller of mashed_launch.ps1 |
| `re/analysis/QOL_PATCH_PLAN_2026-08.md:270,414` | historical plan text | kept |
| `re/frida/record_attach.py:6` | matched "Play MASHED" in a comment, not the launcher | kept |
| `verify/d5autoloadsweep_20261030`, `verify/d5knobsd1_20261030`, `verify/d5shimconsumers_20261030` RESULT files | historical evidence | kept |

No live caller remained: no CLAUDE.md, skills, .claude, build files, or docs referenced the launcher or told people to use it. `build.bat` never called `build_qol_asi.bat` (only the launcher hint did).

## Not touched (per task)
dinput8 shim, `mashedmod/src/dinput8_shim`, `build_dinput8_shim.bat`, build.bat loop, `mashedmod/src/qol_asi/mashed_qol.cpp`, `original\`. Those belong to D5-SHIM-STEP3-REMOVE (still blocked). Note `DEFERRED.md` D-11075 still mentions the launcher in the QoL strand text (tracker, edit only via re-classify).

## Gates
Not run: text/script deletion only, no mashedmod source changed.

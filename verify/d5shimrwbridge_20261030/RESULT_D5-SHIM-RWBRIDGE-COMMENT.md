# RESULT D5-SHIM-RWBRIDGE-COMMENT (GREEN)

Date 2026-10-30. Branch orch/D5-SHIM-RWBRIDGE-COMMENT from 4b04c3841.

## Change
`mashedmod/src/mashed_re/LibRw/RwBridge.cpp` comment after the "presented N frames" LogLine (R2 of
verify/d5shimconsumers_20261030/RESULT_D5-SHIM-CONSUMERS.md line 68). The old text said the standalone
"deliberately binds System32's d3d9" and pointed at build.bat's RUNTIME NOTE, which described a rename
mitigation. New text states the mechanism that now holds (D5-SHIM-GUARD): d3d9.dll is /DELAYLOAD-ed and
exe_main.cpp's __pfnDliNotifyHook2 loads it with LOAD_LIBRARY_SEARCH_SYSTEM32, so a shim beside the exe is
not picked up. Diff: 1 file, comment lines only (+5 -3).

## Gates
| Gate | Result | Reference |
|---|---|---|
| Build | OK (MASHED_NO_DEPLOY=1) | |
| Exe .text SHA-256 (first 16) | 0ae512899451b006 both | same-path build of 4b04c3841 vs 4b04c3841 + edit |
| Exe .rdata | 2 bytes differ at 0x4c968 (link timestamp) | |
| G-CTRL H_DEF step/gates/ptrace | 8284be46 / ba407a16 / 31fe043e | merge_train.json gctrl.ref |
| B K0 / K1 / K26 | 1d5bf4f9 x3 | merge_train.json b.ref |
| rva-lint | 0 NEW (16 allowlisted CROSS-TARGET) | lint.txt |

Control note: a first comparison between two worktrees with different path lengths differed in .text and
.rdata (`__FILE__` strings, 388096 vs 389120 bytes of .rdata). The valid comparison builds base and
base+edit in the same worktree path (base exe saved as D:/MashedWT/d5shimrwbridge_data/base_mashed_re.exe).
Arms ran from the worktree exe 3985f909 (different path, so its own hash), private root
D:/MashedWT/D5-SHIM-RWBRIDGE-COMMENT_root, under mashed_lock.py.

## Raw data
D:/MashedWT/d5shimrwbridge_data/ (arms/, arms.log, edit.patch, base_mashed_re.exe). Not committed.

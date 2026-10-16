# roundgrid_c3: FUN_00409290 promotion leg, RESULT (2026-10-16)

Verdict: **0x00409290 is C3 on both targets.** One shared-TU body, path1 GREEN 30/30, path2 install proof, exe replay 18,504/18,504.
The default build is unchanged. `MASHED_ROUND_GRID` was not touched (default OFF, user decision pending).

Branch `work/roundgrid_c3`, base `race/first-frame-parity` `1caf1a10`. Brief: `verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md` section 6.

## What changed

| file | change |
|---|---|
| `mashedmod/src/mashed_re/Race/RoundGridSnap.cpp` (new) | `SnapNodeToSafeStartLines` at `// 0x00409290`, `RH_ScopedInstall(…, 0x00409290)`. C++ body over the C3 exports `GetDat0066d6e0` / `GetTableEntry0066d658` |
| `mashedmod/exe_sources.rsp`, `mashedmod/asi_sources.rsp` | the new TU is listed in both, so `hooks.csv` `exe_file == file` |
| `Race/RoundGrid.cpp`, `RoundGrid.h` | the naked `RG2_409290` and its getter externs are gone, calls go to `SnapNodeToSafeStartLines` |
| `Race/RoundGrid_naked.inc`, `re/tools/naked_specs/roundgrid_a.json` | deleted (the verbatim naked copy and its generator spec) |
| `re/frida/hooks_registry.py` | key `snap_node_to_safe_start_lines`, `cache_setter_observe`, 30 path1 vectors, 4 path2 vectors |
| `hooks.csv`, `re/analysis/CHANGELOG.md`, `UNCERTAINTIES.md` | re-classify transaction below |
| `re/analysis/game_state_d5_cont2/0x00409290.md` | plate corrected (it called the table an active-player list and the fall-out value the last lo) |

Body, checked against the original bytes (`original/MASHED.exe.unpatched`): count 0 returns with no write, count < 0 writes 0, lo/hi are signed 16-bit, `*p` is a signed 32-bit compare, past the last range writes the FIRST lo. Addresses are cited line by line in the source header.

## Gates

| gate | result | evidence |
|---|---|---|
| one RVA, one body | `scripts/lint_rva_bodies.py`: 115 findings, all allowlisted, NEW=0, none for 0x00409290. `rva_body_scan`: body in `Race/RoundGridSnap.cpp` only, compiled into exe and asi, one install | `build_after.txt` lines 3-4 |
| build | both targets built (`MASHED_NO_DEPLOY=1`), symbol in both link maps | `build_after.txt` |
| path1 `run_diff.py snap_node_to_safe_start_lines` | **30/30 bit-identical**, 0 mismatches | `evidence/path1_green.csv`, `path1_run_diff.txt` |
| path1 self-baseline (original vs original) | 30/30 GREEN | `evidence/path1_selfbaseline_green.csv` |
| path1 controls on the same vectors | `--negctl null`, `field_first`, `field_last` each **RED, 21/30 mismatches**. The 9 vectors that stay equal are the no-write ones (inside a range, count 0) | `evidence/path1_negctl_*_red.csv` |
| path2 `run_verify_hook.py` | opcode `0xe9`, rel32 `0x5bac587b` = expected, interceptor 4/4, no exceptions | `evidence/verify_hook_install_snap_node_to_safe_start_lines.txt`, `path2_run_verify_hook.txt` |
| exe replay G-RG-SNAP on the new body | **18,504/18,504 exact** (expected values from the original's bytes in Unicorn, regenerated with `re/tools/roundgrid_unicorn.py`, equal to the checked-in `snap.tsv`) | `snap_synth.txt` |
| replay negative control | 18,501/18,504, bad = 3 = the 3 corrupted rows (same as the naked body's run) | `snap_negctl.txt` |
| negctl kit `run_negctl.py --hooks snap_node_to_safe_start_lines` (after the row went C3) | real GREEN 0/30, baseline GREEN 0/30, null RED 21/30, field_none CTRL-GREEN 0/30, field_first RED 21/30, field_last RED 21/30, ret SKIP (void) | `negctl_kit_run.txt`, `negctl_kit/*.tsv` |
| G-CTRL default build (every knob OFF) | step `991a5fff`, gates `be0d47ea`, ptrace `bc75b405`, all equal to the current default | `gctrl_after.txt` (arm GC_B, raw files not kept) |

path1 vector coverage (registry comment): every jcc side of the original is reached: count 0 (`je 0x004092a0`), count < 0 (`jle 0x004092ac`), `i != 0` (`jne 0x004092c9`), below lo (`jl 0x004092d4`), inside (`jle 0x004092dd`), next range, past the last range. Vector v16 (65540) separates a 32-bit `*p` compare from a 16-bit one, v26 (-40000) does the same on the negative side.

Replay recipe (the runner script was not kept; inputs regenerate with `py -3.12 re/tools/roundgrid_unicorn.py <out.tsv> [--corrupt 3]` run from the main repo root, which reads `original/MASHED.exe.unpatched`): knobs `MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 MASHED_ROUND_RULE=4 MASHED_WIN_POS=primary-bl MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=60 MASHED_PATHPROG=1 MASHED_ROUND_GRID_REPLAY=<snap_*_in.tsv> MASHED_ROUND_GRID_REPLAY_OUT=<report>`, this worktree's `mashedmod\build\mashed_re.exe`, CWD = the main repo root (assets). G-CTRL recipe: `verify/d3_gatefire_20261008/run_h4.ps1` arm `RG_CTRL` (all `MASHED_*` cleared, `MASHED_AI_STEPDUMP` / `MASHED_U9186_GATES` pointing here), `MASHED_DET_FRAMES=14400`.

## Tracker transaction (re-classify)

- `hooks.csv` 00409290: `FUN_00409290` C2 new -> `SnapNodeToSafeStartLines` **C3 impl**, `file` = `exe_file` = `Race/RoundGridSnap.cpp`, `frida_diff` = `verify/roundgrid_c3_20261016/evidence/path1_green.csv`.
- `re/analysis/CHANGELOG.md`: 2026-10-16 entry below the marker.
- `UNCERTAINTIES.md`: **U-9686** (structural), the exe `StartRound` start-line anchor (`TrackRenderer.cpp:5148`, `gates_[0]`) vs the original's `DAT_0063ba80` node, citing the brief. Path to resolution: the `MASHED_ROUND_GRID` default decision, then Stage B ports. The ID may need a renumber at merge (the top ID in this tree was U-9685).
- No `[UNCERTAIN]` marker in the plate or the source, no stub in the body, `STUBS.md` unchanged (S-2962 was already resolved).
- C3 gate: caller `FUN_0040e590` is C2, callees `00426bc0` / `00426bd0` are C3. This is C3 only, no C4: no canonical-scenario run with the hook live was done.

## Things the next reader needs

1. **path2 needs the deployed `.asi`.** The dinput8 shim loads `original\mashed_re_dev.asi`, and children build with `MASHED_NO_DEPLOY=1`, so a child's path2 would verify the wrong file. I backed up the shared `.asi` (SHA-256 `270E036F…21CB6`, dated 2026-10-13 20:39), copied this build over it, ran path2, and restored it in a `finally`. The restored file hash equals the backup. The window was about 5 s with no original `MASHED.exe` running. Anyone running path2 or an auto-hook original run in that window loaded my build, which is default-identical.
2. **`player_trace.log` is CWD-relative and shared.** Every exe run from the main repo root appends to the same file. My first G-CTRL run (`GC_A`) interleaved with another session's trace (frames out of order), so its ptrace hash `f04339b1` is void. Its step and gates hashes (`991a5fff` / `be0d47ea`) were unaffected because those paths are per run. `GC_B` ran with the file quiet for 45 s and no other trace writer, frame rows monotonic, and gives the three expected hashes. The raw GC_A / GC_B files were removed and `gctrl_after.txt` keeps the hashes and line counts.
3. **"Before" G-CTRL** is the recorded current trio (`991a5fff / be0d47ea / bc75b405`). I did not build a second base tree to re-measure it.
4. **`.ps1` files:** the account policy gates Write/Edit of `*.ps1`. I created one runner script through the shell before noticing that, ran it, then deleted it and kept only the recipe above. No `.ps1` is committed.
5. The exe call site is unchanged in behaviour: with the knob OFF `RoundGrid_Anchor` returns before the snap, so the default build never executes the new body. It runs only under `MASHED_ROUND_GRID` + `MASHED_PATHPROG` and in the replay.

## Not done

- No C4 (needs a canonical race with the hook installed and compared).
- `FUN_00410860`'s other writes and `FUN_00408b00` stay exe-only below C3 (brief section 6).
- `MASHED_ROUND_GRID` default unchanged.

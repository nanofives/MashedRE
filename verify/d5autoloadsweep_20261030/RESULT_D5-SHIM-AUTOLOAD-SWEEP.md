# RESULT D5-SHIM-AUTOLOAD-SWEEP (static, text-only, 2026-10-30)

Method: file scan of `re/`, `scripts/`, `verify/` (1855 files `*.py *.ps1 *.bat *.cmd *.sh *.js`, `prior_art` skipped) on `orch/D5-SHIM-AUTOLOAD-SWEEP` (from `orch/integration` d6448c31e). A file is a candidate when it names `MASHED.exe` and calls a spawn API (`frida` spawn, `Popen`, `subprocess.*`, `Start-Process`, `CreateProcess`, `ProcessStartInfo`). The scan was done with a PowerShell regex pass plus per-file reads, not with `rg` (the failure named in D5-SHIM-CONSUMERS section 3). No build, no run, no Ghidra, no source or script edit. Source questions: `verify/d5shimconsumers_20261030/RESULT_D5-SHIM-CONSUMERS.md` section 3, section 5 step 3, section 6 item 6.

**VERDICT: removing the dinput8 shim breaks 46 live spawners (22 that never set `MASHED_RE_NO_AUTO_HOOK` plus 24 that unset it for their hooks-ON arm) and 25 historical `verify/` ones (24 + 1). 0 unclassified rows.**

Counts: 283 candidate files, 142 set `MASHED_RE_NO_AUTO_HOOK` somewhere, 141 do not. Of the 142, 25 unset it on a hooks-ON path (section 2), so they rely on autoload too. Of the 141, 46 rely on autoload (22 live, 24 hist), 95 do not.

## 1. Classification rules (mechanical)

- `yes`: spawns original `MASHED.exe`, never sets the var, never calls `Module.load` on the asi, and sets `MASHED_RE_DEV` / `MASHED_HOOK_ONLY` or names the asi module or says in its own docstring that the proxy loads it. The asi reaches the process only through `dinput8.dll` (`dinput8_shim.cpp:34-96`).
- `yes (indirect)`: does not spawn itself. It launches `scenario_launch.py --hooks ...` (which pops the var, `re/frida/scenario_launch.py:5073-5083`) or is a shared-asi deploy window that exists so a proxy-loaded child sees the deployed `original\mashed_re_dev.asi`.
- `no`: spawns original with no asi reference (the proxy loads the asis into it today as a side effect, so removal changes its baseline to stock and breaks nothing it asks for), or loads the asi itself with `Module.load`, or spawns only `mashed_re.exe`, or only attaches, or is not a game spawner, or delegates to a launcher that sets the var for stock arms (`scenario_launch.py:5084-5085`, `camera_probe.py:853`, `u9667_menu_table_dump.py:107`).
- `scope`: `live` = under `re/` or `scripts/`. `hist` = under `verify/` (dated evidence folders, not re-run).

## 2. Spawners that set the var but unset it for hooks (rely on autoload, 25 files)

These are in the "sets-var" group by text match, but each has a hooks-ON arm that pops the var so the proxy loads `mashed_re_dev.asi`. Line numbers are the lines that pop or condition the var.

| path | scope | lines | sets-var | relies on autoload |
|---|---|---|---|---|
| `re/frida/c4_champ_complete_scenario.py` | live | 159,161 | conditional | yes (hooks-ON arm) |
| `re/frida/c4_detect_e_scenario.py` | live | 168,170 | conditional | yes (hooks-ON arm) |
| `re/frida/c4_hud_scenario.py` | live | 365,367 | conditional | yes (hooks-ON arm) |
| `re/frida/c4_m_scenario.py` | live | 268,270 | conditional | yes (hooks-ON arm) |
| `re/frida/c4_pb2_scenario.py` | live | 279,281 | conditional | yes (hooks-ON arm) |
| `re/frida/nav_champ_probe.py` | live | 99 | conditional | yes (hooks-ON arm) |
| `re/frida/observe_c4_render_math_hooks.py` | live | 111 | conditional | yes (hooks-ON arm) |
| `re/frida/observe_c4_save_hooks.py` | live | 411 | conditional | yes (hooks-ON arm) |
| `re/frida/observe_c4_sweep_20260517.py` | live | 224 | conditional | yes (hooks-ON arm) |
| `re/frida/observe_frontend_full_30s.py` | live | 195 | conditional | yes (hooks-ON arm) |
| `re/frida/observe_hooks_at_menu.py` | live | 191 | conditional | yes (hooks-ON arm) |
| `re/frida/observe_lap_hooks_at_menu.py` | live | 127 | conditional | yes (hooks-ON arm) |
| `re/frida/phys_c4_bodydiff.py` | live | 123 | conditional | yes (hooks-ON arm) |
| `re/frida/phys_c4_telemetry.py` | live | 77 | conditional | yes (hooks-ON arm) |
| `re/frida/pickup_pos_probe2.py` | live | 291 | conditional | yes (hooks-ON arm) |
| `re/frida/replay_verify.py` | live | 275 | conditional | yes (hooks-ON arm) |
| `re/frida/run_verify_hook.py` | live | 171 | conditional | yes (hooks-ON arm) |
| `re/frida/scenario_launch.py` | live | 5078,5083 | conditional | yes (hooks-ON arm) |
| `re/frida/statenav.py` | live | 295 | conditional | yes (hooks-ON arm) |
| `re/frida/verify_scoring_hooks.py` | live | 76,77 | conditional | yes (hooks-ON arm) |
| `scripts/ttd/ttd_reimpl_diff.py` | live | 136 | conditional | yes (hooks-ON arm) |
| `scripts/bisect_hooks_set.py` | live | 93 | conditional | yes (hooks-ON arm) |
| `scripts/bisect_hooks.py` | live | 42 | conditional | yes (hooks-ON arm) |
| `scripts/catch_wedge.py` | live | 120 | conditional | yes (hooks-ON arm) |
| `verify/dupinstall2_20261016/nav_probe.py` | hist | 49 | conditional | yes (hooks-ON arm) |

## 3. Spawners that set the var unconditionally (117 files + 1)

117 files set the var with no pop or conditional on the same line (list in the raw data, with_var_plain.txt). They do not rely on autoload for the dev asi. Two notes: e/frida/nav_reach_probe.py sets it and was missed by the first scan because it has no literal MASHED.exe (it spawns through a variable), so it joins this group. scripts/mashed_launch.ps1:113 sets the var (keeps the dev asi out of play sessions) and still needs dinput8.dll plus mashed_qol.asi (required list at mashed_launch.ps1:57), so it relies on the proxy for the QoL asi. That is the player-launcher dependency already named in D5-SHIM-CONSUMERS section 3, listed here so the count of proxy users is complete.

## 4. Table: the 141 spawners that do not set the var

| # | path | scope | kind | sets-var | relies on autoload |
|---|---|---|---|---|---|
| 1 | `verify/capfix_20261016/run_capture.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 2 | `verify/carlod2_20261016/asi_window.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 3 | `verify/d1r3_20261016/run_gctrl.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 4 | `verify/d3_pathprog_a_20261016/run_pathprog_a.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 5 | `verify/d3_u9683_20261016/run_u9683.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 6 | `verify/d3_u9685_20261016/run_u9685.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 7 | `verify/d3close_20261021/run_modes.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 8 | `verify/d3close_20261021/run_pu.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 9 | `verify/d4wsbportbroad1_20261029/drive_pb1.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 10 | `verify/d4wsbportbvh_20261029/drive_pb.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 11 | `verify/d4wsbportsolv_20261029/drive_ps.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 12 | `verify/d4wsbsolvkv_20261029/uc_arm.py` | hist | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 13 | `verify/d4wsbsolvkvarmu_20261030/uc_arm.py` | hist | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 14 | `verify/dualcopy3_20261016/window.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 15 | `verify/dualcopy4_20261016/window.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 16 | `verify/dupinstall_20261016/install_observe.py` | hist | spawns original MASHED.exe | no | yes |
| 17 | `verify/dupinstall_20261016/run_path2_deployed.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 18 | `verify/dupinstall2_20261016/c4_racediff.py` | hist | spawns original MASHED.exe | no | yes |
| 19 | `verify/dupinstall2_20261016/c4_verify.py` | hist | spawns original MASHED.exe | no | yes |
| 20 | `verify/dupinstall2_20261016/install_observe.py` | hist | spawns original MASHED.exe | no | yes |
| 21 | `verify/dupinstall2_20261016/window.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 22 | `verify/fe_residuals_20261016/fx/orig_fx_count.py` | hist | attaches to a running process, no spawn call | no | no |
| 23 | `verify/fe_residuals_20261016/u9672/orig_hold.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 24 | `verify/fxflip_20261027/flip_driver.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 25 | `verify/fxgates_20261026/orig_fx_gates_count.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 26 | `verify/fxinputs_20261026/orig_fx_inputs.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 27 | `verify/fxrebase_20261027/orig_fx_rebase.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 28 | `verify/fxslot2_20261027/orig_alive_series.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 29 | `verify/fxslot2_20261027/orig_fx_gates_count.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 30 | `verify/fxstate7_20261027/orig_fx_state7.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 31 | `verify/intromovies_20261028/run_intro.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 32 | `verify/intromovies_20261030/i1b_driver.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 33 | `verify/librwrest_checks_20261016/run_orig_arms.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 34 | `verify/librwrest_checks_20261016/run_stand.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 35 | `verify/pontoon_20261016/capture_op.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 36 | `verify/r10_20261016/bt_capture.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 37 | `verify/rfx_emitters_20261028/emit_unicorn.py` | hist | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 38 | `verify/rres_20261016/probe_shadow.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 39 | `verify/rsky_20261016/capture_one.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 40 | `verify/seek414f00_20261016/capture.py` | hist | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 41 | `verify/seek414f00_20261016/window.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 42 | `verify/ws_e_lighting/orig_parity/compare.py` | hist | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 43 | `verify/wsbbvh_20261026/c4_racediff_bvh.py` | hist | spawns original MASHED.exe | no | yes |
| 44 | `verify/wsbbvh_20261026/make_tool.py` | hist | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 45 | `verify/wsbbvh_20261027/c4_racediff_v6.py` | hist | spawns original MASHED.exe | no | yes |
| 46 | `verify/wsbc4trk1_20261028/drive_trk1.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 47 | `verify/wsbc4trk2_20261029/bootcheck.py` | hist | spawns original MASHED.exe | no | yes |
| 48 | `verify/wsbc4trk2_20261029/drive_trk2.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 49 | `verify/wsbnarrow_20261026/run_c4n.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 50 | `verify/wsbnarrow_20261027/run_c4n_fix.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 51 | `verify/wsbnarrow_20261027/run_c4n_fix2.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 52 | `verify/wsbnarrowflip_20261027/flip_driver.py` | hist | spawns mashed_re.exe or non-game process only | no | no |
| 53 | `verify/wsbnarrowflip_20261027/run_c4n_flip.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 54 | `verify/wsbsolvrgn_20261028/drive_rgn.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 55 | `verify/wsbstackonly_20261028/drive_so.py` | hist | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 56 | `re/frida/auto_count_at_menu.py` | live | spawns original, no .asi reference | no | no |
| 57 | `re/frida/bisect_save_hooks.py` | live | spawns original, Module.load of the .asi itself | no | no |
| 58 | `re/frida/canon_observe_refused.py` | live | spawns original, no .asi reference | no | no |
| 59 | `re/frida/canonical_c4_autodiff.py` | live | spawns original MASHED.exe | no | yes |
| 60 | `re/frida/canonical_c4_leafdiff.py` | live | spawns original MASHED.exe | no | yes |
| 61 | `re/frida/canonical_c4_navigate.py` | live | spawns original MASHED.exe | no | yes |
| 62 | `re/frida/canonical_c4_racediff.py` | live | spawns original MASHED.exe | no | yes |
| 63 | `re/frida/canonical_c4_sidediff.py` | live | spawns original MASHED.exe | no | yes |
| 64 | `re/frida/canonical_c4_stalkerdiff.py` | live | spawns original MASHED.exe | no | yes |
| 65 | `re/frida/canonical_c4_verify_hotpath.py` | live | spawns original MASHED.exe | no | yes |
| 66 | `re/frida/canonical_c4_verify.py` | live | spawns original MASHED.exe | no | yes |
| 67 | `re/frida/canonical_install_observe.py` | live | spawns original MASHED.exe | no | yes |
| 68 | `re/frida/canonical_observe_viewport_init.py` | live | spawns original MASHED.exe | no | yes |
| 69 | `re/frida/capture_jump_bug.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 70 | `re/frida/catch_crash_eip.py` | live | spawns original, no .asi reference | no | no |
| 71 | `re/frida/catch_standalone_phys_crash.py` | live | spawns mashed_re.exe or non-game process only | no | no |
| 72 | `re/frida/count_natural_invocations.py` | live | attaches to a running process, no spawn call | no | no |
| 73 | `re/frida/diag_smoke.py` | live | spawns original MASHED.exe | no | yes |
| 74 | `re/frida/find_bindings.py` | live | attaches to a running process, no spawn call | no | no |
| 75 | `re/frida/find_lock_gate.py` | live | attaches to a running process, no spawn call | no | no |
| 76 | `re/frida/force_keyboard.py` | live | attaches to a running process, no spawn call | no | no |
| 77 | `re/frida/force_unlock_live.py` | live | attaches to a running process, no spawn call | no | no |
| 78 | `re/frida/hot_path_behavioral.py` | live | spawns original MASHED.exe | no | yes |
| 79 | `re/frida/implicit_survival_sweep.py` | live | spawns original MASHED.exe | no | yes |
| 80 | `re/frida/launch_with_crash_catch.py` | live | spawns original, Module.load of the .asi itself | no | no |
| 81 | `re/frida/manual_test_config7.py` | live | spawns original MASHED.exe | no | yes |
| 82 | `re/frida/mass_canonical_observe_r2.py` | live | spawns original MASHED.exe | no | yes |
| 83 | `re/frida/mass_canonical_observe_r3.py` | live | spawns original MASHED.exe | no | yes |
| 84 | `re/frida/mass_canonical_observe_wave0.py` | live | spawns original MASHED.exe | no | yes |
| 85 | `re/frida/mass_canonical_observe_wave2.py` | live | spawns original, Module.load of the .asi itself | no | no |
| 86 | `re/frida/mass_canonical_observe_wave3.py` | live | spawns original, no .asi reference | no | no |
| 87 | `re/frida/mass_canonical_observe.py` | live | spawns original MASHED.exe | no | yes |
| 88 | `re/frida/patch_skip_powerups.py` | live | spawns original, no .asi reference | no | no |
| 89 | `re/frida/poll_attach_catch_crash.py` | live | attaches to a running process, no spawn call | no | no |
| 90 | `re/frida/probe_005ab100.py` | live | spawns original, no .asi reference | no | no |
| 91 | `re/frida/probe_005abf80.py` | live | spawns original MASHED.exe | no | yes |
| 92 | `re/frida/probe_ab380_corrupt.py` | live | attaches to a running process, no spawn call | no | no |
| 93 | `re/frida/probe_appinit_callees.py` | live | attaches to a running process, no spawn call | no | no |
| 94 | `re/frida/probe_appinit_checkpoints.py` | live | attaches to a running process, no spawn call | no | no |
| 95 | `re/frida/probe_callstack.py` | live | attaches to a running process, no spawn call | no | no |
| 96 | `re/frida/probe_createdevice.py` | live | attaches to a running process, no spawn call | no | no |
| 97 | `re/frida/probe_dispatch_0x44.py` | live | attaches to a running process, no spawn call | no | no |
| 98 | `re/frida/probe_eip_sampler.py` | live | attaches to a running process, no spawn call | no | no |
| 99 | `re/frida/probe_eip0_dump.py` | live | attaches to a running process, no spawn call | no | no |
| 100 | `re/frida/probe_f4_dispatch.py` | live | attaches to a running process, no spawn call | no | no |
| 101 | `re/frida/probe_fontctx_build.py` | live | attaches to a running process, no spawn call | no | no |
| 102 | `re/frida/probe_fontctx_open.py` | live | attaches to a running process, no spawn call | no | no |
| 103 | `re/frida/probe_fontctx_texname.py` | live | attaches to a running process, no spawn call | no | no |
| 104 | `re/frida/probe_piz_dir_dump.py` | live | attaches to a running process, no spawn call | no | no |
| 105 | `re/frida/probe_piz_txd_search.py` | live | attaches to a running process, no spawn call | no | no |
| 106 | `re/frida/probe_prompt_calls.py` | live | spawns original, no .asi reference | no | no |
| 107 | `re/frida/probe_srs.py` | live | attaches to a running process, no spawn call | no | no |
| 108 | `re/frida/probe_stalker_5a7b60.py` | live | attaches to a running process, no spawn call | no | no |
| 109 | `re/frida/probe_statemachine.py` | live | attaches to a running process, no spawn call | no | no |
| 110 | `re/frida/probe_watchpoint.py` | live | attaches to a running process, no spawn call | no | no |
| 111 | `re/frida/probe_winmain_chain.py` | live | attaches to a running process, no spawn call | no | no |
| 112 | `re/frida/read_unlock_state.py` | live | attaches to a running process, no spawn call | no | no |
| 113 | `re/frida/run_diff_parallel.py` | live | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 114 | `re/frida/run_trace_boot_crash.py` | live | spawns original, no .asi reference | no | no |
| 115 | `re/frida/test_bypass5.py` | live | spawns original, Module.load of the .asi itself | no | no |
| 116 | `re/frida/verify_restore_patch.py` | live | attaches to a running process, no spawn call | no | no |
| 117 | `re/frida/watch_toast_corruption.py` | live | attaches to a running process, no spawn call | no | no |
| 118 | `re/orchestrator/merge_train.ps1` | live | spawns mashed_re.exe or non-game process only | no | no |
| 119 | `re/tools/ai_data.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 120 | `re/tools/console/xtwin.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 121 | `re/tools/contcfg_ui.py` | live | attaches to a running process, no spawn call | no | no |
| 122 | `re/tools/decomp_pc.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 123 | `re/tools/find_cam_angles.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 124 | `re/tools/focus_pause_check.py` | live | spawns original, no .asi reference | no | no |
| 125 | `re/tools/orig_rampwatch.py` | live | delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | no | no |
| 126 | `re/tools/pointer_callers.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 127 | `re/tools/shadow_batch.py` | live | wrapper (scenario_launch --hooks / .asi deploy window) | no | yes (indirect) |
| 128 | `re/tools/veccap/replay_c3.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 129 | `re/tools/veccap/replay_offline.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 130 | `re/tools/veccap/veccap_registry.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 131 | `re/tools/xref.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 132 | `scripts/bisect_asi_boot.ps1` | live | spawns original MASHED.exe | no | yes |
| 133 | `scripts/bisect_hook_index.ps1` | live | spawns original MASHED.exe | no | yes |
| 134 | `scripts/diag.py` | live | spawns mashed_re.exe or non-game process only | no | no |
| 135 | `scripts/fix_boot_elevated.ps1` | live | spawns original, no .asi reference | no | no |
| 136 | `scripts/ghidra/sweep_const_width.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 137 | `scripts/launch_heapshift.py` | live | spawns original, no .asi reference | no | no |
| 138 | `scripts/mashed_shot.ps1` | live | spawns original, no .asi reference | no | no |
| 139 | `scripts/observe_runtime.ps1` | live | spawns original, no .asi reference | no | no |
| 140 | `scripts/repatch_original.py` | live | not a game spawner (tasklist, git, static tool, own harness.exe) | no | no |
| 141 | `scripts/ttd/record_menu.ps1` | live | attaches to a running process, no spawn call | no | no |

## 5. Roll-up

| group | live | hist |
|---|---|---|
| spawns original MASHED.exe | 21 | 7 |
| wrapper (scenario_launch --hooks / .asi deploy window) | 1 | 17 |
| spawns original, Module.load of the .asi itself | 4 | 0 |
| spawns original, no .asi reference | 13 | 0 |
| delegates (scenario_launch / camera_probe / run_diff set the var for stock arms) or passes args through | 2 | 16 |
| spawns mashed_re.exe or non-game process only | 3 | 9 |
| attaches to a running process, no spawn call | 30 | 1 |
| not a game spawner (tasklist, git, static tool, own harness.exe) | 12 | 5 |
| sets var, pops it for hooks-ON (section 2) | 24 | 1 |

Relies on autoload: live 22 + 24 = **46**, hist 24 + 1 = **25**.

## 6. Findings that change the D5-SHIM-CONSUMERS plan

1. Section 5 step 3 (remove the dinput8 shim) is blocked, not just "run the sweep first". The hook lane depends on it: `scenario_launch.py --hooks`, `run_verify_hook.py`, `statenav.py`, the `c4_*_scenario.py` family, `phys_c4_*`, `canonical_c4_*`, `mass_canonical_observe*` and the bisect scripts all load `mashed_re_dev.asi` through the proxy. The CONSUMERS line "the Frida lane does not need it" is true only for the `Module.load` arms (`run_diff.py`, 4 self-loaders in section 4).
2. Removal order stays as in CONSUMERS section 5, but step 3 must follow step 6 (retire diff-original and the asi) or follow a rewrite of those 46 spawners to `Module.load` the asi with the var set. A rewrite touches `.py`/`.ps1` files, so it needs its own row. Nothing here is edited.
3. Four self-loaders (`bisect_save_hooks.py`, `launch_with_crash_catch.py`, `test_bypass5.py`, `mass_canonical_observe_wave2.py`) call `Module.load` on the asi without setting the var. With the proxy present the asi can load twice. Removing the proxy fixes them. Report only, no edit.
4. 13 live spawners with no asi reference (section 4, `no asi`) get the dev asi today only as a side effect of the proxy. Removing the proxy turns them into true stock-original runs. No script change needed.
5. Two `.ps1` spawners need the proxy (`scripts/bisect_asi_boot.ps1`, `scripts/bisect_hook_index.ps1`), plus `scripts/mashed_launch.ps1` for the QoL asi (section 3). They are reported to the lead. No `.ps1`/`.bat` edit made.

## 7. Raw data

Path `D:/MashedWT/D5-SHIM-AUTOLOAD-SWEEP_data/`:

- `without_var_classified.csv`, SHA-256 `9D7A864D54A60A19E578ACA94FD2BD360FCEC4844C700578767CB0BAA74B560F`
- `with_var_conditional.csv`, SHA-256 `9AC1D4D9E3BEEC4D737A8EC240135A3138F43E27A51D9BE63AC69D258C21745C`
- `with_var_plain.txt`, `without_var_paths.txt` (path lists)

## Limits

- Static. "Relies" is read from spawn env, docstrings and asi references, not from a boot. A `yes (indirect)` wrapper only relies when its passed arguments include hooks, and the listed ones hard-code `--hooks`.
- `deleg` rows and `fx*`/`pontoon`/`seek414f00` drivers accept extra argv, so a caller could still pass `--hooks`. None of the scripts do.
- Candidate detection needs a spawn API and the literal `MASHED.exe`. A second pass over frida `spawn` calls without that literal found 4 more files (3 spawn `mashed_re.exe` only, 1 sets the var). Spawners through other wrappers (`os.system`, `.bat` calls) are not covered.


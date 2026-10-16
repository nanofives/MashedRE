# Evidence gaps, 2026-10-16

ROADMAP.md "Open decisions" follow-up #4: evidence that tracked docs cite but git does not hold.

## Method

- Sources: every tracked `.md`, `.csv` and `.tsv` file, using the same set as the cite sweep (`cite_sweep.py`). Scanned for `log/...` and `verify/...` paths.
- Cited paths: 2781 distinct. 560 are tracked, 206 are directories, 1204 exist as untracked files, 811 exist nowhere in the working tree.
- All 1204 existing untracked cited files are gitignored (`/log/` line 16, `/verify/*` lines 24-25, `*.log` line 18). Untracked-but-not-ignored cited files: 0.
- Size threshold: 1 MB = 1,000,000 bytes.

## Counts

| Bucket | Files | Size (MB) | Action |
| --- | ---: | ---: | --- |
| Force-added, text, <= 1 MB | 1067 | 10.21 | Committed: 1062 under `log/` (c0086931), 5 under `verify/` (d53b38bd) |
| Skipped, ignored, <= 1 MB, non-text | 61 | 25.83 | Not added (types: .png 34, .bmp 23, .pre_r0 2, .exe 1, .obj 1) |
| Skipped, ignored, > 1 MB | 76 | 558.10 | Not added. Listed below |
| Missing (cited, not on disk) | 811 | - | Not resolved. 147 citing docs, 962 (doc, path) pairs. Listed below |

Added text types: `.csv` 835, `.log` 89, `.txt` 83, `.json` 53, `.tsv` 6, `.md` 1.

Note: the earlier cite sweep reported 75 cited files over 1 MB. This manifest, at the same threshold, finds 76. The table below is the authoritative list.

## Cited files over 1 MB, not added

| Path | Size (MB) | Citing doc |
| --- | ---: | --- |
| `log/branches_backup_2026-07-06.bundle` | 158.90 | re/analysis/CHANGELOG.md |
| `log/librw_scene.txt` | 28.48 | UNCERTAINTIES.md (+3) |
| `verify/d3_contact_20260928b/m2.msd` | 22.21 | re/analysis/D3_CONTACT_PORT_2026-09-28.md |
| `verify/d3_contact_20260928b/m1.msd` | 22.21 | re/analysis/D3_CONTACT_PORT_2026-09-28.md |
| `verify/d3_contact_20260928/g2.msd` | 22.18 | re/analysis/D3_CONTACT_PORT_2026-09-28.md |
| `verify/d3_contact_20260927/c2.msd` | 22.11 | re/analysis/D3_CONTACT_2026-09-27.md |
| `log/drawstream_re_stand.json` | 15.40 | re/analysis/race_hud_capture_20260902.md |
| `log/drawstream_re.json` | 12.37 | re/analysis/frontend_feedback_20260612.md (+2) |
| `verify/d3_modes37_20261002/o1.msd` | 12.12 | verify/d3_modes37_20261002/PREREG_STEP2.md (+1) |
| `verify/d3_drive_20260928/e3.msd` | 12.11 | re/analysis/D3_DRIVE_2026-09-28.md (+1) |
| `verify/d3_elim_20261003/o_t1.msd` | 12.09 | UNCERTAINTIES.md (+4) |
| `verify/d3_gatefire_20261008/AL_CTRL.ptrace.log` | 10.15 | verify/d3_bulk_20261011/aileaf/RESULT_AILEAF.md |
| `verify/d3_force_20260929/o_c0.msd` | 9.09 | UNCERTAINTIES.md |
| `verify/d3_pu_20260926/o3.msd` | 9.07 | re/analysis/D3_POWERUPS_2026-09-26.md |
| `verify/d3_omega_20261005/o1.msd` | 8.12 | verify/d3_omega_20261005/RESULT_OMEGA.md |
| `verify/player_reg_20260929/orig_elim/orig_a8_elim.msd` | 7.79 | re/analysis/D2_REOPEN_2026-09-29.md |
| `verify/a8_steer_20260823/orig_steerR.msd` | 7.79 | ROADMAP.md (+2) |
| `verify/a8_steer_20260824/orig_steerR.msd` | 7.79 | UNCERTAINTIES.md (+9) |
| `verify/d2_latbr_20260930/orig_lb1.msd` | 7.79 | re/analysis/D2_REOPEN_2026-09-29.md |
| `verify/d2_latbr_20260930/orig_lb3.msd` | 7.79 | re/analysis/D2_REOPEN_2026-09-29.md |
| `verify/a8_steer_20260824/orig_steerL.msd` | 7.79 | UNCERTAINTIES.md (+2) |
| `verify/d2_magpr_20260930/read1.msd` | 7.79 | re/analysis/D2_REOPEN_2026-09-29.md (+1) |
| `verify/d2_reopen_20260929/orig_solo3.msd` | 7.78 | UNCERTAINTIES.md (+11) |
| `verify/d2_reopen_20260929/orig_solo4.msd` | 7.78 | re/analysis/D2_REOPEN_2026-09-29.md |
| `verify/d2_bounce_20260930/orig_fp1.msd` | 7.78 | UNCERTAINTIES.md |
| `verify/d2_magpr_20260930/count.msd` | 7.77 | re/analysis/D2_REOPEN_2026-09-29.md |
| `verify/d2_b18_20261002/orig_ax.msd` | 7.27 | verify/d2_b18_20261002/PREREG_STEP1.md |
| `verify/d2_sched_20261001/ph1/a6a_dump.log` | 7.16 | verify/d2_sched_20261001/PREREG_STEP2B.md |
| `verify/d2_sink_20261001/arm/a6a_dump.log` | 7.16 | verify/d2_l60_20261001/PREREG_STEP1.md |
| `log/backups/hooks.csv.pre_c4tag` | 3.33 | re/analysis/archive/CHANGELOG_pre20260615.md |
| `log/backups/hooks.csv.pre_r0` | 3.32 | re/analysis/archive/CHANGELOG_pre20260615.md |
| `log/wedge_run0.log` | 3.20 | re/analysis/u9025_stream_lock_race.md |
| `log/wedge_run1.log` | 3.19 | re/analysis/u9025_stream_lock_race.md |
| `log/handling_telemetry.jsonl` | 2.53 | re/analysis/r6_demand_map_2026-07.md (+1) |
| `log/sector_probe.json` | 1.74 | re/analysis/promote_c2_vehicle_lowrva/replay_ghost_family_witness_20260902.md |
| `log/wedge_run2.log` | 1.61 | re/analysis/u9025_stream_lock_race.md |
| `verify/abilsel_2p.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/abilsel_4p.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/abilsel_clamp.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/abilsel_ctl.bmp` | 1.44 | hooks.csv (+2) |
| `verify/abilsel_hdr_byid.bmp` | 1.44 | re/analysis/CHANGELOG.md (+1) |
| `verify/abilsel_keys.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/chalsel_panel.bmp` | 1.44 | re/analysis/CHANGELOG.md (+1) |
| `verify/chalsel_panel_teamplay.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/chalsel_panel_unlocked.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/chalsel_save_edited.bmp` | 1.44 | re/analysis/CHANGELOG.md (+1) |
| `verify/chalsel_save_ref.bmp` | 1.44 | re/analysis/CHANGELOG.md (+1) |
| `verify/opt19_byid.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/opt19_byid_manual.bmp` | 1.44 | re/analysis/CHANGELOG.md (+1) |
| `verify/opt32_byid_on.bmp` | 1.44 | re/analysis/CHANGELOG.md (+1) |
| `verify/run_13464/r6/round2_result.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/run_20684/r6/round2_result.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/run_31960/race1/00_results.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/run_31960/race1/01_grid.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/run_32604/r6/round2_go.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/run_32604/r6/round2_result.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/run_32632/race1/00_results.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_2v2.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_default.bmp` | 1.44 | re/analysis/CHANGELOG.md (+1) |
| `verify/teamsel_hdr_byid.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_keys.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_keys2.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_reject_default.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_reject_legal.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_reject_sameteam.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_rows2p.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_rows4p.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_sameteam.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/teamsel_split.bmp` | 1.44 | re/analysis/race_hud_capture_20260902.md |
| `verify/d2_bounce_20260930/p1/player_trace.log` | 1.18 | verify/d2_gain_20261001/RESULT.md |
| `verify/d2_budget_20261002/s1/motion_diag.log` | 1.16 | verify/d2_sink_20261002/PREREG_STEP23.md |
| `verify/d2_b0c_20261002/p1/motion_diag.log` | 1.16 | verify/d2_budget_20261002/PREREG_STEP1.md (+1) |
| `verify/d2_wheelstate_20261002/s1/motion_diag.log` | 1.15 | UNCERTAINTIES.md |
| `verify/car_gray_20260929/sahi_train_t16.png` | 1.12 | re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md |
| `verify/d2_l60_20261001/sc1/motion_diag.log` | 1.04 | verify/d2_sched_20261001/PREREG_STEP2.md |
| `verify/car_gray_20260929/nodummy_t16.png` | 1.00 | re/analysis/CAR_BRIGHTNESS_2026-09-30.md |

## Missing citations, grouped by citing doc

These paths are cited by tracked docs but do not exist in the working tree. Each row needs a decision: restore the file, update the citation, or accept the gap.

| Citing doc | Missing path |
| --- | --- |
| `.claude/skills/diff-original/SKILL.md` | `log/diff_modded_` |
| `.claude/skills/diff-original/SKILL.md` | `log/diff_original_` |
| `.claude/skills/promote-c3-batch/SKILL.md` | `log/build_` |
| `.claude/skills/promote-c3-batch/SKILL.md` | `log/diff_` |
| `.claude/skills/promote-c3-batch/SKILL.md` | `log/diff_scenario_batch_` |
| `AGENTS.md` | `log/diff_modded_` |
| `AGENTS.md` | `log/diff_original_` |
| `DEFERRED.md` | `verify/r6/round` |
| `DEFERRED.md` | `verify/vp_split.png` |
| `ROADMAP.md` | `verify/d2_reopen_20260929/orig_solo` |
| `ROADMAP.md` | `verify/d2_sched_20261001/RESULT_STEP` |
| `ROADMAP.md` | `verify/d3_ai_20260914/orig_step_slots` |
| `ROADMAP.md` | `verify/d3_ai_20260926/o_spread1..3` |
| `ROADMAP.md` | `verify/d3_ai_20260927b/p` |
| `ROADMAP.md` | `verify/d3_contact_20260927/c2` |
| `ROADMAP.md` | `verify/d3_contact_20260928/g2` |
| `ROADMAP.md` | `verify/d3_contact_20260928b/m1` |
| `ROADMAP.md` | `verify/d3_drive_20260928/e3` |
| `ROADMAP.md` | `verify/d3_noboost_20261003/a1` |
| `ROADMAP.md` | `verify/run_` |
| `STUBS.md` | `log/print` |
| `UNCERTAINTIES.md` | `log/u9059_` |
| `UNCERTAINTIES.md` | `verify/cfgedit_` |
| `UNCERTAINTIES.md` | `verify/d2_latbr_20260930/orig_lb` |
| `UNCERTAINTIES.md` | `verify/d2_reopen_20260929/solo_a6bfix` |
| `UNCERTAINTIES.md` | `verify/d2_score_20260930/s` |
| `UNCERTAINTIES.md` | `verify/d3_elim_20261003/o_t` |
| `UNCERTAINTIES.md` | `verify/d3_modes37_20261009/o_rwlut` |
| `UNCERTAINTIES.md` | `verify/parity_race_20260830/orig_frame.json` |
| `UNCERTAINTIES.md` | `verify/player_reg_20260929/pt3_` |
| `UNCERTAINTIES.md` | `verify/player_reg_20260929/true_solo_head` |
| `hooks.csv` | `log/b5e-solver-island/b5e_k2_acceptance_2026-07-16.log` |
| `hooks.csv` | `log/b5e-solver-island/b5e_k3_acceptance_2026-07-16.log` |
| `hooks.csv` | `log/b5e-solver-island/b5e_k4_acceptance_2026-07-16.log` |
| `hooks.csv` | `log/b5e-solver-island/b5e_k5_hook_manifest_2026-07-16.txt` |
| `hooks.csv` | `log/b5e-solver-island/b5e_k6_hook_manifest_2026-07-17.txt` |
| `hooks.csv` | `log/c4_stalkerdiff_result.json` |
| `hooks.csv` | `log/diff_CarSlotInit.csv` |
| `hooks.csv` | `log/diff_MenuButtonDetectC.csv` |
| `hooks.csv` | `log/diff_MenuButtonDetectE.csv` |
| `hooks.csv` | `log/diff_MenuCursorBack.csv` |
| `hooks.csv` | `log/diff_audio_arena_block_is_free.csv` |
| `hooks.csv` | `log/diff_audio_byte54_bit3_get.csv` |
| `hooks.csv` | `log/diff_audio_find_extension.csv` |
| `hooks.csv` | `log/diff_audio_fmt_convert_byte_length.csv` |
| `hooks.csv` | `log/diff_audio_list_index_of_key.csv` |
| `hooks.csv` | `log/diff_audio_list_min_key_select.csv` |
| `hooks.csv` | `log/diff_audio_media_subtype_from_tag.csv` |
| `hooks.csv` | `log/diff_audio_pcm_saturated_add.csv` |
| `hooks.csv` | `log/diff_audio_renderer_field3c_set.csv` |
| `hooks.csv` | `log/diff_audio_shift_add_mul64.csv` |
| `hooks.csv` | `log/diff_audio_slot_pair_zero.csv` |
| `hooks.csv` | `log/diff_audio_source_loop_set.csv` |
| `hooks.csv` | `log/diff_crt_get_env_strings.csv` |
| `hooks.csv` | `log/diff_crt_set_envp.csv` |
| `hooks.csv` | `log/diff_font_sys_set_active_camera.csv` |
| `hooks.csv` | `log/diff_menu_slot_init.csv` |
| `hooks.csv` | `log/diff_player_slot_zero.csv` |
| `hooks.csv` | `log/diff_timer_array_zero.csv` |
| `hooks.csv` | `log/mass_canonical_observe_r2.txt` |
| `hooks.csv` | `log/mass_canonical_wave0.txt` |
| `hooks.csv` | `log/negctl_reearn/b02/negctl_20261011_175835.tsv` |
| `hooks.csv` | `log/negctl_reearn/b08_rundiff_frontend_raycast_forward.log` |
| `hooks.csv` | `log/observe_cold_boot_to_menu_c4-batch-a-s13.txt` |
| `hooks.csv` | `log/observe_cold_boot_to_menu_c4-batch-a-s16.txt` |
| `hooks.csv` | `log/observe_main_menu_idle_10s_c4-batch-a-s1.txt` |
| `hooks.csv` | `log/observe_main_menu_idle_10s_c4-batch-a-s4.txt` |
| `hooks.csv` | `log/verify_hook_install_AudioBitScanForward.txt` |
| `hooks.csv` | `log/verify_hook_install_CarSlotInit.txt` |
| `hooks.csv` | `log/verify_hook_install_FontCtx_SetTranslation.txt` |
| `hooks.csv` | `log/verify_hook_install_GetDat0067ea64.txt` |
| `hooks.csv` | `log/verify_hook_install_GetDat0067ecb4.txt` |
| `hooks.csv` | `log/verify_hook_install_GetEventFlag.txt` |
| `hooks.csv` | `log/verify_hook_install_GetPlayerStateBits.txt` |
| `hooks.csv` | `log/verify_hook_install_MenuEntryArrayInit.txt` |
| `hooks.csv` | `log/verify_hook_install_RwV3dTransformPoint.txt` |
| `hooks.csv` | `log/verify_hook_install_RwV3dTransformVector.txt` |
| `hooks.csv` | `log/verify_hook_install_TimerArrayClear.txt` |
| `hooks.csv` | `log/verify_hook_install_TimerGlobalsReset.txt` |
| `hooks.csv` | `log/verify_hook_install_Vec2Length.txt` |
| `hooks.csv` | `log/verify_hook_install_Vec2Normalize.txt` |
| `hooks.csv` | `log/verify_hook_install_crt_pre_init_loop.txt` |
| `hooks.csv` | `log/verify_hook_install_get_event_flag.txt` |
| `hooks.csv` | `log/verify_hook_install_get_player_state_bits.txt` |
| `hooks.csv` | `verify/u9147_20260929/a6bexe` |
| `re/ACCOUNT2_CAPABILITIES.md` | `log/status/diff/show` |
| `re/AREA_LEDGER.md` | `log/area-hud` |
| `re/AREA_LEDGER.md` | `log/diff_rw_str` |
| `re/AREA_LEDGER.md` | `log/diff_rw_texture_set_` |
| `re/HARNESS_BACKLOG.md` | `verify/nav_shots/chall_step` |
| `re/NEXT_SESSION.md` | `log/drawstream_` |
| `re/NEXT_SESSION.md` | `verify/d3_elim_20261003/o_t` |
| `re/PROMOTION_QUEUE.md` | `log/area-track/build_r1b.log` |
| `re/PROMOTION_QUEUE.md` | `log/diff_` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioContextLookup.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioFieldEndianPack.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioFmtDescCopy.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioFmtDescEqual.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioFmtEntryMatch.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioFmtGlobalScan.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioFmtSizeCalc.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioFmtTableSearch.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioListDrain.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioListInsertHead.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioListRemoveByValue.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioPoolBlockAlloc.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioPoolConstruct.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioPoolDestroy.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioPoolFree.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioSubStructBufCleanup.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioWaveFmtCopy.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_AudioWaveNodeFree.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CarSlotInit.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_ConfigFilenameGet.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_ConfigLoad.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_ConfigLogDebug.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_ConfigLogError.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CrtExit.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CrtExitNormal.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CrtExitProcess.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CrtFastErrorExit.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CrtSehEpilog.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CrtSehProlog.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_CrtStackProbe.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_EntityFieldSet.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_HotkeyStringBaseGet.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_LapFracGetBySlot.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_LapLapsGetBySlot.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_LapTimeALessThanB.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_MenuCursorBack.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_MenuModeSync.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_MenuOptionSlotGet.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_MenuSlotCount.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_ModeScoreGetBySlot.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_PlayerHotkeyTableGet.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_RenderState_GetTexturingOverride.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_RwsChunkHeaderRead.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_RwsChunkSeek.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_SaveFileExists.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_SaveLoad.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_SaveStatusClear.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_SaveWrite.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_SplitScreenTrackAssignment.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_SpriteAnimFrameThunk.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_arena_block_is_free.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_byte54_bit3_get.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_find_extension.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_fmt_convert_byte_length.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_list_index_of_key.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_list_min_key_select.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_media_subtype_from_tag.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_pcm_saturated_add.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_renderer_field3c_set.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_shift_add_mul64.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_slot_pair_zero.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_audio_source_loop_set.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_com_release_thunk.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_crt_get_env_strings.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_crt_set_envp.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_exception_filter_install.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_font_sys_set_active_camera.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_hud_draw_enabled.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_indexed_table_lookup.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_intro_splash_` |
| `re/PROMOTION_QUEUE.md` | `log/diff_menu_menus_ba..be.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_menu_slot_init.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_player_slot_zero.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_race_guard_concluded_and_p1_won.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_rank_sort_dispatcher.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_save_autosave_trigger.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_save_deserialize_from_buffer.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_save_serialize_to_buffer.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_time_display_set_entry.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_timer_array_zero.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_timer_subarray_init.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_track_phys_world_1_get.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_track_phys_world_2_get.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_track_phys_world_3_get.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_array_get_8a94e0.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_deref_55dec0.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_field_add_8a94f0.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_init_898a90.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_reset_8a9500.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_return_one_4d8560.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_set_8a95ac.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_table_init_8a9620.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_table_read_46b4f0.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_write_ptr.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_zero_scatter_45c480.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_zero_table_86a4a0.csv` |
| `re/PROMOTION_QUEUE.md` | `log/diff_util_zero_table_8964c0.csv` |
| `re/PROMOTION_QUEUE.md` | `log/milestone_b5_screenshot.png` |
| `re/PROMOTION_QUEUE.md` | `log/observe_cold_boot_to_menu_c4-batch-a-s13.txt` |
| `re/PROMOTION_QUEUE.md` | `log/observe_cold_boot_to_menu_c4-batch-a-s16.txt` |
| `re/PROMOTION_QUEUE.md` | `log/observe_main_menu_navigate_all_c4-batch-a-s10.txt` |
| `re/PROMOTION_QUEUE.md` | `log/observe_main_menu_navigate_all_c4-batch-a-s11.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_AudioAlignedAlloc.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_AudioBitScanForward.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_FUN_0042aff0.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_FUN_0042b180.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_FUN_0042b310.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_FUN_0042b770.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_FontCtx_SetTranslation.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_FrontendArrayGet.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_FrontendGlobalGet.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_GetDat0067ea64.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_GetDat0067ecb4.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_GetDat0067ecb8.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_GetRaceEndFlag.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_HotkeyStringBaseGet.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_HudSlotTypePlayer0.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_HudSlotTypePlayer1.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_HudSlotTypePlayer2.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_LapLapsGetBySlot.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_PlayerHotkeyTableGet.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_RwMatrixScale.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_RwV3dTransformPoint.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_RwV3dTransformVector.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_SpriteAnimFrameThunk.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_SpriteLookupTableB.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_SpriteSlotGate.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_Vec2Length.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_Vec2Normalize.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_post_trophy_event.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_thunk_replay_save.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_world_render_dispatch_begin.txt` |
| `re/PROMOTION_QUEUE.md` | `log/verify_hook_install_world_render_dispatch_end.txt` |
| `re/SESSION_PROMPTS_WAVE11.md` | `verify/flip` |
| `re/SESSION_PROMPTS_WAVE13.md` | `verify/flip` |
| `re/SESSION_PROMPTS_WAVE14.md` | `verify/flip` |
| `re/analysis/B5b_RWP37_QHULL_VENDOR_2026-07-14.md` | `log/b5b_qhull_capture/body` |
| `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` | `log/b5e-solver-island/b5e_cluster1_build.log` |
| `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` | `log/b5e-solver-island/b5e_k16_` |
| `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` | `log/b5e-solver-island/b5e_k2_acceptance_2026-07-16.log` |
| `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` | `log/b5e-solver-island/b5e_k3_acceptance_2026-07-16.log` |
| `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` | `log/b5e-solver-island/b5e_k4_acceptance_2026-07-16.log` |
| `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` | `log/b5e-solver-island/b5e_k5_acceptance_2026-07-16.log` |
| `re/analysis/B5e_SOLVER_ISLAND_2026-07-15.md` | `log/b5e-solver-island/b5e_k6_acceptance_2026-07-17.log` |
| `re/analysis/CAR_BRIGHTNESS_2026-09-30.md` | `verify/car_bright_20260930/bin/mashed_re.exe` |
| `re/analysis/CAR_BRIGHTNESS_2026-09-30.md` | `verify/car_bright_20260930/sadrive` |
| `re/analysis/CAR_BRIGHTNESS_2026-09-30.md` | `verify/car_bright_fix_20260930/bin/mashed_re.exe` |
| `re/analysis/CAR_BRIGHTNESS_2026-09-30.md` | `verify/run_` |
| `re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md` | `verify/car_gray_20260929/bin/mashed_re.exe` |
| `re/analysis/CAR_GRAY_FIX_ACCEPTANCE_2026-09-30.md` | `verify/car_gray_fix_20260930/bin/` |
| `re/analysis/CHANGELOG.md` | `log/ai_ab_tick_00418860_` |
| `re/analysis/CHANGELOG.md` | `log/ai_ab_vehstep_00418560_rule` |
| `re/analysis/CHANGELOG.md` | `log/c4_stalkerdiff_result.json` |
| `re/analysis/CHANGELOG.md` | `log/d1_spike_` |
| `re/analysis/CHANGELOG.md` | `log/diff_menu_slot_init.csv` |
| `re/analysis/CHANGELOG.md` | `log/diff_scenario_batch_audio_pool_free_guard_` |
| `re/analysis/CHANGELOG.md` | `log/diff_scenario_batch_audio_table_dispatch_` |
| `re/analysis/CHANGELOG.md` | `log/drawstream_` |
| `re/analysis/CHANGELOG.md` | `log/drivehold_` |
| `re/analysis/CHANGELOG.md` | `log/foo.txt` |
| `re/analysis/CHANGELOG.md` | `log/force_screen_5` |
| `re/analysis/CHANGELOG.md` | `log/negctl_reearn/b02/negctl_20261011_175835.tsv` |
| `re/analysis/CHANGELOG.md` | `log/negctl_reearn/b08_rundiff_frontend_raycast_forward.log` |
| `re/analysis/CHANGELOG.md` | `log/race_a6_` |
| `re/analysis/CHANGELOG.md` | `verify/cfgedit_` |
| `re/analysis/CHANGELOG.md` | `verify/chalsel_panel` |
| `re/analysis/CHANGELOG.md` | `verify/d08_override` |
| `re/analysis/CHANGELOG.md` | `verify/d2_latbr_20260930/orig_lb` |
| `re/analysis/CHANGELOG.md` | `verify/d2_reopen_20260929/orig_solo` |
| `re/analysis/CHANGELOG.md` | `verify/d2_score_20260930/s` |
| `re/analysis/CHANGELOG.md` | `verify/d3_contact_20260928b/s2` |
| `re/analysis/CHANGELOG.md` | `verify/d3_elim_20261003/o_e` |
| `re/analysis/CHANGELOG.md` | `verify/d3_elim_20261003/o_t` |
| `re/analysis/CHANGELOG.md` | `verify/d3_noboost_20261003/a1` |
| `re/analysis/CHANGELOG.md` | `verify/parity_race_20260830/orig_frame.json` |
| `re/analysis/CHANGELOG.md` | `verify/player_reg_20260929/true_solo_head` |
| `re/analysis/CHANGELOG.md` | `verify/run_` |
| `re/analysis/CHANGELOG.md` | `verify/screen_id/screen_` |
| `re/analysis/CHANGELOG.md` | `verify/vp_split.png` |
| `re/analysis/CHANGELOG.md` | `verify/wedge_rate` |
| `re/analysis/COLLISION_GATE_BRIEF_D1_2026-07.md` | `log/d1_spike_` |
| `re/analysis/D1_SPIKE_PROXY_BYPASS_2026-07-06.md` | `log/d1_spike_` |
| `re/analysis/D2_CONTROLLED_ARM_2026-09-29.md` | `verify/d2_ctrl_20260929/g56_` |
| `re/analysis/D2_CONTROLLED_ARM_2026-09-29.md` | `verify/d2_ctrl_20260929/head_` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/.../c2.msd` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/a8_steer_20260824/orig_steer` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/d2_bounce_20260930/score` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/d2_offmesh_20260929/tr` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/d2_reopen_20260929/guards/g_e1` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/d2_reopen_20260929/orig_solo` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/d2_reopen_20260929/solo_a6bfix` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/d2_reopen_20260929/solo_post` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/d2_score_20260930/s` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9147_20260929/a6bexe` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9147_20260929/a6bfix` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9147_20260929/fix` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9147_20260929/guards/g_e1` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9154_20260929/h` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9156_20260929/f` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9156_20260929/guards/g_e` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9156_20260929/o` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9156_20260929/s` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9156b_20260930/guards/arc` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9156b_20260930/guards/g_e` |
| `re/analysis/D2_REOPEN_2026-09-29.md` | `verify/u9156b_20260930/s` |
| `re/analysis/D3_AI_PORT_2026-09-26.md` | `verify/d3_ai_20260914/orig_step_slots` |
| `re/analysis/D3_AI_PORT_2026-09-26.md` | `verify/d3_ai_20260926/o_spread1..3` |
| `re/analysis/D3_AI_PORT_2026-09-26.md` | `verify/d3_ai_20260926/sa2` |
| `re/analysis/D3_AI_PORT_2026-09-26.md` | `verify/d3_ai_20260926/sa4` |
| `re/analysis/D3_AI_RESIDUE_2026-09-27.md` | `verify/d3_ai_20260927b/pN.msd` |
| `re/analysis/D3_BOX_STATE_2026-09-28.md` | `verify/d3_contact_20260928b/s2` |
| `re/analysis/D3_DRIVE_2026-09-28.md` | `verify/d3_ai_20260927b/p` |
| `re/analysis/D3_DRIVE_2026-09-28.md` | `verify/d3_drive_20260928/b_` |
| `re/analysis/D3_DRIVE_2026-09-28.md` | `verify/d3_drive_20260928/e` |
| `re/analysis/D3_MODES_2026-09-14.md` | `log/rules_oracle_rule` |
| `re/analysis/D3_MODES_2026-09-26.md` | `verify/d3_modes_20260926/sweep` |
| `re/analysis/D3_SPEED_GAP_2026-09-28.md` | `verify/d3_speed_20260928/a6` |
| `re/analysis/D3_SPEED_GAP_2026-09-28.md` | `verify/d3_speed_20260928/cd` |
| `re/analysis/D3_SPEED_GAP_2026-09-28.md` | `verify/d3_speed_20260928/d1.msd` |
| `re/analysis/D3_SPEED_GAP_2026-09-28.md` | `verify/d3_speed_20260928/e1.msd` |
| `re/analysis/DEGENERATE_GREEN_AUDIT_2026-06-12.md` | `log/diff_` |
| `re/analysis/DIVERGENCE_LEDGER_3D.md` | `verify/frontend_ref/menu_burst_` |
| `re/analysis/DIVERGENCE_LEDGER_3D.md` | `verify/parity3d/orig_race_t01..t05.png` |
| `re/analysis/LIBRW_SIZING_2026-08.md` | `verify/dbg_backbuffer.bmp` |
| `re/analysis/LIBRW_SIZING_2026-08.md` | `verify/s3bank_iso/vdump_` |
| `re/analysis/MOVIE_SKIP_ROOTCAUSE_2026-06-22.md` | `verify/_intro_skip_clean_menu` |
| `re/analysis/NEXT_SESSION_20260831.md` | `verify/screen_id/footers_` |
| `re/analysis/NEXT_SESSION_race_parity_20260831.md` | `verify/u9062_` |
| `re/analysis/PICKUPS_LOOK_PLACEMENT_2026-09-29.md` | `verify/pickups_20260929/bin/mashed_re.exe` |
| `re/analysis/PLAYER_REGRESSION_2026-09-29.md` | `verify/player_reg_20260929/fix_` |
| `re/analysis/PLAYER_REGRESSION_2026-09-29.md` | `verify/player_reg_20260929/pt2_` |
| `re/analysis/PLAYER_REGRESSION_2026-09-29.md` | `verify/player_reg_20260929/pt3_` |
| `re/analysis/PLAYER_REGRESSION_2026-09-29.md` | `verify/player_reg_20260929/tr_` |
| `re/analysis/QOL_PATCH_PLAN_2026-08.md` | `verify/qol_asi_20260801/screen_1440p` |
| `re/analysis/RACE_POSITION_RECON_SCOPE_2026-10-06.md` | `verify/d3_elim_20261003/o_e` |
| `re/analysis/RACE_POSITION_RECON_SCOPE_2026-10-06.md` | `verify/d3_overspeed_20261006/RESULT_U9186_` |
| `re/analysis/SEA_LEVEL_2026-09-29.md` | `verify/sea_level_20260929/bin/mashed_re.exe` |
| `re/analysis/SEA_LEVEL_2026-09-29.md` | `verify/sea_level_20260929/sa_fixed/race1/01_grid.bmp` |
| `re/analysis/SESSION_VERIFICATION_AUDIT_2026-06-16.md` | `log/diff_` |
| `re/analysis/SESSION_VERIFICATION_AUDIT_2026-08-15.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/STEER_CALIB_RUNTIME_2026-06-18.md` | `verify/flip` |
| `re/analysis/U9138_FIX_2026-09-28.md` | `verify/u9138_20260928/postfix/` |
| `re/analysis/ai_update/0x00418560.md` | `log/ai_ab_vehstep_00418560_rule` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/build_pizclose.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_CarSlotAssign.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_CarSlotInit.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_CarSlotInit1P.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_EntityFieldSet.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_MenuCursorBack.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_arena_block_is_free.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_byte54_bit3_get.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_find_extension.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_fmt_convert_byte_length.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_list_index_of_key.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_list_min_key_select.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_media_subtype_from_tag.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_pcm_saturated_add.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_renderer_field3c_set.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_shift_add_mul64.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_slot_pair_zero.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_audio_source_loop_set.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_crt_get_env_strings.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_crt_set_envp.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_font_sys_set_active_camera.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_harness_test_` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_hud_draw_enabled.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_lap_laps_get_by_slot.csv2026-05-14` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_player_slot_zero.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/diff_timer_array_zero.csv` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/mass_canonical_observe_r2.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/mass_canonical_observe_r3b.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/observe_c4_sweep_20260517_` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/verify_hook_install_car_slot_state_get.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/verify_hook_install_crt_pre_init_loop.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/verify_hook_install_post_trophy_event.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/verify_hook_install_thunk_replay_save.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/verify_hook_install_track_node_dispatch14.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `log/verify_hook_install_track_node_dispatch44.txt` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `verify/msm_tree_` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `verify/parity/parity_` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `verify/r4/arctic_fly_` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `verify/r6/round` |
| `re/analysis/archive/CHANGELOG_pre20260615.md` | `verify/r6/round_` |
| `re/analysis/archive/ROADMAP_v2_2026-06-09.md` | `log/ai_ab_` |
| `re/analysis/archive/ROADMAP_v2_2026-06-09.md` | `verify/parity/parity_` |
| `re/analysis/archive/ROADMAP_v2_2026-06-09.md` | `verify/r4/arctic_fly_` |
| `re/analysis/archive/ROADMAP_v2_2026-06-09.md` | `verify/r6/round` |
| `re/analysis/archive/ROADMAP_v2_2026-06-09.md` | `verify/race1/00_results.bmp/png` |
| `re/analysis/argtype_orphans_triage_2026-07.md` | `log/diff_` |
| `re/analysis/bce0_edx_contract_20260910.md` | `log/shadow_ab/launch_20260910_1414` |
| `re/analysis/boot_app_init/00402750.md` | `log/print` |
| `re/analysis/boot_app_init/004996f0.md` | `log/build_s2_v1.txt` |
| `re/analysis/boot_subsystem_d3/0x004955b0.md` | `log/build_dinput_predicate.txt` |
| `re/analysis/bucket_004fcac3/0x005b8c40.md` | `log/dB` |
| `re/analysis/bucket_audio_005ab710_005af040/0x005aeb90.md` | `log/dB-style` |
| `re/analysis/c2_note_pilot_20261011/README.md` | `log/ghidra_funcfacts.jsonl` |
| `re/analysis/cluster_004f_first_pass/004f0940.md` | `log/format` |
| `re/analysis/font_fgdc20_text_law.md` | `verify/orig_backbuffer_f` |
| `re/analysis/formats/track_world_bsp.md` | `verify/r4/arctic_fly_` |
| `re/analysis/frontend_config_screens_REmap_20260614.md` | `verify/parity/cmp/s` |
| `re/analysis/frontend_feedback_20260612.md` | `log/parity_burst_scrN.json` |
| `re/analysis/frontend_feedback_20260612.md` | `log/parity_re_scrN.json` |
| `re/analysis/frontend_feedback_20260612.md` | `verify/.../badge_Button_sprite.png` |
| `re/analysis/frontend_feedback_20260612.md` | `verify/dbg_backbuffer.bmp` |
| `re/analysis/frontend_feedback_20260612.md` | `verify/frontend/screen_5/26/28.bmp` |
| `re/analysis/frontend_feedback_20260612.md` | `verify/orig_backbuffer_f2100.bmp` |
| `re/analysis/frontend_feedback_20260612.md` | `verify/parity_caps/orig_scrN_` |
| `re/analysis/frontend_feedback_20260827.md` | `verify/_cmp_s4_title.png` |
| `re/analysis/frontend_input_nav_trace/TRACE.md` | `log/c4_nav_batch_result` |
| `re/analysis/frontend_input_nav_trace/TRACE.md` | `verify/c4nav_A/c4nav_ON_g` |
| `re/analysis/frontend_input_nav_trace/TRACE.md` | `verify/x.png` |
| `re/analysis/frontend_s_dod_audit_20260517.md` | `log/observe_` |
| `re/analysis/fun_004b6940_scoping/REPORT.md` | `log/build_pizwin32.txt` |
| `re/analysis/fun_004b6940_scoping/REPORT.md` | `verify/pizwin32_track_load.png` |
| `re/analysis/hud_ingame_promote_c2/0x00426ba0.md` | `log/diff_hud_draw_enabled.csv` |
| `re/analysis/hud_ingame_promote_c2/0x00428450.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/negctl/followup_20261012/crash1509b4_RESULT.md` | `verify/crash1509b4_20261012/c4_pb2_` |
| `re/analysis/negctl/followup_20261012/crash1509b4_RESULT.md` | `verify/crash1509b4_20261012/decomp_` |
| `re/analysis/negctl/reearn_20261011/c4_menu_button_detect_e_RESULT.md` | `log/c4_detect_e/c4_detect_e_feas_off.json` |
| `re/analysis/negctl/reearn_20261011/decisions/b02.tsv` | `log/negctl_reearn/b02/negctl_20261011_175835.tsv` |
| `re/analysis/negctl/reearn_20261011/decisions/b08.tsv` | `log/negctl_reearn/b08_rundiff_frontend_raycast_forward.log` |
| `re/analysis/negctl/reearn_20261011/fix_bootio_RESULT.md` | `log/diff_` |
| `re/analysis/negctl/reearn_20261011/fix_fstate_RESULT.md` | `log/build_fstate_restore.txt` |
| `re/analysis/negctl/reearn_20261011/fix_fstate_RESULT.md` | `log/fstate_siblings_warm.txt` |
| `re/analysis/negctl/session_20261012b/drawstream_RESULT.md` | `log/drawstream_` |
| `re/analysis/negctl/session_20261012b/scr32_texty_RESULT.md` | `verify/orig_backbuffer_f300.bmp` |
| `re/analysis/parity_tooling.md` | `verify/dbg_backbuffer.bmp` |
| `re/analysis/parity_tooling.md` | `verify/heat.png` |
| `re/analysis/parity_tooling.md` | `verify/orig_backbuffer_f` |
| `re/analysis/plans/frontend_c4_lift_plan.md` | `log/observe_c4_sweep_` |
| `re/analysis/plans/orchestrator_loop_2026-07-26.md` | `log/diff_` |
| `re/analysis/plans/orchestrator_loop_2026-07-26.md` | `log/diff_palette_` |
| `re/analysis/plans/prescreen_recipe.md` | `verify/prescreen_` |
| `re/analysis/plans/tracker_health_2026-07-24.md` | `log/json` |
| `re/analysis/promote_c2_boot_crt/004a4bb7.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/promote_c2_dinput_init/004971b0.md` | `log/fopen` |
| `re/analysis/promote_c2_input_lua/00495530.md` | `log/print` |
| `re/analysis/promote_c2_piz_loader/00495280.md` | `log/print` |
| `re/analysis/promote_c2_piz_loader/004952f0.md` | `log/print` |
| `re/analysis/promote_c2_render_midrva/00478200.md` | `log/printf` |
| `re/analysis/promote_c2_rw_render_submit/004cc7f0.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/promote_c2_vehicle_lowrva/0x00411870.md` | `log/wprintf` |
| `re/analysis/promote_c2_video_cfg/004997b0.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/r6_demand_map_2026-07.md` | `log/auto_count_at_menu` |
| `re/analysis/r6_demand_map_2026-07.md` | `log/install_observe_` |
| `re/analysis/race_camera/render_camera_child_c_20260830.md` | `log/cam_re.txt` |
| `re/analysis/race_results/0040e470.md` | `log/verify_hook_install_car_slot_state_get.txt` |
| `re/analysis/race_terrain_ambient_20260830.md` | `verify/geomlight_arctic_` |
| `re/analysis/race_terrain_ambient_20260830.md` | `verify/geomlight_train_` |
| `re/analysis/race_terrain_ambient_20260830.md` | `verify/terrain_` |
| `re/analysis/render_2_c1_to_c2_s6/FUN_00494820.md` | `log/assert` |
| `re/analysis/render_c1_to_c2_s2/FUN_0042a530.md` | `log/printf` |
| `re/analysis/shadow_crasher_bisect_20260910.md` | `log/shadow_ab/shadow_ab_20260910_1432` |
| `re/analysis/shadow_lane_20260910.md` | `log/shadow_ab/launch_20260910_0018` |
| `re/analysis/sprite_gate_c3/0x004c5c00.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/standalone_menu_sm/FUN_0043c5b0_port_spec.md` | `verify/msm_tree_` |
| `re/analysis/standalone_menu_sm/PARITY_2026-06-10.md` | `verify/parity/parity_` |
| `re/analysis/structs/contcfg_record.md` | `log/action_sem_a` |
| `re/analysis/structs/contcfg_record.md` | `log/action_sem_menu_a` |
| `re/analysis/structs/contcfg_record.md` | `log/action_sem_sub_a` |
| `re/analysis/structs/contcfg_record.md` | `verify/screen_id/screen_` |
| `re/analysis/timer/0x004950b0.md` | `log/mass_canonical_wave0.txt` |
| `re/analysis/timer_d3_cont1_a/0x0040de00.md` | `log/printf` |
| `re/analysis/track_loader/0x00426e10.md` | `log/print` |
| `re/analysis/u9025_stream_lock_race.md` | `log/semstock_run` |
| `re/analysis/u9025_stream_lock_race.md` | `log/wedge_run` |
| `re/analysis/video_mci/HALT_NOTE.md` | `log/printf` |
| `re/analysis/window_msgpump/00499690.md` | `log/build_s2_v1.txt` |
| `re/diag/KNOWN_ISSUES.md` | `verify/r5/car_` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_ai_race_optionB.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_atlas_full.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_atlas_glyphs.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_badge_cap.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_boot_fix_check.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_boot_fixed_fopen.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_boot_fixed_logpatch.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_boot_fixed_mashedwin.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_boot_fixed_menu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_bootmodal_f150.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_bootmodal_f150.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_bootmodal_f60.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_bootmodal_f60.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_chk_boot.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_chk_disc.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_chk_orig_s1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_chk_orig_s30.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_chk_orig_s4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_chk_orig_s8.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_chk_s4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_clean_baseline_menu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cmp_s4_hi.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cmp_s4_title.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cmp_s4_title2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cmp_s4_title3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cmp_s4_title4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cur_s1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cur_s15.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_cur_s4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_dat/` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_dat/BADGES.TXD` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_dat/ENGLISH.DAT` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_dat/FX.TXD` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_dat/INTERFACE.TXD` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_dat/TEXTURES.TXD` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_1555.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_4444.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_565.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_8bpp.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_compare.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_final.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_gray.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_modulated.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_now.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_disc_warm.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_f2_A_copters.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_f2_B_nocopters.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_f2_copter_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_f2_copters_cropzoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_f3_A_scroll.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_f3_B_noscroll.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_font_fair_s8.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_font_gamma_cmp.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_font_zoom_s8.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_fontdump/` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_fontdump/FGDC20.TXD` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_full_orig_s1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_full_std_s1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_glyph2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_glyph_norm_sound.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_glyph_truepx.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal640.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal_cmp.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal_disc.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal_disc.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal_f10.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal_f10.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal_f40.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadmodal_f40.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadsuccess.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_loadsuccess.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_backbuffer_f120.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_backbuffer_f180.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_backbuffer_f20.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_backbuffer_f60.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_check_f250.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_check_f450.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_check_f650.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_s15_2x.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_s4_2x.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_orig_s4_tiles.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_prompt2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_prompt3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_prompt4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_prompt_cmp.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_pu/` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_pu/POWERUPS_GOLD.LUA` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_re_footer.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_re_s18.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_re_s18b.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_re_s4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_re_s6.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_s1_flag.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_s1_now.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_s4_kbd.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_s4_rowicon.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_s4_rowicon2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_s4_v2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_s8_badge.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_splash_disc.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_splash_disc.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_splash_disc2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_splash_disc2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_std_window_s8.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_v640_cmp.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_v640_s1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_v640_s4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_v640_s8.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_walk_02_mainmenu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_walk_04_colourselect.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_walk_05_abilityselect.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/_walk_06_challengeselect.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/anim_scr1_f200.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/anim_scr1_f40.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/anim_scr1_f6.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/anim_scr1_f90.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/cdaudio_20s.wav` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/red_car_10s.wav` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/sfx_explosion1.wav` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/sfx_go.wav` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/sfx_menu_navigation.wav` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/sfx_menu_selection.wav` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/audio/sfx_threetwoone.wav` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_b19a_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_b19a_run02.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_b19b_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_b19b_run02.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_b19final_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_b19font_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_b19mm_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_baseline_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_baseline_run02.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_harden2_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_harden2_run02.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_harden_run02.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_harden_run03.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_rebase20_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_rebase20_run02.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_rebase20_run03.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_rebase_run01.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/b17_rebase_run02.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/badge_Button.raw` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/badge_Button_overmagenta.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/badge_SemiC.raw` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/badge_SemiC2.raw` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/badge_SemiC2_overmagenta.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/badge_SemiC_overmagenta.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/boot_asi_disabled_menu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/boot_asi_disabled_menu2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/boot_asi_disabled_menu3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/boot_powerups_removed.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4_nav_probe_midnav.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4nav_B_options/c4nav_ON_g0.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4nav_B_options/c4nav_ON_g1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4nav_B_options/c4nav_ON_g2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4nav_B_options/c4nav_ON_g3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4nav_B_options/c4nav_ON_g4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4nav_B_options/c4nav_ON_g5.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/c4nav_B_options/c4nav_ON_g6.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/csel7_recheck.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/csel_goto2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/csel_goto4.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/csel_goto4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/csel_goto7.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/csel_goto8.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/csel_screen4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/dinput8_era_scene_t10.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/disc_orig_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/flow_loading.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/flow_menu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/flow_title.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/flow_triptych.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/fontA_orig.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/fontA_re.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/fontB_re.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_FGDC20.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_FGMC20.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_NEURO.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_P_orig.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_P_pair.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_P_re.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_anaglyph_exit.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_anaglyph_exit2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit4x.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_dev.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_native.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_native2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_native3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_offsetfix.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_ss2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_v2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_exit_v4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_footer.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_header.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_header2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_items.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_items_offsetfix.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_cmp_watermark2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_fair.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_full_pair.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_montage.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_pb_sidebyside.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_pb_sidebyside2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_pb_sidebyside3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_re_chk.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_re_title.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_re_title2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_re_title3_small.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_re_title_640.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_zoom_orig_pb.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/font_zoom_re_pb.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/footer_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/footer_zoom2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/orig_scr8_cap.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_dbg_both_off.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_dbg_committed.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_dbg_committed_top.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_dbg_noarc.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_dbg_screencap.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_post_splash_title.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_scr1_wave1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_scr8_now.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_title_postreboot.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/re_title_wave1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_O.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_OR.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_R.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_R2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_band_compare.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_compare.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_orig.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_orig_real.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/cap_re.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/font_O.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/font_R.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/orig_footer.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/our_cap_tight.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/our_footer.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/frontend_parity2/zoom/our_footer2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/fullscreen.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/inproc_feed_down3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/inproc_feed_menu_moved.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/item_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/locked_screen.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/mainmenu_title.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/mainmenu_title_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/mashed_t08.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/mashed_t14.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/mashed_t20.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/menu_footer.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/msm_tree_` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/msm_tree_1_root.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/msm_tree_2_root_item1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/msm_tree_3_screen8.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/msm_tree_4_screen19.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/msm_tree_5_back_screen8.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/msm_tree_6_back_root.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_backbuffer_f` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_backbuffer_f1000.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_backbuffer_f1100.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_backbuffer_f1400.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_backbuffer_f300.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_backbuffer_f500.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_backbuffer_f800.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_boot_grid.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_chk_f1500.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_chk_f1600.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_chk_f1900.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_chk_f2100.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_chk_f2400.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_chk_f2800.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_clean_items_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_clean_scr1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/orig_load_f200.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/our_slide_f14.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/_orig_s15_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/_orig_s16_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/_orig_s24_rows.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/_orig_s4_rows.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/_orig_s6_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s1.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s15.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s16.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s18.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s19.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s29.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s30.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s31.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s32.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s33.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s6.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s7.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/cmp/s8.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/orig_1_root.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/orig_2_mainmenu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/sa_gts.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity/sa_sound_adjusted.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_liveries_t07.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_liveries_t08.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_realcam2_t07.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_realcam2_t09.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_realcam2_t11.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_realcam_t06.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_realcam_t10.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_realcam_t22.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_scoring_t12.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity3d/sa_scoring_t25.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr19_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr19_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr19_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr1_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr1_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr1_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr2_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr2_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr2_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr30_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr30_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr30_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr32_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr32_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr32_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr3_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr3_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr3_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr4_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr4_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr4_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr8_0.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr8_1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/orig_scr8_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_modal.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr1.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr19.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr19_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr1_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr2_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr3.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr30.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr30_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr32.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr32_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr3_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr4.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr4_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr8.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_scr8_win.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_splash.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/parity_caps/re_splash2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/r6/round` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/00_challengeselect.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/00_challengeselect_prog.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_action.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_action_lit.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_grid.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_grid_egypt.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_grid_pu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_inrace_track.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_inrace_track_egypt.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_inrace_track_laps.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_results.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/01_results_laps.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/02_back_to_menu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/race1/02_results.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_0.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_10.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_101.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_111.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_122.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_132.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_142.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_20.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_30.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_40.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_50.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_60.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_61.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_70.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_71.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_80.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_81.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scenario/sa_round_91.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_baseline_title.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t10.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t101.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t111.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t121.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t20.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t30.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t40.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t60.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t70.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t81.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_t91.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_title.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/scoring/scoring_track.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/screen_2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/screen_25.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/screen_26.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/screen_28.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/screen_3.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/screen_5.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/slide_mid.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/slide_pair.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/slide_settled.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/unlock_00_menu.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/view_s15.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/view_s16.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/view_s18.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/view_s4.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/view_s6.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/view_s7.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_01_title.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_02_mainmenu.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_03_singleplayer.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_04_colourselect.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_05_abilityselect.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_06_challengeselect.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_07_back_mainmenu.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_08_options_highlighted.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_09_options.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_10_sound.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_11_sound_adjusted.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/walk_12_back_mainmenu2.bmp` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/win_capture_bg.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/win_capture_bg_small.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/wm_postmsg_down.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/wm_zoom.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/ws_e_lighting/AFTER_chase.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/ws_e_lighting/AFTER_standings.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/ws_e_lighting/BEFORE_chase.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/ws_e_lighting/BEFORE_standings.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/ws_e_lighting/ORIGINAL_reference.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_footer_orig.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_footer_re.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_insults_orig.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_loading_f200.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_loading_f260.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_loading_f320.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_splash_orig.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_splash_orig2.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_splash_orig3.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_watermark_orig.png` |
| `re/diag/cruft_triage_2026-07-02.md` | `verify/zoom_watermark_re.png` |
| `re/orchestrator/read_fleet/runs/w1_relink/w1b_doc_roadmap.md` | `verify/d1_` |
| `re/scenarios/README.md` | `verify/scenarios/NNN-` |
| `scripts/ttd/README.md` | `log/ttd/MASHED_` |
| `verify/d1_evidence/README.md` | `verify/d1_` |
| `verify/d1_fxcut/RESULT.md` | `verify/d1_` |
| `verify/d1_fxcut/RESULT.md` | `verify/d1_iso_ctl` |
| `verify/d1_fxcut/RESULT.md` | `verify/d1_sky_d3d9_nofog/` |
| `verify/d1_fxcut/RESULT.md` | `verify/d1_sky_librw` |
| `verify/d1_lens/RESULT.md` | `verify/d1_` |
| `verify/d1_lens/RESULT.md` | `verify/d1_far_d3d9/` |
| `verify/d1_lens/RESULT.md` | `verify/d1_far_librw/` |
| `verify/d1_lens/RESULT.md` | `verify/d1_lens_base_d3d9/` |
| `verify/d1_lens/RESULT.md` | `verify/d1_lens_base_librw/` |
| `verify/d1_lens/RESULT.md` | `verify/d1_sky_librw/` |
| `verify/d1_mirrorfix/AUDIT.md` | `verify/d1_` |
| `verify/d1_mirrorfix/AUDIT.md` | `verify/d1_far_d3d9/` |
| `verify/d1_sky_orig/RESULT.md` | `verify/d1_` |
| `verify/d1_sky_orig/RESULT.md` | `verify/d1_sky_sa_d3d9/` |
| `verify/d1_sky_orig/RESULT.md` | `verify/d1_sky_sa_d3d9/race1/01_inrace_track.bmp` |
| `verify/d1_sky_orig/RESULT.md` | `verify/d1_sky_sa_librw/` |
| `verify/d2_l60_20261001/RESULT_STEP0.md` | `log/CSV` |
| `verify/d2_sched_20261001/PREREG_STEP1.md` | `verify/d2_l60_20261001/sc` |
| `verify/d2_sched_20261001/RESULT_STEP1.md` | `verify/d2_l60_20261001/sc` |
| `verify/d2_writer_20261001/PREREG_STEP1.md` | `verify/d2_writer_20261001/orig_bp1.boostprobe.csv` |
| `verify/d3_arm_20261005/BASELINE_DISTANCE.md` | `verify/d3_elim_20261003/o_t1` |
| `verify/d3_arm_20261005/PREREG_ARM.md` | `verify/d3_arm_20261005/A1` |
| `verify/d3_bulk_20261011/aileaf/RESULT_AILEAF.md` | `verify/d3_gatefire_20261008/AL_CTRL` |
| `verify/d3_bulk_20261011/roundgrid/RESULT_STAGE_A.md` | `verify/d3_gatefire_20261008/RG_CTRL` |
| `verify/d3_carcar_20261005/RESULT_LEG1.md` | `verify/d3_carcar_20261005/P1` |
| `verify/d3_carcar_20261005/RESULT_LEG3.md` | `verify/d3_elim_20261003/o_t` |
| `verify/d3_elim_20261003/RESULT_STEP1.md` | `verify/d3_noboost_20261003/a1` |
| `verify/d3_elim_20261003/RESULT_STEP2.md` | `verify/d3_noboost_20261003/a1` |
| `verify/d3_gatefire_20261008/RESULT_LEG1A.md` | `log/diff_` |
| `verify/d3_gatefire_20261008/RESULT_LEG2.md` | `log/diff_cam_` |
| `verify/d3_noboost_20261003/PREREG.md` | `verify/d3_rebase_20261002/r1/r2/r3.csv` |
| `verify/d3_noboost_20261003/RESULT.md` | `verify/d3_rebase_20261002/r1..r3` |
| `verify/d3_racepos_20261006/RESULT_LEGC_LEGA.md` | `verify/d3_elim_20261003/o_e` |
| `verify/d3_rebase_20261002/RESULT_STEP1.md` | `verify/d3_rebase_20261002/r` |
| `verify/d3_yaww_20261005/RESULT_YAWW.md` | `verify/d3_yaww_20261005/y1` |
| `verify/d4_save_20261005/PREREG_SAVE.md` | `verify/d4_save_20261005/gamesave.orig.bak` |
| `verify/freecam_title_20261001/PREREGISTRATION.md` | `verify/freecam_title_20261001/bin/mashed_re.exe` |
| `verify/freecam_title_20261001/RESULTS.md` | `verify/freecam_title_20261001/bin/mashed_re.exe` |
| `verify/geomlight_waterfold/RESULT.md` | `verify/arctic_ref/sea_search/s` |
| `verify/parity_20260819/DRAWLIST_scr1.md` | `log/foo.txt` |
| `verify/pickups_fix_20261001/RESULT_STAGE1.md` | `verify/pickups_fix_20261001/pose_` |
| `verify/scenarios/README.md` | `verify/scenarios/NNN-` |
| `verify/sea_fix_20260930/PREREG.md` | `verify/sea_fix_20260930/bin/mashed_re_postfix.exe` |
| `verify/sea_fix_20260930/PREREG.md` | `verify/sea_fix_20260930/bin/mashed_re_prefix.exe` |

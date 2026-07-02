# Cruft triage — 2026-07-02 (branch ws-r6-ai-control-chain)

Solo-session triage of 717 untracked working-tree files (688 under verify/).
Rule: keep anything cited by trackers or analysis notes (repo-wide grep over
*.md/*.csv/*.tsv/*.txt/*.json, basename + path + prefix/glob matching);
delete the rest. This manifest lists EVERY deletion, written before executing.

## Decisions

- **Committed (evidence, moved to subsystem subfolders)** — 32 files whose exact
  paths are cited by analysis notes; citations updated in the citing docs:
  verify/boot/ (3, BOOT_PATCHES.md), verify/font/ (5, font_fgdc20_text_law.md +
  frontend_feedback_20260612.md), verify/menu_crash/ (18 scene_t*.png,
  menu_crash_scoping), verify/ai/ (1, ai_spline_lookahead.md),
  verify/frontend/ (5 screen_*.bmp, frontend_feedback_20260612.md).
- **Committed in place (cited, siblings already tracked)** — 10 files:
  - verify/c4nav_A/c4nav_ON_g2.png
  - verify/c4nav_A/c4nav_ON_g4.png
  - verify/c4nav_A/c4nav_ON_g5.png
  - verify/c4nav_A/c4nav_ON_g6.png
  - verify/c4nav_A/c4nav_ON_g7.png
  - verify/frontend_parity2/zoom/our_cap_fixed.png
  - verify/parity/cmp/s24.png
  - verify/parity/orig_s24.bmp
  - verify/r5/car_4_chase.bmp
  - verify/r5/car_5_chase.bmp
- **Kept on disk, gitignored (regenerable dir-cited capture pools, ~98 MB)** —
  verify/p2/ (40), verify/frontend_ref/ (39), verify/allscreens/ (33),
  verify/cmp/ (17), verify/orig_screens/ (17), verify/std_screens/ (17).
  Cited as directories by frontend_faithfulness_20260613.md / TRACE.md /
  DIVERGENCE_LEDGER_3D.md / frontend_screen_map.md; regenerate via the parity
  and nav-coverage harnesses.
- **Left untracked, flagged to WS-R6 (active-workstream evidence, not this
  triage's call)** — verify/r6/round{4..7}_{go,result}.bmp (2026-06-30) and
  verify/race1/{00_challengeselect,01_action,01_grid,01_inrace_track,
  02_back_to_menu}.bmp (2026-07-01). Uncited but <48 h-to-4-day-old captures
  from the active race-loop lane; the WS-R6 session should cite-and-commit or
  drop them.
- **Deleted** — every file below. Uncited by any tracker/analysis note.
  Notable sub-calls: verify/_dat/, verify/_fontdump/, verify/_pu/ are
  piz-extracted asset duplicates (regenerable; extracted/ now gitignored);
  verify/msm_tree_*.bmp are raw dumps superseded by the tracked same-name
  .png evidence; verify/orig_backbuffer_f*.{png,bmp} are MASHED_ORIG_BBDUMP
  shim output (tool-doc prefix mentions only, regenerable per
  parity_tooling.md); verify/ws_e_lighting/ backed the already-merged WS-E s2
  commit 8019c27e; verify/scenario/ (singular) is NOT the cited
  verify/scenarios/ (plural); scripts/resfix/out*.txt are one-off run logs of
  the committed resfix{,2}.cpp diagnostics.

## Moves (citations updated in the citing docs)

- verify/_ai_race_optionB.png -> verify/ai/_ai_race_optionB.png
- verify/_boot_fixed_fopen.png -> verify/boot/_boot_fixed_fopen.png
- verify/_boot_fixed_menu.png -> verify/boot/_boot_fixed_menu.png
- verify/boot_powerups_removed.png -> verify/boot/boot_powerups_removed.png
- verify/font_anaglyph_exit.png -> verify/font/font_anaglyph_exit.png
- verify/font_cmp_exit_native3.png -> verify/font/font_cmp_exit_native3.png
- verify/font_cmp_exit_offsetfix.png -> verify/font/font_cmp_exit_offsetfix.png
- verify/font_cmp_items.png -> verify/font/font_cmp_items.png
- verify/font_cmp_items_offsetfix.png -> verify/font/font_cmp_items_offsetfix.png
- verify/scene_t007.png -> verify/menu_crash/scene_t007.png
- verify/scene_t012.png -> verify/menu_crash/scene_t012.png
- verify/scene_t017.png -> verify/menu_crash/scene_t017.png
- verify/scene_t022.png -> verify/menu_crash/scene_t022.png
- verify/scene_t027.png -> verify/menu_crash/scene_t027.png
- verify/scene_t032.png -> verify/menu_crash/scene_t032.png
- verify/scene_t036.png -> verify/menu_crash/scene_t036.png
- verify/scene_t041.png -> verify/menu_crash/scene_t041.png
- verify/scene_t046.png -> verify/menu_crash/scene_t046.png
- verify/scene_t051.png -> verify/menu_crash/scene_t051.png
- verify/scene_t056.png -> verify/menu_crash/scene_t056.png
- verify/scene_t061.png -> verify/menu_crash/scene_t061.png
- verify/scene_t066.png -> verify/menu_crash/scene_t066.png
- verify/scene_t070.png -> verify/menu_crash/scene_t070.png
- verify/scene_t075.png -> verify/menu_crash/scene_t075.png
- verify/scene_t080.png -> verify/menu_crash/scene_t080.png
- verify/scene_t085.png -> verify/menu_crash/scene_t085.png
- verify/scene_t090.png -> verify/menu_crash/scene_t090.png
- verify/screen_2.bmp -> verify/frontend/screen_2.bmp
- verify/screen_26.bmp -> verify/frontend/screen_26.bmp
- verify/screen_28.bmp -> verify/frontend/screen_28.bmp
- verify/screen_3.bmp -> verify/frontend/screen_3.bmp
- verify/screen_5.bmp -> verify/frontend/screen_5.bmp

## Deletions (472 files)

### scripts/resfix/ (2)

- scripts/resfix/out.txt
- scripts/resfix/out2.txt

### verify/ (root) (255)

- verify/_atlas_full.png
- verify/_atlas_glyphs.png
- verify/_badge_cap.png
- verify/_boot_fix_check.png
- verify/_boot_fixed_logpatch.png
- verify/_boot_fixed_mashedwin.png
- verify/_bootmodal_f150.bmp
- verify/_bootmodal_f150.png
- verify/_bootmodal_f60.bmp
- verify/_bootmodal_f60.png
- verify/_chk_boot.png
- verify/_chk_disc.png
- verify/_chk_orig_s1.png
- verify/_chk_orig_s30.png
- verify/_chk_orig_s4.png
- verify/_chk_orig_s8.png
- verify/_chk_s4.png
- verify/_clean_baseline_menu.png
- verify/_cmp_s4_hi.png
- verify/_cmp_s4_title.png
- verify/_cmp_s4_title2.png
- verify/_cmp_s4_title3.png
- verify/_cmp_s4_title4.png
- verify/_cur_s1.png
- verify/_cur_s15.png
- verify/_cur_s4.png
- verify/_disc_1555.png
- verify/_disc_4444.png
- verify/_disc_565.png
- verify/_disc_8bpp.png
- verify/_disc_compare.png
- verify/_disc_final.bmp
- verify/_disc_gray.png
- verify/_disc_modulated.bmp
- verify/_disc_now.png
- verify/_disc_warm.bmp
- verify/_f2_A_copters.bmp
- verify/_f2_B_nocopters.bmp
- verify/_f2_copter_zoom.png
- verify/_f2_copters_cropzoom.png
- verify/_f3_A_scroll.bmp
- verify/_f3_B_noscroll.bmp
- verify/_font_fair_s8.png
- verify/_font_gamma_cmp.png
- verify/_font_zoom_s8.png
- verify/_full_orig_s1.png
- verify/_full_std_s1.png
- verify/_glyph2.png
- verify/_glyph_norm_sound.png
- verify/_glyph_truepx.png
- verify/_loadmodal640.bmp
- verify/_loadmodal_cmp.png
- verify/_loadmodal_disc.bmp
- verify/_loadmodal_disc.png
- verify/_loadmodal_f10.bmp
- verify/_loadmodal_f10.png
- verify/_loadmodal_f40.bmp
- verify/_loadmodal_f40.png
- verify/_loadsuccess.bmp
- verify/_loadsuccess.png
- verify/_orig_backbuffer_f120.png
- verify/_orig_backbuffer_f180.png
- verify/_orig_backbuffer_f20.png
- verify/_orig_backbuffer_f60.png
- verify/_orig_check_f250.png
- verify/_orig_check_f450.png
- verify/_orig_check_f650.png
- verify/_orig_s15_2x.png
- verify/_orig_s4_2x.png
- verify/_orig_s4_tiles.png
- verify/_prompt2.png
- verify/_prompt3.png
- verify/_prompt4.png
- verify/_prompt_cmp.png
- verify/_re_footer.png
- verify/_re_s18.png
- verify/_re_s18b.png
- verify/_re_s4.png
- verify/_re_s6.png
- verify/_s1_flag.png
- verify/_s1_now.png
- verify/_s4_kbd.png
- verify/_s4_rowicon.png
- verify/_s4_rowicon2.png
- verify/_s4_v2.png
- verify/_s8_badge.png
- verify/_splash_disc.bmp
- verify/_splash_disc.png
- verify/_splash_disc2.bmp
- verify/_splash_disc2.png
- verify/_std_window_s8.png
- verify/_v640_cmp.png
- verify/_v640_s1.png
- verify/_v640_s4.png
- verify/_v640_s8.png
- verify/_walk_02_mainmenu.png
- verify/_walk_04_colourselect.png
- verify/_walk_05_abilityselect.png
- verify/_walk_06_challengeselect.png
- verify/anim_scr1_f200.bmp
- verify/anim_scr1_f40.bmp
- verify/anim_scr1_f6.bmp
- verify/anim_scr1_f90.bmp
- verify/b17_b19a_run01.png
- verify/b17_b19a_run02.png
- verify/b17_b19b_run01.png
- verify/b17_b19b_run02.png
- verify/b17_b19final_run01.png
- verify/b17_b19font_run01.png
- verify/b17_b19mm_run01.png
- verify/b17_baseline_run01.png
- verify/b17_baseline_run02.png
- verify/b17_harden2_run01.png
- verify/b17_harden2_run02.png
- verify/b17_harden_run02.png
- verify/b17_harden_run03.png
- verify/b17_rebase20_run01.png
- verify/b17_rebase20_run02.png
- verify/b17_rebase20_run03.png
- verify/b17_rebase_run01.png
- verify/b17_rebase_run02.png
- verify/badge_Button.raw
- verify/badge_Button_overmagenta.png
- verify/badge_SemiC.raw
- verify/badge_SemiC2.raw
- verify/badge_SemiC2_overmagenta.png
- verify/badge_SemiC_overmagenta.png
- verify/boot_asi_disabled_menu.png
- verify/boot_asi_disabled_menu2.png
- verify/boot_asi_disabled_menu3.png
- verify/c4_nav_probe_midnav.png
- verify/csel7_recheck.png
- verify/csel_goto2.bmp
- verify/csel_goto4.bmp
- verify/csel_goto4.png
- verify/csel_goto7.png
- verify/csel_goto8.bmp
- verify/csel_screen4.png
- verify/dinput8_era_scene_t10.png
- verify/disc_orig_zoom.png
- verify/flow_loading.png
- verify/flow_menu.png
- verify/flow_title.png
- verify/flow_triptych.png
- verify/fontA_orig.png
- verify/fontA_re.png
- verify/fontB_re.png
- verify/font_FGDC20.png
- verify/font_FGMC20.png
- verify/font_NEURO.png
- verify/font_P_orig.png
- verify/font_P_pair.png
- verify/font_P_re.png
- verify/font_anaglyph_exit2.png
- verify/font_cmp_exit.png
- verify/font_cmp_exit4x.png
- verify/font_cmp_exit_dev.png
- verify/font_cmp_exit_native.png
- verify/font_cmp_exit_native2.png
- verify/font_cmp_exit_ss2.png
- verify/font_cmp_exit_v2.png
- verify/font_cmp_exit_v4.png
- verify/font_cmp_footer.png
- verify/font_cmp_header.png
- verify/font_cmp_header2.png
- verify/font_cmp_watermark2.png
- verify/font_fair.png
- verify/font_full_pair.png
- verify/font_montage.png
- verify/font_pb_sidebyside.png
- verify/font_pb_sidebyside2.png
- verify/font_pb_sidebyside3.png
- verify/font_re_chk.png
- verify/font_re_title.png
- verify/font_re_title2.png
- verify/font_re_title3_small.png
- verify/font_re_title_640.png
- verify/font_zoom_orig_pb.png
- verify/font_zoom_re_pb.png
- verify/footer_zoom.png
- verify/footer_zoom2.png
- verify/fullscreen.png
- verify/inproc_feed_down3.png
- verify/inproc_feed_menu_moved.png
- verify/item_zoom.png
- verify/locked_screen.png
- verify/mainmenu_title.png
- verify/mainmenu_title_zoom.png
- verify/mashed_t08.png
- verify/mashed_t14.png
- verify/mashed_t20.png
- verify/menu_footer.png
- verify/msm_tree_1_root.bmp
- verify/msm_tree_2_root_item1.bmp
- verify/msm_tree_3_screen8.bmp
- verify/msm_tree_4_screen19.bmp
- verify/msm_tree_5_back_screen8.bmp
- verify/msm_tree_6_back_root.bmp
- verify/orig_backbuffer_f1000.bmp
- verify/orig_backbuffer_f1100.png
- verify/orig_backbuffer_f1400.png
- verify/orig_backbuffer_f300.png
- verify/orig_backbuffer_f500.png
- verify/orig_backbuffer_f600.bmp
- verify/orig_backbuffer_f800.png
- verify/orig_boot_grid.png
- verify/orig_chk_f1500.png
- verify/orig_chk_f1600.png
- verify/orig_chk_f1900.png
- verify/orig_chk_f2100.png
- verify/orig_chk_f2400.png
- verify/orig_chk_f2800.png
- verify/orig_clean_items_zoom.png
- verify/orig_clean_scr1.png
- verify/orig_load_f200.png
- verify/our_slide_f14.png
- verify/scene_t095.png
- verify/screen_25.bmp
- verify/slide_mid.png
- verify/slide_pair.png
- verify/slide_settled.png
- verify/unlock_00_menu.png
- verify/view_s15.png
- verify/view_s16.png
- verify/view_s18.png
- verify/view_s4.png
- verify/view_s6.png
- verify/view_s7.png
- verify/walk_01_title.bmp
- verify/walk_02_mainmenu.bmp
- verify/walk_03_singleplayer.bmp
- verify/walk_04_colourselect.bmp
- verify/walk_05_abilityselect.bmp
- verify/walk_06_challengeselect.bmp
- verify/walk_07_back_mainmenu.bmp
- verify/walk_08_options_highlighted.bmp
- verify/walk_09_options.bmp
- verify/walk_10_sound.bmp
- verify/walk_11_sound_adjusted.bmp
- verify/walk_12_back_mainmenu2.bmp
- verify/win_capture_bg.png
- verify/win_capture_bg_small.png
- verify/wm_postmsg_down.png
- verify/wm_zoom.png
- verify/zoom_footer_orig.png
- verify/zoom_footer_re.png
- verify/zoom_insults_orig.png
- verify/zoom_loading_f200.png
- verify/zoom_loading_f260.png
- verify/zoom_loading_f320.png
- verify/zoom_splash_orig.png
- verify/zoom_splash_orig2.png
- verify/zoom_splash_orig3.png
- verify/zoom_watermark_orig.png
- verify/zoom_watermark_re.png

### verify/_dat/ (5)

- verify/_dat/BADGES.TXD
- verify/_dat/ENGLISH.DAT
- verify/_dat/FX.TXD
- verify/_dat/INTERFACE.TXD
- verify/_dat/TEXTURES.TXD

### verify/_fontdump/ (1)

- verify/_fontdump/FGDC20.TXD

### verify/_pu/ (1)

- verify/_pu/POWERUPS_GOLD.LUA

### verify/audio/ (7)

- verify/audio/cdaudio_20s.wav
- verify/audio/red_car_10s.wav
- verify/audio/sfx_explosion1.wav
- verify/audio/sfx_go.wav
- verify/audio/sfx_menu_navigation.wav
- verify/audio/sfx_menu_selection.wav
- verify/audio/sfx_threetwoone.wav

### verify/c4nav_B_options/ (7)

- verify/c4nav_B_options/c4nav_ON_g0.png
- verify/c4nav_B_options/c4nav_ON_g1.png
- verify/c4nav_B_options/c4nav_ON_g2.png
- verify/c4nav_B_options/c4nav_ON_g3.png
- verify/c4nav_B_options/c4nav_ON_g4.png
- verify/c4nav_B_options/c4nav_ON_g5.png
- verify/c4nav_B_options/c4nav_ON_g6.png

### verify/frontend_parity2/ (26)

- verify/frontend_parity2/orig_scr8_cap.png
- verify/frontend_parity2/re_dbg_both_off.png
- verify/frontend_parity2/re_dbg_committed.png
- verify/frontend_parity2/re_dbg_committed_top.png
- verify/frontend_parity2/re_dbg_noarc.png
- verify/frontend_parity2/re_dbg_screencap.png
- verify/frontend_parity2/re_post_splash_title.png
- verify/frontend_parity2/re_scr1_wave1.png
- verify/frontend_parity2/re_scr8_now.png
- verify/frontend_parity2/re_title_postreboot.png
- verify/frontend_parity2/re_title_wave1.png
- verify/frontend_parity2/zoom/cap_O.png
- verify/frontend_parity2/zoom/cap_OR.png
- verify/frontend_parity2/zoom/cap_R.png
- verify/frontend_parity2/zoom/cap_R2.png
- verify/frontend_parity2/zoom/cap_band_compare.png
- verify/frontend_parity2/zoom/cap_compare.png
- verify/frontend_parity2/zoom/cap_orig.png
- verify/frontend_parity2/zoom/cap_orig_real.png
- verify/frontend_parity2/zoom/cap_re.png
- verify/frontend_parity2/zoom/font_O.png
- verify/frontend_parity2/zoom/font_R.png
- verify/frontend_parity2/zoom/orig_footer.png
- verify/frontend_parity2/zoom/our_cap_tight.png
- verify/frontend_parity2/zoom/our_footer.png
- verify/frontend_parity2/zoom/our_footer2.png

### verify/parity/ (58)

- verify/parity/_orig_s15_zoom.png
- verify/parity/_orig_s16_zoom.png
- verify/parity/_orig_s24_rows.png
- verify/parity/_orig_s4_rows.png
- verify/parity/_orig_s6_zoom.png
- verify/parity/cmp/s1.png
- verify/parity/cmp/s15.png
- verify/parity/cmp/s16.png
- verify/parity/cmp/s18.png
- verify/parity/cmp/s19.png
- verify/parity/cmp/s2.png
- verify/parity/cmp/s29.png
- verify/parity/cmp/s3.png
- verify/parity/cmp/s30.png
- verify/parity/cmp/s31.png
- verify/parity/cmp/s32.png
- verify/parity/cmp/s33.png
- verify/parity/cmp/s4.png
- verify/parity/cmp/s6.png
- verify/parity/cmp/s7.png
- verify/parity/cmp/s8.png
- verify/parity/orig_1_root.png
- verify/parity/orig_2_mainmenu.png
- verify/parity/orig_s1.bmp
- verify/parity/orig_s15.bmp
- verify/parity/orig_s16.bmp
- verify/parity/orig_s18.bmp
- verify/parity/orig_s19.bmp
- verify/parity/orig_s2.bmp
- verify/parity/orig_s29.bmp
- verify/parity/orig_s3.bmp
- verify/parity/orig_s30.bmp
- verify/parity/orig_s31.bmp
- verify/parity/orig_s32.bmp
- verify/parity/orig_s33.bmp
- verify/parity/orig_s4.bmp
- verify/parity/orig_s6.bmp
- verify/parity/orig_s7.bmp
- verify/parity/orig_s8.bmp
- verify/parity/re_s1.bmp
- verify/parity/re_s15.bmp
- verify/parity/re_s16.bmp
- verify/parity/re_s18.bmp
- verify/parity/re_s19.bmp
- verify/parity/re_s2.bmp
- verify/parity/re_s24.bmp
- verify/parity/re_s29.bmp
- verify/parity/re_s3.bmp
- verify/parity/re_s30.bmp
- verify/parity/re_s31.bmp
- verify/parity/re_s32.bmp
- verify/parity/re_s33.bmp
- verify/parity/re_s4.bmp
- verify/parity/re_s6.bmp
- verify/parity/re_s7.bmp
- verify/parity/re_s8.bmp
- verify/parity/sa_gts.bmp
- verify/parity/sa_sound_adjusted.bmp

### verify/parity3d/ (10)

- verify/parity3d/sa_liveries_t07.png
- verify/parity3d/sa_liveries_t08.png
- verify/parity3d/sa_realcam2_t07.png
- verify/parity3d/sa_realcam2_t09.png
- verify/parity3d/sa_realcam2_t11.png
- verify/parity3d/sa_realcam_t06.png
- verify/parity3d/sa_realcam_t10.png
- verify/parity3d/sa_realcam_t22.png
- verify/parity3d/sa_scoring_t12.png
- verify/parity3d/sa_scoring_t25.png

### verify/parity_caps/ (43)

- verify/parity_caps/orig_scr19_0.bmp
- verify/parity_caps/orig_scr19_1.bmp
- verify/parity_caps/orig_scr19_2.bmp
- verify/parity_caps/orig_scr1_0.bmp
- verify/parity_caps/orig_scr1_1.bmp
- verify/parity_caps/orig_scr1_2.bmp
- verify/parity_caps/orig_scr2_0.bmp
- verify/parity_caps/orig_scr2_1.bmp
- verify/parity_caps/orig_scr2_2.bmp
- verify/parity_caps/orig_scr30_0.bmp
- verify/parity_caps/orig_scr30_1.bmp
- verify/parity_caps/orig_scr30_2.bmp
- verify/parity_caps/orig_scr32_0.bmp
- verify/parity_caps/orig_scr32_1.bmp
- verify/parity_caps/orig_scr32_2.bmp
- verify/parity_caps/orig_scr3_0.bmp
- verify/parity_caps/orig_scr3_1.bmp
- verify/parity_caps/orig_scr3_2.bmp
- verify/parity_caps/orig_scr4_0.bmp
- verify/parity_caps/orig_scr4_1.bmp
- verify/parity_caps/orig_scr4_2.bmp
- verify/parity_caps/orig_scr8_0.bmp
- verify/parity_caps/orig_scr8_1.bmp
- verify/parity_caps/orig_scr8_2.bmp
- verify/parity_caps/re_modal.bmp
- verify/parity_caps/re_scr1.bmp
- verify/parity_caps/re_scr19.bmp
- verify/parity_caps/re_scr19_win.png
- verify/parity_caps/re_scr1_win.png
- verify/parity_caps/re_scr2.bmp
- verify/parity_caps/re_scr2_win.png
- verify/parity_caps/re_scr3.bmp
- verify/parity_caps/re_scr30.bmp
- verify/parity_caps/re_scr30_win.png
- verify/parity_caps/re_scr32.bmp
- verify/parity_caps/re_scr32_win.png
- verify/parity_caps/re_scr3_win.png
- verify/parity_caps/re_scr4.bmp
- verify/parity_caps/re_scr4_win.png
- verify/parity_caps/re_scr8.bmp
- verify/parity_caps/re_scr8_win.png
- verify/parity_caps/re_splash.bmp
- verify/parity_caps/re_splash2.bmp

### verify/race1/ (16)

- verify/race1/00_challengeselect.png
- verify/race1/00_challengeselect_prog.png
- verify/race1/00_results.bmp
- verify/race1/01_action.png
- verify/race1/01_action_lit.png
- verify/race1/01_grid.png
- verify/race1/01_grid_egypt.png
- verify/race1/01_grid_pu.png
- verify/race1/01_inrace_track.png
- verify/race1/01_inrace_track_egypt.png
- verify/race1/01_inrace_track_laps.png
- verify/race1/01_results.bmp
- verify/race1/01_results.png
- verify/race1/01_results_laps.png
- verify/race1/02_back_to_menu.png
- verify/race1/02_results.bmp

### verify/scenario/ (22)

- verify/scenario/sa_diffpoint.png
- verify/scenario/sa_gts.png
- verify/scenario/sa_race_enter.png
- verify/scenario/sa_round_0.png
- verify/scenario/sa_round_10.png
- verify/scenario/sa_round_101.png
- verify/scenario/sa_round_111.png
- verify/scenario/sa_round_122.png
- verify/scenario/sa_round_132.png
- verify/scenario/sa_round_142.png
- verify/scenario/sa_round_20.png
- verify/scenario/sa_round_30.png
- verify/scenario/sa_round_40.png
- verify/scenario/sa_round_50.png
- verify/scenario/sa_round_60.png
- verify/scenario/sa_round_61.png
- verify/scenario/sa_round_70.png
- verify/scenario/sa_round_71.png
- verify/scenario/sa_round_80.png
- verify/scenario/sa_round_81.png
- verify/scenario/sa_round_91.png
- verify/scenario/sa_track.png

### verify/scoring/ (14)

- verify/scoring/scoring_baseline_title.png
- verify/scoring/scoring_t10.png
- verify/scoring/scoring_t101.png
- verify/scoring/scoring_t111.png
- verify/scoring/scoring_t121.png
- verify/scoring/scoring_t20.png
- verify/scoring/scoring_t30.png
- verify/scoring/scoring_t40.png
- verify/scoring/scoring_t60.png
- verify/scoring/scoring_t70.png
- verify/scoring/scoring_t81.png
- verify/scoring/scoring_t91.png
- verify/scoring/scoring_title.png
- verify/scoring/scoring_track.png

### verify/ws_e_lighting/ (5)

- verify/ws_e_lighting/AFTER_chase.png
- verify/ws_e_lighting/AFTER_standings.png
- verify/ws_e_lighting/BEFORE_chase.png
- verify/ws_e_lighting/BEFORE_standings.png
- verify/ws_e_lighting/ORIGINAL_reference.png

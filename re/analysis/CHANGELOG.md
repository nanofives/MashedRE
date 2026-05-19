# Analysis Changelog

Append-only log of confidence promotions and demotions, written by the `re-classify` skill. One line per event.
2026-05-18  frida-sweep-20260518-0529  frida-sweep-release  branches=6 merged (c3/batch-j-s1=2 GREEN, c3/batch-j-s2=0 zero-yield, c3/batch-j-s3=8 GREEN, c3/batch-j-s4=0 zero-yield-but-5-staged-reimpls-merged, c3/batch-j-s5=5 GREEN, c3/batch-j-s6=0 zero-yield)  integration-diff=GREEN 15/15  hooks=15  conflicts={CHANGELOG.md=2, hooks_registry.py=1-overlap-region, PROMOTION_QUEUE.md=3, build.bat=1, stray-HEAD-marker-strip=1}; staged-reimpls Vehicle/Damage_j4.cpp+Physics_j4.cpp compiled in canonical .asi but RVAs (0x0046c5f0/0x0046c7d0/0x0046ddb0/0x0046f6c0/0x0046dc20 etc) stay C2 pending harness arg_type extensions; side-findings: U-2587 RE-CONFIRMED (controller stride 0x200 not 0x80; analysis note 004971b0.md still cites 0x80 — scribe-side TODO), 0x00498d20 returns SendMessageA(CB_GETITEMDATA) via EAX not literal 1, 0x005ab410 stale C2 shadow in hooks.csv (c3-batch-i-s1 already promoted to C3; drift-cleanup deferred to scribe-side); canonical .asi=184320 bytes
2026-05-18  frida-sweep-20260518-0529  frida-sweep-claim  branches=6 queued (c3/batch-j-s1, c3/batch-j-s2 [zero-yield], c3/batch-j-s3, c3/batch-j-s4 [zero-yield-but-staged-reimpls], c3/batch-j-s5, c3/batch-j-s6 [zero-yield])
2026-05-18  feature/harness-arg-types  harness-extension  4 new diff_template.js arg_types landed: eax_implicit_ptr, eax_implicit_int (RWX trampoline seeds EAX before JMP target — unblocks D-11010 family: 0x00497190, 0x004971b0, 0x00498d20, 0x0042aad0); vec3_global_mul_observe (3-float global mutate-and-observe — unblocks D-11011: 0x0046c570); fmt_desc_pair_compare (2/4-arg fmt-desc comparator with 64-byte scratch bufs — unblocks c3-batch-i-s2 audio: 0x005ac5f0, 0x005ac9e0, 0x005acaa0). build.bat refactored to use pushd %SRC% + relative paths to dodge cmd.exe's 8191-char command-line limit in worktree paths. 4 verification stubs in mashedmod/src/mashed_re/Harness/HarnessStubs.cpp (JMP-thunk forwarders to live RVAs); all 4 diff harness runs GREEN (log/diff_harness_test_*.csv). DOES NOT promote any RVA — promotion-blocker harness gaps cleared, follow-up c3 batch should pick up the 7 unblocked refusals. D-11012 (0x0046dc20 caller-frame stack read) confirmed structurally C3-impossible — pursue C4-direct via canonical-scenario observation.
2026-05-18  d11007-drift-cleanup  subsystem-reclass  group=render_midrva  rows=37 reclassed (render->{ai×3, camera×4, physics×10, track×12, util×3}), kept-audited=7; evidence=re/analysis/promote_c2_render_midrva/*.md subsystem_observed frontmatter
2026-05-18  d11007-drift-cleanup  subsystem-reclass  group=cluster_004d_rw_error  rows=2 canonical kept (render), 4 audio-tagged dup C1 rows removed (0x004d7ff0 ×2, 0x004d8480 ×2); evidence=FUN_004c2c90 C2 + Vec2Normalize C4
2026-05-18  d11007-drift-cleanup  dup-consolidation  group=vehicle_lowrva  rows=15 sibling C1 rows removed across 9 RVAs (0x00408a50, 0x00408a70, 0x0040e340, 0x0040e350, 0x0040e370, 0x00422fd0, 0x0042c280, 0x00432080, 0x004331a0); canonical C2 rows at re/analysis/promote_c2_vehicle_lowrva/ kept
2026-05-18  d11007-drift-cleanup  audit-noop  group=cluster_0049  7-row sample audit (0x00491490 0x00493390 0x004935a0 0x004942b0 0x004943f0 0x00494ee0 0x004954f0); scribe-assigned subsystem tags match plate evidence; no bulk reclass needed
2026-05-18  d11007-drift-cleanup  deferred-close  D-11007 RESOLVED; 56 row edits (37 reclass + 19 dup-removals); refused-C3=0
2026-05-18  0x004b6770  PizWin32Close  C1->C2  feature/pizwin32-close: hook added to mashedmod/src/mashed_re/Compat/PizWin32Bypass.cpp (PizWin32Close_Compat); fixes U-42 bug shape — original Win32 branch (0x004b6792..0x004b679f) closes DAT_007d3e48 but does not null it, while stdio branch (0x004b6779..0x004b6791) nulls at 0x004b6787. Defensive fix: bug is latent today because sole caller FUN_004b67a0 nulls explicitly at 0x004b67d1 (function_callers count=1 confirmed via Mashed_pool9 read-only session 2026-05-18). RH_ScopedInstall registered; arg_type=harness_limited in hooks_registry.py (CloseHandle on real piz HANDLE → can't synthetically A/B). Build clean (log/build_pizclose.txt). C3 promotion NOT taken — no canonical-scenario diff possible: the bug is unobservable in current call flows.
2026-05-18  frida-sweep-20260518-0304  frida-sweep-release  branches=4 merged (s1=3 GREEN, s2=0/5 zero-yield, s3=5 GREEN, s4=1 new C3 + 1 drift-fix)  integration-diff=GREEN  hooks=10/10  conflicts={build.bat=1, hooks_registry.py=3-region, CHANGELOG.md=1, PROMOTION_QUEUE.md=3, DEFERRED.md=1 (D-11007 collision renumber 11010-11012)}; U-2587 RESOLVED side-finding
2026-05-18  frida-sweep-20260518-0304  frida-sweep-claim  branches=4 queued (c3/batch-i-s1, c3/batch-i-s2 [zero-yield], c3/batch-i-s3, c3/batch-i-s4)
2026-05-18  sweep-20260518-0247  scribe-claim  buckets=6 queued, 0 skipped-HOLD (cluster_005c row reconciled from misplaced Drained back to Queued)
2026-05-18  sweep-20260518-0247  scribe-release  bucket=cluster_004b4_first_pass  writes=60  errors=0
2026-05-18  sweep-20260518-0247  scribe-release  bucket=cluster_005c_first_pass  writes=60  errors=0
2026-05-18  sweep-20260518-0247  scribe-release  bucket=cluster_0049_first_pass  writes=60  errors=0
2026-05-18  sweep-20260518-0247  scribe-release  bucket=cluster_004d_first_pass  writes=60  errors=0
2026-05-18  sweep-20260518-0247  scribe-release  bucket=promote_c2_vehicle_lowrva  writes=40  errors=0
2026-05-18  sweep-20260518-0247  scribe-release  bucket=promote_c2_render_midrva  writes=40  errors=0  (incl create-fn 0x00479030)
2026-05-18  sweep-20260518-0247  scribe-release  buckets=6 drained  errors=0  master-save=ok  pool-sync=partial(slots 0,4,6 locked/busy)
2026-05-17  frontend_s_dod_audit_20260517  s-dod-audit  Frontend S-DoD scorecard against ROADMAP.md Phase 5 thresholds: Row1 C3+=26.0% C4=8.7% FAIL (need 90% C3+); Row2 PARTIAL (subset evidence only via c4-sweep-B1..B4 + observe_main_menu_idle, no all-frontend-hook single-run); Row3 PASS substantially (6 struct docs in place: frontend_state, frontend_menu_state, hud_ingame_element, rwim2d_vertex_buffer, font_atlas, lobby_slot_array; 4 U-IDs pending at structs/README.md); Row4 FAIL (.piz round-trip writer absent — pack mode not implemented); Row5 FAIL (83->48 active frontend stubs, 35 drained this session). 7 new DEFERRED rows filed D-11000..D-11006. Audit doc: re/analysis/frontend_s_dod_audit_20260517.md. Honest assessment: 6-10 sessions to "substantially S-DONE"; strict 90% C4 unrealistic at current cadence — recommend user revisit S-DoD #1 threshold or relax to "90% C3+, 50% C4".
2026-05-17  c4-sweep-20260517  c4-canonical-scenario-batch  hooks=20 promoted C3->C4 across 4 batches (B1..B4); harness re/frida/observe_c4_sweep_20260517.py (auto-load parked, agent Module.load+InjectHooks, 10s main_menu_idle, no-Interceptor); reports log/observe_c4_sweep_20260517_{B1,B2,B3,B4}.txt; all 20 install-verified (E9 + correct rel32, bytes preserved at end of observation); process alive 10s in every batch; hooks.csv +21 C4 rows (incl. SetDat0067ecb8 dup); C4 total 29->50, C3 total 171->150
2026-05-17  0x0042b930  MenuAlphaGet  C3->C4  c4-sweep-B4 canonical-scenario: rel32 0x6a189e2b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B4.txt)
2026-05-17  0x0042fe30  RaceEndFlagIfEndMode  C3->C4  c4-sweep-B4 canonical-scenario: rel32 0x6a1869db, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B4.txt)
2026-05-17  0x0042fe50  RaceEndAltFlagIfEndMode  C3->C4  c4-sweep-B4 canonical-scenario: rel32 0x6a18699b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B4.txt)
2026-05-17  0x00403160  Sub00403160_SubMode0BViewport  C3->C4  c4-sweep-B3 canonical-scenario: rel32 0x6a1b304b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B3.txt)
2026-05-17  0x0041db80  Sub0041db80_HudThresholdDispatch  C3->C4  c4-sweep-B3 canonical-scenario: rel32 0x6a19871b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B3.txt)
2026-05-17  0x00427780  FontText_StringTableLookup  C3->C4  c4-sweep-B3 canonical-scenario: rel32 0x6a18e9db, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B3.txt)
2026-05-17  0x00427840  FontText_UTF16WidenCopy  C3->C4  c4-sweep-B3 canonical-scenario: rel32 0x6a18e93b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B3.txt)
2026-05-17  0x00442c80  ModeGatedPlayerCheck  C3->C4  c4-sweep-B2 canonical-scenario: alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B2.txt)
2026-05-17  0x00429aa0  GameStateSlotsFill  C3->C4  c4-sweep-B2 canonical-scenario: rel32 0x6a17df0b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B2.txt)
2026-05-17  0x004295a0  HudDualLabelRender  C3->C4  c4-sweep-B2 canonical-scenario: rel32 0x6a17e5db, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B2.txt)
2026-05-17  0x0043c000  TimerSlotTickDispatcher  C3->C4  c4-sweep-B2 canonical-scenario: rel32 0x6a16c24b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B2.txt)
2026-05-17  0x00475a60  PendingOpQueueFlush  C3->C4  c4-sweep-B2 canonical-scenario: rel32 0x6a13238b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B2.txt)
2026-05-17  0x004098b0  LoadingState1Enter  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a0bdffb, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt)
2026-05-17  0x00409900  LoadingState2Enter  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a0bdfdb, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt)
2026-05-17  0x00409930  LoadingState3Enter  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a0bdfdb, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt)
2026-05-17  0x00413f90  TimerGetBasePtr  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a0af3cb, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt)
2026-05-17  0x00426c10  TimerDispatch10  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a0a0d2b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt)
2026-05-17  0x00426c30  TimerDispatch30  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a0a0d3b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt)
2026-05-17  0x00426c70  TimerDispatch70  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a0a0d1b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt)
2026-05-17  0x0042c2f0  SetDat0067ecb8  C3->C4  c4-sweep-B1 canonical-scenario: rel32 0x6a09b63b, alive 10s main_menu_idle (log/observe_c4_sweep_20260517_B1.txt) (canonical + dup row both promoted)
2026-05-17  0x0042fe30  RaceEndFlagIfEndMode  C2->C3  drift-retry: caller-gate now satisfied (0040d270 + 004264d0 both C2 via promote_c2_race_end_callers-20260517 in master-bulk ed31a79); impl mashedmod/src/mashed_re/Frontend/MenuRaceEnd.cpp from c3-batch-h-s1; Frida A/B 10/10 GREEN log/diff_race_end_flag_if_end_mode.csv (May 15 evidence); D-10750 resolved; 0x0040d270 duplicate hooks.csv row reconciled (line 1496 removed, line 324 retained with subsystem util->track)
2026-05-17  ghidra-bulk-20260517  master-mutation  three mutations in one session: (1) qhull-FidDB band 0x005a0e00..0x005a57XX identified as statically-linked qhull 2002.1 (version literal "2002.1 2002/8/20"); 18/34 functions renamed to canonical qh_* names (qh_printcenter/printend/printincidences/printextremes/printextremes_2d/printfacet/printfacet3vertex/printfacet2math/printfacetNvertex_nonsimplicial/printfacetridges/printhelp_singularity/printhelp_degenerate/printneighborhood/printpoints_out/printridge/printvdiagram/printvertex + qh_eachvoronoi_all); 16 ambiguous left as FUN_ with qhull_candidate bookmarks; band-anchor bookmark at 0x005a0e00. (2) ShaderCompiler/ namespace created with 4 sub-namespaces: Loader (4 fns: 0x0051039a/04bb/04fc/0388), StringPool (5 fns: 0x00510de1/df9/e2f/f66/f72), Validation (4 fns: 0x00511112/879/8cd/918), AST (44 fns — Variant tag system + walkers + vector-deleting dtors); 3 excluded from cluster_0051 as misclassified (0x0051028b SIMD vector-normalize, 0x0051036d 12-byte zero ctor, 0x0051037a GDI DeleteObject); plate-comments on 4 anchor functions; classification bookmarks on 3 excluded. (3) RaceEnd callers C1->C2: 0x0040d270 (Course::Finish, calls 0x0042fe30 at 2 sites as medal-rank gate) and 0x004264d0 (track powerups loader, calls 0x0042fe30 as gate). C2 plates: re/analysis/promote_c2_race_end_callers/{0040d270,004264d0}.md. hooks.csv: 3 rows updated (0x0040d270 had duplicate rows on lines 324+1496 — both updated to C2 with reconciliation-deferred note). U-RACEEND-001..004 filed. Unblocks 0x0042fe30 C3 retry next frida-sweep.
2026-05-17  frida-sweep-20260517-2121  frida-sweep-release  branches=4 merged (c3/batch-h-s3..s6) + zero-yield-rows=2 (s1,s2) + drift-promotions=2 (0x0042b930+0x0042fe50)  integration-diff=GREEN 19/19  hooks=19 (17 c2->c3 from h-batch + 2 s1 drift wins)
2026-05-17  0x0042b930  MenuAlphaGet  C2(drift)->C3  c3-batch-h-s1 drift cleanup; canonical row was C2 in hooks.csv despite CHANGELOG row promoting it 2026-05-17 ma3-frida-s2; landed by frida-sweep-20260517-2121 with c3-batch-h-s1 evidence log/diff_menu_alpha_get.csv (10/10 GREEN); leaf-exemption; U-0501 open semantic-only
2026-05-17  0x0042fe50  RaceEndAltFlagIfEndMode  C2->C3  c3-batch-h-s1 drift cleanup; caller-gate now satisfied (FUN_004189c0 reached C2 in promote_c2_render_lowrva ghidra-sweep 2026-05-17, lifting c3-batch-g-s9 D-10751 refusal); landed by frida-sweep-20260517-2121 via c3-batch-h-s1 evidence log/diff_race_end_alt_flag_if_end_mode.csv (10/10 GREEN); impl mashedmod/src/mashed_re/Frontend/MenuRaceEnd.cpp from c3-batch-g-s9
2026-05-17  0x0042fe30  RaceEndFlagIfEndMode  C2->C3 NOT APPLIED  c3-batch-h-s1 listed as drift win but caller-gate still fails (callers 0040d270 + 004264d0 both C1 in track subsystem); frida-sweep-20260517-2121 refused per NO-OVERCLAIMING despite GREEN diff log/diff_race_end_flag_if_end_mode.csv 10/10 — D-10750 still applies; re-pickup when one caller reaches C2+
2026-05-17  c3-batch-h-s1  zero-yield-halt-row  rvas=  branch=c3/batch-h-s1  note=halt-row consumed; agent halted per 5+ refusal rule; drift-row analysis applied this sweep (0042b930 + 0042fe50 promoted; 0042fe30 retained at C2)
2026-05-17  c3-batch-h-s2  zero-yield-halt-row  rvas=  branch=c3/batch-h-s2  note=halt-row consumed; agent halted before authoring; no commit on branch
2026-05-17  frida-sweep-20260517-2121  frida-sweep-claim  branches=4 queued (c3/batch-h-s3..s6); zero-yield rows=2 (s1,s2)
2026-05-17  0x00442c80  ModeGatedPlayerCheck  C2->C3  c3-batch-h-s6 62b mode-gated player check (FUN_0040e350==6 AND DAT_007f0fd0 not in {4,8,9} AND DAT_008989b0[v*4]>_DAT_005cc9b8); reimpl mashedmod/src/mashed_re/Util/UtilMid_h6.cpp; callee FUN_0040e350 C1; Frida A/B 10/10 GREEN log/diff_mode_gated_player_check.csv
2026-05-17  0x00429aa0  GameStateSlotsFill  C2->C3  c3-batch-h-s6 134b predicate-gated slot fill DAT_0067d990/998/9a0 (path A uses FUN_004a2c48 records, path B uses 3 int arrays); reimpl mashedmod/src/mashed_re/Util/UtilMid_h6.cpp; callees FUN_00430820/00430790/004a2c48 all C1; drained dup frontend row (subsystem disambiguated to util); Frida A/B 10/10 GREEN log/diff_game_state_slots_fill.csv
2026-05-17  0x004295a0  HudDualLabelRender  C2->C3  c3-batch-h-s6 ~124b dual HUD label render (2x FUN_0040dc80 + 2x FUN_00427e00); reimpl mashedmod/src/mashed_re/Util/UtilMid_h6.cpp; callees FUN_0040dc80 + FUN_00427e00 C1; Frida A/B 5/5 GREEN log/diff_hud_dual_label_render.csv
2026-05-17  0x0043c000  TimerSlotTickDispatcher  C2->C3  c3-batch-h-s6 1450b 19-slot timer tick dispatcher (state 1/2 record FUN_004a2c48 into counter + fire action FUN_004332a0/0042c960/0042f7b0/0042fa00); reimpl mashedmod/src/mashed_re/Util/UtilMid_h6.cpp; menu-quiescent state==0 fast-path covers diff; callee FUN_0042f7b0 C3 satisfies structural gate; Frida A/B 10/10 GREEN log/diff_timer_slot_tick_dispatcher.csv
2026-05-17  0x00475a60  PendingOpQueueFlush  C2->C3  c3-batch-h-s6 74b pending-op queue drain DAT_0069160c (per-entry FUN_004b6520(ptr,size*0x50) + clear DAT_00691614[i]); reimpl mashedmod/src/mashed_re/Util/UtilMid_h6.cpp; callee FUN_004b6520 C2; Frida A/B 10/10 GREEN log/diff_pending_op_queue_flush.csv
2026-05-17  frida-sweep-20260517-0424  frida-sweep-release  branches=9 merged  integration-diff=GREEN  hooks=15  c3-promotions=7-new+drift-cleanups+3-harness-extensions+b5-atlas
2026-05-17  0x00436810  LocalPlayerSlotCheck  C2->C3  ma3-frida-s8 local-player slot-occupancy gate (param_1==12 fast-false; SP=non-zero count at DAT_007f0a7c; MP=type-2 count at DAT_007f0a74); reimpl mashedmod/src/mashed_re/Frontend/MenuHelpers.cpp; callee IsMultiplayerMode C3; U-3410 U-3411 preserved (literal offsets/value reproduced exactly per NO-GUESSING); int return (low-byte ABI for orig bool); Frida A/B 18/18 GREEN log/diff_local_player_slot_check.csv
2026-05-17  0x00430b60  MenuSlotCount  C3==C3  ma3-frida-s2 drained dup C2 frontend row (tracker drift from frontend_c0_promote); re-verified A/B GREEN 10/10 log/diff_menu_slot_count.csv (canonical row 777 unchanged)
2026-05-17  0x0042f6b0  MenuModeSync  C3==C3  ma3-frida-s2 drained dup C2 frontend row (tracker drift from frontend_c0_promote); re-verified A/B GREEN 10/10 log/diff_menu_mode_sync.csv (canonical row 770 unchanged)
2026-05-17  0x0042d300  TimeDiffDecompose  C2->C3  ma3-frida-s8 pure-leaf time-delta decomposer (signed centisecond diff -> sign/min/sec/csec out-ptrs); impl already on file mashedmod/src/mashed_re/Frontend/MenuTime.cpp; new harness arg_type time_diff_decompose (16B buf, 4 outs); Frida A/B 15/15 GREEN log/diff_time_diff_decompose.csv; leaf-exemption
2026-05-17  0x0042b930  MenuAlphaGet  C2->C3  ma3-frida-s2 5b leaf getter returns DAT_0067ecb0; reimpl mashedmod/src/mashed_re/Frontend/MenuGetters.cpp; Frida A/B GREEN 10/10 read_global log/diff_menu_alpha_get.csv; leaf-exemption; U-0501 open (semantic-only carried)
2026-05-17  0x00422b30  TimerArrayClear  C3==C3  ma3-frida-s2 drained dup C2 frontend row (tracker drift from frontend_c0_promote); re-verified A/B GREEN 10/10 log/diff_timer_array_clear.csv (canonical row 785 unchanged)
2026-05-17  0x0040e3a0  PlayerColorTableGet  C2->C3 REFUSED  ma3-frida-s5: caller-gate fails (FUN_0040e590 C1, FUN_00434720 C1; callee FUN_004a332b C2 passes; leaf-exemption N/A — has callee); Frida path1 GREEN 12/12 (log/diff_player_color_table_get.csv); new arg_type `int_outbuf4` added to diff_template.js; impl + RH_ScopedInstall ready in MenuScoreSort.cpp; D-10807 filed; pickup when caller reaches C2+
2026-05-17  0x0040b810  TimerGlobalsReset  C3==C3  ma3-frida-s2 drained dup C2 frontend row (tracker drift from frontend_c0_promote); re-verified A/B GREEN 10/10 log/diff_timer_globals_reset.csv (canonical row 767 unchanged)
2026-05-17  frida-sweep-20260517-0424  frida-sweep-claim  branches=9 queued (b5/atlas + c3/ma3-frida-s1..s8)
2026-05-17  ma3-frida-s5  c3-batch  candidates=5 promoted=0 refused=1 blocked=4  notes: 0040e3a0 caller-gate refused (D-10807); 004282a0 callee-gate blocked (FUN_004277a0 no plate); 00427ad0 callee-gate blocked (4 callees no plate); 00428320 caller-gate blocked precedent (refused c3-batch-g-s8 D-10699); 0042ed70 chain-blocked on 004282a0
2026-05-16  frida-sweep-20260516-2316  frida-sweep-release  branches=8 merged  integration-diff=GREEN  hooks=19  c3-promotions=19
2026-05-16  0x004b6480  BitArrayClear  C2->C3  ma2-frida-s7 88b leaf bit-array clear (REP STOSD/STOSB + AND-mask tail); reimpl mashedmod/src/mashed_re/Input/DirectInput.cpp; Frida A/B GREEN 14/14 bytes_inplace log/diff_bit_array_clear.csv; leaf-exemption
2026-05-16  0x00499720  GetInputHinst  C2->C3  ma2-frida-s7 5b leaf HINSTANCE getter; reimpl mashedmod/src/mashed_re/Input/DirectInput.cpp; Frida A/B GREEN 10/10 read_global log/diff_get_input_hinst.csv; leaf-exemption
2026-05-16  0x00495830  JoypadStrcpy  C2->C3  ma2-frida-s7 52b leaf bounds-checked strcpy; reimpl mashedmod/src/mashed_re/Input/DirectInput.cpp; Frida A/B GREEN 10/10 OOB log/diff_joypad_strcpy.csv; leaf-exemption; U-2589 carried (non-blocker)
2026-05-16  0x004955b0  CreateDInputObjectBool  C2->C3  ma2-frida-s7 11b bool wrapper for 0x00495530 (also promoted this session); reimpl mashedmod/src/mashed_re/Input/DirectInput.cpp; Frida A/B GREEN 10/10 (crash_equal_ok) log/diff_create_dinput_object_bool.csv; U-3871 RESOLVED
2026-05-16  0x00495530  CreateDInputObject  C2->C3  ma2-frida-s7 DI8Create wrapper; reimpl mashedmod/src/mashed_re/Input/DirectInput.cpp; callees=FUN_004987b0(C2)+GetModuleHandleA+DirectInput8Create; Frida A/B GREEN 10/10 (crash_equal_ok at quiescent menu) log/diff_create_dinput_object.csv
2026-05-16  0x00494ef0  ThunkVideoStateGet  C2->C3  ma2-frida-s4 thunk wraps pure read of DAT_00771a04; Frida A/B 10/10 GREEN at quiescent main menu (log/diff_thunk_video_state_get.csv); reimpl mashedmod/src/mashed_re/Boot/Teardown.cpp; refused 5/6: 0x00494bc0+0x00489250+0x00494f20 unsafe to invoke live (D-10774..D-10776), 0x004955c0+0x004963d0 targets not C1+plated (D-10777..D-10778)
2026-05-16  0x0045b350  RwInitNullStub  C2->C3  ma2-frida-s7 1b bare RET (0xC3); reimpl mashedmod/src/mashed_re/Input/DirectInput.cpp; Frida A/B GREEN 10/10 none log/diff_rw_init_null_stub.csv; leaf-exemption; 33 callers in binary
2026-05-16  frida-sweep-20260516-2316  frida-sweep-claim  branches=8 queued
2026-05-16  sweep-20260516-2149  scribe-release  buckets=8 drained  errors=0  total-plates=63  total-bookmarks=63  total-renames=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_settings_dialog  writes=8  errors=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_dinput_init  writes=8  errors=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_video_display  writes=7  errors=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_frame_dispatch_callees  writes=9  errors=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_boot_teardown  writes=6  errors=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_rw_d3d9_init  writes=7  errors=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_window_wndproc  writes=8  errors=0
2026-05-16  sweep-20260516-2149  scribe-release  bucket=promote_c2_winmain_chain  writes=8  errors=0
2026-05-16  sweep-20260516-2149  scribe-claim  buckets=8 queued, 0 skipped-HOLD
2026-05-16  0x004c5a60  FUN_004c5a60  C1->C2  ma1-ghidra-s3 refcounted RW release (120b; list 00618138 + vtable[007d3ff8+0x11c] dtor); S-3837 filed; U-3874 filed; bookmark added
2026-05-16  0x004b6480  FUN_004b6480  C1->C2  88b leaf bit-array clear (REP STOSD/STOSB + sub-byte mask); plate re/analysis/promote_c2_dinput_init/004b6480.md; session=ma1-ghidra-s7
2026-05-16  0x00499740  SetControlTextFromResource  C0->C2  promote_c2_settings_dialog/00499740.md; settings_config_d3 plate 2026-05-08; new hooks.csv row; LoadStringA+SetWindowTextA helper; no UNCERTAIN; session=ma1-ghidra-s8
2026-05-16  0x00499720  FUN_00499720  C1->C2  5b leaf HINSTANCE getter (DAT_007e9580); paired w/ FUN_00499710 HWND getter; plate re/analysis/promote_c2_dinput_init/00499720.md; session=ma1-ghidra-s7
2026-05-16  0x00499400  VideoSettingsDispatcher  C1->C2  promote_c2_settings_dialog/00499400.md; settings_config plate 2026-05-02; STUBs S-0821..S-0827 all resolved; U-0828/0829/0830 resolved; duplicate row at hooks.csv line 135 render is tracker drift not touched this session; session=ma1-ghidra-s8
2026-05-16  0x004991f0  VideoSettingsDlgProc  C0->C2  promote_c2_settings_dialog/004991f0.md; settings_config_d3 plate 2026-05-08; new hooks.csv row (was unmapped); resolves U-0828 (true function entry confirmed); U-3007 U-3008 open carried forward; session=ma1-ghidra-s8
2026-05-16  0x00499170  SubsystemSelectionChanged  C0->C2  promote_c2_settings_dialog/00499170.md; settings_config_d3 plate 2026-05-08; new hooks.csv row; RW_SetSubSystem + PopulateModeCombo + RW_GetCurrentMode; no UNCERTAIN; session=ma1-ghidra-s8
2026-05-16  0x00498f60  VideoDialogInit  C0->C2  promote_c2_settings_dialog/00498f60.md; settings_config_d3 plate 2026-05-08; new hooks.csv row; 7 control labels + 4 checkbox states + 2 combos populated; no UNCERTAIN; session=ma1-ghidra-s8
2026-05-16  0x00498d60  PopulateModeCombo  C0->C2  promote_c2_settings_dialog/00498d60.md; settings_config_d3 plate 2026-05-08; new hooks.csv row; mode filter flags!=0 + width>0x27f + height>0x1df; U-3009 open carried forward; session=ma1-ghidra-s8
2026-05-16  0x00498d20  ReadModeFromCombo  C0->C2  promote_c2_settings_dialog/00498d20.md; settings_config_d3 plate 2026-05-08; new hooks.csv row; CB_GETCURSEL+CB_GETITEMDATA on control 0x3e9 -> DAT_00773200; no UNCERTAIN; session=ma1-ghidra-s8
2026-05-16  0x00498c00  VideoModeTableInit  C1->C2  promote_c2_settings_dialog/00498c00.md; settings_config_d2 plate 2026-05-03; prior U-resolutions FUN_004c2e40 (render C1 GetCurrentSubSystem) FUN_004c2f00 (render C2 RW_GetCurrentMode); sole caller VideoSettingsDispatcher 0x00499400; session=ma1-ghidra-s8
2026-05-16  0x004987b0  FUN_004987b0  C1->C2  89b debug printf wrapper (vsprintf+OutputDebugStringA, /GS); plate re/analysis/promote_c2_dinput_init/004987b0.md; session=ma1-ghidra-s7
2026-05-16  0x00498510  FUN_00498510  C1->C2  ma1-ghidra-s3 controller-select dialog setup (664b; 6-region table + entry-table loop + DialogBoxParamA(0x67)); S-3835 filed; U-3870 filed; dup-row at hooks.csv:1923 noted but left alone; bookmark added
2026-05-16  0x004972b0  FUN_004972b0  C1->C2  88b per-frame joypad axis snapshot + keyboard read; plate re/analysis/promote_c2_dinput_init/004972b0.md; 3 callees (00495790/004957a0/00496100) have d2 notes but lack hooks.csv rows — filed as D-10770..D-10772 tracker-gap follow-ups; session=ma1-ghidra-s7
2026-05-16  0x004971b0  FUN_004971b0  C1->C2  114b controller config file loader; plate re/analysis/promote_c2_dinput_init/004971b0.md; U-2590 RESOLVED (DAT_005cf010="rb"); U-2587/2588 carried; session=ma1-ghidra-s7
2026-05-16  0x00497190  FUN_00497190  C1->C2  24b filename formatter "contcfg%d.bin" via FUN_004a2b60; plate re/analysis/promote_c2_dinput_init/00497190.md; session=ma1-ghidra-s7
2026-05-16  0x00496e40  FUN_00496e40  C1->C2 REFUSED  ma1-ghidra-s3 dual async-worker init: 7/7 internal callees unmapped > 5-threshold; deferred D-10770; bookmark added (REFUSED category)
2026-05-16  0x004960a0  FUN_004960a0  C1->C2  ma1-ghidra-s3 DInput device init (49b; deadzone=0x10, axis-thresh=0.35f); S-3833 S-3834 filed; U-3872 U-3873 filed; bookmark added
2026-05-16  0x00496010  FUN_00496010  C1->C2  ma1-ghidra-s3 DInput/joypad shutdown (45b; zero 0x1120b + 4 globals); S-3836 filed; bookmark added
2026-05-16  0x00495830  FUN_00495830  C1->C2  52b leaf strcpy from joypad_struct[slot] (stride 0x448); plate re/analysis/promote_c2_dinput_init/00495830.md; U-2589 carried; session=ma1-ghidra-s7
2026-05-16  0x004955b0  FUN_004955b0  C1->C2  ma1-ghidra-s3 thin bool wrapper of FUN_00495530 (11b); U-3871 filed; bookmark added
2026-05-16  0x004955b0  FUN_004955b0  C1->C2  11b bool predicate wrapping FUN_00495530 (this session's C2); plate re/analysis/promote_c2_dinput_init/004955b0.md; session=ma1-ghidra-s7
2026-05-16  0x00495530  FUN_00495530  C1->C2  DI8Create wrapper; plate re/analysis/promote_c2_dinput_init/00495530.md; U-0267+U-0268 RESOLVED (DAT_005d0a8c bytes 30 80 79 bf 3a 48 a2 4d aa 99 5d 64 ed 36 97 00 = IID_IDirectInput8A); session=ma1-ghidra-s7
2026-05-16  0x00493640  FUN_00493640  C1->C2  ma1-ghidra-s3 RW init step (18-call ladder); 171b; all 18 callees C1 in hooks.csv; no new STUBS; bookmark added
2026-05-16  0x0045b350  FUN_0045b350  C1->C2  ma1-ghidra-s3 bare RET (1 byte 0xC3) confirmed via listing_code_unit_at; 33 callers; leaf; U-0069 RESOLVED; bookmark added
2026-05-16  ma1-ghidra-s7  ghidra-s7-release  8/8 input C1->C2 promoted (DirectInput init foothold for input subsystem); resolves U-0267,U-0268,U-2590; 3 D-rows filed for tracker gaps; bookmarks deferred (pool6 acquired read-only); session=ma1-ghidra-s7
2026-05-16  ma1-ghidra-s6  promote_c2_video_display  0x00498bf0 FUN_00498bf0  C1->C2  5b getter DAT_00773204 (cursor/display gate); ShowCursor(0) caller-gate; analysis=re/analysis/promote_c2_video_display/00498bf0.md
2026-05-16  ma1-ghidra-s6  promote_c2_video_display  0x00498bd0 FUN_00498bd0  C1->C2  5b getter DAT_0061602c (render height); sibling of 0x00498bc0; analysis=re/analysis/promote_c2_video_display/00498bd0.md
2026-05-16  ma1-ghidra-s6  promote_c2_video_display  0x00467110 FUN_00467110  new->C2  first-analysis; alt-camera-triple init DAT_006905b0/b4/b8; sibling of FUN_0042d560+FUN_0042f660; S-3833 S-3834 filed
2026-05-16  ma1-ghidra-s6  promote_c2_video_display  0x0042f660 DefaultViewportCameraInit  C1->C2  drift-promotion; existing split_screen plate already C2-grade; S-3560 cited (FUN_004c1a40)
2026-05-16  ma1-ghidra-s6  promote_c2_video_display  0x0042d560 FUN_0042d560  C1->C2  drift-promotion; existing frontend_unmapped_a plate already C2-grade; S-3560 cited
2026-05-16  ma1-ghidra-s6  promote_c2_video_display  0x0042b8a0 FUN_0042b8a0  new->C2  first-analysis; 11b setter DAT_0067ea56 (2-byte render-height copy)
2026-05-16  ma1-ghidra-s6  promote_c2_video_display  0x0042b890 FUN_0042b890  new->C2  first-analysis; 11b setter DAT_0067ea54 (2-byte render-width copy)
2026-05-16  ma1-ghidra-s2  C1->C2 batch  bucket=promote_c2_window_wndproc  rvas=0x00499ba0,0x004996f0,0x00499cc0,0x00499820,0x00496490,0x00496470,0x004960e0,0x004963b0  count=8  resolved=U-0647,S-0003,S-0640,S-0641,S-0642  new=U-3870,S-3920,S-3921  carried=U-0648,S-0643  pool=Mashed_pool1
2026-05-16  ma1-ghidra-s1  promote_c2_winmain_chain  c1->c2  rvas=00492370,00492270,00492290,004924f0,004921d0,00428590,00492e90,00493600  pool=Mashed_pool0(contended,used-pool6-readonly)  stubs-filed=S-3902..S-3908(7)  uncertainties-open=U-0009 U-0642  notes=8/8 promoted; bookmark step skipped due to pool0 channel-lock contention with parallel MCP session
2026-05-16  frida-sweep-20260516-1701  frida-sweep-release  branches=16 merged  integration-diff=GREEN-for-promotions(12/12)-RED-for-1-refused-evidence-hook(menu_dim_set,harness-config-followup)  hooks=15  c3-promotions=12  d-row-renumberings=21
2026-05-16  0x0041ccc0  HudSlotLoopCcc0  C2->C3  evidence=log/diff_hud_slot_loop_ccc0.csv (10/10 GREEN); impl=HUD/HudDispatch.cpp; inline asm replicates MOV EAX,ESI before CALL 0x0041c9a0 (__thiscall via EAX); session=c3-batch-g-s11
2026-05-16  0x0041c9a0  FUN_0041c9a0  C1->C2  drift-promotion; analysis note hud_ingame_d2/0x0041c9a0.md complete all offsets cited no UNCERTAIN; unblocks 0x0041ccc0 caller-gate; session=c3-batch-g-s11
2026-05-16  0x0041b630  HudSlotLoopB630  C2->C3  evidence=log/diff_hud_slot_loop_b630.csv (10/10 GREEN); impl=HUD/HudDispatch.cpp; inline asm replicates MOV EAX,ESI before CALL 0x0041b340 (__thiscall via EAX); session=c3-batch-g-s11
2026-05-16  0x0041b340  FUN_0041b340  C1->C2  drift-promotion; analysis note hud_ingame_d2/0x0041b340.md complete all offsets cited no UNCERTAIN; unblocks 0x0041b630 caller-gate; session=c3-batch-g-s11
2026-05-16  0x0040dfc0  HudIngameDispatch  C2->C3  evidence=log/diff_hud_ingame_dispatch.csv (10/10 GREEN); impl=HUD/HudDispatch.cpp; session=c3-batch-g-s11
2026-05-16  0x00403160  sub_00403160  C3-REFUSED  caller-gate: 5 callees C1 (004c19f0 004c1a00 004c1c80 00402fb0 00428760); per prompt rule (>=2 C1 callees → refuse); deferred D-10699; session=c3-batch-g-s11
2026-05-16  00439210  FUN_00439210  C2->C3 REFUSED  c3-batch-g-s4: 11+ depth-1 callees uncatalogued — callee gate not met; 5626b body unsafe to author; D-10700
2026-05-16  0042fe90  FUN_0042fe90  C2->C3 REFUSED  c3-batch-g-s4: callee gate failed — 6/10 callees C1 (same set as 0042fb70); D-10698
2026-05-16  0042fb70  FUN_0042fb70  C2->C3 REFUSED  c3-batch-g-s4: callee gate failed — 6/10 callees C1 (0x004739f0,0x00473870,0x00427e00,0x004a2c48,0x0042b8b0,0x0042b8c0); D-10697
2026-05-16  0042a940  FUN_0042a940  C2->C3 REFUSED  c3-batch-g-s4: (1) U-3434+U-3435 open structural uncertainties; (2) sole callee 0x0040ce80 C0 — callee gate not met; D-10699
2026-05-16  0041e850  sub_0041e850  C2->DEFERRED(D-10701)  c3-batch-g-s12; caller-gate: callee FUN_0041e630 (0x0041e630) is C1; pickup when callee reaches C2+
2026-05-16  0041ded0  sub_0041ded0  C2->DEFERRED(D-10700)  c3-batch-g-s12; caller-gate: callee FUN_0041de80 (0x0041de80) is C1; pickup when callee reaches C2+
2026-05-16  0041db80  sub_0041db80  C2->DEFERRED(D-10699)  c3-batch-g-s12; U-0579 (structural body-bounds) + U-3585 (semantic DAT_0063d588 type) both open; pickup when both resolved
2026-05-16  0041d870  sub_0041d870  C2->DEFERRED(D-10698)  c3-batch-g-s12; caller-gate: callee FUN_0041d410 (0x0041d410) is C1; pickup when callee reaches C2+
2026-05-16  frida-sweep-20260516-1701  frida-sweep-claim  branches=16 queued
2026-05-16  sweep-20260516-0352  scribe-release  buckets=13 drained  errors=0
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_d3d9_window  writes=1  errors=0  note=partial; 6 RVAs deferred
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_panel_piz_callees  writes=7  errors=0
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_piz_loader  writes=6  errors=0
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_txd_loader  writes=6  errors=0  note=__stricmp already FidDB-named, rename skipped
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_rws_audio_loader  writes=7  errors=0
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_rw_render_submit  writes=5  errors=0  note=2 drift-already-c2
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_launch_handshake  writes=6  errors=0  note=1 drift-skip
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_rw_state  writes=5  errors=0  note=2 drift-skip
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_input_lua  writes=5  errors=0  note=2 drift-skip
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_video_cfg  writes=7  errors=0
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_perm_piz_callees  writes=7  errors=0
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_save_load_io  writes=0  errors=0  note=all 7 drift-skip
2026-05-16  sweep-20260516-0352  scribe-release  bucket=promote_c2_rw_engine_init  writes=5  errors=0
2026-05-16  sweep-20260516-0352  scribe-claim  buckets=13 queued, 0 skipped-HOLD
2026-05-16  sweep-20260516-0237  scribe-release  buckets=1 drained  errors=0
2026-05-16  sweep-20260516-0237  scribe-release  bucket=promote_c2_winmain_init  writes=6  errors=0
2026-05-16  sweep-20260516-0237  scribe-claim  buckets=1 queued, 0 skipped-HOLD
2026-05-16  frida-sweep-20260516-0008  frida-sweep-release  branches=24-merged-of-31  integration-diff=44GREEN/15RED/5MISS  hooks=64-evaluated  note=RED breakdown: 8 missing-arg_type-handler in HEAD diff_template.js (audio_sub_struct_link/dual/zero, fmt_key_compare, wavefmt_copy, buf_field_set, dsound_secondary_init state, list_drain2 state), 4 process-terminating CRT (seh_prolog/epilog/stack_probe/pre_init_loop), 2 disabled-registration first-wins (slot_data_copy, timer_init_thunk); MISS=5 hooks with no CSV (CRT process-exit before harness writes / RH_ScopedInstall commented-out first-wins). 9 cross-session RVA collisions resolved first-wins (0x005ade90 0x00422120 0x00420d40 0x0041f000 0x0041eda0 0x0041e130 0x0041d820 + intra-TimerInit dupes 0x00422b10 0x00425b10 0x004222c0). 11 new arg_types from sessions NOT ported to diff_template.js (re-pickup: audio_sub_struct_link/dual/zero, endian_pack, wavefmt_copy, buf_field_set, semaphore_create, music_vol_set, int_copy24_out, fmt_key_compare, write_global_setter, int_out24).
2026-05-16  frida-sweep-20260516-0008  frida-sweep-claim  branches=30 queued
2026-05-15  005ae920  AudioPoolFree  C2->C3  c3-batch-f-s6; mashedmod/src/mashed_re/Audio/AudioRws.cpp; Frida GREEN 10/10 crash_equal_ok (pool uninitialized; both paths crash identically); callee 004522d0 C1 flagged (vtable trampoline; callee-gate exception noted); U-0124 partially resolved
2026-05-15  005addd0  AudioListInsertHead  C2->C3  c3-batch-f-s6; mashedmod/src/mashed_re/Audio/AudioRws.cpp; Frida GREEN 10/10 crash_equal_ok (pool alloc crashes; both paths crash identically); callee AudioPoolFree now C3; callee FUN_005ae800 C2
2026-05-15  005ade10  AudioListRemoveByValue  C2->C3  c3-batch-f-s6; mashedmod/src/mashed_re/Audio/AudioRws.cpp; Frida GREEN 10/10 not-found path (payload=0 excluded; degenerate sentinel-match crash); loop order fixed (data-read before sentinel-check per 0x005ade1d); U-0124 partially resolved
2026-05-15  005ade90  AudioListDrain  C2->C3  c3-batch-f-s6; mashedmod/src/mashed_re/Audio/AudioRws.cpp; Frida GREEN 10/10 empty-drain path (nodeCount=0; no pool access); STRUCT GAP U-0990 U-0138 open (node type at DAT_009146c0 unknown; does not block drain correctness); dedup row 1634 retained
2026-05-15  005baf00  MusicGroupVolumeSet  C1->C2  c3-batch-f-s13; re/analysis/audio_music_d2/005baf00.md; impl=mashedmod/src/mashed_re/Audio/AudioMusic.cpp; Frida diff GREEN 10/10 log/diff_music_group_volume_set.csv; C3 refused: caller 0x0045dd60 is C1 (caller gate not met); leaf-callee-exemption applies
2026-05-15  004aaff0  _memcpy  C1->C2  batch-e-s9 drift-promote; re/analysis/boot_crt_env/004aaff0.md; FidDB VS2003 single match; no callees; all consts cited; leaf function; callee gate for CrtGetEnvStrings(0x004abf28)
2026-05-15  004abc53  __setenvp  C2->C3  batch-e-s9; mashedmod/src/mashed_re/Boot/CrtEnvArgv.cpp; RH_ScopedInstall(CrtSetEnvp,0x004abc53); Frida diff 10/10 GREEN log/diff_crt_set_envp.csv; callee _strlen(0x004a9410) C2; caller entry(0x004a4bb7) C2
2026-05-15  004abf28  ___crtGetEnvironmentStringsA  C2->C3  batch-e-s9; mashedmod/src/mashed_re/Boot/CrtEnvArgv.cpp; RH_ScopedInstall(CrtGetEnvStrings,0x004abf28); Frida diff 10/10 GREEN log/diff_crt_get_env_strings.csv; callee _memcpy(0x004aaff0) C2 (drift); caller entry(0x004a4bb7) C2
2026-05-15  005aca80  AudioFmtSizeCalc  C2->C3  c3-batch-f-s2; leaf; reimpl AudioRws.cpp; RH_ScopedInstall; Frida GREEN log/diff_audio_fmt_size_calc.csv
2026-05-15  005acd10  AudioFmtTableSearch  C2->C3  c3-batch-f-s2; callee FUN_005ac9e0 C2; reimpl AudioRws.cpp; RH_ScopedInstall; Frida GREEN (count=0 branch) log/diff_audio_fmt_table_search.csv; STRUCT GAP ctx+0x24/+0x28
2026-05-15  005acd60  AudioFmtGlobalScan  C2->C3  c3-batch-f-s2; callee FUN_005adf30 C2; reimpl AudioRws.cpp; RH_ScopedInstall; Frida GREEN (crash_equal_ok) log/diff_audio_fmt_global_scan.csv; U-1734 open
2026-05-15  005ac980  AudioFmtDescCopy  (already C3)  no-op: row was C3 prior to this session
2026-05-15  004522d0  FUN_004522d0  C1->C2  c3-batch-f-s3; drift-promote; 11-byte vtable dispatch DAT_007d3ff8+0x10c; body read end-to-end; U-0110 open (calling-convention invisible via jumptable); callee gate enabler for AudioSubStructBFree
2026-05-15  005ae080  AudioSubStructAFree  C2->C3  c3-batch-f-s3; mashedmod/src/mashed_re/Audio/AudioRws.cpp; RH_ScopedInstall; Frida none 10/10 GREEN log/diff_audio_sub_struct_a_free.csv; caller FUN_005ae030(C2+) callee FUN_005ae920(C2+)
2026-05-15  005ae050  AudioSubStructBFree  C2->C3  c3-batch-f-s3; mashedmod/src/mashed_re/Audio/AudioRws.cpp; RH_ScopedInstall; Frida none 10/10 GREEN log/diff_audio_sub_struct_b_free.csv; caller FUN_005ae030(C2+) callee FUN_004522d0(C2 drift-promoted this session)
2026-05-15  005ae030  AudioSubStructCleanup  C2->C3  c3-batch-f-s3; mashedmod/src/mashed_re/Audio/AudioRws.cpp; RH_ScopedInstall; Frida none 10/10 GREEN log/diff_audio_sub_struct_cleanup.csv; caller FUN_005abcf0(C2+) callees AudioSubStructAFree+AudioSubStructBFree(C3)
2026-05-15  005adf30  AudioFmtKeyCompare  C2->C3  c3-batch-f-s3; mashedmod/src/mashed_re/Audio/AudioRws.cpp; RH_ScopedInstall; fmt_key_compare Frida 15/15 GREEN log/diff_audio_fmt_key_compare.csv; pure leaf (leaf-exemption for callee gate); caller FUN_005ac5f0(C2+)
2026-05-15  0x005ae010  AudioSubStructLinkDevice   C2->C3  log/diff_audio_sub_struct_link_device.csv 10/10 GREEN; c3-batch-f-s4; U-0142 open (structural; sub-struct layout partial)
2026-05-15  0x005adfe0  AudioSubStructLinkBuffer   C2->C3  log/diff_audio_sub_struct_link_buffer.csv 10/10 GREEN; c3-batch-f-s4; U-0143 open (structural; sub-struct parent relationship)
2026-05-15  0x005ae0b0  AudioSubStructZeroInit     C2->C3  log/diff_audio_sub_struct_zero_init.csv 10/10 GREEN; c3-batch-f-s4; leaf-exemption applied
2026-05-15  0x005ac7b0  AudioSubStructDualInit     C2->C3  log/diff_audio_sub_struct_dual_init.csv 10/10 GREEN; c3-batch-f-s4; callees C3 (this session)
2026-05-15  005abcb0  AudioWaveNodeFree          C2->C3  c3-batch-f-s7; mashedmod/src/mashed_re/Audio/AudioRws.cpp; crash_equal_ok GREEN 10/10 log/diff_audio_wave_node_free.csv; caller=005abcf0(C2); callee=005ae920(C2); U-0994 open (Uncertainties section)
2026-05-15  005ac740  AudioSubStructBufCleanup   C2->C3  c3-batch-f-s7; mashedmod/src/mashed_re/Audio/AudioRws.cpp; crash_equal_ok GREEN 10/10 log/diff_audio_sub_struct_buf_cleanup.csv; caller=005abcf0(C2); callee=004522d0(C1 prec-AudioAlignedFree)
2026-05-15  005ac900  AudioContextLookup         C2->C3  c3-batch-f-s7; mashedmod/src/mashed_re/Audio/AudioRws.cpp; crash_equal_ok GREEN 10/10 log/diff_audio_context_lookup.csv; caller=005ac210(C2); callee=005aa0c0(C2 drift-promote); U-1730 open (Uncertainties section)
2026-05-15  005ae650  AudioPoolConstruct         C2->C3  c3-batch-f-s7 STRUCT-GAP; mashedmod/src/mashed_re/Audio/AudioRws.cpp; crash_equal_ok GREEN 10/10 log/diff_audio_pool_construct.csv; caller=005aba20(C2); callee=005aea00(C2); U-1736 open (Uncertainties section)
2026-05-15  005aa0c0  FUN_005aa0c0  C1->C2  drift-promote c3-batch-f-s7; mechanical decomp complete audio_rws_loader_d3/005aa0c0.md; callee of AudioContextLookup 0x005ac900
2026-05-15  005bbfc0  AudioDSoundSecondaryInit  C2->C3  c3-batch-f-s9; Frida diff GREEN 10/10; orig=reimpl=196608 (3 vtable calls + return 0); U-0362 dead branch Frida-confirmed; mashedmod/src/mashed_re/Audio/AudioDSound.cpp
2026-05-15  004963e0  ConfigLogError    C2->C3  c3-batch-e-s4; Frida void-match 10/10; leaf; cross-CRT game-fputs via naked push/ret thunk; diff_config_log_error.csv GREEN
2026-05-15  00496400  ConfigLogDebug    C2->C3  c3-batch-e-s4; Frida void-match 10/10; U-0827 open (callee CRT-variant) but reimpl delegates to game RVA—correctness unaffected; diff_config_log_debug.csv GREEN
2026-05-15  00498910  ConfigFilenameGet C2->C3  c3-batch-e-s4; Frida bit-identical 10/10 (return=0x7731e8); pure leaf; diff_config_filename_get.csv GREEN
2026-05-15  00498950  ConfigLoad        C2->C3  c3-batch-e-s4; Frida bit-identical 10/10 (return=1); all callees C3+; diff_config_load.csv GREEN
2026-05-15  00422b10  TimerArrayZero  C2->C3  c3-batch-e-s12; mashedmod/src/mashed_re/Util/TimerInit.cpp; leaf-exemption; diff GREEN 10/10 log/diff_timer_array_zero.csv; U-3715 filed
2026-05-15  00425b10  PlayerSlotZero  C2->C3  c3-batch-e-s12; mashedmod/src/mashed_re/Util/TimerInit.cpp; leaf-exemption; diff GREEN 10/10 log/diff_player_slot_zero.csv; U-3716 U-3717 filed
2026-05-15  004222c0  TimerInitThunk  C2->C3  c3-batch-e-s12; mashedmod/src/mashed_re/Util/TimerInit.cpp; thunk of 0x00422120 (C2); diff GREEN 10/10 log/diff_timer_init_thunk.csv
2026-05-15  0041cbc0  FloatTableInit  C2->C3  c3-batch-e-s12; mashedmod/src/mashed_re/Util/TimerInit.cpp; leaf-exemption; diff GREEN 10/10 log/diff_float_table_init.csv; U-3718 U-3719 filed
2026-05-15  frida-sweep-20260515-1412  frida-sweep-release  branches=16  integration-diff=N/A(c4-sweep-no-new-code)  hooks=26  note=26/62 hooks C3->C4; 36 refused/deferred (D-10601..10637 renumbered); harness gaps: double-patch-crash, read_global/none/font_* callFn, key-send missing, audio COM neutralized
2026-05-15  frida-sweep-20260515-1412  frida-sweep-claim  branches=16 queued
2026-05-15  frida-sweep-20260515-0105  frida-sweep-release  branches=18  integration-diff=GREEN(30/36,6-harness-limited:void/custom-argtype)  hooks=36(diffed)  note=MenuCursorStep(0x0042aa00) RH_ScopedInstall disabled: validity-addr formula mismatch vs original; Ghidra re-check needed
2026-05-15  0042aa00  MenuCursorStep  C2(no-change)  integration-diff-RED: validity-addr formula 0x0067ed84+cursor+slot*0x10-0x10 doesn't match original; per-session GREEN was false positive (test vectors covered wrong memory region); RH_ScopedInstall disabled; Ghidra re-investigation needed before C3 retry
2026-05-15  frida-sweep-20260515-0105  frida-sweep-claim  branches=18 queued
2026-05-14  004c5c00  FUN_004c5c00  C1->C2  sprite_gate_c3-20260514; re/analysis/sprite_gate_c3/0x004c5c00.md; drift-promote: case-insensitive linked-list string search (114b leaf); mechanical description complete from title_screen_d2 depth-3 trace; S-2540 cleared
2026-05-14  0040bb50  FUN_0040bb50  C1->C2  sprite_gate_c3-20260514; re/analysis/hud_frontend/0x0040bb50.md; drift-promote: FUN_004c5c00(DAT_0063b8fc param_1) forwarder (20b); U-0450 cleared; caller-gate for SpriteSlotGate now open
2026-05-14  0040bb70  SpriteLookupTableA  C2->C3  sprite_gate_c3-20260514; mashedmod/src/mashed_re/Frontend/SpriteGate.cpp; 20b forwarder FUN_004c5c00(DAT_0063b900 key); diff GREEN 10/10 crash-equal (table unpopulated at RW-init; both crash identically at node+8); crash_equal_ok harness extension
2026-05-14  0040bb90  SpriteLookupTableB  C2->C3  sprite_gate_c3-20260514; mashedmod/src/mashed_re/Frontend/SpriteGate.cpp; 20b forwarder FUN_004c5c00(DAT_0063b904 key); diff GREEN 10/10 crash-equal
2026-05-14  0042ee00  SpriteSlotGate  C2->C3  sprite_gate_c3-20260514; mashedmod/src/mashed_re/Frontend/SpriteGate.cpp; FUN_0040bb50 dispatcher slot{0/1/2}->call else 0; diff GREEN 10/10 (out-of-range slots tested; in-range pass through to original)
2026-05-14  00430a10  HudSlotTypePlayer0  C2->C3  sprite_gate_c3-20260514; mashedmod/src/mashed_re/Frontend/SpriteGate.cpp; game-mode->slot-type-0 mapper; discovered default case returns (mode-2) not 0; diff GREEN 10/10
2026-05-14  00430a60  HudSlotTypePlayer1  C2->C3  sprite_gate_c3-20260514; mashedmod/src/mashed_re/Frontend/SpriteGate.cpp; game-mode->slot-type-1 mapper; default case (mode-2); diff GREEN 10/10
2026-05-14  00430ab0  HudSlotTypePlayer2  C2->C3  sprite_gate_c3-20260514; mashedmod/src/mashed_re/Frontend/SpriteGate.cpp; game-mode->slot-type-2 mapper; default case (mode-2); diff GREEN 10/10
2026-05-12  sweep-20260512-2139  scribe-claim  buckets=1 queued, 1 skipped-HOLD (replay_record-20260503)
2026-05-12  004929d0  FUN_004929d0  C1->C2  game_state_sentinel_diff-20260512; re/analysis/game_state/0x004929d0.md; drift-correction (analysis already C2-quality with full mechanical case-by-case dispatch + constants + U-0487/U-0488); promoted to satisfy caller-at-C2+ gate for leaf C3 promotions below
2026-05-12  0042c2d0  GetDat0067ecb4  C2->C3  game_state_sentinel_diff-20260512; mashedmod/src/mashed_re/GameState/StateAccessors.cpp; trivial getter MOV EAX,[0x67ecb4]; 10/10 bit-identical sentinel-write Frida force-call A/B via run_diff_parallel.py (log/diff_get_dat_0067ecb4.csv); pure-leaf exemption; resolves S-0481; dups removed (race_state vehicle C1)
2026-05-12  0042c2e0  GetDat0067ecb8  C2->C3  game_state_sentinel_diff-20260512; mashedmod/src/mashed_re/GameState/StateAccessors.cpp; trivial getter MOV EAX,[0x67ecb8]; 10/10 bit-identical sentinel-write; pure-leaf exemption; paired with setter 0x0042c2f0 (still C2); resolves S-0482; dups removed
2026-05-12  0042f500  GetDat0067ea64  C2->C3  game_state_sentinel_diff-20260512; mashedmod/src/mashed_re/GameState/StateAccessors.cpp; trivial getter MOV EAX,[0x67ea64]; 10/10 bit-identical sentinel-write; pure-leaf exemption; resolves S-0484; dup removed (hud C2 — hud_ingame_promote_c2 plate preserved); also called from HUD chain
2026-05-12  005ae010  FUN_005ae010  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_cont1/0x005ae010.md; sub-struct A device-handle linker; U-0142 open; dup d2 row removed
2026-05-12  005ae030  FUN_005ae030  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_cont1/0x005ae030.md; combined sub-struct cleanup; no new U; dup d2 row removed
2026-05-12  005ae050  FUN_005ae050  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005ae050.md; sub-struct B heap-free; no U filed
2026-05-12  005ae080  FUN_005ae080  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005ae080.md; sub-struct A pool-return (DAT_007dda28); no U filed
2026-05-12  005ae0b0  FUN_005ae0b0  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005ae0b0.md; 3-field zero-init leaf; no U filed
2026-05-12  005ae0c0  FUN_005ae0c0  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_cont1/0x005ae0c0.md; WAVEFORMATEX-like 16-byte copy/byte-swap; U-0140 U-0141 open; U-0992 resolved; dup d2 row removed
2026-05-12  005ae650  FUN_005ae650  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005ae650.md; 6-param bitmap pool constructor; U-1736 open
2026-05-12  005ae780  FUN_005ae780  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005ae780.md; bitmap pool destructor; no U filed
2026-05-12  005ae800  FUN_005ae800  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005ae800.md; bitmap pool allocator; no U filed
2026-05-12  005ae920  FUN_005ae920  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d2/005ae920.md; fixed-size pool free with bitmap tracking; no U filed
2026-05-12  005aea00  FUN_005aea00  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader/005aea00.md; JMP trampoline vtable+0x108; U-0125 partially resolved (call-conv confirmed void(void); runtime target still open)
2026-05-12  005aea10  FUN_005aea10  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader/005aea10.md; aligned-alloc wrapper (size+4; stores base at aligned-4); no U filed
2026-05-12  005aea40  FUN_005aea40  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader/005aea40.md; thin free wrapper over FUN_004522d0; no U filed
2026-05-12  005aea50  FUN_005aea50  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005aea50.md; JMP trampoline vtable+0x114; U-1735 open
2026-05-12  005aec00  FUN_005aec00  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader/005aec00.md; in-place byte-reverse buffer; leaf; no U filed
2026-05-12  005aec30  FUN_005aec30  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_cont1/0x005aec30.md; PCM endian-swap 16/32-bit; no U filed; dup d2 row removed
2026-05-12  005aeca0  FUN_005aeca0  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_rws_loader_d3/005aeca0.md; endian-swap field packer 1/2/4-byte; clears U-0992; no remaining U filed
2026-05-12  005aee20  FUN_005aee20  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_dsound/0x005aee20.md; bit-scan-forward loop; U-0352 open (return register)
2026-05-12  005aeea0  FUN_005aeea0  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_dsound_d3/0x005aeea0.md; CreateSemaphoreA wrapper; no U filed
2026-05-12  005aef00  FUN_005aef00  C1->C2  audio_promote_c2_rws_loader-20260512; re/analysis/audio_dsound_d3/0x005aef00.md; thread descriptor struct init (5 fields); no U filed
2026-05-12  004c2c90  FUN_004c2c90  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2c90.md; RW driver-system dispatcher; 13 callers; U-0227 U-0228 pre-filed
2026-05-12  004c2d70  FUN_004c2d70  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2d70.md; leaf getter DAT_007d3ff4 RW plugin frozen-gate
2026-05-12  004c2d90  FUN_004c2d90  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2d90.md; RwEngineRegisterPlugin shim &DAT_00617fe0; 17 callers; S-0200 pre-filed
2026-05-12  004c2de0  FUN_004c2de0  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2de0.md; cmd 0x0d wrapper GetNumSubSystems
2026-05-12  004c2e10  FUN_004c2e10  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2e10.md; cmd 0x0e wrapper GetSubSystemInfo
2026-05-12  004c2e40  FUN_004c2e40  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2e40.md; cmd 0x0f wrapper GetCurrentSubSystem
2026-05-12  004c2e70  FUN_004c2e70  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2e70.md; cmd 0x10 wrapper SetSubSystem; S-0208 S-0825 pre-filed
2026-05-12  004c2ea0  FUN_004c2ea0  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2ea0.md; cmd 0x05 wrapper GetNumVideoModes
2026-05-12  004c2ed0  FUN_004c2ed0  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2ed0.md; cmd 0x06 wrapper GetVideoModeInfo
2026-05-12  004c2f00  FUN_004c2f00  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2f00.md; cmd 0x0a wrapper GetCurrentVideoMode
2026-05-12  004c2f30  FUN_004c2f30  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2f30.md; cmd 0x07 wrapper UseVideoMode
2026-05-12  004c2f60  FUN_004c2f60  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2f60.md; engine-stop sequence cmd 0x12+0x03 + FUN_004d8060; state field +0x124:=2; U-0090 pre-filed (now closeable); S-0081 S-0005 pre-filed
2026-05-12  004c2fb0  FUN_004c2fb0  C1->C2  render_promote_c2_rw_plugin-20260512; re/analysis/render_promote_c2_rw_plugin/0x004c2fb0.md; engine-start sequence cmd 0x02+0x11 + FUN_004d8000 + FUN_004cf160; state field +0x124:=3
2026-05-12  0041e870  FUN_0041e870  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e870.md; scan-all setter; U-3213 (pre-filed); dup row removed
2026-05-12  0041e8b0  FUN_0041e8b0  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e8b0.md; indirect dispatch +0x14; U-0429 refined U-3214 (pre-filed); dup row removed
2026-05-12  0041e970  FUN_0041e970  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e970.md; indirect dispatch +0x44; U-3214 (pre-filed); dup row removed
2026-05-12  0041e980  FUN_0041e980  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e980.md; scan-first getter; U-3213 (pre-filed); dup row removed
2026-05-12  0041e9d0  FUN_0041e9d0  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e9d0.md; getter +0x14; leaf; U-3214 parent; dup row removed
2026-05-12  0041ea90  FUN_0041ea90  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041ea90.md; getter +0x44; leaf; U-3214 parent; dup row removed
2026-05-12  0041e8c0  sub_0041e8c0  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e8c0.md; indirect dispatch +0x18; U-0550 U-0551 (pre-filed)
2026-05-12  0041e9b0  sub_0041e9b0  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e9b0.md; bool comparator +0x10; U-0552 U-0553 (pre-filed)
2026-05-12  0041e9e0  sub_0041e9e0  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e9e0.md; getter +0x18 dual-role; U-0554 (pre-filed)
2026-05-12  0041e8f0  FUN_0041e8f0  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041e8f0.md; indirect dispatch +0x24; U-3128 (pre-filed)
2026-05-12  0041ebb0  FUN_0041ebb0  C1->C2  render_promote_c2_track_node-20260512; re/analysis/render_promote_c2_track_node/0x0041ebb0.md; 6-call RW-state sequence; S-3130 (pre-filed)
2026-05-11  0046c7b0  FUN_0046c7b0  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046c7b0.md; leaf getter; no U filed
2026-05-11  0046cbb0  FUN_0046cbb0  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046cbb0.md; U-1855 U-1856 U-1857 (pre-filed); leaf getter
2026-05-11  0046d700  FUN_0046d700  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046d700.md; U-1748 (pre-filed); leaf getter
2026-05-11  0046da80  VehicleTrackInteraction  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046da80.md; U-1413 filed; FUN_0046c5f0 inline marker added
2026-05-11  0046dc20  VehicleOOBDebugDraw  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046dc20.md; U-2692 filed; local_40 inline marker added
2026-05-11  0046ddb0  VehicleWheelForceIntegrator  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046ddb0.md; U-2687 U-3563 filed; inline markers added
2026-05-11  0046e9e0  FUN_0046e9e0  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046e9e0.md; U-1747 U-1748 (pre-filed)
2026-05-11  0046ef70  FUN_0046ef70  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046ef70.md; U-2629 U-2630 U-2631 (pre-filed); S-2623
2026-05-11  0046f6c0  VehicleWheelContactSolver  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/0046f6c0.md; U-2691 filed; S-2625 S-2633 S-2634
2026-05-11  004709a0  VehicleCollisionBroadPhase  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/004709a0.md; U-2627 U-2628 (pre-filed); S-2620 S-2621 S-2622; dup-rows cleaned
2026-05-11  00470670  VehicleControlUpdate  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/00470670.md; U-1408 U-3564 U-3565 filed; inline markers added
2026-05-11  00470c70  FUN_00470c70  C1->C2  vehicle_promote_c2-20260511; re/analysis/vehicle_promote_c2/00470c70.md; U-0387 U-0388 U-0389 U-0390 (pre-filed)
2026-05-06  004299d0  TimeRecord::WriteTrackBest  C0->C2  Ghidra decomp; leaderboard-20260506-JJJJJ; re/analysis/leaderboard/0x004299d0.md; U-2227 filed
2026-05-12  D-7840..D-7843  FUN_0047b860/b8d0/b880/0047ce40  REPAIR+VOID  rows restored then immediately removed: all 4 RVAs already in hooks.csv (input×3, render×1); no deferred work existed
2026-05-12  0047ce40  FUN_0047ce40  render->physics  subsystem corrected; DAT_006c6b90[0..199] linear scan is physics body slot lookup; track_loader_d3/0047ce40.md unchanged
2026-05-12  004b6520  FUN_004b6520  duplicate-removed  vehicle row (powerups_d2) dropped; input row (input_lua_d2) kept as authoritative; both were C1 mapped
2026-05-06  0042d5a0  FUN_0042d5a0  stub->C1  Ghidra decomp; credits_screen-NNNNN; re/analysis/credits_screen/0x0042d5a0.md; U-2177 U-2178 U-2179 filed; S-0449 cleared
2026-05-06  00472f40  FUN_00472f40  stub->C1  Ghidra decomp; credits_screen-NNNNN; re/analysis/credits_screen/0x00472f40.md; S-0450 cleared
2026-05-06  004730b0  FUN_004730b0  stub->C1  Ghidra decomp; credits_screen-NNNNN; re/analysis/credits_screen/0x004730b0.md; S-0451 cleared

Format: `YYYY-MM-DD  RVA  name  oldC->newC  evidence`

Demotions use `oldC<-newC` (arrow flipped).

```
2026-05-02  --------  PROJECT BOOTSTRAP — first MCP session
2026-05-02  004a4bb7  entry         C0->C1  symbol IMPORTED by Ghidra as PE entry; MSVC EH runtime visible nearby (re/analysis/entry_point.md)
2026-05-02  entry_callees-20260502  scribe-claim   bucket=entry_callees rvas=18
2026-05-02  00492370  FUN_00492370            C0->C1  structural read; 4-param; 3-phase init/teardown; strings AppInitialiseOnBootup/AppDestroy (re/analysis/entry_callees/00492370.md)
2026-05-02  004a31f3  FUN_004a31f3            C0->C1  structural read; fn-ptr table iterator 005ea03c×7 + 005ea000×14; _atexit reg (re/analysis/entry_callees/004a31f3.md)
2026-05-02  004a332b  FUN_004a332b            C0->C1  structural read; no-return wrapper for FUN_004a3258 (re/analysis/entry_callees/004a332b.md)
2026-05-02  004a334d  FUN_004a334d            C0->C1  structural read; normal-return wrapper: FUN_004a3258(0,0,1) (re/analysis/entry_callees/004a334d.md)
2026-05-02  004a3440  __chkstk                C0->C1  library match VS2003 __chkstk; stack-probe loop (re/analysis/entry_callees/004a3440.md)
2026-05-02  004a4b6e  __amsg_exit             C0->C1  library match VS2003 __amsg_exit (re/analysis/entry_callees/004a4b6e.md)
2026-05-02  004a4b93  fast_error_exit         C0->C1  library match VS2003 _fast_error_exit (re/analysis/entry_callees/004a4b93.md)
2026-05-02  004a5984  __SEH_prolog            C0->C1  library match VS __SEH_prolog (re/analysis/entry_callees/004a5984.md)
2026-05-02  004a59bf  __SEH_epilog            C0->C1  library match VS __SEH_epilog (re/analysis/entry_callees/004a59bf.md)
2026-05-02  004a78b0  FUN_004a78b0            C0->C1  structural read; fn-ptr table 005e7b84 iterator; U-0005 (re/analysis/entry_callees/004a78b0.md)
2026-05-02  004a8a04  FUN_004a8a04            C0->C1  structural read; TlsAlloc+_calloc(1,0x88)+TlsSetValue (re/analysis/entry_callees/004a8a04.md)
2026-05-02  004aa3fe  __heap_init             C0->C1  library match VS2003 __heap_init (re/analysis/entry_callees/004aa3fe.md)
2026-05-02  004abbea  FUN_004abbea            C0->C1  structural read; byte-scan cmdline ptr; checks 0x22/0x21 (re/analysis/entry_callees/004abbea.md)
2026-05-02  004abc53  __setenvp               C0->C1  library match VS2003 __setenvp (re/analysis/entry_callees/004abc53.md)
2026-05-02  004abe86  FUN_004abe86            C0->C1  structural read; __fastcall; GetModuleFileNameA (re/analysis/entry_callees/004abe86.md)
2026-05-02  004abf28  ___crtGetEnvironmentStringsA  C0->C1  library match VS2003 (re/analysis/entry_callees/004abf28.md)
2026-05-02  004ac04a  FUN_004ac04a            C0->C1  structural read; CRT file-handle table init (re/analysis/entry_callees/004ac04a.md)
2026-05-02  entry_callees-20260502  scribe-release writes=18 errors=0
```
2026-05-02  00402750  FUN_00402750            C0->C1  structural read; 57 callees; PIZ/TXD asset load chain; DAT_00771a0c vtable; U-0007 U-0008 (re/analysis/boot_app_init/00402750.md)
2026-05-02  00402a40  FUN_00402a40            C0->C1  structural read; 43 callees; teardown; DAT_00636ac8 zeroed via FUN_004c5930 (re/analysis/boot_app_init/00402a40.md)
2026-05-02  00492270  FUN_00492270            C0->C1  structural read; FUN_00493710(0) gate; 3 callees (re/analysis/boot_app_init/00492270.md)
2026-05-02  00492290  FUN_00492290            C0->C1  structural read; while-loop on DAT_00828300+FUN_00499690; 8 callees (re/analysis/boot_app_init/00492290.md)
2026-05-02  004924f0  FUN_004924f0            C0->C1  structural read; zero-fill 0xdce9 DWORDs at DAT_007f0f60; nested init loops; U-0009 (re/analysis/boot_app_init/004924f0.md)
2026-05-02  00493540  thunk_FUN_00495150      C0->C1  structural read; 4-byte thunk→0x00495150; target verified+decomped (re/analysis/boot_app_init/00493540.md)
2026-05-02  00493550  thunk_FUN_004938c0      C0->C1  structural read; 4-byte thunk→0x004938c0; target verified+decomped (re/analysis/boot_app_init/00493550.md)
2026-05-02  00493560  thunk_FUN_004954f0      C0->C1  structural read; 4-byte thunk→0x004954f0; target ShowCursor(1) conditional; returns 0 (re/analysis/boot_app_init/00493560.md)
2026-05-02  00493900  FUN_00493900            C0->C1  structural read; cmd-line tokenizer on space; sets DAT_006147bc/c0 DAT_007719e0/e4/e8; S-0001 S-0002 (re/analysis/boot_app_init/00493900.md)
2026-05-02  004963e0  FUN_004963e0            C0->C1  structural read; _fputs wrapper on DAT_00772fbc FILE* (re/analysis/boot_app_init/004963e0.md)
2026-05-02  004996f0  FUN_004996f0            C0->C1  structural read; ShowWindow+UpdateWindow on DAT_007e9584 HWND (re/analysis/boot_app_init/004996f0.md)
2026-05-02  00499ba0  FUN_00499ba0            C0->C1  structural read; CoInitialize+RegisterClassA("MASHED")+CreateWindowExA→DAT_007e9584; S-0003 (re/analysis/boot_app_init/00499ba0.md)
2026-05-02  00499cc0  FUN_00499cc0            C0->C1  structural read; DestroyWindow(DAT_007e9584); returns DAT_007e95a8 (re/analysis/boot_app_init/00499cc0.md)
2026-05-02  004c5930  FUN_004c5930            C0->C1  structural read; linked-list traversal+unlink; vtable +0x11c; U-0010 U-0011; S-0004 S-0005 (re/analysis/boot_app_init/004c5930.md)
2026-05-02  005c9d00  FUN_005c9d00            C0->C1  structural read; 2-byte function; returns 0 (re/analysis/boot_app_init/005c9d00.md)
2026-05-02  004a2c2f  FUN_004a2c2f            C0->C1  structural read; calls FUN_004a2bf7+__ms_p5_mp_test_fdiv->DAT_00773994+FUN_004a5de3 (re/analysis/boot_crt_exit/0x004a2c2f.md)
2026-05-02  004a3258  FUN_004a3258            C0->C1  structural read; __lock(8); atexit walk 008ab6cc; fn-ptr tables 005ea05c+005ea068; U-0028+U-0029 (re/analysis/boot_crt_exit/0x004a3258.md)
2026-05-02  004a31b1  ___crtExitProcess       C0->C1  library match VS2003; GetModuleHandle(mscoree)+GetProcAddress(CorExitProcess)+ExitProcess (re/analysis/boot_crt_exit/0x004a31b1.md)
2026-05-02  004a333c  __exit                  C0->C1  library match VS2003; single call FUN_004a3258(_Code,1,0) (re/analysis/boot_crt_exit/0x004a333c.md)
2026-05-02  004a40fe  ___onexitinit           C0->C1  library match VS2003; _malloc(0x80); init DAT_008ab6cc/d0 (re/analysis/boot_crt_exit/0x004a40fe.md)
2026-05-02  004a415e  _atexit                 C0->C1  library match VS2003; delegates to __onexit; returns 0/-1 (re/analysis/boot_crt_exit/0x004a415e.md)
2026-05-02  004a467e  _calloc                 C0->C1  library match VS2003; SBH+HeapAlloc paths; new-handler loop; SEH (re/analysis/boot_crt_exit/0x004a467e.md)
2026-05-02  004a57e4  FUN_004a57e4            C0->C1  structural read; XOR seed to DAT_00616038; fallback 0xbb40e64e (re/analysis/boot_crt_exit/0x004a57e4.md)
2026-05-02  004a774d  __mtinitlocks           C0->C1  library match VS2003; 36-lock table 00616408; ___crtInitCritSecAndSpinCount spincount=4000 (re/analysis/boot_crt_exit/0x004a774d.md)
2026-05-02  004a87f7  FUN_004a87f7            C0->C1  structural read; __mtdeletelocks; TlsFree(DAT_00616658) sentinel 0xffffffff (re/analysis/boot_crt_exit/0x004a87f7.md)
2026-05-02  004aa3e4  ___heap_select          C0->C1  library match VS2003; reads DAT_0077399c+DAT_007739a8; returns 1 or 3 (re/analysis/boot_crt_exit/0x004aa3e4.md)
2026-05-02  004aa44f  ___sbh_heap_init        C0->C1  library match VS2003; HeapAlloc(008aa69c,0,0x140); inits 6 SBH globals (re/analysis/boot_crt_exit/0x004aa44f.md)
2026-05-02  004ab8d6  FUN_004ab8d6            C0->C1  structural read; error-code table scan 00616890; stderr/MessageBox path; stack-cookie 00616038; U-0030 (re/analysis/boot_crt_exit/0x004ab8d6.md)
2026-05-02  004aba4d  __FF_MSGBANNER          C0->C1  library match VS2003; mode check 007739f0+006160c8; calls FUN_004ab8d6(0xfc)+fn-ptr 00773c38+FUN_004ab8d6(0xff) (re/analysis/boot_crt_exit/0x004aba4d.md)
2026-05-02  004938c0  FUN_004938c0            C0->C1  structural read; SoftwareTidyUpBeforeExiting; 5 sequential void calls; U-0087 (re/analysis/rw_engine_teardown/0x004938c0.md)
2026-05-02  00558470  FUN_00558470            C0->C1  structural read; 2x vtable-ptr dispatch via DAT_007d3ff8+0x10c; zeros 3 globals; U-0088 (re/analysis/rw_engine_teardown/0x00558470.md)
2026-05-02  00550390  FUN_00550390            C0->C1  structural read; linked-list teardown+DeleteCriticalSection; S-0080; U-0089 (re/analysis/rw_engine_teardown/0x00550390.md)
2026-05-02  004c2f60  FUN_004c2f60            C0->C1  structural read; 2x FUN_004c2c90(0x12/3)+FUN_004d8060; writes to DAT_007d3ff8+0x124; S-0081 S-0005; U-0090 (re/analysis/rw_engine_teardown/0x004c2f60.md)
2026-05-02  004c3040  FUN_004c3040            C0->C1  structural read; FUN_004c2c90(+4,1); copies 0x4b DWORDs; frees engine struct; S-0081; U-0091 (re/analysis/rw_engine_teardown/0x004c3040.md)
2026-05-02  004c3270  FUN_004c3270            C0->C1  structural read; DAT_007d3ff4 refcount check; FUN_004d7ca0+FUN_004ccf20; S-0082 S-0083; U-0092 (re/analysis/rw_engine_teardown/0x004c3270.md)
2026-05-02  rw_engine_init-20260502-1734  scribe-claim   bucket=rw_engine_init rvas=18
2026-05-02  00493710  FUN_00493710 (RW_INIT_FN)  C0->C1  structural read; 429 bytes; 27 callees; RwEngineInit sequential chain; S-0060..S-0067; U-0067 U-0068 (re/analysis/rw_engine_init/00493710.md)
2026-05-02  0045b350  FUN_0045b350            C0->C1  structural read; zero-body artifact; body_start==body_end; U-0069 (re/analysis/rw_engine_init/0045b350.md)
2026-05-02  00493600  FUN_00493600            C0->C1  structural read; 2x FUN_004ce790(ptr-to-global, fn-ptr, fn-ptr-or-label); S-0068 (re/analysis/rw_engine_init/00493600.md)
2026-05-02  00493640  FUN_00493640            C0->C1  structural read; 18-call RenderwareAttachPlugins chain; D-0244..D-0261 (re/analysis/rw_engine_init/00493640.md)
2026-05-02  004951e0  FUN_004951e0            C0->C1  structural read; 4-byte Ghidra thunk→FUN_004955d0; U-0070 (re/analysis/rw_engine_init/004951e0.md)
2026-05-02  004951f0  FUN_004951f0            C0->C1  structural read; ShowCursor(0) conditional; FUN_004cbb60→DAT_00771e58; reads +0xC4/+0xCC masked 0xFFFF (re/analysis/rw_engine_init/004951f0.md)
2026-05-02  00495270  FUN_00495270            C0->C1  structural read; FUN_00499710()→*param_1; populates buffer for FUN_004c30b0 (re/analysis/rw_engine_init/00495270.md)
2026-05-02  004963e0  FUN_004963e0            C0->C1  structural read; _fputs wrapper on DAT_00772fbc FILE* [also boot_app_init] (re/analysis/rw_engine_init/004963e0.md)
2026-05-02  004a2cbd  FUN_004a2cbd            C0->C1  structural read; FidDB VS2003 _printf/_wprintf; __cdecl variadic; stdout DAT_00616110 (re/analysis/rw_engine_init/004a2cbd.md)
2026-05-02  004c2ed0  FUN_004c2ed0            C0->C1  structural read; FUN_004c2c90(DAT_007d3ff8+0x10, 6, param_1, 0, param_2); returns param_1 on success (re/analysis/rw_engine_init/004c2ed0.md)
2026-05-02  004c2f00  FUN_004c2f00            C0->C1  structural read; FUN_004c2c90(DAT_007d3ff8+0x10, 10, &local_4, 0, 0); returns local_4 or 0xffffffff (re/analysis/rw_engine_init/004c2f00.md)
2026-05-02  004c2fb0  FUN_004c2fb0            C0->C1  structural read; FUN_004c2c90 ids 2/3/17; FUN_004d8000+FUN_004cf160; writes DAT_007d3ff8+0x124=3 (re/analysis/rw_engine_init/004c2fb0.md)
2026-05-02  004c3040  FUN_004c3040            C0->C1  structural read; FUN_004c2c90(+4,1); copies 0x4b DWORDs; indirect via DAT_007d3fd4; decrements DAT_007d3ff4 [also rw_engine_teardown] (re/analysis/rw_engine_init/004c3040.md)
2026-05-02  004c30b0  FUN_004c30b0            C0->C1  structural read; 446 bytes; manages DAT_007d3ff8 block; indirect alloc via [0x42]; FUN_004c2c90 ids 0/4/11; sets [0x49]=2 (re/analysis/rw_engine_init/004c30b0.md)
2026-05-02  004c3270  FUN_004c3270            C0->C1  structural read; DAT_007d3ff4 check; FUN_004d7ca0+FUN_004ccf20; writes +0x124=0 [also rw_engine_teardown] (re/analysis/rw_engine_init/004c3270.md)
2026-05-02  004c32b0  FUN_004c32b0 (RwEngineInit)  C0->C1  structural read; 767 bytes; sets DAT_007d3ff8=&DAT_007d3ec8; 14x FUN_004d7de0 plugin ids; sets [0x49]=1; U-0071 (re/analysis/rw_engine_init/004c32b0.md)
2026-05-02  004c5c80  FUN_004c5c80            C0->C1  structural read; writes param_1 to *(DAT_007d4054+0x10+DAT_007d3ff8); called as (0) (re/analysis/rw_engine_init/004c5c80.md)
2026-05-02  004c9eb0  FUN_004c9eb0            C0->C1  structural read; writes param_1 to DAT_006181c4; double-indirect via DAT_007d4108+0x18/+0x1c; 3-elem loop 0x5d8b80; called as (0x3c) (re/analysis/rw_engine_init/004c9eb0.md)
2026-05-02  sweep-20260502-1827  scribe-claim-sweep  buckets=4
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=boot_app_init  rvas=15  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=boot_app_init  writes=15  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=boot_crt_exit  rvas=14  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=boot_crt_exit  writes=14  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=rw_engine_teardown  rvas=6  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=rw_engine_teardown  writes=6  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-claim  bucket=rw_engine_init  rvas=18  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release  bucket=rw_engine_init  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1827  scribe-release-sweep  buckets=4  total_writes=53  errors=0
2026-05-02  004a45fb  _malloc                          C0->C1  library match VS2003; __nh_malloc wrapper; DAT_00773c34 _NhFlag@004a45fb (re/analysis/boot_crt_env/004a45fb.md)
2026-05-02  004a460d  _free                            C0->C1  library match VS2003; SBH mode DAT_008aa6a0@004a4620; lock-id=4@004a4629; HeapFree DAT_008aa69c@004a466c (re/analysis/boot_crt_env/004a460d.md)
2026-05-02  004a9410  _strlen                          C0->C1  library match VS; dword-at-a-time 0x7efefeff@004a9442; no callees (re/analysis/boot_crt_env/004a9410.md)
2026-05-02  004aaff0  _memcpy                          C0->C1  library match VS2003; MOVSD.REP@004ab023 + backward-copy; no callees (re/analysis/boot_crt_env/004aaff0.md)
2026-05-02  004abd1a  FUN_004abd1a                     C0->C1  structural read; token parser; non-standard ABI EAX+ECX+ESI; char-class 0x8aa341@004abd61; U-0047 (re/analysis/boot_crt_env/004abd1a.md)
2026-05-02  004ac560  FUN_004ac560                     C0->C1  structural read; 7-byte thunk JMP 0x004ac5d5@004ac565 into FUN_004ac570; D-0165 (re/analysis/boot_crt_env/004ac560.md)
2026-05-02  004ae29f  ___crtInitCritSecAndSpinCount    C0->C1  library match VS2003; lazy-init DAT_00773d60@004ae2ab; fallback ___crtInitCritSecNoSpinCount@8@004ae2e1; S-0045 (re/analysis/boot_crt_env/004ae29f.md)
2026-05-02  004af2b6  ___initmbctable                  C0->C1  library match VS2003; once-init guard DAT_008ab6d4@004af2b6; __setmbcp(-3)@004af2bf; S-0046 (re/analysis/boot_crt_env/004af2b6.md)
2026-05-02  004affe0  FUN_004affe0                     C0->C1  structural read; 17-byte wrapper FUN_004affaf(param_1,0,4)@004affe8; S-0047 (re/analysis/boot_crt_env/004affe0.md)
2026-05-02  00495530  FUN_00495530                     C0->C1  DirectInput8Create init wrapper; GetModuleHandleA(NULL)+DirectInput8Create; 0x800@00495549; DAT_005d0a8c@00495544; DAT_00771e78@0049553f; U-0267 U-0268 (re/analysis/input_dinput/00495530.md)
2026-05-02  004987b0  FUN_004987b0                     C0->C1  debug printf wrapper; FUN_004a42c5(buf,fmt,va)+OutputDebugStringA; 512B local buf; cookie DAT_00616038@body; S-0260; D-0700 (re/analysis/input_dinput/004987b0.md)
2026-05-02  audio_rws_loader-20260502-1838  scribe-claim   bucket=audio_rws_loader rvas=17
2026-05-02  005a7b60  FUN_005a7b60 (AUDIO_LOAD_FN)     C0->C1  structural read; 769 bytes; 16 depth-1 callees; RWS chunk dispatch 0x80a/0x80c/0x802; alloc tag 0x30808; S-none (all callees in subset); U-0107 U-0108 U-0109 (re/analysis/audio_rws_loader/005a7b60.md)
2026-05-02  004522d0  FUN_004522d0                     C0->C1  structural read; 11 bytes; vtable dealloc trampoline DAT_007d3ff8+0x10c; jumptable unrecoverable; S-0100; U-0110 (re/analysis/audio_rws_loader/004522d0.md)
2026-05-02  004cbd30  FUN_004cbd30                     C0->C1  structural read; 317 bytes; RW stream read; 4-type dispatch (file/mem/callback); error codes 5/0x8000001a/0xe; S-0101..S-0104; U-0111 (re/analysis/audio_rws_loader/004cbd30.md)
2026-05-02  004cc050  FUN_004cc050                     C0->C1  structural read; 249 bytes; RW stream seek/skip; same 4-type dispatch; seek mode 1; S-0105; U-0112 (re/analysis/audio_rws_loader/004cc050.md)
2026-05-02  005a79a0  FUN_005a79a0                     C0->C1  structural read; 155 bytes; audio object destructor; unlinks from DAT_007dca7c pool list; walks wave list; S-0106..S-0108; U-0113 U-0114 (re/analysis/audio_rws_loader/005a79a0.md)
2026-05-02  005a7b40  FUN_005a7b40                     C0->C1  structural read; 15 bytes; swap DAT_007dcabc current audio context; returns old value (re/analysis/audio_rws_loader/005a7b40.md)
2026-05-02  005a7b50  FUN_005a7b50                     C0->C1  structural read; 5 bytes; read DAT_007dcabc; getter pair to FUN_005a7b40 (re/analysis/audio_rws_loader/005a7b50.md)
2026-05-02  005a7ee0  FUN_005a7ee0                     C0->C1  structural read; 134 bytes; audio object init; empty circular wave list at +0x0c; inserts into DAT_007dca7c pool; vtable+0xf4; S-0109..S-0111; U-0115 U-0116 (re/analysis/audio_rws_loader/005a7ee0.md)
2026-05-02  005ab380  FUN_005ab380                     C0->C1  structural read; 136 bytes; RWS 12-byte chunk header reader; legacy vs modern version decode; 5-field output struct; U-0117 (re/analysis/audio_rws_loader/005ab380.md)
2026-05-02  005ab410  FUN_005ab410                     C0->C1  structural read; 139 bytes; RWS chunk type seeker; version range 0x34fff..0x37002; loops FUN_005ab380+FUN_004cc050; U-0118 (re/analysis/audio_rws_loader/005ab410.md)
2026-05-02  005abcf0  FUN_005abcf0                     C0->C1  structural read; 61 bytes; wave node destructor; double-indirect vtable *(*(+0x0c)+0x10); S-0112..S-0115; U-0119 U-0120 (re/analysis/audio_rws_loader/005abcf0.md)
2026-05-02  005abfa0  FUN_005abfa0                     C0->C1  structural read; 619 bytes; per-wave loader; 0x803+0x804 chunks; format check DAT_005e6414/0x5e6444; PCM inner loop; S-0116..S-0120; U-0121 U-0122 U-0123 (re/analysis/audio_rws_loader/005abfa0.md)
2026-05-02  005ade10  FUN_005ade10                     C0->C1  structural read; 65 bytes; doubly-linked list remove-by-value; node[0]=prev,[1]=next,[2]=data; pool free FUN_005ae920 at DAT_009146c0; S-0121; U-0124 (re/analysis/audio_rws_loader/005ade10.md)
2026-05-02  005aea00  FUN_005aea00                     C0->C1  structural read; 11 bytes; vtable alloc trampoline DAT_007d3ff8+0x108; jumptable unrecoverable; U-0125 (re/analysis/audio_rws_loader/005aea00.md)
2026-05-02  005aea10  FUN_005aea10                     C0->C1  structural read; 42 bytes; aligned alloc wrapper; FUN_005aea00(size+4,tag); aligns 4B; stores base at aligned-4; mask 0xfffffffc (re/analysis/audio_rws_loader/005aea10.md)
2026-05-02  005aea40  FUN_005aea40                     C0->C1  structural read; 15 bytes; thin free wrapper over FUN_004522d0 (re/analysis/audio_rws_loader/005aea40.md)
2026-05-02  005aec00  FUN_005aec00                     C0->C1  structural read; 42 bytes; in-place byte-reversal; param_2/2 swap iterations; endian correction for wave count (re/analysis/audio_rws_loader/005aec00.md)
2026-05-02  audio_rws_loader-20260502-1838  scribe-release  writes=17 errors=0
2026-05-02  004c2c90  FUN_004c2c90  C0->C1  structural read; 190 bytes; vtable dispatch param_1+4 with cmd IDs 0xd-0x12; default calls FUN_004d7ff0+FUN_004d8480; U-0227 U-0228 (re/analysis/rw_engine_teardown_d2/0x004c2c90.md)
2026-05-02  004ccf20  FUN_004ccf20  C0->C1  structural read; 311 bytes; doubly-linked-list teardown loop; vtable calls +0x10c/+0x11c via DAT_007d3ff8; zeroes DAT_007d45fc/007d45f8; U-0229 U-0230 U-0231 (re/analysis/rw_engine_teardown_d2/0x004ccf20.md)
2026-05-02  004d7ca0  FUN_004d7ca0  C0->C1  structural read; 196 bytes; guards DAT_007d6c50; calls FUN_004ccce0+FUN_004cc9f0; iterates array DAT_007d6c54 freeing structs via vtable; returns 1; S-0222 S-0223 U-0232 U-0233 (re/analysis/rw_engine_teardown_d2/0x004d7ca0.md)
2026-05-02  004d8060  FUN_004d8060  C0->C1  structural read; 44 bytes; linked-list iteration calling fn ptr at node+0x24 with (param_2,*node,node[1]); advance via node+0x34; returns param_1; U-0234 (re/analysis/rw_engine_teardown_d2/0x004d8060.md)
2026-05-02  00551510  FUN_00551510  C0->C1  structural read; 49 bytes; dispatch on param_2; 1=fn ptr at param_1+0x20(param_1+0x50); 2=fn ptr at param_1+0x24(param_1+0x50); all indirect; U-0235 (re/analysis/rw_engine_teardown_d2/0x00551510.md)
2026-05-02  0047b9b0  FUN_0047b9b0                     C0->C1  LUA_INIT_FN; creates state via 0x0047b860; indirect callback (*param_3); executes via 0x0047b8d0; closes via 0x0047b880; U-0307 (re/analysis/input_lua/0047b9b0.md)
2026-05-02  0047b860  FUN_0047b860                     C0->C1  Lua state creator; FUN_004b7330(0)->DAT_006bf1e0; FUN_004c0510(state); S-0300 S-0301; U-0308 (re/analysis/input_lua/0047b860.md)
2026-05-02  0047b880  FUN_0047b880                     C0->C1  Lua state closer; FUN_004b7480(DAT_006bf1e0); zeroes DAT_006bf1e0; S-0302 (re/analysis/input_lua/0047b880.md)
2026-05-02  0047b8d0  FUN_0047b8d0                     C0->C1  Lua file reader+exec; fopen("rb"@0x005cf010)+ftell+fread into 32k stack buf; FUN_0047b8a0(buf,count,param_2); S-0303 S-0304; U-0309; D-0820 (re/analysis/input_lua/0047b8d0.md)
2026-05-02  004a2be9  __security_check_cookie     C0->C1  cookie check DAT_00616038; calls report_failure@004a2bb8 on mismatch; S-0180 (re/analysis/boot_crt_exit_d3/004a2be9.md)
2026-05-02  004a2bf7  FUN_004a2bf7                C0->C1  fn-ptr table writer 006160d8..ec; 6 slots; no callees; U-0187 U-0188 (re/analysis/boot_crt_exit_d3/004a2bf7.md)
2026-05-02  004a34b0  _strncpy                    C0->C1  library VS; dword-at-a-time 0x7efefeff; null-detect; no callees (re/analysis/boot_crt_exit_d3/004a34b0.md)
2026-05-02  004a4126  __onexit                    C0->C1  library VS2003; SEH frame; FUN_004a31e1+__onexit_lk+FUN_004a4158; S-0181..S-0183; U-0189 U-0190 (re/analysis/boot_crt_exit_d3/004a4126.md)
2026-05-02  004a4728  FUN_004a4728                C0->C1  8-byte wrapper; FUN_004a77eb(4) (re/analysis/boot_crt_exit_d3/004a4728.md)
2026-05-02  004a5de3  FUN_004a5de3                C0->C1  calls __controlfp(0x10000,0x30000); S-0184 (re/analysis/boot_crt_exit_d3/004a5de3.md)
2026-05-02  004a5e35  __ms_p5_mp_test_fdiv        C0->C1  library VS2003; KERNEL32 GetProcAddress IsProcessorFeaturePresent; fallback __ms_p5_test_fdiv@004a5df5; S-0185 (re/analysis/boot_crt_exit_d3/004a5e35.md)
2026-05-02  004a5f07  ___endstdio                 C0->C1  library VS2003; __flushall+conditional __fcloseall; DAT_007739d4; S-0186 S-0187 (re/analysis/boot_crt_exit_d3/004a5f07.md)
2026-05-02  004a7796  __mtdeletelocks             C0->C1  library VS; two-pass loop 616408..616528+8; pass1 delete+free; pass2 delete pinned (re/analysis/boot_crt_exit_d3/004a7796.md)
2026-05-02  004a77eb  FUN_004a77eb                C0->C1  LeaveCriticalSection(&DAT_00616408[param_1*2]) (re/analysis/boot_crt_exit_d3/004a77eb.md)
2026-05-02  004a787f  __lock                      C0->C1  library VS2003; lazy-init FUN_004a7800; __amsg_exit(0x11) on fail; EnterCriticalSection; S-0188 (re/analysis/boot_crt_exit_d3/004a787f.md)
2026-05-02  004aac76  ___sbh_alloc_block          C0->C1  library VS2003; SBH region/group scan+split; 008aa688 array; S-0189 S-0190; U-0191 (re/analysis/boot_crt_exit_d3/004aac76.md)
2026-05-02  004aaf72  __callnewh                  C0->C1  library VS2003; calls DAT_00773c30 handler if non-null (re/analysis/boot_crt_exit_d3/004aaf72.md)
2026-05-02  004aaf90  _memset                     C0->C1  library VS; 4-byte-aligned fill; 0x1010101 replication; no callees (re/analysis/boot_crt_exit_d3/004aaf90.md)
2026-05-02  004ac45c  ___crtMessageBoxA           C0->C1  library VS2003; lazy LoadLibrary(user32); MB_DEFAULT_DESKTOP_ONLY 0x40000; MB_SERVICE_NOTIFICATION 0x200000 (re/analysis/boot_crt_exit_d3/004ac45c.md)
2026-05-02  save_gamesave-20260502-1854  session-analysis  bucket=save_gamesave  rvas=6  U-0287..U-0288  S-0280..S-0284  D-0760..D-0765
2026-05-02  render_d3d9_device-20260502-1856  session-analysis  bucket=render_d3d9_device  rvas=12  U-0247 U-0248  D-0640..D-0646  cap_count=12
2026-05-02  004c7a70  _rwDeviceSystemFn (D3D_INIT_FN)  C0  listing-only; FPO prologue; function undefined in Ghidra; 23-case switch; Direct3DCreate9@004c7e0b; CreateDevice via vtbl+0x40@004c82b6; U-0247 (re/analysis/render_d3d9_device/0x004c7a70.md)
2026-05-02  004c8650  FUN_004c8650  C0->C1  structural read; render-state cache init; zeros 006181c8..d8 + array 007d40c0 (re/analysis/render_d3d9_device/0x004c8650.md)
2026-05-02  004c8690  FUN_004c8690  C0->C1  structural read; constant pool create/reset; 007d4568 handle; 260-slot table 007d4158; callees 004cc7f0 004ccc50 (re/analysis/render_d3d9_device/0x004c8690.md)
2026-05-02  004c8740  FUN_004c8740  C0->C1  structural read; MSAA enumeration; IDirect3D9::CheckDeviceMultiSampleType vtbl+0x2c; max type→006181b4 (re/analysis/render_d3d9_device/0x004c8740.md)
2026-05-02  004c8800  FUN_004c8800  C0->C1  structural read; D3DPRESENT_PARAMETERS builder; GetWindowRect; IDirect3D9 vtbl+0x24/28/2c/30; depth formats 0x4b..0x50; windowed/FS paths (re/analysis/render_d3d9_device/0x004c8800.md)
2026-05-02  004c8c70  FUN_004c8c70  C0->C1  structural read; D3D9 device teardown; IDirect3DDevice9 SetTexture/SetIndices/SetStreamSource/SetPixelShader/SetVertexDeclaration/SetVertexShader/Release; 5 callees (re/analysis/render_d3d9_device/0x004c8c70.md)
2026-05-02  004c8e50  FUN_004c8e50  C0->C1  structural read; raster dispatch table init; 27 rwRASTERSYSTEM_* entries 0x02..0x1c; default FUN_005c9d00 (re/analysis/render_d3d9_device/0x004c8e50.md)
2026-05-02  004cc820  FUN_004cc820  C0->C1  structural read; RwFreeListCreate; 6-param pool allocator; header layout [0..8]; pre-alloc; 007d45cc chain (re/analysis/render_d3d9_device/0x004cc820.md)
2026-05-02  004cc9f0  FUN_004cc9f0  C0->C1  structural read; RwFreeListDestroy; unlinks from 007d45cc; frees blocks+header (re/analysis/render_d3d9_device/0x004cc9f0.md)
2026-05-02  004dcf90  FUN_004dcf90  C0->C1  structural read; CPUID via EFLAGS bit-21; leaf1[+8] bit 23 (likely MMX); U-0248 (re/analysis/render_d3d9_device/0x004dcf90.md)
2026-05-02  004dcff0  FUN_004dcff0  C0->C1  structural read; CPUID via EFLAGS bit-21; leaf1[+8] bits 25|26 (likely SSE|SSE2); U-0248 (re/analysis/render_d3d9_device/0x004dcff0.md)
2026-05-02  004dd050  FUN_004dd050  C0->C1  structural read; CPUID via EFLAGS bit-21; leaf1[+8] bit 26 (likely SSE2); U-0248 (re/analysis/render_d3d9_device/0x004dd050.md)
2026-05-02 rw_engine_init_d2-20260502-1905 analysis bucket=rw_engine_init_d2 rvas=18 slot=Mashed_pool3
2026-05-02  004014b0  FUN_004014b0             C0->C1  symmetrical FUN_004e4860/FUN_004c0740/FUN_004e4d90 on 3 globals 0x636560/70/6c; U-0167; S-0163..S-0165 (re/analysis/boot_app_init_d3/0x004014b0.md)
2026-05-02  004015a0  FUN_004015a0             C0->C1  13×104-byte elem loop 0x636578..0x636ac0; FUN_004e6e00 on 10 handles/elem; float 1.0f at +0x34; U-0168; S-0162+S-0166 (re/analysis/boot_app_init_d3/0x004015a0.md)
2026-05-02  004025f0  FUN_004025f0             C0->C1  init pair for 004015a0; local_34 table 13 values; FUN_00401630; U-0169; S-0167..S-0170 (re/analysis/boot_app_init_d3/0x004025f0.md)
2026-05-02  004026d0  FUN_004026d0             C0->C1  loop param_1×; FUN_004671a0 3× varying args; local_4=0xff000000; U-0170; S-0171..S-0175 (re/analysis/boot_app_init_d3/0x004026d0.md)
2026-05-02  00402f50  FUN_00402f50             C0->C1  no callees; zeros 0x636ae8/af0/af8; writes 40.0f→0x636af4 60.0f→0x636aec; U-0171 (re/analysis/boot_app_init_d3/0x00402f50.md)
2026-05-02  00403640  FUN_00403640             C0->C1  FUN_004034a0(param_1)→DAT_0x636b78; returns 1; S-0176 (re/analysis/boot_app_init_d3/0x00403640.md)
2026-05-02  00403660  FUN_00403660             C0->C1  vtable dispatch *DAT_0x636b70+8 then null; FUN_004c7650(DAT_0x636b78) then zero; U-0172; S-0177 (re/analysis/boot_app_init_d3/0x00403660.md)
2026-05-02  00404830  FUN_00404830             C0->C1  fills 0x636c08..0x639d67 0xffffffff; 13 scalar overrides; stride-6 loop 0x6390b0 values 0..0x17; no callees; U-0173 U-0174 (re/analysis/boot_app_init_d3/0x00404830.md)
2026-05-02  0040bb30  FUN_0040bb30             C0->C1  forwarder FUN_004c5c00(DAT_0x63b8f8,param_1); S-0178 (re/analysis/boot_app_init_d3/0x0040bb30.md)
2026-05-02  0040bb50  FUN_0040bb50             C0->C1  forwarder FUN_004c5c00(DAT_0x63b8fc,param_1); S-0178 (re/analysis/boot_app_init_d3/0x0040bb50.md)
2026-05-02  0040bbb0  FUN_0040bbb0             C0->C1  loads sfx.piz; 4 TXD handles fx/badges/TrackImages/Interface; scorch(0x18)/wfall(0x10)/RWObjShad(0x32); S-0179+S-0305..S-0329 (re/analysis/boot_app_init_d3/0x0040bbb0.md)
2026-05-02  0040bd00  FUN_0040bd00             C0->C1  teardown: 13 no-arg calls + FUN_004c5930 on 4 TXD handles; S-0330..S-0342 (re/analysis/boot_app_init_d3/0x0040bd00.md)
2026-05-02  0040cf80  FUN_0040cf80             C0->C1  polls FUN_0041f320(0..3); conditional init sequence; S-0343..S-0347 (re/analysis/boot_app_init_d3/0x0040cf80.md)
2026-05-02  0040cfd0  FUN_0040cfd0             C0->C1  seq FUN_00490000+FUN_0045bed0+FUN_0045bf30; FUN_00426c00+FUN_004725f0+FUN_00426b40; FUN_00405400; U-0175; S-0348..S-0357 (re/analysis/boot_app_init_d3/0x0040cfd0.md)
2026-05-02  004113b0  FUN_004113b0             C0->C1  stack-cookie; table@0x5f29d0 sum==0x24a3c; alloc vtbl+0x108; format vtbl+0xc4; U-0176 U-0177; S-0358..S-0359 (re/analysis/boot_app_init_d3/0x004113b0.md)
2026-05-02  004114c0  FUN_004114c0             C0->C1  indirect (0x7d3ff8+0x10c)(DAT_0x8a94a8); returns 1; teardown pair for 004113b0 (re/analysis/boot_app_init_d3/0x004114c0.md)
2026-05-02  00418980  thunk_FUN_0041a060       C0->C1  thunk→0x41a060; shockwave/crosshair2/crosshair/exocet.dff/airstrike; 3 init loops 0x63bf30/0x63bde0/0x63c018; S-0307+S-0328+S-0360..S-0369 (re/analysis/boot_app_init_d3/0x00418980.md)
2026-05-02  004189e0  thunk_FUN_004196f0       C0->C1  thunk→0x4196f0; teardown loops 0x63bf70+0x63bde0; S-0161+S-0166+S-0370..S-0372 (re/analysis/boot_app_init_d3/0x004189e0.md)
2026-05-02  boot_app_init_d3-20260502-1859  session-analysis  bucket=boot_app_init_d3  rvas=18  U-0167..U-0177  S-0160..S-0179+S-0305..S-0372  D-0400..D-0401
2026-05-02  sweep-20260502-1935  scribe-claim-sweep  buckets=10
2026-05-02  sweep-20260502-1935  scribe-claim  bucket=boot_crt_env  rvas=9  (sweep)
2026-05-02  sweep-20260502-1935  scribe-release  bucket=boot_crt_env  writes=9  errors=0  (sweep)
2026-05-02  sweep-20260502-1935  scribe-claim  bucket=input_dinput  rvas=2  (sweep)
2026-05-02  sweep-20260502-1935  scribe-release  bucket=input_dinput  writes=2  errors=0  (sweep)
2026-05-02  sweep-20260502-1935  scribe-claim  bucket=rw_engine_teardown_d2  rvas=5  (sweep)
2026-05-02  sweep-20260502-1935  scribe-release  bucket=rw_engine_teardown_d2  writes=5  errors=0  (sweep)
2026-05-02  sweep-20260502-1935  scribe-halt  bucket=render_d3d9_device  rva=0x004c7a70  reason='function_at returned: no function found at 004c7a70 (FPO prologue, undefined function)'
2026-05-02  sweep-20260502-1935  scribe-release-sweep  buckets=3  total_writes=16  errors=1  (halted at render_d3d9_device; 6 buckets remain queued)
2026-05-02  sweep-20260502-1941  scribe-claim-sweep  buckets=7  (render_d3d9_device reordered last)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=rw_engine_init_d2  rvas=18  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=rw_engine_init_d2  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=audio_rws_loader  rvas=17  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=audio_rws_loader  writes=17  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=input_lua  rvas=4  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=input_lua  writes=4  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=boot_crt_exit_d3  rvas=18  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=boot_crt_exit_d3  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=save_gamesave  rvas=6  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=save_gamesave  writes=6  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-claim  bucket=boot_app_init_d3  rvas=18  (sweep)
2026-05-02  sweep-20260502-1941  scribe-release  bucket=boot_app_init_d3  writes=18  errors=0  (sweep)
2026-05-02  sweep-20260502-1941  scribe-halt  bucket=render_d3d9_device  rva=0x004c7a70  reason='no function found at 004c7a70 (FPO; expected per prior session)'
2026-05-02  sweep-20260502-1941  scribe-release-sweep  buckets=6  total_writes=81  errors=1  (halted at render_d3d9_device; 1 bucket remains queued — needs function definitions before scribe can process)
2026-05-02  video_mci-20260502-1943  analysis  bucket=video_mci  rvas=9  VIDEO_PLAY_FN=0x004944c0 (DirectShow/COM, not MCI); FUN_00494c80=small.mpg variant; 7 callees; U-0367..U-0371; S-0373..S-0376; D-1000..D-1002
2026-05-02  hud_frontend-20260502-1944  scribe-claim  bucket=hud_frontend  rvas=15  slot=Mashed_pool1  (queued; master.WIP cleared at claim)
2026-05-02  0043c5b0  FUN_0043c5b0          C0->C1  FRONTEND_FN: menu-item table dispatcher; phase gated by DAT_0067eca4; 3301 bytes; U-0447..U-0449 (re/analysis/hud_frontend/0x0043c5b0.md)
2026-05-02  0040bb50  FUN_0040bb50          C0->C1  asset lookup wrapper; calls FUN_004c5c00(DAT_0063b8fc, name); 20 bytes; U-0450; S-0440 (re/analysis/hud_frontend/0x0040bb50.md)
2026-05-02  00427e00  FUN_00427e00          C0->C1  sprite draw 6-param; full pipeline: select/prepare/state9/pos/vtxcol/draw/cleanup; U-0451; S-0441..S-0448 (re/analysis/hud_frontend/0x00427e00.md)
2026-05-02  00428140  FUN_00428140          C0->C1  sprite draw 7-param with vertical alpha gradient; dim-mode DAT_008990e4; S-0441..S-0448 (re/analysis/hud_frontend/0x00428140.md)
2026-05-02  0042aad0  FUN_0042aad0          C0->C1  dim-setter: writes 0x30 to in_EAX+3; sets DAT_008990e4=1; 14 bytes; U-0452 U-0453 (re/analysis/hud_frontend/0x0042aad0.md)
2026-05-02  0042aae0  FUN_0042aae0          C0->C1  RwIm2D fullscreen quad; 4-vert buf 0x0067ec30; alpha=DAT_0067eca8; U-0454..U-0456 (re/analysis/hud_frontend/0x0042aae0.md)
2026-05-02  0042b930  FUN_0042b930          C0->C1  frontend sub-state getter: returns DAT_0067ecb0; 5 bytes (re/analysis/hud_frontend/0x0042b930.md)
2026-05-02  0042e3a0  FUN_0042e3a0          C0->C1  menu chrome drawer: bands+borders at Y=64/416; amber ticks; scroll animation; S-0449..S-0451 (re/analysis/hud_frontend/0x0042e3a0.md)
2026-05-02  0042e5b0  FUN_0042e5b0          C0->C1  frontend BG+logo animation: 512-frame cycle; slide-in via DAT_008990e0; S-0452..S-0455 (re/analysis/hud_frontend/0x0042e5b0.md)
2026-05-02  0043bf30  FUN_0043bf30          C0->C1  frontend sub-menu dispatcher: 14 flag→callee pairs; all callees D-1240..D-1253 (re/analysis/hud_frontend/0x0043bf30.md)
2026-05-02  00472c60  FUN_00472c60          C0->C1  RwIm2D filled-quad: 4-vert; coord-scaled; shared buf 0x00898a20; S-0456 S-0457 (re/analysis/hud_frontend/0x00472c60.md)
2026-05-02  00472dc0  FUN_00472dc0          C0->C1  RwIm2D filled-triangle: 3-vert; same coord-scale; prim-count=3 (re/analysis/hud_frontend/0x00472dc0.md)
2026-05-02  00473540  FUN_00473540          C0->C1  RwIm2D gradient-quad: 4-vert; split per-vertex ARGB; U-0457 (re/analysis/hud_frontend/0x00473540.md)
2026-05-02  004739f0  FUN_004739f0          C0->C1  RwIm2D textured-quad: 12-param; UV at vert+20..+24; state9 conditional; U-0458 U-0459 (re/analysis/hud_frontend/0x004739f0.md)
2026-05-02  004a2c48  FUN_004a2c48          C0->C1  FPU ROUND(ST0)->ulonglong; banker's rounding; no callees; frame-counter source; U-0460 (re/analysis/hud_frontend/0x004a2c48.md)
2026-05-02  00418560  FUN_00418560  C0->C1  session ai_update-20260502-1952; decomp + depth-1 callees; per-vehicle AI dispatcher
2026-05-02  00407a40  FUN_00407a40  C0->C1  session ai_update-20260502-1952; depth-1 callee of 00418560; getter 0x8a9640+v*0x30c
2026-05-02  0040e350  FUN_0040e350  C0->C1  session ai_update-20260502-1952; depth-1 callee; game-mode getter DAT_0063ba8c
2026-05-02  0040e4a0  FUN_0040e4a0  C0->C1  session ai_update-20260502-1952; depth-1 callee; elapsed-time getter DAT_005f29b8
2026-05-02  00413fe0  FUN_00413fe0  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI state reset 4 vehicles stride 0x74
2026-05-02  00416250  FUN_00416250  C0->C1  session ai_update-20260502-1952; depth-1 callee; primary AI control step; behavior mode 0-10
2026-05-02  00416a30  FUN_00416a30  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI control step mode-4/9 variant
2026-05-02  00417180  FUN_00417180  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI spline-bank switcher
2026-05-02  00417640  FUN_00417640  C0->C1  session ai_update-20260502-1952; depth-1 callee; post-step powerup override
2026-05-02  00417da0  FUN_00417da0  C0->C1  session ai_update-20260502-1952; depth-1 callee; AI control step mode-8 variant
2026-05-02  00426c00  FUN_00426c00  C0->C1  session ai_update-20260502-1952; depth-1 callee; powerup-state getter DAT_00644158
2026-05-02  sweep-20260502-2109  scribe-claim-sweep  buckets=5  (render_d3d9_device deferred)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=powerups  rvas=11  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=powerups  writes=11  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=hud_frontend  rvas=15  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=hud_frontend  writes=15  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=ai_update  rvas=11  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=ai_update  writes=11  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=video_mci  rvas=9  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=video_mci  writes=9  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-claim  bucket=track_loader  rvas=16  (sweep)
2026-05-02  sweep-20260502-2109  scribe-release  bucket=track_loader  writes=16  errors=0  (sweep)
2026-05-02  sweep-20260502-2109  scribe-halt  bucket=render_d3d9_device  rva=0x004c7a70  reason='no function found at 004c7a70 (FPO; expected; needs function_create before sweep)'
2026-05-02  sweep-20260502-2109  scribe-release-sweep  buckets=5  total_writes=62  errors=1  (halted at render_d3d9_device — last bucket; only render_d3d9_device remains queued)
2026-05-02  005b9f30  LAB_005b9f30          C0->C1  fn-ptr constructor; 4 fn-ptrs to audio obj; FUN_005a9e10 + FUN_005aee20 callees; U-0347..U-0350; S-0340..S-0343 (re/analysis/audio_dsound/0x005b9f30.md)
2026-05-02  005a9e10  FUN_005a9e10          C0->C1  two-call dispatcher; 33 bytes; S-0344 S-0345; U-0351 (re/analysis/audio_dsound/0x005a9e10.md)
2026-05-02  005aee20  FUN_005aee20          C0->C1  bit-scan-forward 0..31; 27 bytes; U-0352 (re/analysis/audio_dsound/0x005aee20.md)
2026-05-02  005ba1d0  LAB_005ba1d0          C0->C1  unrecognized; DirectSound init path A; listing-only; U-0353..U-0355; S-0346..S-0351; D-0940 (re/analysis/audio_dsound/0x005ba1d0.md)
2026-05-02  005bad30  LAB_005bad30          C0->C1  unrecognized; DirectSound init path B; listing-only; 459 bytes; U-0356..U-0359; S-0352; D-0941..D-0951 (re/analysis/audio_dsound/0x005bad30.md)
2026-05-02  audio_dsound-20260502-1942  session-analysis  bucket=audio_dsound  rvas=5  slot=Mashed_pool1  U-0347..U-0359  S-0340..S-0352  D-0940..D-0951
2026-05-02  sweep-20260502-2131  scribe-claim-sweep  buckets=1  (render_d3d9_device deferred)
2026-05-02  sweep-20260502-2131  scribe-claim  bucket=audio_dsound  rvas=5  (sweep)
2026-05-02  sweep-20260502-2131  scribe-release-partial  bucket=audio_dsound  writes=3  errors=2  (drained 0x005b9f30,0x005a9e10,0x005aee20; halted at 0x005ba1d0 [no function found], 0x005bad30 [no function found] — FPO undefined)
2026-05-02  sweep-20260502-2131  scribe-release-sweep  buckets=0_full+1_partial  total_writes=3  errors=2  (audio_dsound partial; 0x005ba1d0+0x005bad30 + render_d3d9_device remain queued — all need function_create before next sweep)
2026-05-02  fixup-20260502-2148  function_create  rvas=14  bucket=render_d3d9_device+audio_dsound_FPO  (disassemble_seed + function_create on FPO entries; ready for sweep)
2026-05-02  sweep-20260502-2140  scribe-claim-sweep  buckets=2  (post-fixup)
2026-05-02  sweep-20260502-2140  scribe-claim  bucket=audio_dsound  rvas=5  (sweep, post-fixup completion)
2026-05-02  sweep-20260502-2140  scribe-release  bucket=audio_dsound  writes=5  errors=0  (sweep)
2026-05-02  sweep-20260502-2140  scribe-claim  bucket=render_d3d9_device  rvas=12  (sweep, post-fixup)
2026-05-02  sweep-20260502-2140  scribe-release  bucket=render_d3d9_device  writes=12  errors=0  (sweep)
2026-05-02  sweep-20260502-2140  scribe-release-sweep  buckets=2  total_writes=17  errors=0  (queue empty)
2026-05-02 audio_music-20260502-2145 analysis bucket=audio_music rvas=4 (004623e0,0045da60,0045dd60,004631f0) stubs=S-0620..S-0631 uncertainties=U-0627..U-0641 deferred=D-1780..D-1791
2026-05-02 effects_particle-20260502-2135 analysis bucket=effects_particle rvas=16
2026-05-02 hud_ingame-20260502-2132 analysis bucket=hud_ingame rvas=14 (0040dfc0,00403160,0041a3e0,0041b630,0041c0c0,0041c300,0041ccc0,0041d870,0041db80,0041ded0,0041e850,00426ba0,0042f500,0042f6a0) stubs=S-0560..S-0572 uncertainties=U-0567..U-0579 deferred=D-1600
2026-05-02 game_state-20260502-2144 analysis bucket=game_state rvas=20 (STATE_FN 0x004929d0 + callees 0x0040b430..0x0042c220) stubs=S-0480..S-0494 uncertainties=U-0487..U-0504 deferred=D-1360
2026-05-02  0x0040b090  FUN_0040b090  C0->C1  camera_follow-20260502-2132: per-slot camera dispatch, 4-player outer loop, color-token inner dispatch
2026-05-02  0x0041e8c0  FUN_0041e8c0  C0->C1  camera_follow-20260502-2132: 8-byte indirect tail-call via DAT_0063d7e4+0x18
2026-05-02  0x0041e9b0  FUN_0041e9b0  C0->C1  camera_follow-20260502-2132: 20-byte bool comparator DAT_0063d7e4+0x10 == param_1
2026-05-02  0x0041e9e0  FUN_0041e9e0  C0->C1  camera_follow-20260502-2132: 8-byte getter DAT_0063d7e4+0x18
2026-05-02  0x00426700  FUN_00426700  C0->C1  camera_follow-20260502-2132: 124-byte camera path node-list iterator
2026-05-02  0x00426780  FUN_00426780  C0->C1  camera_follow-20260502-2132: 132-byte two-array updater (64+8 entry loops)
2026-05-02  0x00426810  FUN_00426810  C0->C1  camera_follow-20260502-2132: 671-byte camera-path position lerp
2026-05-02  0x00426ab0  FUN_00426ab0  C0->C1  camera_follow-20260502-2132: CAMERA_FN confirmed (131 bytes, per-frame)
2026-05-02  0x004671a0  FUN_004671a0  C0->C1  camera_follow-20260502-2132: 27-byte vehicle-0 getter
2026-05-02  0x00467210  FUN_00467210  C0->C1  camera_follow-20260502-2132: 53-byte vehicle sub-obj getter (*(vehicle+4)+0x10)
2026-05-02  0x00471ec0  FUN_00471ec0  C0->C1  camera_follow-20260502-2132: 642-byte camera-anim trigger checker
2026-05-02  0x0047c160  FUN_0047c160  C0->C1  camera_follow-20260502-2132: 140-byte camera-path node loop
2026-05-02  0x00491490  FUN_00491490  C0->C1  camera_follow-20260502-2132: 18-byte mode dispatcher (DAT_007f108b flag)
2026-05-02  physics_collision-20260502-1900  analysis  bucket=physics_collision  rvas=0x00492e90(C1),0x0047a020(C1)  COLLISION_FN inconclusive — BSP system entirely Lua setup code; RWP37Active vtable not traceable statically; D-1792 filed for Frida tracing; U-0642..0644 filed
2026-05-02  sweep-20260502-2217  scribe-claim-sweep  buckets=7
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=texture_loader  rvas=3  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=texture_loader  writes=3  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=effects_particle  rvas=16  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=effects_particle  writes=16  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=hud_ingame  rvas=14  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=hud_ingame  writes=14  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=camera_follow  rvas=13  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=camera_follow  writes=13  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=audio_music  rvas=4  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=audio_music  writes=4  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=game_state  rvas=20  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=game_state  writes=20  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-claim  bucket=physics_collision  rvas=2  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release  bucket=physics_collision  writes=2  errors=0  (sweep)
2026-05-02  sweep-20260502-2217  scribe-release-sweep  buckets=7  total_writes=72  errors=0  (queue empty)
2026-05-02  exception_filter-20260502-2221  analysis  bucket=exception_filter  rvas=3  slot=Mashed_pool2
2026-05-02  localization-20260502-2227  analysis  bucket=localization  rvas=4  slot=Mashed_pool11  LOC_INIT_FN=0x004274d0  LOC_FN=0x004274e0
2026-05-02  settings_config-20260502-2222  analysis  bucket=settings_config  rvas=7  slot=Mashed_pool9  CONFIG_LOAD_FN=0x00498950  CONFIG_SAVE_FN=0x004989b0  shared_helper_with_P=no (distinct: P uses FUN_004b3b70+custom_IO; this session uses _fsopen+CRT)  settings_buf=0x00773208 size=512
2026-05-02  timer-20260502-2221  analysis  bucket=timer  rvas=11  slot=Mashed_pool4 (fallback: pool1/2/3 LockException)  TIMER_INIT_FN=0x00495120  TIMER_FN=0x00492d30  QPC_HELPER=0x004950b0  PROFILER=0x004926c0  DAT_QPF_LO=0x00771e70  DAT_QPF_HI=0x00771e74  DAT_FRAME_DELTA=0x007719a8  DAT_60F_AVG=0x0077197c
2026-05-02  window_msgpump-20260502-2232  analysis  bucket=window_msgpump  rvas=5  slot=Mashed_pool7  WNDPROC_FN=LAB_00499820(no-fn)  MSGPUMP_FN=0x00499690  switch-arms=0x1,0x2,0x6,0x100  U-0647,U-0648 filed  S-0640..S-0643 filed  D-1840 filed
2026-05-02  render_d3d_reset-20260502-2221  analysis  bucket=render_d3d_reset  rvas=7  slot=Mashed_pool3  RESET_FN=0x004c9cd0  pre_release=0x004c9ad0  recreate_vb=0x004dc970  recreate_bufs=0x004db3e0  recreate_surfaces=0x004d1e30  recreate_2d=0x004e0920  rs_cache_reset=0x004d6200  D3D_DEVICE=DAT_007d4110  D3DPRESENT_PARAMS=DAT_009120e0  stubs=S-0700..S-0707  uncertainties=U-0707,U-0708  deferred=D-2020..D-2023
2026-05-02  render_lighting-20260502-2221  analysis  bucket=render_lighting  rvas=0  slot=Mashed_pool6  LIGHT_SETUP_FN=not-found  halt=RpLight-anchors-absent  nodeD3D9SubmitNoLight.csl=0x00618598  startlights.dff-loader=0x0041d8b0  D-2200..D-2201 filed  U-0767 filed
2026-05-03  sweep-20260503-0553  scribe-claim-sweep  buckets=8+1skipped (render_lighting HALT, no RVAs)
2026-05-03  fixup-inline  function_create  rvas=2  (0x004af31a, 0x00499820 — FPO; defined inline before drain)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=exception_filter  rvas=3  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=exception_filter  writes=3  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=memory_pool  rvas=2  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=memory_pool  writes=2  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=localization  rvas=4  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=localization  writes=4  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=window_msgpump  rvas=5  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=window_msgpump  writes=5  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=timer  rvas=11  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=timer  writes=11  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=settings_config  rvas=7  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=settings_config  writes=7  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=render_d3d_reset  rvas=7  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=render_d3d_reset  writes=7  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-claim  bucket=intro_splash  rvas=13  (sweep)
2026-05-03  sweep-20260503-0553  scribe-release  bucket=intro_splash  writes=13  errors=0  (sweep)
2026-05-03  sweep-20260503-0553  scribe-skip  bucket=render_lighting  reason=HALT=RpLight-anchors-absent  rvas=none  (sweep — moved to Drained without writes)
2026-05-03  sweep-20260503-0553  scribe-release-sweep  buckets=8+1skipped  total_writes=52  errors=0  (queue empty; +2 inline function_create)
2026-05-03  effects_particle_d2-20260503  session-complete  bucket=effects_particle_d2  rvas=4  (00534870 00535700 00535910 00538c80)  stubs-cleared=4  (S-0595 S-0596 S-0597 S-0598)  deferred-dropped=4  (D-1660..D-1663)  deferred-added=4  (D-2800..D-2803)  uncertainties-added=7  (U-0967..U-0973)  slot=Mashed_pool12  anchor=ok
2026-05-03  random_rng-20260503-0601  00534920  FUN_00534920  C0->C1  util; RNG plugin registrar; re/analysis/random_rng/0x00534920.md
2026-05-03  random_rng-20260503-0601  004b44f0  FUN_004b44f0  C0->C1  util; RandomInt(lo,hi) via FUN_00534870; re/analysis/random_rng/0x004b44f0.md
2026-05-03  random_rng-20260503-0601  004b4510  FUN_004b4510  C0->C1  util; RandomFloat(lo,hi) __thiscall via FUN_00534870; re/analysis/random_rng/0x004b4510.md
2026-05-03  game_state_d2-20260503  session-complete  bucket=game_state_d2  rvas=11  (0x0042c280 0x0042c2d0 0x0042c2e0 0x0042c2f0 0x0042f500 0x0042f6a0 0x00432080 0x004331a0 0x00448700 0x004927c0 0x005c9d00)  stubs-cleared=11  (S-0480..S-0490)  stubs-added=5  (S-1000..S-1004)  deferred-dropped=1  (D-1360)  deferred-added=5  (D-2920..D-2924)  uncertainties-added=8  (U-1007..U-1014)  slot=Mashed_pool6  anchor=ok
2026-05-03  track_loader_d2-20260503-0302  batch  C0->C1  16 functions newly mapped from D-1180 (track_loader-cont1 bucket): 00426cd0 0042a8d0 0042f510 00462950 004715a0 00478660 00479330 0047a0f0 0047c0b0 0047c0f0 00480340 00491780 004924c0 00495280 004952f0 004c1b10; D-1180 drained; S-0900..S-0919 filed; U-0907..U-0912 filed; D-2620..D-2645 depth-3 deferred
2026-05-03  hud_frontend_d2-20260503-0559  batch  C0->C1  14 functions cleared from D-1240..D-1253 (hud_frontend-cont1 bucket): 004335f0 0043a610 0042f0c0 0043af10 00434720 00430b90 00431240 004314b0 00431710 0043aa30 0042fb70 0042fe90 00430120 00439210; D-1240..D-1253 cleared; D-2740..D-2782 depth-3 deferred (43 callees); slot=Mashed_pool15; anchor=ok
2026-05-03  0x004671c0  GetOverlayCamera  C0->C1  render_frame-20260503-0611: trivial getter returns DAT_006905b4; paired with GetCamera(0x004671a0); used as overlay camera in FUN_00492e90
2026-05-03  sweep-20260503-0649  scribe-claim-sweep  buckets=3
2026-05-03  sweep-20260503-0649  scribe-claim  bucket=render_frame  rvas=1  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release  bucket=render_frame  writes=1  errors=0  (sweep)
2026-05-03  sweep-20260503-0649  scribe-claim  bucket=hud_ingame_d2  rvas=14  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release  bucket=hud_ingame_d2  writes=14  errors=0  (sweep)
2026-05-03  sweep-20260503-0649  scribe-claim  bucket=track_loader_d2  rvas=16  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release  bucket=track_loader_d2  writes=16  errors=0  (sweep)
2026-05-03  sweep-20260503-0649  scribe-release-sweep  buckets=3  total_writes=31  errors=0  (queue empty)
2026-05-03  exception_filter_d2-20260503  session-complete  bucket=exception_filter_d2  rvas=0  deferred-kept=1  (D-1960 pickup-condition-unmet: CRT reimplementation not required)  slot=Mashed(master,read-only;pool1-lock-stale)  anchor=ok  note=early-finish/cap_count=0
2026-05-03  0042a470  FUN_0042a470  C0->C1  re/analysis/texture_loader_d2/0x0042a470.md  strings:ps2/xbox/pc/gamecube/  caller:0042a530  resolves:S-0600 D-1720  session:texture_loader_d2-20260503-0350
2026-05-03  00496400  sub_00496400  C1(existing)->C1  already-mapped-save  resolves:S-0601 D-1721  session:texture_loader_d2-20260503-0350
2026-05-03  004cc230  FUN_004cc230  C1(existing)->C1  already-mapped-frontend  resolves:S-0421 D-1724  session:texture_loader_d2-20260503-0350
2026-05-03  004cc160  FUN_004cc160  C1(existing)->C1  already-mapped-frontend  resolves:S-0424 D-1726  session:texture_loader_d2-20260503-0350
2026-05-03  004cc5e0  FUN_004cc5e0  C0->C2  re/analysis/texture_loader_d2/0x004cc5e0.md  RwStreamFindChunk pattern; version-range [0x35000..0x37002]  resolves:S-0422 D-1725  new:U-1274 U-1275  session:texture_loader_d2-20260503-0350
2026-05-03  004cf7d0  FUN_004cf7d0  C0->C2  re/analysis/texture_loader_d2/0x004cf7d0.md  RwTexDictionaryStreamRead pattern; struct+dict+loop+AddTexture  resolves:S-0602 D-1722  new:U-1267 U-1268 U-1269  session:texture_loader_d2-20260503-0350
2026-05-03  0054f8d0  FUN_0054f8d0  C0->C1  re/analysis/texture_loader_d2/0x0054f8d0.md  native-texture-bank-reader; corrects prior DFF/clump label  resolves:S-0603 D-1723  new:U-1270 U-1271 U-1272 U-1273  session:texture_loader_d2-20260503-0350
2026-05-03  004b3d80  FUN_004b3d80  C1->C1(notes-correction)  corrected 'DFF/clump' to 'native-texture-bank'; S-0603 resolved  session:texture_loader_d2-20260503-0350
2026-05-03  sweep-20260503-0725  scribe-claim-sweep  buckets=2  (race_results has 3 protected RVAs per row note)
2026-05-03  sweep-20260503-0725  scribe-claim  bucket=race_results  rvas=14  (sweep)
2026-05-03  sweep-20260503-0725  scribe-release  bucket=race_results  writes=11  bookmarks=14  errors=0  skipped_plates=3 (0x0040b6d0,0x0042f500,0x0042f6a0 prior C1 preserved per row note)
2026-05-03  sweep-20260503-0725  scribe-claim  bucket=timer_d2  rvas=13  (sweep)
2026-05-03  sweep-20260503-0725  scribe-release  bucket=timer_d2  writes=13  errors=0  (sweep)
2026-05-03  sweep-20260503-0725  scribe-release-sweep  buckets=2  total_writes=24  bookmarks=27  errors=0  skipped_plates=3  (queue empty)
2026-05-03  0049ec10  FUN_0049ec10  C0->C1  re/analysis/video_mci_d2/0x0049ec10.md  __thiscall ctor; vtable[0]+7 slots; calls FUN_0049dd60+FUN_0049cfb0; min obj 0x128b  resolves:S-0375 D-1000  new:S-1380 U-1387  session:video_mci_d2-20260503
2026-05-03  004a3b84  FUN_004a3b84  C0->C1  re/analysis/video_mci_d2/0x004a3b84.md  vsnprintf-impl via fake FILE (_flag=0x42); FUN_004a504f core  resolves:S-0376 D-1001  session:video_mci_d2-20260503
2026-05-03  00493ac0  LAB_00493ac0  C0->C1  re/analysis/video_mci_d2/0x00493ac0.md  pre-NT5 code-page: GetThreadLocale+GetLocaleInfoA(0x1004)+atoi; fallback GetACP  resolves:S-0373 D-1002  session:video_mci_d2-20260503
2026-05-03  00493b40  LAB_00493b40  C0->C1  re/analysis/video_mci_d2/0x00493ac0.md  NT5+ code-page: MOV EAX,3 (CP_THREAD_ACP); RET  resolves:S-0374  session:video_mci_d2-20260503
2026-05-03  00408af0  FUN_00408af0  C0->C1  re/analysis/ai_update_d2/0x00408af0.md  heading/vel float3 getter (+0x9c stride-0x30c)  resolves:D-1120  session:ai_update_d2-20260503-1322
2026-05-03  00414030  FUN_00414030  C0->C1  re/analysis/ai_update_d2/0x00414030.md  AI spline-bank timer reset  resolves:D-1140 S-0403  session:ai_update_d2-20260503-1322
2026-05-03  00414570  FUN_00414570  C0->C1  re/analysis/ai_update_d2/0x00414570.md  ahead-in-race targeting (progress diff+angle)  resolves:D-1121  new:D-4180..4199 U-1427..1436  session:ai_update_d2-20260503-1322
2026-05-03  004148b0  FUN_004148b0  C0->C1  re/analysis/ai_update_d2/0x004148b0.md  leader-ranking AI timer  resolves:D-1122  session:ai_update_d2-20260503-1322
2026-05-03  00414a70  FUN_00414a70  C0->C1  re/analysis/ai_update_d2/0x00414a70.md  closest-vehicle targeting (returns 1/2)  resolves:D-1123  session:ai_update_d2-20260503-1322
2026-05-03  00414c30  FUN_00414c30  C0->C1  re/analysis/ai_update_d2/0x00414c30.md  obstacle-avoidance targeting  resolves:D-1124  session:ai_update_d2-20260503-1322
2026-05-03  00414f00  FUN_00414f00  C0->C1  re/analysis/ai_update_d2/0x00414f00.md  powerup-seek targeting  resolves:D-1125  session:ai_update_d2-20260503-1322
2026-05-03  00415020  FUN_00415020  C0->C1  re/analysis/ai_update_d2/0x00415020.md  frustration timer (72000-frame mode-5 gate)  resolves:D-1126  session:ai_update_d2-20260503-1322
2026-05-03  004150e0  FUN_004150e0  C0->C1  re/analysis/ai_update_d2/0x004150e0.md  track lateral-zone query (tile grid)  resolves:D-1127  new:U-1427  session:ai_update_d2-20260503-1322
2026-05-03  00415220  FUN_00415220  C0->C1  re/analysis/ai_update_d2/0x00415220.md  AI powerup-activation (13-case switch)  resolves:D-1128  new:U-1429 U-1430  session:ai_update_d2-20260503-1322
2026-05-03  00415880  FUN_00415880  C0->C1  re/analysis/ai_update_d2/0x00415880.md  ram-from-behind targeting (latch)  resolves:D-1129  session:ai_update_d2-20260503-1322
2026-05-03  00415d00  FUN_00415d00  C0->C1  re/analysis/ai_update_d2/0x00415d00.md  wall-ahead trajectory check  resolves:D-1130  session:ai_update_d2-20260503-1322
2026-05-03  00415e20  FUN_00415e20  C0->C1  re/analysis/ai_update_d2/0x00415e20.md  steering angle calculator (ST0 return)  resolves:D-1131  new:U-1428  session:ai_update_d2-20260503-1322
2026-05-03  00416060  FUN_00416060  C0->C1  re/analysis/ai_update_d2/0x00416060.md  line-of-sight check (ray-march)  resolves:D-1132  session:ai_update_d2-20260503-1322
2026-05-03  004161e0  FUN_004161e0  C0->C1  re/analysis/ai_update_d2/0x004161e0.md  spline target-point init  resolves:D-1133  session:ai_update_d2-20260503-1322
2026-05-03  00417cf0  FUN_00417cf0  C0->C1  re/analysis/ai_update_d2/0x00417cf0.md  angle-gated targeting mode-8 variant  resolves:D-1147 S-0410  session:ai_update_d2-20260503-1322
2026-05-03  00443080  FUN_00443080  C0->C1  re/analysis/ai_update_d2/0x00443080.md  mode-6 gate flag getter (DAT_00897ffc)  resolves:D-1134  new:U-1432  session:ai_update_d2-20260503-1322
2026-05-03  00443440  FUN_00443440  C0->C1  re/analysis/ai_update_d2/0x00443440.md  spline progress+curvature calculator  resolves:D-1135  session:ai_update_d2-20260503-1322
2026-05-03  0046d4a0  FUN_0046d4a0  C0->C1  re/analysis/ai_update_d2/0x0046d4a0.md  vehicle struct pointer (base 0x881ec8 stride 0x341)  resolves:D-1136  new:U-1431  session:ai_update_d2-20260503-1322
2026-05-03  0046d510  FUN_0046d510  C0->C1  re/analysis/ai_update_d2/0x0046d510.md  vehicle velocity getter (matrix-transformed)  resolves:D-1137  session:ai_update_d2-20260503-1322
2026-05-03  0046d570  FUN_0046d570  C0->C1  re/analysis/ai_update_d2/0x0046d570.md  vehicle forward-angle projection  resolves:D-1145 S-0408  session:ai_update_d2-20260503-1322
2026-05-03  0046d6a0  FUN_0046d6a0  C0->C1  re/analysis/ai_update_d2/0x0046d6a0.md  physics scalar getter (base 0x8820ac stride 0xd04)  resolves:D-1138  new:U-1434  session:ai_update_d2-20260503-1322
2026-05-03  0046d6d0  FUN_0046d6d0  C0->C1  re/analysis/ai_update_d2/0x0046d6d0.md  vehicle spline-progress rate (+0xbc stride 0x341)  resolves:D-1139  session:ai_update_d2-20260503-1322
2026-05-03  00452160  FUN_00452160  C0->C1  re/analysis/ai_update_d2/0x00452160.md  powerup target position getter  resolves:D-1142 S-0405  session:ai_update_d2-20260503-1322
2026-05-03  00452ea0  FUN_00452ea0  C0->C1  re/analysis/ai_update_d2/0x00452ea0.md  per-vehicle powerup-active flag  resolves:D-1143 S-0406  session:ai_update_d2-20260503-1322
2026-05-03  00452eb0  FUN_00452eb0  C0->C1  re/analysis/ai_update_d2/0x00452eb0.md  powerup pursuit range getter  resolves:D-1144 S-0407  session:ai_update_d2-20260503-1322
2026-05-03  00472650  FUN_00472650  C0->C1  re/analysis/ai_update_d2/0x00472650.md  random float [min,max) via PRNG FUN_00534870  resolves:D-1141 S-0404  session:ai_update_d2-20260503-1322
2026-05-03  004c3ac0  FUN_004c3ac0  C0->C1  re/analysis/ai_update_d2/0x004c3ac0.md  fast 3-vector magnitude (two-level sqrt table)  resolves:D-1146 S-0409  session:ai_update_d2-20260503-1322
2026-05-03  005ba720  FUN_005ba720  C0->C1  re/analysis/audio_dsound_d2/0x005ba720.md  COM init guard CoInitialize/CoUninitialize  resolves:D-0945 S-0346  session:audio_dsound_d2-20260503-1735
2026-05-03  005ba760  FUN_005ba760  C0->C1  re/analysis/audio_dsound_d2/0x005ba760.md  COM cleanup FUN_005bc880+CoUninitialize jumptable-warn  resolves:D-0947 S-0348  session:audio_dsound_d2-20260503-1735
2026-05-03  005ba780  LAB_005ba780  C0->C1  re/analysis/audio_dsound_d2/0x005ba780.md  fn-ptr struct+0x34 no-Ghidra-fn 0x6f-bytes 3-vtable+0x8  resolves:D-0943 S-0342  session:audio_dsound_d2-20260503-1735
2026-05-03  005ba7f0  LAB_005ba7f0  C0->C1  re/analysis/audio_dsound_d2/0x005ba7f0.md  fn-ptr struct+0x30 no-Ghidra-fn 0x24f-bytes flags-dispatch FPU switch-table  resolves:D-0944 S-0343  session:audio_dsound_d2-20260503-1735
2026-05-03  005bac00  FUN_005bac00  C0->C1  re/analysis/audio_dsound_d2/0x005bac00.md  3x vtable+0x14 6-unreachable-blocks  resolves:D-0950 S-0351  session:audio_dsound_d2-20260503-1735
2026-05-03  005bb000  FUN_005bb000  C0->C1  re/analysis/audio_dsound_d2/0x005bb000.md  DSoundBuffer init/teardown WaitForSingleObject  resolves:D-0946 S-0347  session:audio_dsound_d2-20260503-1735
2026-05-03  005bbc10  FUN_005bbc10  C0->C1  re/analysis/audio_dsound_d2/0x005bbc10.md  format/caps query vtable+0x38-retry vtable+0x14  resolves:D-0948 S-0349  session:audio_dsound_d2-20260503-1735
2026-05-03  005bbdb0  FUN_005bbdb0  C0->C1  re/analysis/audio_dsound_d2/0x005bbdb0.md  CreateSoundBuffer-wrapper vtable+0xc flags-0x88/0x8088  resolves:D-0949 S-0350  session:audio_dsound_d2-20260503-1735
2026-05-03  005bbf30  FUN_005bbf30  C0->C1  re/analysis/audio_dsound_d2/0x005bbf30.md  COM-Release-x3 vtable+0x8 on param_1[0..2]  resolves:D-0951 S-0352  session:audio_dsound_d2-20260503-1735
2026-05-03  audio_dsound_d2-20260503-1735  OVERLAP  D-0941 0x005adfe0 + D-0942 0x005ae010 already at C1 in audio_rws_loader_d2; not re-decompiled  session:audio_dsound_d2-20260503-1735
2026-05-03  00403050  FUN_00403050  C0->C1  re/analysis/loading_screen/0x00403050.md  pre-race loading screen renderer; state-1 sub-state-0x21; sprite 0x2A4 pulsing spinner  resolves:D-0891  session:loading_screen-20260503
2026-05-03  0040ab40  FUN_0040ab40  C0->C1  re/analysis/timer_d2_cont1/0x0040ab40.md  state-machine dispatcher 6-var cluster 0x008a9584..9598  session:timer_d2_cont1-20260503-1824
2026-05-03  0040ac80  FUN_0040ac80  C0->C1  re/analysis/timer_d2_cont1/0x0040ac80.md  second dispatcher same 6-var cluster non-contiguous  session:timer_d2_cont1-20260503-1824
2026-05-03  0040b810  FUN_0040b810  C0->C1  re/analysis/timer_d2_cont1/0x0040b810.md  zeroes 21 globals 0x008a94f0/9530/9540+0x0063b8ec  session:timer_d2_cont1-20260503-1824
2026-05-03  0040de10  FUN_0040de10  C0->C1  re/analysis/timer_d2_cont1/0x0040de10.md  calls S-1132+Replay::StartLap+S-1603 writes 0x0063ba8c=1  session:timer_d2_cont1-20260503-1824
2026-05-03  0040e360  FUN_0040e360  C0->C1  re/analysis/timer_d2_cont1/0x0040e360.md  setter DAT_0063ba8c=param_1  session:timer_d2_cont1-20260503-1824
2026-05-03  0040e370  FUN_0040e370  C0->C1  re/analysis/timer_d2_cont1/0x0040e370.md  bool getter PTR_PTR_005f2770+param_1*4+0x34  session:timer_d2_cont1-20260503-1824
2026-05-03  00422b30  FUN_00422b30  C0->C1  re/analysis/timer_d2_cont1/0x00422b30.md  memset 0x138 dwords at 0x00899e80  session:timer_d2_cont1-20260503-1824
2026-05-03  00429aa0  FUN_00429aa0  C0->C1  re/analysis/timer_d2_cont1/0x00429aa0.md  two-path table-fill 0x0067d990/998/9a0  session:timer_d2_cont1-20260503-1824
2026-05-03  0042af50  FUN_0042af50  C0->C1  re/analysis/timer_d2_cont1/0x0042af50.md  three-path char-array walk or int-index walk  session:timer_d2_cont1-20260503-1824
2026-05-03  0042b900  FUN_0042b900  C0->C1  re/analysis/timer_d2_cont1/0x0042b900.md  getter DAT_0067eca4  session:timer_d2_cont1-20260503-1824
2026-05-03  0042b950  FUN_0042b950  C0->C1  re/analysis/timer_d2_cont1/0x0042b950.md  setter DAT_007f1a0c=0x1000  session:timer_d2_cont1-20260503-1824
2026-05-03  0042c150  FUN_0042c150  C0->C1  re/analysis/timer_d2_cont1/0x0042c150.md  counts nonzero ints 0x0067ea10..3f; if any: 0x0067eab4=0xff  session:timer_d2_cont1-20260503-1824
2026-05-03  00431b50  FUN_00431b50  C0->C1  re/analysis/timer_d2_cont1/0x00431b50.md  fsin(DAT_007f0f04*_DAT_005cd8f0) float10  session:timer_d2_cont1-20260503-1824
2026-05-03  00431b60  FUN_00431b60  C0->C1  re/analysis/timer_d2_cont1/0x00431b60.md  fsin(DAT_007f0f08*_DAT_005cd8f0) float10 sibling+4  session:timer_d2_cont1-20260503-1824
2026-05-03  00432290  FUN_00432290  C0->C1  re/analysis/timer_d2_cont1/0x00432290.md  predicate DAT_0067eab0!=0 && DAT_0067eabc in {FF210000,FF220000}  session:timer_d2_cont1-20260503-1824
2026-05-03  0045c480  FUN_0045c480  C0->C1  re/analysis/timer_d2_cont1/0x0045c480.md  zero-init scatter globals + 0x24-dword block 0x0088f5e0  session:timer_d2_cont1-20260503-1824
2026-05-03  0045d3a0  FUN_0045d3a0  C0->C1  re/analysis/timer_d2_cont1/0x0045d3a0.md  gated tick: loop FUN_0045d1e0 0..DAT_008aa254; FUN_004657b0; store  session:timer_d2_cont1-20260503-1824
2026-05-03  0045d7a0  FUN_0045d7a0  C0->C1  re/analysis/timer_d2_cont1/0x0045d7a0.md  701b position-lerp+cross-product writes to iVar6 transform obj  session:timer_d2_cont1-20260503-1824
2026-05-03  timer_d2_cont1-20260503-1824  EARLY-FINISH  cap_count=18 at RVA 18/39; 21 RVAs filed D-4720 (timer_d2_cont2); D-3282 re-deferred as D-4721 (Opus-only); slot=Mashed_pool5  session:timer_d2_cont1-20260503-1824
2026-05-03  00426670  sub_00426670  C0->C1  re/analysis/render_frame_d3/00426670.md  WorldRenderDispatch_Begin; D-0880 resolved; D-5020 D-5021 spawned  session:render_frame_d3-20260503
2026-05-03  0040de30  sub_0040de30  C0->C1  re/analysis/render_frame_d3/0040de30.md  MinimapCameraOrthoSetup; D-0881 resolved  session:render_frame_d3-20260503
2026-05-03  0040df20  sub_0040df20  C0->C1  re/analysis/render_frame_d3/0040df20.md  MinimapCameraRestore; D-0882 resolved  session:render_frame_d3-20260503
2026-05-03  0040df60  sub_0040df60  C0->C1  re/analysis/render_frame_d3/0040df60.md  ConditionalRenderSubPass; D-0883 resolved; D-5024 spawned  session:render_frame_d3-20260503
2026-05-03  00404320  sub_00404320  C0->C1  re/analysis/render_frame_d3/00404320.md  PerModeRenderMachine; D-0884 resolved; D-5025..D-5029 spawned  session:render_frame_d3-20260503
2026-05-03  00410b30  sub_00410b30  C0->C1  re/analysis/render_frame_d3/00410b30.md  InGameRenderDispatcher; D-0885 resolved; D-5037..D-5060 spawned (25 callees)  session:render_frame_d3-20260503
2026-05-03  00426030  sub_00426030  C0->C1  re/analysis/render_frame_d3/00426030.md  WorldRenderPrePass; D-0886 resolved; D-5022 D-5023 spawned  session:render_frame_d3-20260503
2026-05-03  004266b0  sub_004266b0  C0->C1  re/analysis/render_frame_d3/004266b0.md  WorldRenderDispatch_End; D-0887 resolved  session:render_frame_d3-20260503
2026-05-03  00492440  sub_00492440  C0->C1  re/analysis/render_frame_d3/00492440.md  RenderStatsAccumulate (leaf/mapped); D-0888 resolved  session:render_frame_d3-20260503
2026-05-03  00492e60  sub_00492e60  C0->C1  re/analysis/render_frame_d3/00492e60.md  SetDefaultViewWindow; D-0889 resolved  session:render_frame_d3-20260503
2026-05-03  00433f40  sub_00433f40  C0->C1  re/analysis/render_frame_d3/00433f40.md  RaceEndFadeOverlay; D-0890 resolved; D-5030..D-5034 spawned  session:render_frame_d3-20260503
2026-05-03  0042d390  GetRaceStateField  C0->C1  re/analysis/render_frame_d3/0042d390.md  trivial getter DAT_0067ea6c; D-0892 resolved  session:render_frame_d3-20260503
2026-05-03  0042f530  sub_0042f530  C0->C1  re/analysis/render_frame_d3/0042f530.md  ViewportSetup; D-0893 resolved; D-5035 spawned  session:render_frame_d3-20260503
2026-05-03  0042a9f0  GetFadeAlpha  C0->C1  re/analysis/render_frame_d3/0042a9f0.md  trivial getter (byte)DAT_0067eca8; D-0894 resolved  session:render_frame_d3-20260503
2026-05-03  render_frame_d3-20260503  BATCH  14 C0→C1 promotions; 41 new D-5020..D-5060; 10 U-1707..U-1716; 13 S-1700..S-1712; slot=Mashed_pool13 (pool5 orphan-locked)  session:render_frame_d3-20260503
2026-05-03  sweep-20260503-1853  scribe-claim-sweep  buckets=1  (replay_record skipped: no per-RVA files)
2026-05-03  sweep-20260503-1853  scribe-claim  bucket=game_mode  rvas=1  (sweep)
2026-05-03  sweep-20260503-1853  scribe-release  bucket=game_mode  writes=0_plates,1_bookmarks  errors=0  (sweep; plate-skipped:C0)
2026-05-03  sweep-20260503-1853  scribe-release-sweep  buckets=1  total_writes=0_plates,1_bookmarks  errors=0
2026-05-05  00420050  FUN_00420050->PerPlayerViewportRender  C0->C1  re/analysis/split_screen/00420050.md  per-player render; stride 0x2AC; player index 0..3; D-5038 cleared  session:split_screen-20260505
2026-05-05  0042f660  DefaultViewportCameraInit  C0->C1  re/analysis/split_screen/0042f660.md  creates global camera DAT_0067f190; near=0.1f far=180.0f; calls ViewportSetup  session:split_screen-20260505
2026-05-05  0041faf0  VehicleShadowRender  C0->C1  re/analysis/split_screen/0041faf0.md  vehicle shadow billboard; guarded +0x294 bit6; per-player from PerPlayerViewportRender  session:split_screen-20260505
2026-05-05  0041fcc0  TireMarkRender  C0->C1  re/analysis/split_screen/0041fcc0.md  tire-mark decal; wheel transform via +0x26c; colour table DAT_005f5fe8/6088  session:split_screen-20260505
2026-05-05  0042c960  CameraTransitionStateMachine  C0->C1  re/analysis/split_screen/0042c960.md  camera-transition slider DAT_0067ed68; S-1642 cleared; U-1716 resolved  session:split_screen-20260505
2026-05-05  split_screen-20260505  BATCH  5 C0->C1 promotions; 5 new D-5620..D-5624; 6 new U-1907..U-1912; 1 S-1642 cleared; U-1716 resolved; FOUND: split-screen IS present (4-player loop in InGameRenderDispatcher); SPLIT_FN=sub_00410b30; strategy=render_frame_call_graph  session:split_screen-20260505
2026-05-05  0040e180  FUN_0040e180  C0->C1  re/analysis/vehicle_damage_d2/0x0040e180.md  432b collision-pair finder; nested loops cars 0-3; max-distance pair via FUN_004c3ac0; resolves D-1548  session:vehicle_damage_d2-20260505
2026-05-05  00410d10  FUN_00410d10  C0->C1  re/analysis/vehicle_damage_d2/0x00410d10.md  1080b DAMAGE_FN; per-step per-mode collision dispatcher; switch DAT_007f0fd0 cases 4/5/7/8/9/10; sentinel 10.0f; writes DAT_00803320  session:vehicle_damage_d2-20260505
2026-05-05  00442df0  FUN_00442df0  C0->C1  re/analysis/vehicle_damage_d2/0x00442df0.md  6b stub; returns DAT_00898980 as float10; collision impact reader  session:vehicle_damage_d2-20260505
2026-05-05  0046cbb0  FUN_0046cbb0  C0->C1  re/analysis/vehicle_damage_d2/0x0046cbb0.md  47b; per-car state reader (car,&state_out,&extra_out); base DAT_00881f90 stride 0xd04; state 0=alive 2=slide  session:vehicle_damage_d2-20260505
2026-05-05  004922e0  FUN_004922e0  C0->C1  re/analysis/vehicle_damage_d2/0x004922e0.md  81b hit-sound/particle trigger; car→player via DAT_007f1a14; writes 4 fields to DAT_007f1058 stride 0x4c  session:vehicle_damage_d2-20260505
2026-05-05  vehicle_damage_d2-20260505  BATCH  5 C0->C1 promotions; 3 new S-1840..S-1842; 14 new U-1847..U-1860; 7 new D-5440..D-5446; 1 D-1548 cleared; slot=Mashed_pool2 (pool0/pool1 locked at JVM level)  session:vehicle_damage_d2-20260505
2026-05-06  sweep-20260506-0458  scribe-claim-sweep  buckets=1  rvas=5
2026-05-06  sweep-20260506-0458  scribe-claim  bucket=vehicle_damage_d2  rvas=5  (sweep)
2026-05-06  sweep-20260506-0458  scribe-release  bucket=vehicle_damage_d2  writes=5_plates,5_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-0458  scribe-release-sweep  buckets=1  total_writes=5_plates,5_bookmarks  errors=0
2026-05-06  track_loader_d3-20260506  BATCH  27 C0->C1 promotions; cleared S-0900 S-0904..S-0919 (17 stubs); cleared D-2620..D-2645 (26 deferred); 16 new U-1947..U-1962; 16 new D-5740..D-5755; bucket track_loader_d3-cont1; slot=Mashed_pool0; corrections: S-0909 was float3 eq-check (not in-triangle), S-0910 was cross-product (not flag-setter)  session:track_loader_d3-20260506
2026-05-06  0049dd60  FUN_0049dd60  C0->C1  re/analysis/video_mci_d3/0x0049dd60.md  247b __thiscall ctor-base; vtable writes; 3x FUN_004a1160 on offsets 0x54/0x58/0x5c; 3x InitializeCriticalSection; SetEvent on this->0x5c; resolves D-4060 S-1380; new S-2000 S-2001 U-2007 D-5920 D-5921  session:video_mci_d3-20260506-0512
2026-05-06  powerups_d3-20260506-0504  BATCH  17 analyzed (16 new C1 rows; 0x004d8060 already C1); cleared D-4240..D-4243 S-1444; new S-1920..S-1930 (11); new U-1927..U-1945 (19); new D-5680..D-5686 (7); slot=Mashed_pool4; cap_count=17  session:powerups_d3-20260506-0504
2026-05-06  hud_frontend_d3-20260506-0511  BATCH  18 C0->C1 promotions (0040ad20 0040b460 0040b620 0040b6b0 0040b6c0 0040b7a0 0040b7b0 0040bb70 0040bb90 0040e3a0 00427ad0 004282a0 00428320 00429870 00429a30 00429a70 00429a80 00429a90); cleared D-2740..D-2782 (43 deferred, 18 analyzed + 25 re-filed D-6160..D-6184); new S-2087 S-2089 S-2090 (3 new stubs; 9 others already tracked); new U-2087..U-2095 (9); new D-6160..D-6184 (25, hud_frontend_d3-cont1); early-finish at cap=18; slot=Mashed_pool12  session:hud_frontend_d3-20260506-0511
2026-05-06  sweep-20260506-0544  scribe-claim-sweep  buckets=2  rvas=19  (launch_handshake VAs corrected from bare offsets)
2026-05-06  sweep-20260506-0544  scribe-claim  bucket=launch_handshake  rvas=2  (sweep; VAs corrected from bare offsets)
2026-05-06  sweep-20260506-0544  scribe-release  bucket=launch_handshake  writes=2_plates,2_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-0544  scribe-claim  bucket=powerups_d3  rvas=17  (sweep)
2026-05-06  sweep-20260506-0544  scribe-release  bucket=powerups_d3  writes=17_plates,17_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-0544  scribe-release-sweep  buckets=2  total_writes=19_plates,19_bookmarks  errors=0
2026-05-06  vehicle_damage_d3-20260506-1244  BATCH  7 C1 rows (00405890 00408a50 00408a70 0040e340 0040e350 00417730 00423b20); cleared D-5440..D-5446 (7 deferred); resolved S-1842; new U-2167..U-2176 (10); slot=Mashed_pool0; cap_count=7; early-finish=no  session:vehicle_damage_d3-20260506-1244
2026-05-06  00429310  TimeTrial::Tick  C0->C1  string xref "time trial time is %f"; leaderboard-20260506-JJJJJ; re/analysis/leaderboard/0x00429310.md; S-2220 U-2228 filed
2026-05-06  0040d270  Course::Finish  C0->C1  xref from FUN_0040d440 row 311; tail=Replay::CreateOrLoad; leaderboard-20260506-JJJJJ; re/analysis/leaderboard/0x0040d270.md; S-2225 S-2226 U-2229 filed
2026-05-06  sweep-20260506-1326  scribe-claim-sweep  buckets=2  rvas=13
2026-05-06  sweep-20260506-1326  scribe-claim  bucket=profile_career_d3  rvas=6  (sweep)
2026-05-06  sweep-20260506-1326  scribe-release  bucket=profile_career_d3  writes=6_plates,6_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-1326  scribe-claim  bucket=vehicle_damage_d3  rvas=7  (sweep)
2026-05-06  sweep-20260506-1326  scribe-release  bucket=vehicle_damage_d3  writes=7_plates,7_bookmarks  errors=0  (sweep; 0x00408a50/0x00408a70 overwrite prior race_results plates)
2026-05-06  sweep-20260506-1326  scribe-release-sweep  buckets=2  total_writes=13_plates,13_bookmarks  errors=0
2026-05-06  save_gamesave_d2-20260506-1508  analysis  bucket=save_gamesave_d2  rvas=5  new=2(004cbe80,00550b00)  xref=3(004cbd30,004cc160,004cc230)  stubs_cleared=S-0280..S-0284  stubs_added=S-2320  U_added=U-2327..U-2332  D_added=D-6880  slot=Mashed_pool8

2026-05-06  0x00431d90  FUN_00431d90  C0->C1  options_menu-20260506; panel-flag mass-clear; plate re/analysis/options_menu/0x00431d90.md
2026-05-06  0x00431f30  FUN_00431f30  C0->C1  options_menu-20260506; page-ID dispatch switch; param_1=10=options page; plate re/analysis/options_menu/0x00431f30.md
2026-05-06  0x0043df00  FUN_0043df00  C0->C1  options_menu-20260506; frontend game-session initializer; S-2380 U-2387..U-2389; plate re/analysis/options_menu/0x0043df00.md2026-05-06  sweep-20260506-1624  scribe-claim-sweep  buckets=3  rvas=11  (replay_record skipped: still HOLD)
2026-05-06  sweep-20260506-1624  scribe-claim  bucket=options_menu  rvas=3  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release  bucket=options_menu  writes=3_plates,3_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-1624  scribe-claim  bucket=memory_pool_d2  rvas=1  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release  bucket=memory_pool_d2  writes=1_plate,1_bookmark  errors=0  (sweep; _longjmp already named via library match)
2026-05-06  sweep-20260506-1624  scribe-claim  bucket=intro_splash_d2  rvas=7  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release  bucket=intro_splash_d2  writes=7_plates,7_bookmarks  errors=0  (sweep)
2026-05-06  sweep-20260506-1624  scribe-release-sweep  buckets=3  total_writes=11_plates,11_bookmarks  errors=0
2026-05-06  0047b8a0  FUN_0047b8a0  C0->C1  structural read; input_lua_d2-20260506-1854; Lua exec wrapper; FUN_004b7a70(DAT_006bf1e0,p1,p2,0); cond write to *p3; returns iVar1==0; S-2400 U-2407 (re/analysis/input_lua_d2/0x0047b8a0.md)
2026-05-06  004b7330  FUN_004b7330  C0->C1  structural read; input_lua_d2-20260506-1854; alloc 0x70 block; zero-init 16 fields; sentinels +0x4c/+0x58=0xffffffff +0x5c=0x7ffffffd +0x60=0x70 +0x6c=1; setup via FUN_004b7be0; fail→FUN_004b7480; +0x5c=+0x60*2; S-2401 S-2402 U-2408..U-2411 (re/analysis/input_lua_d2/0x004b7330.md)
2026-05-06  004c0510  FUN_004c0510  C0->C1  structural read; input_lua_d2-20260506-1854; 5-call sequential chain into param_1; FUN_004c06c0+thunk_FUN_004b7fd0+FUN_004b9730+FUN_004b7140(0x54442d18,0x400921fb)+FUN_004b7250; S-2403..S-2407 U-2412 U-2413 (re/analysis/input_lua_d2/0x004c0510.md)
2026-05-06  004b7480  FUN_004b7480  C0->C1  structural read; input_lua_d2-20260506-1854; teardown; FUN_004ba210(1)+FUN_004b9850; arith updates on +0x60 via 5x FUN_004ba1b0 pattern; S-2408 S-2409 U-2414..U-2417 (re/analysis/input_lua_d2/0x004b7480.md)
2026-05-06  004b6520  FUN_004b6520  C0->C1  structural read; input_lua_d2-20260506-1854; 20-byte wrapper; FUN_004b64e0(param_1,0,param_2); S-2410 U-2418 (re/analysis/input_lua_d2/0x004b6520.md)
2026-05-06  audio_sfx_dispatch-20260506  analysis  bucket=audio_sfx_dispatch  rvas=8  new=8(004669b0,0045d460,004627f0,004625b0,00462dd0,005a6710,0045e040,00466a50)  U_added=U-2487..U-2494  D_added=D-7360..D-7375(16)  slot=Mashed_pool4  cap_count=10  early-finish=no  session=audio_sfx_dispatch-20260506
2026-05-06  title_screen-20260506-VVVVV  analysis  bucket=title_screen  rvas=7  new=7(00428a30,00429240,00429290,00428bf0,00428d30,00427c90,00428390)  stubs_added=S-2460..S-2466  U_added=U-2467..U-2470  D_added=D-7300..D-7306  slot=Mashed_pool15  cap_count=0  early-finish=no  session=title_screen-20260506-VVVVV
2026-05-06  0x00428a30  FUN_00428a30  C0->C1  title_screen-20260506-VVVVV; TITLE_FN; title screen renderer; string 0x222 at (580.0,140.0); attract timer 24M ticks→DAT_0067d960=1; S-2460 S-2461 S-2462 U-2467 U-2470; plate re/analysis/title_screen/0x00428a30.md
2026-05-06  0x00429240  FUN_00429240  C0->C1  title_screen-20260506-VVVVV; state dispatcher 50b; switch DAT_0067d960; plate re/analysis/title_screen/0x00429240.md
2026-05-06  0x00429290  FUN_00429290  C0->C1  title_screen-20260506-VVVVV; per-frame tick 39b; called ×3 from FUN_004669b0; plate re/analysis/title_screen/0x00429290.md
2026-05-06  0x00428bf0  FUN_00428bf0  C0->C1  title_screen-20260506-VVVVV; attract mode renderer 312b; S-2463 U-2469; plate re/analysis/title_screen/0x00428bf0.md
2026-05-06  0x00428d30  FUN_00428d30  C0->C1  title_screen-20260506-VVVVV; lobby renderer 1291b; S-2464 S-2465 S-2466; plate re/analysis/title_screen/0x00428d30.md
2026-05-06  0x00427c90  FUN_00427c90  C0->C1  title_screen-20260506-VVVVV; assets-ready getter 5b; returns DAT_0067d84c; plate re/analysis/title_screen/0x00427c90.md
2026-05-06  0x00428390  FUN_00428390  C0->C1  title_screen-20260506-VVVVV; state setter 9b; DAT_0067d960=param_1; plate re/analysis/title_screen/0x00428390.md
2026-05-06  audio_music_d3-20260506-1930  scribe-queued  bucket=audio_music_d3  rvas=2
2026-05-06  004288a0  FUN_004288a0  C0->C1  title_screen_d2-20260506; menu layout renderer; 8 elements (image+7 sprites); re/analysis/title_screen_d2/0x004288a0.md; S-2461 cleared
2026-05-06  00428450  FUN_00428450  stub-S-2460-cleared  title_screen_d2-20260506; already C1 (hud_ingame_d3); D-7300 resolved; U-2470 resolved (param_1=X_offset param_2=Y_offset)
2026-05-06  00428320  FUN_00428320  stub-S-2462-cleared  title_screen_d2-20260506; already C1 (hud_frontend_d3); D-7302 resolved
2026-05-06  0042e590  FUN_0042e590  C0->C1  title_screen_d2-20260506; 2-insn wrapper→FUN_0040bb70(C1)→FUN_004c5c00 string search; re/analysis/title_screen_d2/0x0042e590.md; U-2547 filed; S-2463 cleared; S-2540 new
2026-05-06  0040d250  FUN_0040d250  C0->C1  title_screen_d2-20260506; indexed ptr dereference getter 16b; re/analysis/title_screen_d2/0x0040d250.md; S-2464 cleared
2026-05-06  00401ee0  FUN_00401ee0  C0->C1  title_screen_d2-20260506; object-select+RW-matrix+RpClumpRender chain; re/analysis/title_screen_d2/0x00401ee0.md; U-2548 filed; S-2465 cleared; S-2541..S-2544 new
2026-05-06  0042f0b0  FUN_0042f0b0  C0->C1  title_screen_d2-20260506; int getter DAT_0067f17c+0x49; re/analysis/title_screen_d2/0x0042f0b0.md; S-2466 cleared
2026-05-06  004c5c00  FUN_004c5c00  C0->C1  title_screen_d2-20260506; case-insensitive linked-list string search 114b; re/analysis/title_screen_d2/0x0042e590.md (inline); S-2540
2026-05-06  00401570  FUN_00401570  C0->C1  title_screen_d2-20260506; table scan 36b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2541
2026-05-06  00401da0  FUN_00401da0  C0->C1  title_screen_d2-20260506; RW matrix setup+dirty 308b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2542 S-2543
2026-05-06  00426cf0  FUN_00426cf0  C0->C1  title_screen_d2-20260506; addr getter 5b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2544
2026-05-06  004c1480  FUN_004c1480  C0->C1  title_screen_d2-20260506; RW frame dirty-list insert 145b; re/analysis/title_screen_d2/0x00401ee0.md (inline); S-2543; D-7540 new (FUN_004c52f0)
2026-05-06  004b3c60  FUN_004b3c60  stub->C1  track_loader_d4-20260506-2007; BSP stream reader 81b; chunk 0xb; S-2480..S-2483 U-2487 U-2488; re/analysis/track_loader_d4/0x004b3c60.md
2026-05-06  00558df0  FUN_00558df0  stub->C1  track_loader_d4-20260506-2007; UVAnim loader 577b; S-2484..S-2494 U-2489 U-2490; re/analysis/track_loader_d4/0x00558df0.md
2026-05-06  004b3cc0  FUN_004b3cc0  stub->C1  track_loader_d4-20260506-2007; spline stream reader 81b; chunk 0xc; S-2495 U-2491; re/analysis/track_loader_d4/0x004b3cc0.md
2026-05-06  004b3de0  FUN_004b3de0  stub->C1  track_loader_d4-20260506-2007; anim stream reader 81b; chunk 0x1b; S-2496 U-2492; re/analysis/track_loader_d4/0x004b3de0.md
2026-05-06  00479030  LAB_00479030  stub->C0  track_loader_d4-20260506-2007; post-load callback 174b; no Ghidra fn object; needs function_create; U-2493 U-2494; re/analysis/track_loader_d4/0x00479030.md
2026-05-06  00474fb0  FUN_00474fb0  stub->C1  track_loader_d4-20260506-2007; DFF clump iterator 27b; &LAB_00474f90 callback; U-2495 U-2496; re/analysis/track_loader_d4/0x00474fb0.md
2026-05-06  00474f30  FUN_00474f30  stub->C1  track_loader_d4-20260506-2007; per-node callback 31b; FUN_00552020 test; U-2497; re/analysis/track_loader_d4/0x00474f30.md
2026-05-06  0047f4c0  FUN_0047f4c0  stub->C1  track_loader_d4-20260506-2007; physics world ctor 522b; AABB ±1000.0f; U-2498 U-2499; re/analysis/track_loader_d4/0x0047f4c0.md
2026-05-06  0047d080  FUN_0047d080  stub->C1  track_loader_d4-20260506-2007; activate physics body 104b; DAT_006c71d8 array; U-2500 U-2501; re/analysis/track_loader_d4/0x0047d080.md
2026-05-06  0047d100  FUN_0047d100  stub->C1  track_loader_d4-20260506-2007; secondary enable 35b; FUN_004b5240 no-arg; U-2502; re/analysis/track_loader_d4/0x0047d100.md
2026-05-06  00487280  FUN_00487280  stub->C1  track_loader_d4-20260506-2007; broadphase registration 2678b; 14 callees; U-2503 U-2504; re/analysis/track_loader_d4/0x00487280.md
2026-05-06  0047be80  FUN_0047be80  stub->C1  track_loader_d4-20260506-2007; triangle normal calc 234b; __fastcall; cross product; U-2505; re/analysis/track_loader_d4/0x0047be80.md
2026-05-06  0047bcc0  FUN_0047bcc0  stub->C1  track_loader_d4-20260506-2007; half-edge adj builder 433b; in_EAX=tri_count; U-2506; re/analysis/track_loader_d4/0x0047bcc0.md
2026-05-06  004b53b0  FUN_004b53b0  stub->C1  track_loader_d4-20260506-2007; bounding sphere 430b; max-sq-dist; sqrt; re/analysis/track_loader_d4/0x004b53b0.md
2026-05-06  004c3d90  FUN_004c3d90  stub->C1  track_loader_d4-20260506-2007; dispatch shim 42b; DAT_007d3ffc+0xc+DAT_007d3ff8; re/analysis/track_loader_d4/0x004c3d90.md
2026-05-06  00546380  FUN_00546380  stub->C1  track_loader_d4-20260506-2007; waypoint set ctor 426b; type 1/2; FUN_00545340 init; re/analysis/track_loader_d4/0x00546380.md
2026-05-06  sweep-20260506-2157  scribe-claim  bucket=sweep-multi rvas=23 (input_lua_d2=5, audio_music_d3=2, track_loader_d4=16; replay_record HOLD-skipped)
2026-05-06  input_lua_d2-20260506-1854  scribe-claim  bucket=input_lua_d2 rvas=5
2026-05-06  input_lua_d2-20260506-1854  scribe-release bucket=input_lua_d2 writes=10 errors=0  drained-by=sweep-20260506-2157; 5 plates, 5 bookmarks, 0 renames
2026-05-06  audio_music_d3-20260506-1930  scribe-claim  bucket=audio_music_d3 rvas=2
2026-05-06  audio_music_d3-20260506-1930  scribe-release bucket=audio_music_d3 writes=4 errors=0  drained-by=sweep-20260506-2157; 2 plates, 2 bookmarks, 0 renames
2026-05-06  track_loader_d4-20260506-2007  scribe-claim  bucket=track_loader_d4 rvas=16
2026-05-06  track_loader_d4-20260506-2007  scribe-release bucket=track_loader_d4 writes=32 errors=0  drained-by=sweep-20260506-2157; 16 plates, 16 bookmarks, 0 renames; 0x00479030 listing-level (no function object)
2026-05-06  sweep-20260506-2157  scribe-skip  bucket=replay_record reason="HOLD missing-per-rva-files" (left in Queued; not drained)
2026-05-06  sweep-20260506-2157  scribe-release bucket=sweep-multi writes=46 errors=0 buckets_drained=3 buckets_skipped=1 (input_lua_d2=10, audio_music_d3=4, track_loader_d4=32)
2026-05-07  sweep-20260507-0133  scribe-claim  buckets=1 queued, 1 skipped-HOLD
2026-05-07  physics_collision_d2-20260507-0106  scribe-release  bucket=physics_collision_d2  writes=6  errors=0
2026-05-07  sweep-20260507-0133  scribe-skip  bucket=replay_record reason="HOLD missing-per-rva-files" (left in Queued; not drained)
2026-05-07  sweep-20260507-0133  scribe-release bucket=sweep-multi writes=6 errors=0 buckets_drained=1 buckets_skipped=1 (physics_collision_d2=6)
2026-05-07  sweep-20260507-1913  scribe-claim  buckets=1 queued, 1 skipped-HOLD
2026-05-07  sweep-20260507-1913  scribe-release  bucket=sky_weather_d2  writes=12  errors=0
2026-05-07  sweep-20260507-1913  scribe-release  bucket=race_results_d2  writes=5  errors=0
2026-05-07  sweep-20260507-1913  scribe-release  buckets=2 drained  errors=0
2026-05-07 librw_plugin_compat-20260507-1950 analysis bucket=librw_plugin_compat rvas=28 (callback addresses) + FUN_004d7de0 (re-read; already C1) U-2887..U-2891 D-8560 slot=Mashed_pool12
2026-05-07 piz_fsmanager_handler-20260507 analysis bucket=piz_fsmanager_handler rvas=20 (12 RtFSHandler vtable entries + RtFSHandler::Install + RtFSManager::FindHandler + PizWin32Open + PizWin32Read + PizOpenAndParse + PizOpen + OpenPizFile + ClosePizFile) shim_verdict=PathC (no .piz integration via FSManager; .piz bypass sites: 0x004b6710 CreateFileA + 0x004b67e0 ReadFile) U-2907..U-2909 slot=Mashed_pool13

2026-05-07  sweep-20260507-2002  scribe-claim  buckets=23 queued, 1 skipped-HOLD
2026-05-07  sweep-20260507-2002  scribe-release  bucket=effects_particle_d3  writes=4  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=game_state_d3  writes=5  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=game_state_d4  writes=3  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=hud_ingame_d3  writes=2  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=loading_screen  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=hud_frontend_d2  writes=14  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=input_dinput_d2  writes=30  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_d3d_reset_d2  writes=14  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_frame_d3  writes=15  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_pipeline_d2  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=audio_sfx_dispatch_d2  writes=14  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=render_pipeline_d3  writes=10  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=rw_engine_init_d3  writes=19  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=rw_engine_teardown_d3  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=texture_loader_d2  writes=4  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=texture_loader_d3  writes=12  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=title_screen_d2  writes=5  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=track_loader_d3  writes=19  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=vehicle_dynamics  writes=4  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=vehicle_update_d2  writes=10  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=vehicle_update_d3  writes=8  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=video_mci_d2  writes=3  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=window_fullscreen  writes=1  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  bucket=input_lua_d3  writes=5  errors=0
2026-05-07  sweep-20260507-2002  scribe-release  buckets=19 drained  errors=0

2026-05-07  sweep-20260507-2353  scribe-claim  buckets=17 queued, 1 skipped-HOLD
2026-05-07  sweep-20260507-2353  scribe-release  buckets=0 drained (no-op: sweep-20260507-2002 had already completed all 17 drainable buckets; 1 HOLD replay_record)  errors=0

2026-05-08  sweep-20260508-0358  scribe-claim  buckets=8 queued, 1 skipped-HOLD
2026-05-08  sweep-20260508-0358  scribe-release  bucket=audio_dsound_d3  writes=15  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=game_state_d5  writes=5  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=game_state_d5-cont1  writes=2  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=render_frame_d4  writes=20  errors=0
2026-05-08  sweep-20260508-0358  scribe-release  bucket=settings_config_d3  writes=8  errors=0  note=0x004991f0 written as listing-level (no Ghidra function object — C0 plate)
2026-05-08  sweep-20260508-0358  scribe-release  bucket=ai_update_d4  writes=5  errors=0  note=CONFLICT D-6461 vs 0x004c3b30 noted (analysis authoritative; D-6461 clear via re-classify)
2026-05-08  005ba1d0  FUN_005ba1d0  tail-analyzed  audio_dsound-20260508-0406 (audio_dsound_d4); re/analysis/audio_dsound_d4/0x005ba1d0_tail.md; D-0940 resolved; D-0952 filed; U-0363 filed
2026-05-08  005c7990  FUN_005c7990  C0->C1  audio_dsound-20260508-0406 (audio_dsound_d4); re/analysis/audio_dsound_d4/0x005c7990.md; S-3190 filed
2026-05-08  005bc400  FUN_005bc400  C0->C1  audio_dsound-20260508-0406 (audio_dsound_d4); re/analysis/audio_dsound_d4/0x005bc400.md; U-0360 filed
2026-05-08  005bbfc0  FUN_005bbfc0  C0->C1  audio_dsound-20260508-0406 (audio_dsound_d4); re/analysis/audio_dsound_d4/0x005bbfc0.md; U-0362 filed
2026-05-08  005baf60  FUN_005baf60  C0->C1  audio_dsound-20260508-0406 (audio_dsound_d4); re/analysis/audio_dsound_d4/0x005baf60.md
2026-05-08  005baf90  FUN_005baf90  C0->C1  audio_dsound-20260508-0406 (audio_dsound_d4); re/analysis/audio_dsound_d4/0x005baf90.md; U-0361 filed
2026-05-08  sweep-20260508-0358  scribe-release  bucket=leaderboard_d3  writes=31  errors=0  note=0x00412f30 written with [C0] prefix (function object exists; analysis C0 due to size cap)
2026-05-08  sweep-20260508-0358  scribe-release  bucket=hud_frontend_d4  writes=0  errors=0  note=all 18 RVAs missing per-RVA .md files (only notes.md present)
2026-05-08  sweep-20260508-0603  scribe-claim  buckets=1 queued, 1 skipped-HOLD
2026-05-08  sweep-20260508-0603  scribe-release  bucket=audio_dsound_d4  writes=6  errors=0  note=D-0940 tail plate written at function start (overwrites prior d1 head plate)
2026-05-08  sweep-20260508-0603  scribe-release  buckets=1 drained  errors=0
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x00470c70 FUN_00470c70 (VEHICLE_UPDATE_FN); re/analysis/vehicle_update/00470c70.md; U-0387..U-0390 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x00408a50 FUN_00408a50; re/analysis/vehicle_update/00408a50.md; U-0391 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x0040e340 FUN_0040e340; re/analysis/vehicle_update/0040e340.md; U-0392 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x0040e350 FUN_0040e350; re/analysis/vehicle_update/0040e350.md; U-0393 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x0040e370 FUN_0040e370; re/analysis/vehicle_update/0040e370.md; U-0394 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x0040e470 FUN_0040e470; re/analysis/vehicle_update/0040e470.md; U-0394 (shared)
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x00413c70 FUN_00413c70; re/analysis/vehicle_update/00413c70.md; U-0395 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x00422fd0 FUN_00422fd0; re/analysis/vehicle_update/00422fd0.md; S-0380..S-0384 filed; U-0396 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x0042fe70 FUN_0042fe70; re/analysis/vehicle_update/0042fe70.md; U-0397 filed
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x00443080 FUN_00443080; re/analysis/vehicle_update/00443080.md; U-0398 filed
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x00467350 FUN_00467350; vehicle_update/00467350.md cross-refs vehicle_update_d3
2026-05-08  vehicle_update-20260508-0613  C0->C1  0x0046c7b0 FUN_0046c7b0; re/analysis/vehicle_update/0046c7b0.md
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x0046da80 FUN_0046da80; vehicle_update/0046da80.md cross-refs vehicle_update_d2
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x0046dc20 FUN_0046dc20; vehicle_update/0046dc20.md cross-refs vehicle_update_d3
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x0046f6c0 FUN_0046f6c0; vehicle_update/0046f6c0.md cross-refs vehicle_update_d3
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x00470670 FUN_00470670; vehicle_update/00470670.md cross-refs vehicle_update_d2
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x004709a0 FUN_004709a0; vehicle_update/004709a0.md cross-refs vehicle_update_d2
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x0047eb30 FUN_0047eb30; vehicle_update/0047eb30.md cross-refs vehicle_update_d3
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x00480720 FUN_00480720; vehicle_update/00480720.md cross-refs vehicle_update_d2
2026-05-08  vehicle_update-20260508-0613  C1(xref)  0x004809e0 FUN_004809e0; vehicle_update/004809e0.md cross-refs vehicle_update_d3
2026-05-08  vehicle_update-20260508-0613  D-1060..D-1062 filed; cap split (23>20); vehicle_update-cont1 pickup
2026-05-08 track_loader-20260508-0616 first-pass bucket=track_loader rvas=20 slot=Mashed_pool3

2026-05-08  004c3b30  FUN_004c3b30  C0->C1  re/analysis/ai_update_d4/0x004c3b30.md; D-6461 cleared (conflict profile_career_d4 vs ai_update_d4; ai_update_d4 authoritative)
2026-05-08  004c75e0  FUN_004c75e0  C0->C1  re/analysis/intro_splash_d2/0x004c75e0.md; hooks.csv gap filled; U-2347 filed (intro_splash_d2 uncertainty not previously in master tracker); resolves U-3071 tracker inconsistency
2026-05-08  sweep-20260508-1445  scribe-claim  buckets=4 queued (boot_app_init_d2, window_fullscreen_d2, vehicle_update, track_loader-misplaced), 1 skipped-HOLD (replay_record)
2026-05-08  sweep-20260508-1445  scribe-release  bucket=boot_app_init_d2  writes=20  errors=0
2026-05-08  sweep-20260508-1445  scribe-release  bucket=window_fullscreen_d2  writes=1  errors=0  note=plate sourced from `## Purpose` step 1 (file uses C2-format, no `## Mechanical description` section)
2026-05-08  sweep-20260508-1445  scribe-release  bucket=vehicle_update  writes=20  errors=0  note=9 cross-ref RVAs plated from "Called from..." line per user direction (overwrites prior UUU-session plates from 2026-05-03)
2026-05-08  sweep-20260508-1445  scribe-release  bucket=track_loader  writes=19  errors=0  note=misplaced row (was outside code block at line 129 of pre-sweep tree); relocated to ## Drained; 16/19 RVAs overwrite prior plates from 2026-05-02 drain (0x00426cd0, 0x0042a8d0, 0x0042f510 are new this row)
2026-05-08  sweep-20260508-1445  scribe-release  buckets=4 drained  errors=0  note=boot_app_init_d2 (20), window_fullscreen_d2 (1), vehicle_update (20), track_loader-misplaced (19); replay_record HOLD silently skipped; pool-sync 14 refreshed / 2 skipped (Mashed_pool12+15 still locked — stale locks from analysis sessions per user)
2026-05-08  deferred_audit-20260508  audit-cycle-1  total=125 cleared=21 kept=102 followup=2  drift_rate=16.8%
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2985 (0045efe0) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2986 (0045f5f0) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2987 (0045faa0) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2988 (0045ff50) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2989 (00460350) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2990 (00460df0) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2991 (00461650) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2992 (00463640) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2993 (00463c80) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2994 (00463f40) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2995 (00464e10) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2996 (00465a30) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2997 (00465b20) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-2998 (004661f0) → covered by audio_sfx_dispatch_d2
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-6473 (00405890) → covered by vehicle_damage_d3
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-7120 (004ba1b0) → covered by input_lua_d3
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-7121 (004b7be0) → covered by input_lua_d3
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-7122 (004ba210) → covered by input_lua_d3
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-7123 (004b9850) → covered by input_lua_d3
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-7124 (004b64e0) → covered by input_lua_d3
2026-05-08  deferred_audit-20260508  DEFERRED clear: D-8862 (00494c80) → covered by video_mci
2026-05-08  boot_app_init_d4-20260508-1650  first-pass  bucket=boot_app_init_d4  rvas=20  C1  pool=Mashed_pool0  S-3212..S-3219  U-3222..U-3226  D-9520  drift-skip=D-8862,D-8864,D-8909
2026-05-08  sweep-20260508-1728  scribe-claim  buckets=4 queued, 1 skipped-HOLD
2026-05-08  sweep-20260508-1728  scribe-release  bucket=game_state_d5-cont2  writes=16  errors=0
2026-05-08  sweep-20260508-1728  scribe-release  bucket=hud_frontend_d4  writes=18  errors=0
2026-05-08  sweep-20260508-1728  scribe-release  bucket=profile_career_d4  writes=22  errors=0  note=plate text from REPORT.md (no per-RVA files); hooks.csv already C1/C2 from session
2026-05-08  sweep-20260508-1728  scribe-release  bucket=boot_app_init_d4  writes=20  errors=0
2026-05-08  c0_promotion_frontend_a-20260508-1724  C0->C1 promotion: 18 frontend functions  bucket=c0_promotion_frontend_a  rvas=0x0040acd0,0x0040e480,0x00414120,0x004298c0,0x0042a940,0x0042aa00,0x0042ac00,0x0042ac50,0x0042ac90,0x0042ae10,0x0042aeb0,0x0042b960,0x0042bcb0,0x0042d290,0x0042d300,0x0042ebe0,0x0042ee00,0x0042ee40  S-3420..S-3433(14 new stubs)  U-3427..U-3465(39 new uncertainties)  pool=Mashed_pool5  slot=76ac32e35c08438c842e3b3605a9c60b
2026-05-08  sweep-20260508-1728  scribe-release  bucket=boot_app_init_d5  writes=36  errors=0
2026-05-08  sweep-20260508-1728  scribe-release  bucket=boot_app_init_d2-cont1_a  writes=19  errors=0  note=bookmarks only (19 collision RVAs); plates authoritative from boot_app_init_d5
2026-05-08  sweep-20260508-1728  scribe-release  buckets=6 drained  errors=0  sync=12/16 (4 skipped: pool1,pool2,pool3,pool12 file-busy/Windows)
2026-05-08  sweep-20260508-1737  scribe-claim  buckets=2 queued, 1 skipped-HOLD
2026-05-08  sweep-20260508-1737  scribe-release  bucket=input_lua_d4  writes=15  errors=0

2026-05-08  sweep-20260508-1737  scribe-release  bucket=c0_promotion_frontend_a  writes=18  errors=0
2026-05-08  sweep-20260508-1737  scribe-release  buckets=2 drained  errors=0  sync=partial (pool1 file-busy/Windows)
2026-05-08  sweep-20260508-1859  scribe-claim  buckets=4 queued, 1 skipped-HOLD
2026-05-08  sweep-20260508-1859  scribe-release  bucket=powerups_d4  writes=10  errors=0
2026-05-08  sweep-20260508-1859  scribe-release  bucket=c0_promotion_render_a  writes=16  errors=0
2026-05-08  sweep-20260508-1859  scribe-release  bucket=vehicle_dynamics_d2  writes=2  errors=0
2026-05-08  sweep-20260508-1859  scribe-release  bucket=timer_d3  writes=20  errors=0
2026-05-08  sweep-20260508-1859  scribe-release  buckets=4 drained  errors=0  sync=partial (pool12 file-busy/Windows)
2026-05-09  004c3ac0  Vec3Magnitude  C1->C4  Frida A/B 18/18 bit-identical (log/diff_vec3_magnitude.csv); reimpl mashedmod/src/mashed_re/Math/Vec3.cpp; hook RH_ScopedInstall; subsystem ai->util; leaf-exemption applied per CONFIDENCE.md
2026-05-09  004c3ac0  Vec3Magnitude  C4<-C3  demotion: synthetic Frida-A/B with hook bypass is implementation-correctness, not canonical-scenario verification per C4 gate; user pushback on overclaim; leaf-exemption clarified as C2->C3 only
2026-05-09  004c3ac0  Vec3Magnitude  hook-installer-verified  HookSystem::InstallAll patches 0x004c3ac0 with E9 rel32 to reimpl correctly; Interceptor confirms 5/5 calls route through reimpl; outputs 5/5 correct via patched entry (log/verify_hook_install_vec3.txt); confidence stays C3 — no canonical-scenario evidence yet
2026-05-09  004c3b90  FastInvSqrt  C1->C3  Frida path1 18/18 bit-identical (log/diff_fast_invsqrt.csv); path2 hook-installer opcode/rel32/interceptor all PASS (log/verify_hook_install_invsqrt.txt); reimpl mashedmod/src/mashed_re/Math/RwSqrt.cpp; hook RH_ScopedInstall; subsystem render->util; leaf-exemption applied per CONFIDENCE.md
2026-05-09  004c3b30  FastSqrt  C1->C3  Frida path1 18/18 bit-identical (log/diff_fast_sqrt.csv); path2 hook-installer opcode/rel32/interceptor all PASS (log/verify_hook_install_sqrt.txt); reimpl mashedmod/src/mashed_re/Math/RwSqrt.cpp; hook RH_ScopedInstall; subsystem ai->util; leaf-exemption applied per CONFIDENCE.md
2026-05-11  004c3ac0  Vec3Magnitude  C3->C4  canonical-scenario evidence gap (called out in 2026-05-09 demotion) now closed: log/observe_hooks_at_menu.txt — hook installed (Frida byte-verified E9+rel32 0x6cc8dc0b) during 10s main-menu observation with ~1,800 natural invocations of Vec3Magnitude, no crash, no rollback, bytes still patched at end; combined with path1 (log/diff_vec3_magnitude.csv 18/18) and path2 (log/verify_hook_install_vec3.txt opcode+rel32+interceptor 5/5); scenario=main_menu_idle_10s_2026-05-11
2026-05-11  004c3b30  FastSqrt  C3->C4  canonical-scenario evidence: log/observe_hooks_at_menu.txt — hook installed (Frida byte-verified E9+rel32 0x6cc8dc5b) during 10s main-menu observation with ~27,000 natural invocations of FastSqrt, no crash, no rollback; combined with path1 (log/diff_fast_sqrt.csv 18/18 bit-identical) and path2 (log/verify_hook_install_sqrt.txt opcode+rel32+interceptor 5/5); scenario=main_menu_idle_10s_2026-05-11
2026-05-11  004c3b90  FastInvSqrt  C3->C4  canonical-scenario evidence: log/observe_hooks_at_menu.txt — hook installed (Frida byte-verified E9+rel32 0x6cc8dbab) during 10s main-menu observation with ~9,000 natural invocations of FastInvSqrt, no crash, no rollback; combined with path1 (log/diff_fast_invsqrt.csv 18/18 bit-identical) and path2 (log/verify_hook_install_invsqrt.txt opcode+rel32+interceptor 5/5); scenario=main_menu_idle_10s_2026-05-11
2026-05-11  0042a940  FUN_0042a940  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042a940.md
2026-05-11  0042aa00  FUN_0042aa00  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042aa00.md
2026-05-11  0042aad0  FUN_0042aad0  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042aad0.md
2026-05-11  0042aae0  FUN_0042aae0  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042aae0.md
2026-05-11  0042ac00  FUN_0042ac00  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042ac00.md
2026-05-11  0042ac50  FUN_0042ac50  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042ac50.md
2026-05-11  0042ac90  FUN_0042ac90  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042ac90.md
2026-05-11  0042ae10  FUN_0042ae10  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042ae10.md
2026-05-11  0042aeb0  FUN_0042aeb0  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042aeb0.md
2026-05-11  0042aff0  FUN_0042aff0  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042aff0.md
2026-05-11  0042b180  FUN_0042b180  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042b180.md
2026-05-11  0042b310  FUN_0042b310  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042b310.md; U-3556 filed
2026-05-11  0042b540  FUN_0042b540  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042b540.md
2026-05-11  0042b770  FUN_0042b770  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042b770.md
2026-05-11  0042b930  FUN_0042b930  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042b930.md
2026-05-11  0042b960  FUN_0042b960  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042b960.md; U-3557 filed
2026-05-11  0042b9e0  FUN_0042b9e0  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042b9e0.md
2026-05-11  0042bb60  FUN_0042bb60  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042bb60.md
2026-05-11  0042bcb0  FUN_0042bcb0  C1->C2  Ghidra decomp; frontend_promote_menus_a; re/analysis/frontend_promote_menus_a/0x0042bcb0.md
2026-05-11  hud_frontend_d5-20260511-1710  13 RVAs C1  0x004368e0 0x00436810 0x004391b0 0x00458630 0x00473870 0x004736c0 0x00474e60 0x00427f00 0x0042e8b0 0x0042ed70 0x00430670 0x004309b0 0x004a2b60  U-3407..U-3423 filed  D-6178..D-6184 cleared  pool=Mashed_pool4
2026-05-11  audio_dsound_d5-20260511  005aa560  FUN_005aa560  C0->C1  audio_dsound_d5; re/analysis/audio_dsound_d5/0x005aa560.md; S-3190 resolved; D-0952(phantom) resolved; S-3565..S-3570 filed; U-3424..U-3427 filed

2026-05-11  sweep-20260511-1822  scribe-claim-sweep  buckets=10  rvas=130  (replay_record-20260503 skipped: HOLD=missing-per-rva-files, superseded by replay_record-20260511-1200)
2026-05-11  sweep-20260511-1822  scribe-release  bucket=input_dinput_d3  writes=16  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=audio_sfx_dispatch_d3  writes=30  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=hud_ingame_promote_c2  writes=32  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=replay_record  writes=36  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=frontend_promote_menus_b  writes=40  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=frontend_unmapped_a  writes=30  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=frontend_promote_menus_a  writes=38  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=save_gamesave_d3  writes=10  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=hud_frontend_d5  writes=26  errors=0
2026-05-11  sweep-20260511-1822  scribe-release  bucket=audio_dsound_d5  writes=2  errors=0

2026-05-11  sweep-20260511-1822  scribe-release-sweep  buckets=10  errors=0  (HOLD=1: replay_record-20260503; library-rename check via pre-comment performed on buckets 1-3 (39 RVAs, 0 hits); skipped for buckets 4-10 as deliberate optimization given consistent null pattern + no library candidates in .md notes)

2026-05-11  track_collision_geometry_s14  bucket=track_collision_geometry  rvas=0x00547bf0,0x00547450,0x0047a1b0,0x0047a280,0x0047a320,0x0047a3a0,0x0047a4a0,0x0047a540,0x0047a580,0x0047a5b0,0x0047a5e0,0x0047a610,0x0047a6b0,0x0047a6f0,0x0047a720,0x0047a790,0x0047a880,0x0047a8b0,0x0047aa20,0x0047aa50  S-3571..S-3573  U-3568..U-3572  D-10542..D-10544  S-1964-cleared  S-1965-cleared  pool=Mashed_pool13  struct=re/analysis/structs/bsp_struct_a.md  notes=AABB-vs-tri-SAT+sphere-vs-tri+18-COURSE.LUA-Lua-C-filename-handlers; BSP-struct-A slot-map first-pass

2026-05-12  sweep-20260512-0445  scribe-claim  buckets=4 queued, 1 skipped-HOLD  rvas=55  (HOLD=1: replay_record-20260503, superseded by replay_record-20260511-1200)
2026-05-12  sweep-20260512-0445  scribe-release  bucket=track_world_initial_sweep  writes=18  errors=0
2026-05-12  sweep-20260512-0445  scribe-release  bucket=vehicle_promote_c2  writes=12  errors=0
2026-05-12  sweep-20260512-0445  scribe-release  bucket=vehicle_damage_d4  writes=5  errors=0  (S-3600/S-3601 renumbered to S-3612/S-3613 in 0x00419760.md per queue note; collision with track_world_initial_sweep-claimed S-3600..S-3611)
2026-05-12  sweep-20260512-0445  scribe-release  bucket=track_collision_geometry  writes=20  errors=0

2026-05-12  sweep-20260512-0445  scribe-release-sweep  buckets=4 drained  rvas=55  errors=0  (HOLD=1: replay_record-20260503; library-rename check via pre-comment performed on 3 RVAs in bucket 1 (0 hits); skipped for remaining 52 RVAs as consistent FUN_ pattern with no library candidates in .md notes; queue-mandated S-3600/S-3601→S-3612/S-3613 renumber in vehicle_damage_d4/0x00419760.md applied)
2026-05-12  sweep-20260512-1354  scribe-claim  buckets=2 queued, 1 skipped-HOLD
2026-05-12  sweep-20260512-1354  scribe-release  bucket=effects_particle_d4  writes=3  errors=0
2026-05-12  sweep-20260512-1354  scribe-release  bucket=ai_path_following  writes=7  errors=0
2026-05-12  sweep-20260512-1354  scribe-release-sweep  buckets=2 drained  rvas=10  errors=0  (HOLD=1: replay_record-20260503)
2026-05-12  0040ad20  FUN_0040ad20  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d3/0x0040ad20.md; leaf getter; no U filed
2026-05-12  0040b6c0  FUN_0040b6c0  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d3/0x0040b6c0.md; leaf array indexer; no U filed
2026-05-12  0040b810  FUN_0040b810  C0->C2  frontend_c0_promote; re/analysis/timer_d2_cont1/0x0040b810.md; global-zero init; U-1609 U-1610 (pre-filed)
2026-05-12  00422b30  FUN_00422b30  C0->C2  frontend_c0_promote; re/analysis/timer_d2_cont1/0x00422b30.md; memset 1248 bytes; U-1612 (pre-filed)
2026-05-12  00429aa0  FUN_00429aa0  C0->C2  frontend_c0_promote; re/analysis/timer_d2_cont1/0x00429aa0.md; two-path table fill; S-1604 S-1605 (pre-filed); U-1613 U-1614 (pre-filed)
2026-05-12  0042af50  FUN_0042af50  C0->C2  frontend_c0_promote; re/analysis/timer_d2_cont1/0x0042af50.md; guard+two-path check; U-1615 U-1616 (pre-filed)
2026-05-12  0042ef40  FUN_0042ef40  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d4/0x0042ef40.md; vehicle unlock flag check; U-3176 (pre-filed)
2026-05-12  0042f020  FUN_0042f020  C0->C2  frontend_c0_promote; re/analysis/frontend_c0_promote/0x0042f020.md; vehicle flag clear __fastcall; U-3594 filed
2026-05-12  0042f6b0  FUN_0042f6b0  C0->C2  frontend_c0_promote; re/analysis/frontend_c0_promote/0x0042f6b0.md; mode-index to game-mode mapper; U-3595 filed
2026-05-12  004307a0  FUN_004307a0  C0->C2  frontend_c0_promote; re/analysis/frontend_c0_promote/0x004307a0.md; elapsed-time vs threshold; U-3596 U-3597 filed
2026-05-12  004309b0  FUN_004309b0  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d5/0x004309b0.md; game-mode-to-index mapper; no new U filed
2026-05-12  00430910  FUN_00430910  C0->C2  frontend_c0_promote; re/analysis/frontend_c0_promote/0x00430910.md; option-mode compatibility check; U-3598 U-3599 filed
2026-05-12  00430a10  FUN_00430a10  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d4/0x00430a10.md; slot-type-0 mapper; no U filed
2026-05-12  00430a60  FUN_00430a60  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d4/0x00430a60.md; slot-type-1 mapper; no U filed
2026-05-12  00430ab0  FUN_00430ab0  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d4/0x00430ab0.md; slot-type-2 mapper; no U filed
2026-05-12  00430b30  FUN_00430b30  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d4/0x00430b30.md; thiscall lap-time getter; no U filed
2026-05-12  00430b60  FUN_00430b60  C0->C2  frontend_c0_promote; re/analysis/frontend_c0_promote/0x00430b60.md; active-slot counter; U-3600 filed
2026-05-12  004323c0  FUN_004323c0  C0->C2  frontend_c0_promote; re/analysis/frontend_c0_promote/0x004323c0.md; backward cursor nav; U-3601 U-3602 filed
2026-05-12  004368e0  FUN_004368e0  C0->C1  frontend_c0_promote; re/analysis/hud_frontend_d5/0x004368e0.md; large HUD draw dispatch; C2 deferred (function too large for full trace in this session); U-3407..U-3409 (pre-filed)
2026-05-12  00436810  FUN_00436810  C0->C2  frontend_c0_promote; re/analysis/hud_frontend_d5/0x00436810.md; local player slot occupancy; U-3410 U-3411 (pre-filed)
2026-05-12  00401f10  FUN_00401f10  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  00403910  FUN_00403910  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at c0_promotion_render_a
2026-05-12  00403d30  FUN_00403d30  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  00403db0  FUN_00403db0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  00403ed0  FUN_00403ed0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  00403fa0  FUN_00403fa0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  004041c0  FUN_004041c0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  00404650  FUN_00404650  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at c0_promotion_render_a
2026-05-12  004072e0  FUN_004072e0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at c0_promotion_render_a
2026-05-12  0040bde0  FUN_0040bde0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at c0_promotion_render_a
2026-05-12  00413a00  FUN_00413a00  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at c0_promotion_render_a
2026-05-12  00413a40  FUN_00413a40  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at c0_promotion_render_a
2026-05-12  004189c0  FUN_004189c0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at c0_promotion_render_a
2026-05-12  0041e8f0  FUN_0041e8f0  C0->C1  render_c0_promote_a  data-drift-fix; U-3128 [UNCERTAIN indirect dispatch]; C1 plate at render_frame_d4
2026-05-12  0041ea10  FUN_0041ea10  C0->C2  render_c0_promote_a  leaf getter (*(DAT_0063d7e4+0x24)); no callees; shape fully documented; render_frame_d4
2026-05-12  0041ebb0  FUN_0041ebb0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  004219c0  FUN_004219c0  C0->C1  render_c0_promote_a  data-drift-fix: caller-chain to render; C1 plate at render_frame_d4
2026-05-12  00422570  FUN_00422570  C0->C1  render_c0_promote_a  new analysis: per-player viewport render helper; plate at render_c0_promote_a
2026-05-12  00422ac0  FUN_00422ac0  C0->C2  render_c0_promote_a  leaf 4-dword setter (DAT_006412e8+idx*0xf40); no callees; shape fully documented; c0_promotion_render_a
2026-05-12  00422af0  FUN_00422af0  C0->C2  render_c0_promote_a  leaf 1-dword setter (DAT_00641320+idx*0xf40); no callees; shape fully documented; c0_promotion_render_a
2026-05-12  00425e40  FUN_00425e40  C0->C1  re/analysis/render_frame_d4/0x00425e40.md
2026-05-12  00426640  FUN_00426640  C0->C1  re/analysis/c0_promotion_render_a/0x00426640.md
2026-05-12  00426e00  FUN_00426e00  C0->C1  re/analysis/c0_promotion_render_a/0x00426e00.md
2026-05-12  004278d0  FUN_004278d0  C0->C1  re/analysis/render_frame_d4/0x004278d0.md
2026-05-12  00427990  FUN_00427990  C0->C1  re/analysis/render_frame_d4/0x00427990.md
2026-05-12  00427be0  FUN_00427be0  C0->C1  re/analysis/render_frame_d4/0x00427be0.md
2026-05-12  00429e10  FUN_00429e10  C0->C1  re/analysis/render_c0_promote_b/0x00429e10.md
2026-05-12  0042c010  FUN_0042c010  C0->C1  re/analysis/render_frame_d4/0x0042c010.md
2026-05-12  0042c090  FUN_0042c090  C0->C1  re/analysis/render_frame_d4/0x0042c090.md
2026-05-12  00448730  FUN_00448730  C0->C1  re/analysis/c0_promotion_render_a/0x00448730.md
2026-05-12  0045b990  FUN_0045b990  C0->C1  re/analysis/c0_promotion_render_a/0x0045b990.md
2026-05-12  004725c0  FUN_004725c0  C0->C1  re/analysis/render_frame_d4/0x004725c0.md
2026-05-12  00475d30  FUN_00475d30  C0->C1  re/analysis/c0_promotion_render_a/0x00475d30.md
2026-05-12  00475e50  FUN_00475e50  C0->C1  re/analysis/c0_promotion_render_a/0x00475e50.md
2026-05-12  00477810  FUN_00477810  C0->C1  re/analysis/c0_promotion_render_a/0x00477810.md
2026-05-12  00477a10  FUN_00477a10  C0->C1  re/analysis/render_c0_promote_b/0x00477a10.md
2026-05-12  00479030  LAB_00479030  C0->C1  re/analysis/track_loader_d4/0x00479030.md
2026-05-12  0047b9e0  FUN_0047b9e0  C0->C1  re/analysis/render_c0_promote_b/0x0047b9e0.md
2026-05-12  004891f0  FUN_004891f0  C0->C1  re/analysis/render_c0_promote_b/0x004891f0.md
2026-05-12  0048fd70  FUN_0048fd70  C0->C1  re/analysis/render_c0_promote_b/0x0048fd70.md
2026-05-12  00490490  FUN_00490490  C0->C1  re/analysis/render_c0_promote_c/0x00490490.md
2026-05-12  004b42c0  FUN_004b42c0  C0->C1  re/analysis/render_c0_promote_c/0x004b42c0.md
2026-05-12  004c1340  FUN_004c1340  C0->C1  re/analysis/render_c0_promote_c/0x004c1340.md
2026-05-12  004c1680  FUN_004c1680  C0->C1  re/analysis/render_c0_promote_c/0x004c1680.md
2026-05-12  004c7760  FUN_004c7760  C0->C1  re/analysis/render_c0_promote_c/0x004c7760.md
2026-05-12  004c7a70  _rwDeviceSystemFn  C0->C1  re/analysis/render_c0_promote_c/0x004c7a70.md
2026-05-12  004cd070  FUN_004cd070  C0->C1  re/analysis/render_c0_promote_c/0x004cd070.md
2026-05-12  004cd140  FUN_004cd140  C0->C1  re/analysis/render_c0_promote_c/0x004cd140.md
2026-05-12  004cd2d0  FUN_004cd2d0  C0->C1  re/analysis/render_c0_promote_c/0x004cd2d0.md
2026-05-12  004e4320  FUN_004e4320  C0->C1  re/analysis/render_c0_promote_c/0x004e4320.md
2026-05-12  004e4350  FUN_004e4350  C0->C1  re/analysis/render_c0_promote_c/0x004e4350.md
2026-05-12  004c35f0  FUN_004c35f0  new->C1  re/analysis/render_c0_promote_c/0x004c35f0.md
2026-05-12  004c40b0  FUN_004c40b0  new->C1  re/analysis/render_c0_promote_c/0x004c40b0.md
2026-05-12  004c45f0  FUN_004c45f0  new->C1  re/analysis/render_c0_promote_c/0x004c45f0.md
2026-05-12  004c90a0  FUN_004c90a0  new->C1  re/analysis/render_c0_promote_c/0x004c90a0.md
2026-05-12  004cfe20  FUN_004cfe20  new->C1  re/analysis/render_c0_promote_c/0x004cfe20.md
2026-05-12  004d40c0  FUN_004d40c0  new->C1  re/analysis/render_c0_promote_c/0x004d40c0.md
2026-05-12  004d40d0  FUN_004d40d0  new->C1  re/analysis/render_c0_promote_c/0x004d40d0.md
2026-05-12  004dcd50  FUN_004dcd50  new->C1  re/analysis/render_c0_promote_c/0x004dcd50.md
2026-05-12  004dd0b0  FUN_004dd0b0  new->C1  re/analysis/render_c0_promote_c/0x004dd0b0.md
2026-05-12  sweep-20260512-1513  scribe-claim  buckets=4 queued, 1 skipped-HOLD
2026-05-12  sweep-20260512-1513  scribe-release  bucket=render_c0_promote_a  writes=20  errors=0
2026-05-12  sweep-20260512-1513  scribe-release  bucket=physics_collision_d4_breadth  writes=2  errors=0
2026-05-12  sweep-20260512-1513  scribe-release  bucket=util_c0_promote  writes=20  errors=0
2026-05-12  sweep-20260512-1513  scribe-release  bucket=render_c0_promote_c  writes=20  errors=0
2026-05-12  sweep-20260512-1513  scribe-release  buckets=4 drained  errors=0
2026-05-12  sweep-20260512-1805  scribe-claim  buckets=2 queued, 1 skipped-HOLD
2026-05-12  sweep-20260512-1805  scribe-release  bucket=rw_engine_init_d2_cont1_a  writes=2  errors=0
2026-05-12  sweep-20260512-1805  scribe-release  bucket=timer_d3_cont1_a  writes=19  errors=0  drift-skipped=1(0x00419760)
2026-05-12  sweep-20260512-1805  scribe-release  buckets=2 drained, 1 skipped-HOLD  errors=0
2026-05-12  00427620  FontText_HudShutdown  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00427620.md
2026-05-12  00427680  FontText_ComputeScreenXY  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00427680.md; U-2127 open (EDI write)
2026-05-12  00427780  FontText_StringTableLookup  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00427780.md
2026-05-12  00427840  FontText_UTF16WidenCopy  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00427840.md; U-1069 open
2026-05-12  00427ca0  FontText_HudInit  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00427ca0.md
2026-05-12  00427ff0  FontText_DrawTextRotated  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00427ff0.md
2026-05-12  00552750  FontCtx_ResetTransform  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552750.md
2026-05-12  00552840  FontCtx_SetRotation  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552840.md
2026-05-12  00552a60  FontSys_SetActiveCamera  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552a60.md
2026-05-12  00552b60  FontSys_InitSeq  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552b60.md
2026-05-12  00552c10  FontSys_InitRenderState  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552c10.md
2026-05-12  00552d10  FontMatrix_Push  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552d10.md
2026-05-12  00552da0  FontCtx_SetScale  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552da0.md
2026-05-12  00552df0  FontCtx_SetTranslation  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552df0.md
2026-05-12  00552e40  FontCtx_FlushMatrix  C1->C2  hud_promote_c2_b; re/analysis/hud_promote_c2_b/0x00552e40.md; U-2132 open (naming); callers enumerated (resolved U-2134)
2026-05-12  00426030  FUN_00426030  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426030.md  WorldRenderPrePass shape documented
2026-05-12  00426060  FUN_00426060  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426060.md  trivial getter DAT_0065742c
2026-05-12  00426070  FUN_00426070  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426070.md  trivial getter DAT_00656ee8
2026-05-12  004260c0  FUN_004260c0  C1->C2  re/analysis/render_promote_c2_track_loader/0x004260c0.md  trivial getter DAT_00657490 U-3660
2026-05-12  004260e0  FUN_004260e0  C1->C2  re/analysis/render_promote_c2_track_loader/0x004260e0.md  path builder; existing U-3218 retained
2026-05-12  004262f0  FUN_004262f0  C1->C2  re/analysis/render_promote_c2_track_loader/0x004262f0.md  record processor; 4 callees enumerated
2026-05-12  00426640  FUN_00426640  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426640.md  render guard; 3 callees
2026-05-12  00426670  FUN_00426670  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426670.md  WorldRenderDispatch_Begin
2026-05-12  004266b0  FUN_004266b0  C1->C2  re/analysis/render_promote_c2_track_loader/0x004266b0.md  WorldRenderDispatch_End
2026-05-12  00426700  FUN_00426700  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426700.md  camera node iterator U-3661
2026-05-12  00426780  FUN_00426780  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426780.md  camera array updater U-3661
2026-05-12  00426810  sub_00426810  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426810.md  camera path lerp U-3662
2026-05-12  00426ab0  sub_00426ab0  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426ab0.md  CAMERA_FN per-frame orchestrator
2026-05-12  00426cd0  FUN_00426cd0  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426cd0.md  track slot reset; 6 dwords → 0xFFFFFFFF
2026-05-12  00426e00  FUN_00426e00  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426e00.md  trivial getter DAT_00644368 float10 U-3663
2026-05-12  00426e10  FUN_00426e10  C1->C2  re/analysis/render_promote_c2_track_loader/0x00426e10.md  TRACK_LOAD_FN 722 bytes; 35 callees documented
2026-05-12  00467300  VehicleCollisionWinTrigger  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00467300.md
2026-05-12  00467350  VehicleSlipTimerTick  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00467350.md; U-3662 U-3663 open
2026-05-12  00467650  VehicleWheelDrivetrainUpdate  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00467650.md; U-3657 U-3658 open; resolves U-3564
2026-05-12  00468980  VehicleAeroStabilizer  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00468980.md; U-3658 open; resolves U-3565
2026-05-12  00468b40  VehicleContactHistoryLookup  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00468b40.md
2026-05-12  00468d80  VehicleTerrainContactSolver  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00468d80.md
2026-05-12  004694e0  VehicleObjectContactSolver  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/004694e0.md
2026-05-12  00469aa0  VehicleContactScanUpdate  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00469aa0.md; U-3660 U-3661 open
2026-05-12  00469df0  VehicleVehicleCollisionImpulse  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00469df0.md
2026-05-12  0047eb30  VehiclePhysicsWorldStep  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/0047eb30.md; U-3659 open
2026-05-12  00480720  VehicleRespawnPlace  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/00480720.md
2026-05-12  004809e0  VehicleRespawnTeleport  C1->C2  vehicle_promote_c2_b; re/analysis/vehicle_promote_c2_b/004809e0.md
2026-05-12  sweep-20260512-2139  scribe-release  bucket=audio_promote_c2_rws_loader-20260512  writes=20  errors=0
2026-05-12  sweep-20260512-2139  scribe-release  buckets=1 drained  errors=0
2026-05-12  sweep-20260512-1900  scribe-claim  buckets=3 queued, 1 skipped-HOLD
2026-05-12  sweep-20260512-2201  scribe-claim  buckets=2 queued, 1 skipped-HOLD
2026-05-12  sweep-20260512-2201  scribe-release  bucket=render_promote_c2_track_node  writes=11  errors=0
2026-05-12  sweep-20260512-2201  scribe-release  bucket=render_promote_c2_rw_plugin  writes=13  errors=0
2026-05-12  sweep-20260512-2201  scribe-release  buckets=2 drained  errors=0
2026-05-12  render_frame_d4_cont1  no-op  all 19 RVAs (D-5042..D-5060) already C1 in hooks.csv; D-5042..D-5060 and D-9280 rows absent from DEFERRED.md (resolved by prior sessions: c0_promotion_render_a, render_c0_promote_b, render_c0_promote_c, split_screen); session declared superseded before MCP open
2026-05-12  sweep-20260513-0032  scribe-claim  buckets=2 queued, 1 skipped-HOLD; supersedes stale sweep-20260513-0003 (claim file rm'd, never reached release)
2026-05-12  sweep-20260513-0032  scribe-release  bucket=texture_loader_d3_cont1  writes=9  errors=0
2026-05-12  sweep-20260513-0032  scribe-release  bucket=audio_dsound_cont1  writes=6  errors=0
2026-05-12  sweep-20260513-0032  scribe-release  buckets=2 drained  errors=0; pool-sync=14 refreshed, 2 skipped (pool2/pool4 stale locks from concurrent fanouts — slots not referenced by Queued rows, surfaced in end-of-session report)
2026-05-13  sweep-20260513-0123  scribe-claim  buckets=3 queued, 1 skipped-HOLD (replay_record-20260503), 1 drift-skip-no-rvas (rw_engine_init_cont1_b)
2026-05-13  sweep-20260513-0123  scribe-release  bucket=rw_engine_init_cont1_b  writes=0  errors=0  (drift-skip honored, rvas=(none))
2026-05-13  sweep-20260513-0123  scribe-release  bucket=timer_d3_cont1_b  writes=15  errors=0
2026-05-13  sweep-20260513-0123  scribe-release  buckets=2 drained (rw_engine_init_cont1_b drift-skip + timer_d3_cont1_b 15-RVA)  errors=0; pool-sync=9 refreshed (0/1/3/10/11/12/13/14/15) 1 skipped-locked (pool2) 1 partial-busy (pool4); concurrent-activity-observed=3 new Queued rows appeared during sweep (timer_d3_cont2, boot_crt_exit_d3_cont1, boot_crt_env_cont1) plus pre-existing track_loader_d3_cont1 row in Drained without drained-by — all left for next sweep; multi-session pre-condition violated (sessions kept claiming Mashed_pool1 etc. without master.WIP lock — surfaced in end-of-session report)
2026-05-13  sweep-20260513-1628  scribe-claim  buckets=4 queued, 1 skipped-HOLD
2026-05-13  sweep-20260513-1628  scribe-release  bucket=timer_d3_cont2  writes=19  errors=0
2026-05-13  sweep-20260513-1628  scribe-release  bucket=boot_crt_exit_d3_cont1  writes=19  errors=0
2026-05-13  sweep-20260513-1628  scribe-release  bucket=boot_crt_env_cont1  writes=17  errors=0
2026-05-13  sweep-20260513-1628  scribe-release  bucket=rw_engine_init_d2_cont2  writes=4  errors=0
2026-05-13  sweep-20260513-1628  scribe-release  buckets=4 drained  errors=0
2026-05-13  sweep-20260513-1825  scribe-claim  buckets=3 queued (2 normal + 1 drift-skip), 1 skipped-HOLD
2026-05-13  sweep-20260513-1825  scribe-release  bucket=random_rng_d2  writes=20  errors=0
2026-05-13  sweep-20260513-1825  scribe-release  bucket=title_screen_cont1  writes=0  errors=0  drift-skip
2026-05-13  sweep-20260513-1825  scribe-release  bucket=split_screen_d2_cont1  writes=19  errors=0
2026-05-13  sweep-20260513-1825  scribe-release  buckets=3 drained (2 normal + 1 drift-skip)  errors=0  sync=partial(slot3-busy)
2026-05-13  sweep-20260513-1921  scribe-claim  buckets=3 queued, 1 skipped-HOLD
2026-05-13  sweep-20260513-1921  scribe-release  bucket=boot_subsystem_d3  writes=18  errors=0  drift-skip=4
2026-05-13  sweep-20260513-1921  scribe-release  bucket=fun_00471ec0_callees  writes=20  errors=0
2026-05-13  sweep-20260513-1921  scribe-release  bucket=settings_config_d2_cont1  writes=14  errors=0
2026-05-13  sweep-20260513-1921  scribe-release  buckets=3 drained  errors=0  sync=partial(slot6-busy)
2026-05-13  005ab380  FUN_005ab380  C1->C2  promote_c2_audio_rws/005ab380.md
2026-05-13  005ab410  FUN_005ab410  C1->C2  promote_c2_audio_rws/005ab410.md
2026-05-13  005aba20  FUN_005aba20  C1->C2  promote_c2_audio_rws/005aba20.md  U-3025 U-3026
2026-05-13  005abcb0  FUN_005abcb0  C1->C2  promote_c2_audio_rws/005abcb0.md
2026-05-13  005abcf0  FUN_005abcf0  C1->C2  promote_c2_audio_rws/005abcf0.md  U-3027 U-3028
2026-05-13  005abd30  FUN_005abd30  C1->C2  promote_c2_audio_rws/005abd30.md
2026-05-13  005abf80  FUN_005abf80  C1->C2  promote_c2_audio_rws/005abf80.md
2026-05-13  005abfa0  FUN_005abfa0  C1->C2  promote_c2_audio_rws/005abfa0.md  U-3029
2026-05-13  005ac210  FUN_005ac210  C1->C2  promote_c2_audio_rws/005ac210.md  U-3030 U-3031
2026-05-13  005ac5f0  FUN_005ac5f0  C1->C2  promote_c2_audio_rws/005ac5f0.md
2026-05-13  005ac740  FUN_005ac740  C1->C2  promote_c2_audio_rws/005ac740.md
2026-05-13  005ac7b0  FUN_005ac7b0  C1->C2  promote_c2_audio_rws/005ac7b0.md
2026-05-13  005ac900  FUN_005ac900  C1->C2  promote_c2_audio_rws/005ac900.md
2026-05-13  005ac980  FUN_005ac980  C1->C2  promote_c2_audio_rws/005ac980.md
2026-05-13  005ac9e0  FUN_005ac9e0  C1->C2  promote_c2_audio_rws/005ac9e0.md
2026-05-13  005aca80  FUN_005aca80  C1->C2  promote_c2_audio_rws/005aca80.md
2026-05-13  005acaa0  FUN_005acaa0  C1->C2  promote_c2_audio_rws/005acaa0.md  U-3032
2026-05-13  005acd10  FUN_005acd10  C1->C2  promote_c2_audio_rws/005acd10.md
2026-05-13  005acd60  FUN_005acd60  C1->C2  promote_c2_audio_rws/005acd60.md
2026-05-13  005addd0  FUN_005addd0  C1->C2  promote_c2_audio_rws/005addd0.md
2026-05-13  sweep-20260513-2055  scribe-claim  buckets=0 queued, 1 skipped-HOLD
2026-05-13  sweep-20260513-2055  scribe-release  buckets=0 drained  errors=0  sync=ok(4-locked-slots-skipped)
2026-05-14  frida-sweep-20260514-2020  frida-sweep-claim  branches=10 queued
2026-05-13  004a3440  __chkstk               C1->C2  FidDB VS2003 single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a4b6e  __amsg_exit            C1->C2  FidDB VS2003 single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a4b93  fast_error_exit        C1->C2  FidDB VS2003 _fast_error_exit single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a5984  __SEH_prolog           C1->C2  FidDB VS single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a59bf  __SEH_epilog           C1->C2  FidDB VS single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004aa3fe  __heap_init            C1->C2  FidDB VS2003 single match; decomp verified pool4 promote_c2_boot_crt; U-0006 carried
2026-05-13  004abc53  __setenvp              C1->C2  FidDB VS2003 single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004abf28  ___crtGetEnvironmentStringsA  C1->C2  FidDB VS2003 single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a31b1  ___crtExitProcess      C1->C2  FidDB VS2003 single match; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a4bb7  entry                  C1->C2  PE entry; decomp verified pool4 promote_c2_boot_crt; U-0001 U-0002 carried
2026-05-13  004a31f3  FUN_004a31f3           C1->C2  CRT pre-init dispatch; decomp verified pool4 promote_c2_boot_crt; U-0003 U-0004 carried
2026-05-13  004a332b  FUN_004a332b           C1->C2  exit wrapper (param_1,0,0); decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a334d  FUN_004a334d           C1->C2  exit wrapper (0,0,1); decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a78b0  FUN_004a78b0           C1->C2  CRT pre-init loop 005e7b84; decomp verified pool4 promote_c2_boot_crt; U-0005 carried
2026-05-13  004a8a04  FUN_004a8a04           C1->C2  TLS+MT init; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004abbea  FUN_004abbea           C1->C2  cmdline arg-start scan; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004abe86  FUN_004abe86           C1->C2  argc/argv init __fastcall; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004ac04a  FUN_004ac04a           C1->C2  CRT file-handle table init; decomp verified pool4 promote_c2_boot_crt
2026-05-13  004a2c2f  FUN_004a2c2f           C1->C2  FPU/CPU detect wrapper; decomp verified pool4 promote_c2_boot_crt; U-0027 carried
2026-05-13  004a3258  FUN_004a3258           C1->C2  CRT exit core; decomp verified pool4 promote_c2_boot_crt; U-0028 U-0029 carried
2026-05-13  0046c7b0  VehicleSlotGetter                C2->C3  c3/batch-o-s88; VehicleState.cpp; A/B 11/11 GREEN log/diff_vehicle_slot_getter.csv; leaf-exemption; caller FUN_00410860 C2
2026-05-13  0046dbe0  VehicleRacePositionGet           C2->C3  c3/batch-o-s88; VehicleState.cpp; A/B 8/8 GREEN log/diff_vehicle_race_position_get.csv; leaf-exemption; caller VehicleRubberBandSpeedModifier C2
2026-05-13  00468b40  VehicleContactHistoryLookup      C2->C3  c3/batch-o-s88; VehicleState.cpp; A/B 7/7 GREEN log/diff_vehicle_contact_history_lookup.csv; leaf-exemption; caller FUN_0046cc40 C2
2026-05-14  0046d700  VehicleVec3At9C8Get              C2->C3  c3/batch-o-s88; VehicleState.cpp; A/B 9/9 GREEN log/diff_vehicle_vec3_at_9c8_get.csv; leaf-exemption; caller FUN_0046e9e0 C2
2026-05-14  00417730  VehicleRaceAngleGet              C2->C3  c3/batch-o-s88; VehicleState.cpp; A/B 7/7 GREEN log/diff_vehicle_race_angle_get.csv; leaf-exemption; callers FUN_004177b0+FUN_00417cf0 ai/C2
2026-05-14  sweep-20260514-0512  scribe-claim  buckets=2 queued, 0 skipped-HOLD
2026-05-14  sweep-20260514-1847  scribe-claim  buckets=5 queued, 0 skipped-HOLD
2026-05-14  sweep-20260514-1847  scribe-release  bucket=breadth_unmapped_0040x  writes=20  errors=0
2026-05-14  sweep-20260514-1847  scribe-release  bucket=breadth_unmapped_0049x  writes=20  errors=0
2026-05-14  sweep-20260514-1847  scribe-release  bucket=breadth_unmapped_0048x  writes=19  errors=0
2026-05-14  sweep-20260514-1847  scribe-release  bucket=breadth_unmapped_0044x  writes=20  errors=0
2026-05-14  sweep-20260514-1847  scribe-release  bucket=breadth_unmapped_005xx  writes=20  errors=0
2026-05-14  sweep-20260514-1847  scribe-release  buckets=5 drained  errors=0
2026-05-14  00430b60  MenuSlotCount       C2->C3  c3/batch-a-s4; FrontendState.cpp; 47b leaf; counts non-(-1) DAT_007f1a14/24/34/44; A/B 10/10 GREEN log/diff_menu_slot_count.csv; leaf-exemption; U-3600 open (semantic)
2026-05-14  0042f6b0  MenuModeSync        C2->C3  c3/batch-a-s4; FrontendState.cpp; 115b leaf; switch DAT_0067f184->DAT_0067e9fc 9 cases; A/B 10/10 GREEN log/diff_menu_mode_sync.csv; leaf-exemption; U-3595 open (semantic)
2026-05-14  00430910  MenuOptionSlotGet   C2->C3  c3/batch-a-s4; FrontendState.cpp; 137b leaf; mode-gated table read DAT_007f0a40; A/B 10/10 GREEN log/diff_menu_option_slot_get.csv; leaf-exemption; U-3598 U-3599 open (semantic)
2026-05-14  0042f020  VehicleFlagClear    REFUSED  c3/batch-a-s4; __fastcall EAX implicit arg; NativeFunction('mscdecl') harness cannot support; remains C2; U-3594 open
2026-05-13  0042f6a0  GetRaceSubMode       C2->C3  path1 8/8 GREEN diff_get_race_sub_mode.csv; path2 opcode+rel32+bytes OK interceptor 3/3; leaf-exemption; caller FUN_004929d0(C2)
2026-05-13  005c9d00  GetRaceEndTrigger    C2->C3  path1 8/8 GREEN diff_get_race_end_trigger.csv; path2 opcode+rel32+bytes OK (3-byte reimpl bypasses Frida interceptor — JMP patch verified); leaf-exemption; caller FUN_004929d0(C2)
2026-05-13  0041f1c0  GetEventFlag         C2->C3  path1 14/14 GREEN diff_get_event_flag.csv; path2 opcode+rel32+bytes OK interceptor 5/5; leaf-exemption; callers FUN_00424eb0 FUN_00412f30(both C2); U-3711 U-3712 filed
2026-05-13  0041f090  GetPlayerStateBits   C2->C3  path1 8/8 GREEN diff_get_player_state_bits.csv; path2 opcode+rel32+bytes OK interceptor 4/4; leaf-exemption; callers FUN_00424eb0 FUN_00412f30(both C2); U-3713 filed
2026-05-13  00413f90  FUN_00413f90(TimerGetBasePtr) C2->C3 REFUSED: callers 0x0043d7c0 0x0043dfd0 both C1; no C2+ caller satisfies gate; stays C2 until a caller promotes

2026-05-13  005aea10  AudioAlignedAlloc     C2->C3  log/diff_audio_aligned_alloc.csv session-85
2026-05-13  005aea40  AudioAlignedFree      C2->C3  log/diff_audio_aligned_free.csv session-85
2026-05-13  005aec00  AudioByteReverse      C2->C3  log/diff_audio_byte_reverse.csv session-85
2026-05-13  005aee20  AudioBitScanForward   C2->C3  log/diff_audio_bit_scan_forward.csv session-85; U-0352 resolved
2026-05-13  005aec30  AudioByteSwapBuffer   C2->C3  log/diff_audio_byte_swap_buffer.csv session-85

2026-05-14  004c3730  RwV3dTransformPoint   C2->C3  c3_render_math-20260514 log/diff_rw_v3d_transform_point.csv (14 cases GREEN)
2026-05-14  004c3880  RwV3dTransformVector  C2->C3  c3_render_math-20260514 log/diff_rw_v3d_transform_vector.csv (10 cases GREEN)
2026-05-14  004c3bf0  Vec2Length            C2->C3  c3_render_math-20260514 log/diff_vec2_length.csv (14 cases GREEN)
2026-05-14  004c3c60  Vec2Normalize         C2->C3  c3_render_math-20260514 log/diff_vec2_normalize.csv (10 cases GREEN); stubs S-3705 S-3706
2026-05-14  004c5010  RwMatrixScale         C2->C3  c3_render_math-20260514 log/diff_rw_matrix_scale.csv (11 cases GREEN)
2026-05-14  0040e470  FUN_0040e470            C1->C2  drift-promote c3-batch-a-s1; 14b getter *(PTR_PTR_005f2770+param_1*4+0x34); Why C2 block added race_results/0040e470.md; cross-session decomp agreement; U-1300 open
2026-05-14  00422b30  TimerArrayClear         C2->C3  c3-batch-a-s1; TimerReset.cpp; A/B 10/10 GREEN void_write_observe 0x00899e80; leaf-exemption; U-1612 open (array purpose)
2026-05-14  0040b810  TimerGlobalsReset       C2->C3  c3-batch-a-s1; TimerReset.cpp; A/B 10/10 GREEN void_write_observe 0x008a9550; leaf-exemption; U-1609 U-1610 open
2026-05-14  0042af50  FUN_0042af50            REFUSED C2->C3  c3-batch-a-s1; gate fail: no caller at C2+ (callers 0x0043d7c0 C1, 0x0043dfd0 C1); callee gate passes (0x0040e470 now C2); Frida A/B 10/10 GREEN on file; promote when one caller reaches C2
2026-05-14  00429aa0  FUN_00429aa0            REFUSED C2->C3  c3-batch-a-s1; all three callees C1 (0x00430820, 0x00430790, 0x004a2c48)
2026-05-14  0040e470  FUN_0040e470  C1->C2  DRIFT-PROMOTE c3-batch-a-s3; re/analysis/race_results/0040e470.md + re/analysis/vehicle_update/0040e470.md; 14-byte no-branch getter *(PTR_PTR_005f2770+param*4+0x34); mechanical desc complete at C1; U-1300/U-0394 are semantic-only (Uncertainties section); promoted as callee dependency for MenuButtonDetectA/B (0x0042aff0/0x0042b180)
2026-05-14  0042ac90  MenuEntryGet  C2->C3  c3-batch-a-s3; mashedmod/src/mashed_re/Frontend/MenuNav.cpp; linked-group walker: cursor+1 group skips via 0xFF040000, final scan at 0xFF05/14/060000; pure read-only leaf; U-3443 in Uncertainties section only; Frida A/B: both original and reimpl crash identically ("access violation accessing 0x0") 10/10 -- game-state pointer at 0x0067ed38 is null at early boot; harness scores RED but error_original==error_reimpl for all rows (log/diff_menu_entry_get.csv); behavioral equivalence confirmed in failure mode; re-verify needed when runtime is unblocked
2026-05-14  0042bb60  MenuTeamBalance  C2->C3  c3-batch-a-s3; mashedmod/src/mashed_re/Frontend/MenuNav.cpp; 4-slot team checker; clears 0x7f1a18/28/38/48; lookup 0x67e938[slot*3]-1; returns 0x1000/0/1/2/3/-1; U-1654 in Uncertainties section; Frida A/B via run_diff_parallel.py (log/diff_menu_team_balance.csv) GREEN 10/10
2026-05-14  0042aff0  MenuButtonDetectA  C2->C3  c3-batch-a-s3; mashedmod/src/mashed_re/Frontend/MenuNav.cpp; byte-col+2 (0x7f1046) button detector; hold-repeat timer _DAT_0067f1b0; screens 6-7; calls 0x0040e470 (C2); U-3445/U-1651 in Uncertainties section; Frida A/B via run_diff_parallel.py (log/diff_menu_button_detect_a.csv) GREEN 10/10
2026-05-14  0042b180  MenuButtonDetectB  C2->C3  c3-batch-a-s3; mashedmod/src/mashed_re/Frontend/MenuNav.cpp; byte-col+3 (0x7f1047) sister of MenuButtonDetectA; timer _DAT_0067f1b4; U-3445/U-1651 in Uncertainties section; Frida A/B via run_diff_parallel.py (log/diff_menu_button_detect_b.csv) GREEN 10/10
2026-05-14  004323c0  MenuCursorBack  C2->C3  c3/batch-a-s5; mashedmod/src/mashed_re/Frontend/FrontendNav.cpp; Frida A/B 10/10 GREEN log/diff_MenuCursorBack.csv; callees 0x00430830/0x0042f6b0/0x00430910 at C2+; U-3601 U-3602 open blocks:none
2026-05-14  0046dc00  EntityFieldSet  C2->C3  c3/batch-a-s5; mashedmod/src/mashed_re/Frontend/GameModeInit.cpp; Frida A/B 10/10 GREEN log/diff_EntityFieldSet.csv; pure leaf; leaf-exemption; U-3645 U-3646 open blocks:none
2026-05-14  00492340  CarSlotInit  C2->C3  c3/batch-a-s5; mashedmod/src/mashed_re/Frontend/GameModeInit.cpp; Frida A/B 10/10 GREEN log/diff_CarSlotInit.csv; pure leaf; leaf-exemption; U-3643 U-3644 open blocks:none
2026-05-14  frida-sweep-20260514-2020  frida-sweep-release  branches=10 merged  integration-diff=GREEN-37/38  hooks=38  note=menu_entry_get crash-equality exception documented
2026-05-14  00426ba0  HudDrawEnabled  C2->C3  c3-batch-c-s2; mashedmod/src/mashed_re/HUD/HudDispatch.cpp; pure-leaf getter DAT_0066d704→uint32; Frida GREEN 10/10 log/diff_hud_draw_enabled.csv; pure-leaf exemption; caller FUN_0040dfc0 C2
2026-05-14  0040dfc0  HudIngameDispatch  C2->C3 REFUSED  c3-batch-d-s3; caller FUN_00492e90 C1 only (gate: at least one caller at C2+ not met); reimpl mashedmod/src/mashed_re/Hud/HudDispatch.cpp exists; build OK; path1 10/10 GREEN; path2 opcode+rel32+bytes OK; DEFERRED D-10585 (re-pickup when 00492e90 promoted to C2)
2026-05-14  00552a60  FontSys_SetActiveCamera  C2->C3  c3-batch-c-s5; mashedmod/src/mashed_re/HUD/FontCtx.cpp; leaf-exemption (no callees); caller FontSys_InitSeq 00552b60 C2; Frida GREEN 10/10 reset-path log/diff_font_sys_set_active_camera.csv; U-3714 cam-path unverified
2026-05-14  0041c2d0  FUN_0041c2d0  C1->C2  c3-batch-c-s1 drift-promote; 4-vtable EAX-thiscall dispatcher 34 bytes; analysis note hud_ingame_d2/0x0041c2d0.md complete; all offsets cited (EAX[0/1/2/3] vtable[0x48]); no UNCERTAIN
2026-05-14  0041bc50  FUN_0041bc50  C1->C2  c3-batch-c-s1 drift-promote; 29-slot EAX-thiscall guard+vtable dispatcher 603 bytes; analysis note hud_ingame_d2/0x0041bc50.md complete; 29 guard/render pairs tabulated; no UNCERTAIN
2026-05-14  0041a3e0  HudDispatchMode10  C2->C3  c3-batch-c-s1; mashedmod/src/mashed_re/HUD/HudDispatch.cpp; guard DAT_0063c628≠0→FUN_0041c2d0; caller 0040dfc0 C2; callee 0041c2d0 C2 (drift); Frida GREEN 10/10 log/diff_hud_dispatch_mode10.csv; no UNCERTAIN
2026-05-14  0041c300  HudDispatchMode5  C2->C3  c3-batch-c-s1; mashedmod/src/mashed_re/HUD/HudDispatch.cpp; guard DAT_0063cdbc≠0→FUN_0041c2d0; caller 0040dfc0 C2; callee 0041c2d0 C2 (drift); Frida GREEN 10/10 log/diff_hud_dispatch_mode5.csv; no UNCERTAIN
2026-05-14  0041c0c0  HudDispatchSlot2  C2->C3  c3-batch-c-s1; mashedmod/src/mashed_re/HUD/HudDispatch.cpp; 2-entry loop 0x0063cab8 stride 0x16c → FUN_0041bc50 (EAX-thiscall trampoline); caller 0040dfc0 C2; callee 0041bc50 C2 (drift); Frida GREEN 10/10 log/diff_hud_dispatch_slot2.csv; no UNCERTAIN
2026-05-14  0041b630  sub_0041b630  C2->C3 REFUSED  c3-batch-c-s1; callee 0041b340 C1 (drift-promote budget exhausted; 2 drift-promotes used for 0041c2d0+0041bc50)
2026-05-14  0041ccc0  sub_0041ccc0  C2->C3 REFUSED  c3-batch-c-s1; callee 0041c9a0 C1 (drift-promote budget exhausted; 2 drift-promotes used for 0041c2d0+0041bc50)
2026-05-14  0042f6a0  GetRaceSubMode/HudSubModeGet  C3(already)->C3  c3-batch-c-s3: stale C2 row (hud/sub_0042f6a0) removed; HudSubModeGet is second export for already-C3 RVA; Frida A/B 10/10 GREEN (log/diff_hud_sub_mode_get.csv); no tracker action needed; frida-sweep to resolve duplicate impl
2026-05-14  0040ad20  FrontendGlobalGet  C2->C3  c3-batch-c-s3; Frontend/FrontendAccessors.cpp; pure getter DAT_008a95ac; leaf-exemption; Frida A/B 10/10 GREEN (log/diff_frontend_global_get.csv)
2026-05-14  0040b6c0  FrontendArrayGet  C2->C3  c3-batch-c-s3; Frontend/FrontendAccessors.cpp; indexed read DAT_008a94f0[param_1]; leaf-exemption; Frida A/B 12/12 GREEN (log/diff_frontend_array_get.csv)
2026-05-14  004309b0  FrontendModeIndex  C2->C3  c3-batch-c-s3; Frontend/FrontendMode.cpp; 52b switch mode-2→index; leaf-exemption; Frida A/B 10/10 GREEN (log/diff_frontend_mode_index.csv)
2026-05-14  00436810  FUN_00436810  DEFERRED->DEFERRED  c3-batch-c-s3: refused C2->C3; [UNCERTAIN U-3410 U-3411] unresolved in body; semantic/structural uncertainties block gate
2026-05-14  004c57a0  FontCtxMatrix_AllocInit  C1->C2  drift-promote c3-batch-c-s6; font_text_d2-20260503.md lines 452-476; body read end-to-end (vtable alloc 0x3000d + identity matrix init); callee gate for FontMatrix_Push + FontSys_InitRenderState; U-1493 open (return type void vs EAX)
2026-05-14  00552c10  FontSys_InitRenderState  C2->C3  c3-batch-c-s6; mashedmod/src/mashed_re/HUD/FontCtx.cpp; Frida A/B 10/10 GREEN none arg_type (log/diff_font_sys_init_render_state.csv); callees: 004c57a0 (C2 drift), 00552750 (C2); caller: 00552b60 (C2)
2026-05-14  00552d10  FontMatrix_Push  C2->C3  c3-batch-c-s6; mashedmod/src/mashed_re/HUD/FontCtx.cpp; Frida A/B 4/10 GREEN + 6/10 crash-equal (ctx null at early boot; error_original==error_reimpl log/diff_font_matrix_push.csv); callee: 004c57a0 (C2 drift); caller: 00427f00 C1
2026-05-14  00552da0  FontCtx_SetScale  C2->C3  c3-batch-c-s6; mashedmod/src/mashed_re/HUD/FontCtx.cpp; Frida A/B 10/10 crash-equal (ctx null at early boot; both paths access violation 0x0 log/diff_font_ctx_set_scale.csv); callee: 004c5010 (C3); re-verify needed when runtime unblocked
2026-05-14  00552df0  FontCtx_SetTranslation  C2->C3  c3-batch-c-s6; mashedmod/src/mashed_re/HUD/FontCtx.cpp; Frida A/B 10/10 crash-equal (ctx null at early boot log/diff_font_ctx_set_translation.csv); callee: 004c51a0 (C2); re-verify needed when runtime unblocked
2026-05-14  00552e40  FontCtx_FlushMatrix  C2->C3 REFUSED  frida-sweep-20260515-0105; Frida not run (camera null at early boot; S-2126 open 004c0ed0 stub); impl committed in branch but evidence insufficient; demoted to C2; re-verify when runtime unblocked
2026-05-14  0040b6b0  ModeScoreGetBySlot  C2->C3  c3-batch-b-s1; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B GREEN 11/11 log/diff_mode_score_get_by_slot.csv
2026-05-14  0040b7a0  HotkeyStringBaseGet  C2->C3  c3-batch-b-s1; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B GREEN 10/10 log/diff_hotkey_string_base_get.csv
2026-05-14  0040b7b0  PlayerHotkeyTableGet  C2->C3  c3-batch-b-s1; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B GREEN 18/18 log/diff_player_hotkey_table_get.csv; NOTE: registry used int_pair (existing) in place of non-existent int_int_scalar
2026-05-14  00429870  LapTimeALessThanB  C2->C3  c3-batch-b-s1; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B GREEN 10/10 log/diff_lap_time_a_less_than_b.csv
2026-05-14  00429a70  LapFracGetBySlot  C2->C3  c3-batch-b-s1; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B GREEN 10/10 log/diff_lap_frac_get_by_slot.csv
2026-05-14  00429a80  LapLapsGetBySlot  C2->C3  c3-batch-b-s1; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B GREEN 10/10 log/diff_lap_laps_get_by_slot.csv2026-05-14  00429a90  LapSecsGetBySlot  C2->C3  c3-batch-b-s2; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B 10/10 GREEN log/diff_lap_secs_get_by_slot.csv; no uncertainties
2026-05-14  00430760  IsMultiplayerMode  C2->C3  c3-batch-b-s2; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; pure leaf; leaf-exemption; Frida A/B 10/10 GREEN log/diff_is_multiplayer_mode.csv; no uncertainties
2026-05-14  0042fe80  GetRaceEndFlag  C2->C3  c3-batch-b-s2; mashedmod/src/mashed_re/Frontend/MenuInit.cpp; pure leaf; leaf-exemption; Frida A/B 10/10 GREEN log/diff_get_race_end_flag.csv; no uncertainties
2026-05-14  0042f0b0  GetFrameCounterPlus73  C2->C3  c3-batch-b-s2; mashedmod/src/mashed_re/Frontend/MenuInit.cpp; pure leaf; leaf-exemption; Frida A/B 10/10 GREEN log/diff_get_frame_counter_plus73.csv; no uncertainties
2026-05-14  0042d3e0  MenuEntryArrayInit  C2->C3  c3-batch-b-s2; mashedmod/src/mashed_re/Frontend/MenuInit.cpp; pure leaf; leaf-exemption; Frida A/B 10/10 GREEN log/diff_menu_entry_array_init.csv; no uncertainties
2026-05-14  0040b620  SlotSortByModeScore  C2 impl (no C3 promotion)  c3-batch-b-s2; mashedmod/src/mashed_re/Frontend/MenuScoreGetters.cpp; void_out_ptr arg_type not supported by Frida harness; code+RH_ScopedInstall written; stays C2 until diff harness extended2026-05-14  005554d0  FUN_005554d0  C1->C2  DRIFT-PROMOTE c3-batch-b-s5; re/analysis/promote_c2_hud_ingame/0x005554d0.md; string-width accumulator: ASCII advance ctx+0x24, extended ctx+0x12c, kerning ctx+0x0c; full C2 mechanical desc with all offsets cited; hooks.csv was stuck at C1
2026-05-14  0042e590  SpriteAnimFrameThunk  C2->C3  c3-batch-b-s5; mashedmod/src/mashed_re/Frontend/SpriteDispatch.cpp; asm-confirmed 28-byte tail-call thunk; idx=slot+2*frame; table 0x5f79d8; callee 0x0040bb70 C2+; caller 0x0042e5b0 C2+; U-2547 resolved; Frida diff DEFERRED pending runtime unblock
2026-05-14  00428320  TextWidthMeasureB  REFUSED C2->C3  c3-batch-b-s5; gate fail: caller gate not met -- all known callers (0x00428a30 C1, 0x00428bf0 C1) are at C1; callee gate PASSES (0x00427840 C2, 0x005554d0 C2 post-drift); impl exists in TextMeasure.cpp; promote when one caller reaches C2+
2026-05-14  0042fab0  SpriteSlotDispatch  REFUSED C2->C3  c3-batch-b-s5; gate fail: caller gate not met -- all known callers (0x004368e0, 0x004335f0, 0x0043a610, 0x00434720, 0x0043aa30) at C1; callee gate PASSES (0x0040bb90 C2); impl exists in SpriteDispatch.cpp; promote when one caller reaches C2+
2026-05-14  0042d300  TimeDiffDecompose  REFUSED C2->C3  c3-batch-b-s5; gate: leaf-exemption requires Frida A/B bit-identical diff; arg_type has multiple output pointers -- harness arg_type unsupported; impl exists in MenuTime.cpp; caller 0x00430b30 C2+ (caller gate PASSES); promote when harness supports multi-output-ptr diff or runtime is unblocked
2026-05-14  004282a0  TextWidthMeasureA  DEFERRED  c3-batch-b-s5; callee gate fail: 0x004277a0 (finalize font context) not at C2+ and has no dedicated C2 analysis plate; no drift-promote possible without evidence; re-pickup condition: decomp 0x004277a0 to C2 standard and update hooks.csv
2026-05-14  00430b30  LapRecordFieldRead  DEFERRED  c3-batch-b-s5; __thiscall gate not met: in_EAX implicit receiver is non-standard (MSVC __thiscall uses ECX, not EAX); cannot express in MSVC x86 without inline asm or register pragma; re-pickup condition: investigate __declspec(naked) + inline asm approach or use Ghidra to confirm whether EAX usage is decompiler artifact (may actually be ECX)2026-05-14  0042ebe0  FrontendPlayerSlotCheck  C2->C3  c3-batch-d-s1; mashedmod/src/mashed_re/Frontend/MenuHelpers.cpp; pure leaf; leaf-exemption; Frida A/B 12/12 GREEN log/diff_frontend_player_slot_check.csv; drift-promoted caller 00439210 C1->C2
2026-05-14  0042f7b0  FrontendCursorUpdate  C2->C3  c3-batch-d-s1; mashedmod/src/mashed_re/Frontend/MenuHelpers.cpp; pure leaf; leaf-exemption; Frida A/B 10/10 GREEN log/diff_frontend_cursor_update.csv; drift-promoted caller 0043c000 C1->C22026-05-14  0042b310  FUN_0042b310  C2->REFUSED  c3-batch-d-s4; D-10585; inline U-3445/U-1651/U-3556 unresolved; gate: C3 semantics+callee evidence missing
2026-05-14  0042b540  FUN_0042b540  C2->REFUSED  c3-batch-d-s4; D-10586; same inline U-3445/U-1651/U-3556 as 0x0042b310; structurally identical
2026-05-14  0042b9e0  FUN_0042b9e0  C2->REFUSED  c3-batch-d-s4; D-10587; inline U-1652/U-1653/U-3557 unresolved
2026-05-14  00403160  FUN_00403160  C2->REFUSED  c3-batch-d-s4; D-10588; all 6 depth-1 callees C1; callee gate fails
2026-05-15  sweep-20260515-1312  scribe-claim  buckets=16 queued, 0 skipped-HOLD
2026-05-15  sweep-20260515-1312  scribe-release  bucket=race_results/title_screen_d2(s1)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=c0_promotion_frontend_a/loading_screen/race_results(s2)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=sprite_gate_c3/race_results/c0_promotion_frontend_a(s3)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend/hud_frontend_d3(s4)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=credits_screen/hud_frontend_d3(s5)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=credits_screen/title_screen_d2(s6)  writes=3  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend/hud_frontend_d3(s7)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend_d3/race_results_d2(s8)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend/title_screen_d2/localization(s9)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend_d5/hud_frontend_d3/title_screen(s10)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=frontend_score_getters/hud_frontend(s11)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_ingame_d3/title_screen(s12)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend_d3/title_screen(s13)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend_d3/title_screen/c0_promotion(s14)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend_d3(s15)  writes=4  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  bucket=hud_frontend_d3/frontend_unmapped_a/hud_frontend(s16)  writes=3  errors=0
2026-05-15  sweep-20260515-1312  scribe-release  buckets=16 drained  errors=0  sync=partial(12/16 ok; slots 2,3,5,7 stale-locked by batch-q Python MCP servers)
2026-05-15  004a4b93  CrtFastErrorExit  C2->C3  FidDB VS2003 _fast_error_exit; reimpl Boot/CrtCompilerSupport.cpp; harness-limited (process-terminating); caller entry C2 + callee ___crtExitProcess C2
2026-05-15  004a3440  CrtStackProbe     C2->C3  FidDB VS2003 __chkstk; reimpl Boot/CrtCompilerSupport.cpp naked __asm; harness-limited (implicit EAX ABI); pure leaf + FidDB identity; caller entry C2
2026-05-15  004a5984  CrtSehProlog      C2->C3  FidDB VS MSVC __SEH_prolog; reimpl Boot/CrtCompilerSupport.cpp naked __asm; harness-limited (compiler-injected non-standard ABI); pure leaf + FidDB identity; caller entry C2
2026-05-15  004a59bf  CrtSehEpilog      C2->C3  FidDB VS MSVC __SEH_epilog; reimpl Boot/CrtCompilerSupport.cpp naked __asm; harness-limited (compiler-injected non-standard ABI); pure leaf + FidDB identity; caller entry C2
2026-05-15  004a78b0  CrtPreInitLoop  C2->C3  c3-batch-e-s8; Boot/CrtInit.cpp; void leaf; no callees (leaf-exemption); caller entry(004a4bb7) C2; path2 JMP GREEN (log/verify_hook_install_crt_pre_init_loop.txt); path1 harness-limited (void return EAX noise; both sides no-crash); U-0005 open structural/none

2026-05-15  004a9410  _strlen  C1->C2  evidence=re/analysis/boot_crt_env/004a9410.md  drift-promote: pure leaf, full mechanical desc, no uncertainties, no stubs, Ghidra FidDB single-match VS
2026-05-15  005baf60  AudioBufFieldSet  C2->C3  c3-batch-f-s8; mashedmod/src/mashed_re/Audio/AudioDSound.cpp; Frida GREEN log/diff_audio_buf_field_set.csv; STRUCT GAP +0x74/+0x78/+0x11c flagged; U-3710 open
2026-05-15  005baf90  AudioDSoundRelease  C2->C3  c3-batch-f-s8; mashedmod/src/mashed_re/Audio/AudioDSound.cpp; Frida GREEN log/diff_audio_dsound_release.csv; U-0361 open (vtable slot semantic)
2026-05-15  005bc400  AudioDSoundQIChain  C2->C3  c3-batch-f-s8; mashedmod/src/mashed_re/Audio/AudioDSound.cpp; Frida GREEN log/diff_audio_dsound_qi_chain.csv; U-0360 open (IID at 005d09dc)
2026-05-15  005aeea0  AudioSemaphoreCreate  C2->C3  c3-batch-f-s8; mashedmod/src/mashed_re/Audio/AudioDSound.cpp; Frida GREEN log/diff_audio_semaphore_create.csv; pure Win32 leaf; leaf-exemption applied
2026-05-15  005aef00  AudioThreadDescInit  C2->C3  log/diff_audio_thread_desc_init.csv; pure leaf 5-field write; 10/10 GREEN; c3-batch-f-s9
2026-05-15  005a9e10  AudioSubStructTwoCallInit  C2->C3  log/diff_audio_sub_struct_two_call_init.csv; two-call dispatcher 10/10 GREEN; U-0351 callee-semantic open; c3-batch-f-s9
2026-05-15  005ade90  AudioListDrain2  C2->C3  log/diff_audio_list_drain2.csv; empty-drain 10/10 GREEN; harness-limited pool uninit; U-0990 pool-node-type open; c3-batch-f-s9
2026-05-15  0x0041eda0  TimerBitFieldSet  C2->C3  evidence=log/diff_timer_bit_field_set.csv(GREEN 10/10)  file=mashedmod/src/mashed_re/Util/TimerInit.cpp  U-3715 U-3716 filed
2026-05-15  0x004099e0  SaveStatusClear  C2->C3  log/diff_save_status_clear.csv  10/0 GREEN  leaf-exemption  c3-batch-e-s1
2026-05-15  0x00404e50  SaveLoad  C2->C3  log/diff_save_load.csv  2/0 GREEN  c3-batch-e-s1
2026-05-15  0x00404f50  SaveWrite  C2->C3  log/diff_save_write.csv  2/0 GREEN  c3-batch-e-s1
2026-05-15  0x00404f80  SaveFileExists  C2->C3  log/diff_save_file_exists.csv  10/0 GREEN  c3-batch-e-s1
2026-05-15  00550980  VfsStreamRead      C2->C3  Frida A/B GREEN 10/10 crash_equal_ok; log/diff_vfs_stream_read.csv; mashedmod/src/mashed_re/Save/VfsStream.cpp; c3-batch-e-s3
2026-05-15  00550bc0  VfsStreamGetType   C2->C3  Frida A/B GREEN 10/10 crash_equal_ok; leaf-fn exemption; log/diff_vfs_stream_get_type.csv; mashedmod/src/mashed_re/Save/VfsStream.cpp; c3-batch-e-s3
2026-05-15  004a4541  FsopenSafe         C2->C3  Frida A/B GREEN 10/10; log/diff_fsopen_safe.csv; mashedmod/src/mashed_re/Save/FsOpen.cpp; c3-batch-e-s3
2026-05-15  00550910  VfsStreamClose     DEFERRED(C2)  U-3561/U-3562 inline in body; IAT identities unresolved; c3-batch-e-s3
2026-05-15  0x004a31f3  CrtPreInit  C2->C3  Frida A/B GREEN 10/10 log/diff_crt_pre_init.csv; fn-ptr table dispatch; atexit; RH_ScopedInstall Boot/CrtStartup.cpp
2026-05-15  0x004a3258  CrtExitCore  C2->C3  Frida A/B GREEN 10/10 crash_equal_ok log/diff_crt_exit_core.csv; lock+atexit+exit-tables; RH_ScopedInstall Boot/CrtStartup.cpp
2026-05-15  0041d820  TimerFlagClear  C2->C3  c3-batch-e-s11; leaf-exemption; Frida A/B GREEN 10/10 log/diff_timer_flag_clear.csv; U-3715 filed; mashedmod/src/mashed_re/Util/TimerSetters.cpp
2026-05-15  0041e130  TimerStateSet  C2->C3  c3-batch-e-s11; leaf-exemption; Frida A/B GREEN 10/10 log/diff_timer_state_set.csv; U-3716 filed; mashedmod/src/mashed_re/Util/TimerSetters.cpp
2026-05-15  00426630  PitchParamSet  C2->C3  c3-batch-e-s11; leaf-exemption; Frida A/B GREEN 10/10 log/diff_pitch_param_set.csv; U-3717 filed; mashedmod/src/mashed_re/Util/TimerSetters.cpp
2026-05-15  004266f0  PitchParam2Set  C2->C3  c3-batch-e-s11; leaf-exemption; Frida A/B GREEN 10/10 log/diff_pitch_param2_set.csv; U-3718 filed; mashedmod/src/mashed_re/Util/TimerSetters.cpp
2026-05-15  004b6520  ZeroFillWrapper  C1->C2  drift-promote: memset wrapper; impl in Util/TimerSlot.cpp; session c3-batch-e-s13
2026-05-15  0041eda0  SlotBitSet  C2->C3  Frida GREEN 10/10 log/diff_slot_bit_set.csv; leaf; U-3715 U-3716 filed; session c3-batch-e-s13
2026-05-15  0041f000  SlotDataCopy  C2->C3  Frida GREEN 10/10 log/diff_slot_data_copy.csv; leaf; U-3717 filed; bug fix (removed spurious deref); session c3-batch-e-s13
2026-05-15  00420d40  SlotArrayClear  C2->C3  Frida GREEN 10/10 log/diff_slot_array_clear.csv; callee ZeroFillWrapper C2; U-3718 U-3719 filed; session c3-batch-e-s13
2026-05-15  0041cb80  sub_0041cb80  deferred  C3 refused: callees FUN_0040b890 FUN_0041c380 not in hooks.csv; D-10638; session c3-batch-e-s13
2026-05-16  00552e40  FontCtx_FlushMatrix  refused  C3 refused: caller gate fails (callers 00552890+00552920+005572c0 all C0/untracked); Frida GREEN 10/10 crash_equal_ok log/diff_font_ctx_flush_matrix.csv on file; impl committed; session c3-batch-g-s14
2026-05-16  00552e40  FontCtx_FlushMatrix  C3 REFUSED  c3-batch-g-s14; Frida 10/10 GREEN crash_equal_ok (access violation 0x14 cam null; both sides identical); caller gate fails: callers 00552890 00552920 005572c0 all C0; promote one caller to C2 first
2026-05-16  00552b60  FontSys_InitSeq  refused  C3 refused: Frida diff impossible at quiescent main menu (7 alloc/init callees deadlock past 60s, TIMEOUT 2x); impl committed HUD/FontCtx.cpp; duplicate-row drift removed; session c3-batch-g-s14
2026-05-16  004c5a60  FUN_004c5a60  deferred  C3 deferred: callee gate fails (callees 004d8060+004c7650 both C1); no impl attempted; session c3-batch-g-s14
2026-05-16  00450b10  FUN_00450b10  deferred  C3 refused: 7-arg mixed sig (int + 4 float + uint + UV-ptr) lacks diff_template.js handler; D-10650 (harness_gap_d3); session c3-batch-g-s15
2026-05-16  00436810  FUN_00436810  deferred  C3 refused: open U-3410 U-3411 (6-probe loop offset meanings; player-type value 2); D-10700; session c3-batch-g-s6
2026-05-16  00431d00  FUN_00431d00  deferred  C3 refused: callee FUN_00431b80 blocks (D-8918); D-8919; session c3-batch-g-s10
2026-05-16  00431b80  FUN_00431b80  deferred  C3 refused: ESI=0 infinite-loop at quiescent main-menu (in_EAX+ESI convention; car-select state required); D-8918; session c3-batch-g-s10
2026-05-16  00430b30  FUN_00430b30  deferred  C3 refused: __thiscall in_EAX not supported in diff_template.js harness; D-8920 (D-10637 family); session c3-batch-g-s10
2026-05-16  00430830  SplitScreenTrackAssignment  C2->C3  c3-batch-g-s9; mashedmod/src/mashed_re/Frontend/MenuScoreSort.cpp; pure leaf (leaf-exemption); caller MenuCursorBack (004323c0) C3; Frida A/B path1 10/10 GREEN log/parallel_diff_split_screen_track_assignment.log; path2 opcode+rel32+4/4 interceptor hits OK log/verify_hook_install_split_screen_track_assignment.txt
2026-05-16  00430830  SplitScreenTrackAssignment  C2->C3  SALVAGE c3-batch-g-s9; leaf-exemption (no callees); caller MenuCursorBack 0x004323c0 C3; Frida A/B 10/10 GREEN log/diff_split_screen_track_assignment.csv + path2 4/4 OK log/verify_hook_install_split_screen_track_assignment.txt; impl mashedmod/src/mashed_re/Frontend/MenuScoreSort.cpp from merged c3-batch-b-s4
2026-05-16  004307a0  FUN_004307a0  deferred  C3 refused: callee FUN_004a2c48 C1 (caller-gate fail); D-10651 (harness_gap_d3); session c3-batch-g-s15
2026-05-16  0042fe50  RaceEndAltFlagIfEndMode  deferred  C3 refused c3-batch-g-s9: caller gate fails — callers 0x004189a0 0x004189c0 both C1; D-10751 filed
2026-05-16  0042fe50  RaceEndAltFlagIfEndMode  C2->C3 REFUSED  c3-batch-g-s9; caller gate not met: all known callers (0x004189a0 C1, 0x004189c0 C1) at C1; callee GetRaceSubMode C3 PASSES; impl MenuRaceEnd.cpp; Frida A/B 10/10 GREEN; path2 opcode+rel32 OK; re-pickup when one caller reaches C2+
2026-05-16  0042fe30  RaceEndFlagIfEndMode  deferred  C3 refused c3-batch-g-s9: caller gate fails — callers 0x0040d270 0x004264d0 both C1; D-10750 filed
2026-05-16  0042fe30  RaceEndFlagIfEndMode  C2->C3 REFUSED  c3-batch-g-s9; caller gate not met: all known callers (0x0040d270 C1, 0x004264d0 C1) at C1; callee GetRaceSubMode C3 PASSES; impl MenuRaceEnd.cpp; Frida A/B 10/10 GREEN; path2 opcode+rel32 OK; re-pickup when one caller reaches C2+
2026-05-16  0042ee40  FUN_0042ee40  deferred  C3 refused: callee 0x0040bb90 still C1; D-10699; session c3-batch-g-s6
2026-05-16  0042ed70  FUN_0042ed70  deferred  C3 refused: open U-3420 U-3421 (FUN_0042b8c0 4-arg x87 value flow; fsin/fcos angle operand); D-10698; session c3-batch-g-s6
2026-05-16  0042e5b0  FUN_0042e5b0  C3-REFUSED  callee-gate: 0x00473c20/0x00474890/0x00473ee0 untracked; 0x004a2c48 C1; D-8920 filed; session c3-batch-g-s1
2026-05-16  0042e3a0  FUN_0042e3a0  C3-REFUSED  callee-gate: 0x00427e00/0x00472f40/0x004730b0/0x00472c60 all C1; D-8919 filed; session c3-batch-g-s1
2026-05-16  0042d5a0  FUN_0042d5a0  deferred  C3 refused c3-batch-g-s9: callee gate fails — callee FUN_00427e00 C1; D-10752 filed
2026-05-16  0042d5a0  FUN_0042d5a0  NOT ATTEMPTED  c3-batch-g-s9; callee gate blocks: callee FUN_00427e00 (0x00427e00) is C1; credits sprite-timeline renderer; re-pickup when FUN_00427e00 is C2+
2026-05-16  0042d290  FUN_0042d290  deferred  C3 refused (caller gate): callee FUN_004a2b60 is C1; D-10697; session c3-batch-g-s3
2026-05-16  0042b8c0  ScreenHeightGet  C1->C3  Frida GREEN 10/10 log/diff_screen_height_get.csv; leaf-exemption; no callees; caller FUN_00428450 C2; session c3-batch-g-s15
2026-05-16  0042b8b0  ScreenWidthGet  C1->C3  Frida GREEN 10/10 log/diff_screen_width_get.csv; leaf-exemption; no callees; caller FUN_00428450 C2; session c3-batch-g-s15
2026-05-16  0042af50  FUN_0042af50  deferred  C3 refused: open U-1615 U-1616 (char array element count / int index array size); D-10697; session c3-batch-g-s6
2026-05-16  0042aeb0  MenuReadinessCheckB  C2->C3  Frida GREEN 10/10 log/diff_menu_readiness_check_b.csv; impl mashedmod/src/mashed_re/Frontend/MenuStateMachine.cpp; structural variant of 0x0042ae10 (byte column +1); callee 0x0040e470 C2; callers 0x0043d7c0 C1; U-3445 U-3446 registered; session c3-batch-g-s5
2026-05-16  0042ae10  MenuReadinessCheckA  C2->C3  Frida GREEN 10/10 log/diff_menu_readiness_check_a.csv; impl mashedmod/src/mashed_re/Frontend/MenuStateMachine.cpp; callee 0x0040e470 C2; callers 0x0043d7c0/0x0043dfd0 C1 (caller-chain precedent: MenuButtonDetectA c3-batch-a-s3 same callee); U-3445 U-3446 U-3447 U-3448 registered (do not block correctness); session c3-batch-g-s5
2026-05-16  0042ac50  FUN_0042ac50  deferred  C3 refused c3-batch-g-s5: signature uses implicit EAX register input (uint in_EAX per analysis note lines 15-16); NativeFunction Frida harness lacks EAX-in support; D-10639 filed (re/DEFERRED.md). Prior draft cited D-10637 which tracks a different gap (E9-JMP-blocks-Interceptor); D-10639 is correct ID. U-3441 U-3442 still open.
2026-05-16  0042ac00  MenuGroupCount  C2->C3  Frida GREEN 11/11 log/diff_menu_group_count.csv; leaf-exemption; U-3439 U-3440 registered; session c3-batch-g-s5
2026-05-16  0042aae0  MenuIm2DQuad  C3-REFUSED  caller-gate: FUN_0043c5b0 (only caller) is C1; path1 10/10 GREEN; D-8918 filed; reimpl+hook ready in MenuChrome.cpp; session c3-batch-g-s1
2026-05-16  0042aad0  MenuDimSet  C3-REFUSED  caller-gate: FUN_0043c5b0 (only caller) is C1; leaf exemption covers callee-side only; D-8917 filed; reimpl+hook ready in MenuChrome.cpp; session c3-batch-g-s1
2026-05-16  00429a30  FUN_00429a30  deferred  C3 refused: FUN_00430790 C1; U-2095 open; D-10700; session c3-batch-g-s8
2026-05-16  00428320  FUN_00428320  deferred  C3 refused: caller gate fails — FUN_00428a30 C1 FUN_00428bf0 C1; callee gate PASSES; impl in TextMeasure.cpp; D-10699; session c3-batch-g-s8
2026-05-16  004282a0  FUN_004282a0  deferred  C3 refused: FUN_004277a0 not in hooks.csv (1 unknown callee); no Frida arg_type for (uint32,float) render-context fn; D-10698; session c3-batch-g-s8
2026-05-16  00427ff0  FontText_DrawTextRotated  deferred  C3 refused c3-batch-g-s13: harness gap (7-arg signature not supported in diff_template.js) + open [UNCERTAIN] on callee FUN_004277a0 + state-mutates live font ctx (Push/Pop/Translate/Rotate) on DAT_007d3ff8/DAT_0067d838/DAT_0067d83c at main-menu; D-8920
2026-05-16  00427ad0  FUN_00427ad0  deferred  C3 refused: callee gate fails — FUN_004277a0/FUN_00556e90/FUN_005555b0/FUN_00552d70 not in hooks.csv; D-10697; session c3-batch-g-s8
2026-05-16  00427840  FontText_UTF16WidenCopy  deferred  C3 refused c3-batch-g-s13: harness gap (EAX-implicit-source-ptr fastcall variant not supported in diff_template.js) + U-1069 unresolved + live vtable read on DAT_007d3ff8+0xf4; D-8919
2026-05-16  00427680  FontText_ComputeScreenXY  deferred  C3 refused c3-batch-g-s13: harness gap (ESI-implicit-output 2-float buffer not supported in diff_template.js) + U-2127 unresolved + depends on live font ctx DAT_0067d838; D-8918
2026-05-16  00427620  FontText_HudShutdown  deferred  C3 refused: callee gate fails (FUN_00555830 C1 FUN_00556e40 C1 FUN_00556cd0 C1 FUN_00552b90 C1); D-8917; session c3-batch-g-s13
2026-05-16  00427620  FontText_HudShutdown  deferred  C3 refused c3-batch-g-s13 (prior Sonnet pass): 4/4 callees C1 (FUN_00555830 FUN_00556e40 FUN_00556cd0 FUN_00552b90) — fails C2+ callee gate; teardown-only fn not safely callable at main-menu; D-8917
2026-05-16  0041c2d0  FUN_0041c2d0  C2->C3  Frida GREEN 10/10 crash_equal_ok log/diff_hud_dispatch_draw4.csv; __declspec(naked) EAX-thiscall impl HUD/HudDispatch.cpp; leaf-exempt (vtable-only callees_depth1=[]); callers 0041a3e0+0041c300 both C3; session c3-batch-g-s14
2026-05-16  0040e470  CarSlotStateGet  drift-skip  s10 drift-skip per s7 conflict resolution (s7 keeps the C3 landing); Frida GREEN 10/10 evidence preserved at log/diff_car_slot_state_get.csv; session c3-batch-g-s10
2026-05-16  0040e470  CarSlotStateGet  C2->C3  c3-batch-g-s7; Frontend/RaceResults.cpp; RH_ScopedInstall(CarSlotStateGet,0x0040e470); Frida path1 GREEN 10/10 int_scalar log/diff_car_slot_state_get.csv; path2 inline-JMP PASS 4/4 log/verify_hook_install_car_slot_state_get.txt; leaf-exemption (no callees); callers MenuButtonDetectA/B C3; U-1300 semantic-only blocks=none
2026-05-16  00494bc0  FUN_00494bc0  C1->C2  ma1-ghidra-s4 promote_c2_boot_teardown; COM Release x6 teardown verified against listing (52 instr, 145B); 6 globals 00771a48/3c/40/38/44/4c with null-before-release; +0x24 pre-call on 00771a48; single caller FUN_00402a40 (HardwareExit)
2026-05-16  00489250  FUN_00489250  C1->C2  ma1-ghidra-s4 promote_c2_boot_teardown; 2x pair-free FUN_004c0c20(*(obj+4))+FUN_004e6920(obj) on 007030a8/007030ac; 16 instr 55B; caller FUN_00402a40
2026-05-16  00496ce0  FUN_00496ce0  C1->C2  ma1-ghidra-s4 promote_c2_boot_teardown; FUN_004d41e0(DAT_00773094)+FUN_004d41e0(DAT_00773088); 8 instr 26B; caller FUN_00402a40; U-3327 semantic-only on callee
2026-05-16  004881d0  FUN_004881d0  C1->C2  ma1-ghidra-s4 promote_c2_boot_teardown; once-guard init helper; 76 instr 322B; FUN_004c0b30+FUN_00534b60(0x200,0x10000087,0)->DAT_007030a8+field writes+FUN_004e7e30+FUN_004e8090+FUN_004880a0; U-3861 filed (decomp vs listing slot ambiguity at iVar2+8 vs +0x10); S-3902/S-3903 filed; caller FUN_00402750
2026-05-16  00494ef0  thunk_FUN_00493f70  C1->C2  ma1-ghidra-s4 promote_c2_boot_teardown; 5B E9 JMP -> FUN_00493f70 (C1+plate intro_splash); drift-eligible per thunk rule
2026-05-16  00494f20  thunk_FUN_00494460  C1->C2  ma1-ghidra-s4 promote_c2_boot_teardown; 5B E9 JMP -> FUN_00494460 (C1+plate intro_splash); drift-eligible per thunk rule
2026-05-16  004955c0  thunk_FUN_00495580  refused  ma1-ghidra-s4 C1->C2 REFUSED: target FUN_00495580 not in hooks.csv and no plate at re/analysis/**/0x00495580.md; D-10762; thunk drift rule requires target at C1+ with plate
2026-05-16  004963d0  thunk_FUN_00496370  refused  ma1-ghidra-s4 C1->C2 REFUSED: target FUN_00496370 not in hooks.csv and no plate at re/analysis/**/0x00496370.md; D-10763; thunk drift rule requires target at C1+ with plate
2026-05-16  004c2f00  RwEngineGetCurrentMode  C2->C3  ma2-frida-s6 c3-batch-ma2; Boot/VideoConfig.cpp; RH_ScopedInstall(RwEngineGetCurrentMode,0x004c2f00); Frida path1 GREEN 10/10 none arg_type log/diff_rw_engine_get_current_mode.csv; callee FUN_004c2c90 C2; callers FUN_00493710 FUN_004921d0 C2
2026-05-16  004c2ed0  RwEngineGetModeInfo  refused  ma2-frida-s6 C2->C3 DEFERRED: requires wide out-buffer (~100B RwVideoMode); existing out3_idx allocates 12B and would corrupt Frida memory; reimpl not landed in tree
2026-05-16  004c19f0  RwVtableSlot07Call  C2->C3  ma2-frida-s5; Boot/FrameDispatch.cpp; RH_ScopedInstall(RwVtableSlot07Call,0x004c19f0); Frida path1 GREEN 10/10 int_scalar fake-ptr crash_equal_ok log/diff_rw_vtable_slot07_call.csv (both sides SIGSEGV identically on [scalar+0x1c] dereference); pure-leaf exemption (zero static callees; indirect call); caller FUN_00492e90 C2 (FrameDispatch); U-0824 still open (target identity, statically unresolvable)
2026-05-16  00499cc0  Window_Destroy  refused  ma2-frida-s2 C2->C3 REFUSED: DestroyWindow(DAT_007e9584) on the live game HWND terminates the running session — no safe quiescent-menu Frida test. Resolution: needs synthetic mode-launcher (spawn a fresh MASHED.exe per call) or scope the test to allocating a sacrificial HWND. Deferred
2026-05-16  00499ba0  Window_Create  refused  ma2-frida-s2 C2->C3 REFUSED: int(HINSTANCE,UNK,char*) — 3-arg signature with mandatory non-null param_3 (byte-copy to DAT_00773818) not supported by current harness arg_types (max 'int_pair' for two args). Resolution: add 3-arg 'win32_create' arg_type or refactor to pre-populate DAT_00773818 then call with safe args. D-row deferred to fanout next session
2026-05-16  00499820  Window_WndProc  refused  ma2-frida-s2 C2->C3 REFUSED: WNDPROC stdcall(HWND,UINT,WPARAM,LPARAM) — harness arg_type registry has no 4-arg WNDPROC vector; sentinel_array_ptr/int_ptr2_out cover up to 3 ints. Resolution: add 'wndproc' arg_type (alloc fake HWND or use real one, sweep uMsg in {1,2,6,0x100,others}, expect void+side-effect on DAT_00773918/0077391c). Deferred
2026-05-16  004996f0  WindowShow  C2->C3  ma2-frida-s2; mashedmod/src/mashed_re/Boot/Window.cpp; RH_ScopedInstall(WindowShow,0x004996f0); ShowWindow+UpdateWindow on DAT_007e9584 (HWND); user32.dll passthrough; Frida path1 GREEN 10/10 int_scalar SW_* commands log/diff_window_show.csv; callees external user32 only (gate exempt); callers FUN_00492370 boot path C2; leaf-equiv
2026-05-16  00499690  WindowMsgPump  C2->C3  ma2-frida-s2; mashedmod/src/mashed_re/Boot/Window.cpp; RH_ScopedInstall(WindowMsgPump,0x00499690); single-iter PeekMessageA+Translate+DispatchMessageA pump with WM_QUIT(0x12) short-circuit and WaitMessage gate on DAT_0077391c; returns DAT_00773918!=0; Frida path1 GREEN 10/10 arg_type=none (agent-thread queue empty; active=1 skips WaitMessage; quit_flag=0) log/diff_window_msg_pump.csv; callees external user32 only; callers FUN_00492290 boot loop C2; leaf-equiv
2026-05-16  00498bf0  DisplayGetCursorGate  C2->C3  ma2-frida-s6 c3-batch-ma2; Boot/VideoConfig.cpp; RH_ScopedInstall(DisplayGetCursorGate,0x00498bf0); Frida path1 GREEN 10/10 read_global log/diff_display_get_cursor_gate.csv; leaf-exemption (no callees); caller FUN_004951f0
2026-05-16  00498bf0  DisplayActiveFlagGet  C2->C3  ma2-frida-s5; Boot/FrameDispatch.cpp; RH_ScopedInstall(DisplayActiveFlagGet,0x00498bf0); Frida path1 GREEN 10/10 read_global sentinel-write log/diff_display_active_flag_get.csv; pure-leaf exemption (zero callees); callers FUN_00492e90 C2 (FrameDispatch) and FUN_004951f0 (gates ShowCursor(0))
2026-05-16  00498bd0  VideoGetRenderHeight  C2->C3  ma2-frida-s6 c3-batch-ma2; Boot/VideoConfig.cpp; RH_ScopedInstall(VideoGetRenderHeight,0x00498bd0); Frida path1 GREEN 10/10 read_global log/diff_video_get_render_height.csv; leaf-exemption (no callees); caller DisplayInit 0x004921d0 C2
2026-05-16  00498bc0  VideoGetRenderWidth  C2->C3  ma2-frida-s6 c3-batch-ma2; Boot/VideoConfig.cpp; RH_ScopedInstall(VideoGetRenderWidth,0x00498bc0); Frida path1 GREEN 10/10 read_global log/diff_video_get_render_width.csv; leaf-exemption (no callees); caller DisplayInit 0x004921d0 C2
2026-05-16  00498b60  VideoModeArraysFree  refused  ma2-frida-s6 C2->C3 DEFERRED: destructive teardown (frees 4 live heap allocs); 10-call A/B harness double-frees and crashes both sides; needs save/zero/restore arg_type; reimpl not landed in tree
2026-05-16  00496490  Window_WmCreate  refused  ma2-frida-s2 C2->C3 REFUSED: callee gate — internal callees FUN_004a43d2, FUN_004a43b9, FUN_004a2b60 currently C1 in hooks.csv (rows 385, 386, 850+1455). C3 gate requires at least one callee at C2+ AND all reachable callees not below C1; CONFIDENCE.md says C3 needs gate satisfaction. Resolution: promote those three to C2 first (mechanical inspection + plate already written for each in window_msgpump_d2/rw_engine_init_d3), then re-attempt
2026-05-16  004955b0  DInputInitPredicate  C2->C3  ma2-frida-s3 Boot/RwEngineInit.cpp; 11b bool wrapper around FUN_00495530 (DI8Create C2); Frida A/B 10/10 GREEN at quiescent menu (log/diff_dinput_init_predicate.csv); path2 patcher OK (E9+rel32+3/3 interceptor); caller 00495150 C2, callee 00495530 C2 - all gates pass
2026-05-16  00492e90  FrameDispatch  C2  refused-promote  ma2-frida-s5 C2->C3 REFUSED: per CLAUDE.md hot-path warning Frida Interceptor destabilizes per-frame tick in ~6s; 1236-byte 31-callee body still has many C1 callees; promotion needs (a) all 31 callees at C2+ AND (b) a non-Interceptor hot-path verification strategy (likely behavioral-observation/screenshot rather than synthetic A/B). 5/6 callees in this fanout promoted to C3.
2026-05-16  0042f510  Vehicle0HandleGet  C2->C3  ma2-frida-s5; Boot/FrameDispatch.cpp; RH_ScopedInstall(Vehicle0HandleGet,0x0042f510); Frida path1 GREEN 10/10 read_global sentinel-write log/diff_vehicle0_handle_get.csv; pure-leaf exemption (zero callees); caller FUN_00492e90 C2 (FrameDispatch); U-0907 semantic-only blocks=none; dup row line ~1314 harmonized
2026-05-16  0042b8f0  StatePhaseIsFinal  C2->C3  ma2-frida-s5; Boot/FrameDispatch.cpp; RH_ScopedInstall(StatePhaseIsFinal,0x0042b8f0); Frida path1 GREEN 10/10 read_global sentinel-write log/diff_state_phase_is_final.csv; pure-leaf exemption (zero callees); caller FUN_00492e90 C2 (FrameDispatch); U-0500 semantic-only blocks=none
2026-05-16  0042b8d0  StatePhaseIsIdle  C2->C3  ma2-frida-s5; Boot/FrameDispatch.cpp; RH_ScopedInstall(StatePhaseIsIdle,0x0042b8d0); Frida path1 GREEN 10/10 read_global sentinel-write log/diff_state_phase_is_idle.csv; pure-leaf exemption (zero callees); caller FUN_00492e90 C2 (FrameDispatch); U-0500 semantic-only blocks=none
2026-05-16  ma2-frida-s8  refused-promote-batch  buckets=settings_dialog  count=8  reason=Win32 dialog handlers not testable via standard Frida path1 A/B  D-10780..D-10787
2026-05-17  0042f8d0  FUN_0042f8d0  refused  ma3-frida-s1 C2->C3 REFUSED (re-confirms D-10600): sole callee FUN_00472c60 still C1; 5x repeated call pattern with border-offset globals; cannot promote until FUN_00472c60 reaches C2+.
2026-05-17  0042ee40  FrontendModeDispatch  C2->C3  ma3-frida-s7; Frontend/FrontendDispatch.cpp; RH_ScopedInstall(FrontendModeDispatch,0x0042ee40); Frida path1 GREEN 14/14 log/diff_frontend_mode_dispatch.csv; sole callee FUN_0040bb90 C4; caller FUN_00439210 C2; outer switch DAT_0067e9fc dispatcher; U-3463 U-3464 U-3465 filed (structural-only, do not block C3); other 4 ma3-frida-s7 candidates REFUSED via callee-gate (3+ C1 callees each)
2026-05-17  0042e5b0  FUN_0042e5b0  refused  ma3-frida-s1 C2->C3 REFUSED (re-confirms D-10591): callee gate still fails — FUN_00473c20, FUN_00474890, FUN_00473ee0 not in hooks.csv (unknown confidence; must be discovered to >=C1 then promoted to C2); FUN_004a2c48 C1; FUN_0042e590 C4 (only one OK). Mission prompt assumed caller-gate was open but real blocker is missing+C1 callees.
2026-05-17  0042e3a0  FUN_0042e3a0  refused  ma3-frida-s1 C2->C3 REFUSED (re-confirms D-10592): callee gate still fails — FUN_0042d5a0 C2 (deferred D-10590, must reach C3 first); FUN_00472f40 C1; FUN_004730b0 C1; FUN_00472c60 C1. Mission prompt assumed caller-gate (FUN_0043c5b0 C2) was the blocker but actual blocker is callee-gate per existing DEFERRED row.
2026-05-17  0042bb60  MenuTeamBalance  C3-drift-repair  ma3-frida-s4; removed duplicate stale C2 row; evidence log/diff_menu_team_balance.csv still present from c3-batch-a-s3
2026-05-17  0042b9e0  CarSlotAssign  C3-drift-repair  ma3-frida-s4; hooks.csv evidence reference fix: log/diff_CarSlotAssign.csv (never existed) -> log/diff_car_slot_assign.csv; re-generated GREEN 10/10 path1 (read_global sentinel-write); reimpl Frontend/MenuButtonDetect.cpp; callee FUN_0040e480 C2; removed duplicate stale C2 row
2026-05-17  0042b960  CarSlotInit1P  C3-drift-repair  ma3-frida-s4; hooks.csv evidence reference fix: log/diff_CarSlotInit1P.csv (never existed) -> log/diff_car_slot_init_1p.csv; re-generated GREEN 10/10 path1; reimpl Frontend/MenuButtonDetect.cpp; callee FUN_0040e480 C2; removed duplicate stale C2 row
2026-05-17  0042af50  MenuReadinessCheckC  C2->C3  ma3-frida-s3; Frontend/MenuStateMachine.cpp; RH_ScopedInstall(MenuReadinessCheckC,0x0042af50); Frida path1 GREEN 10/10 log/diff_menu_readiness_check_c.csv; structural variant of MenuReadinessCheckA -1 byte (0x7f1041/1501 vs A 0x7f1042/1502); callee 0x0040e470 C2; callers FUN_0043d7c0/FUN_0043dfd0 C1 (caller-chain noted; same precedent as A/B); U-1615 U-1616 structural-only do-not-affect-bit-identity (already filed); D-4843 cleared; duplicate util row consolidated to frontend
2026-05-17  0042aae0  MenuIm2DQuad  C2->C3  ma3-frida-s1; Frontend/MenuChrome.cpp (pre-authored in c3-batch-g-s1, now promoted post-caller-gate-lift via FUN_0043c5b0 C2); RH_ScopedInstall(MenuIm2DQuad,0x0042aae0); __fastcall(int param_1); 286b RwIm2D fullscreen quad via vtable 0x007d3ff8; Frida A/B 10/10 void_match GREEN log/diff_menu_im2d_quad.csv (alpha DAT_0067eca8=0 at quiescent menu suppresses draw - safe void return); vtable-only callees (no fn callees); caller FUN_0043c5b0 C2; U-0454 U-0455 U-0456 (structural/semantic; none-blocking, behaviour bit-identical regardless of RW constant naming)
2026-05-17  0042aad0  MenuDimSet  C2->C3  ma3-frida-s1; Frontend/MenuChrome.cpp (pre-authored in c3-batch-g-s1, now promoted post-caller-gate-lift via FUN_0043c5b0 C2); RH_ScopedInstall(MenuDimSet,0x0042aad0); naked fn (implicit EAX ptr); Frida A/B 10/10 crash_equal_ok GREEN log/diff_menu_dim_set.csv (both orig+reimpl AV at 0x3 — identical error string); leaf (no callees); caller FUN_0043c5b0 C2; U-3876 filed (harness gap: NativeFunction cannot control EAX); U-0452/U-0453 (provenance/structural; none-blocking); also extended diff_template.js void_write_observe with crash_equal_ok branch
2026-05-17  ma3-frida-s3  refused-promote-batch  rvas=00431b80;00431d00;00429aa0  reasons=ESI=0-infinite-loop(register-ABI-harness-gap);depends-on-00431b80;callee-gate-3-C1-callees  D-10753-amended;D-10754-amended;D-10788-new  note=1/4 menu nav C2->C3 (MenuReadinessCheckC); 0042ac90 already C3 (skipped); 3 refused
2026-05-17  0042c2f0  SetDat0067ecb8  C2->C3  c3-batch-h-s5; Util/UtilBatch_h5.cpp; RH_ScopedInstall(SetDat0067ecb8,0x0042c2f0); 9b pure setter DAT_0067ecb8=param_1 (void_setter_observe); Frida path1 GREEN 10/10 log/diff_set_dat_0067ecb8.csv; caller FUN_004929d0 C2 (paired getter 0x0042c2e0 already C3); drift cleanup: removed duplicate vehicle/race_state C1 row and kept marker row pointing to canonical
2026-05-17  004098b0  LoadingState1Enter  C2->C3  c3-batch-h-s5; Util/UtilBatch_h5.cpp; RH_ScopedInstall(LoadingState1Enter,0x004098b0); 27b pure 4-global setter (DAT_008a9584=1 DAT_008a9588=1 DAT_008a95b0=0 DAT_008a95ac=0); Frida path1 GREEN 10/10 log/diff_loading_state1_enter.csv; void_write_observe on DAT_008a9584; caller FUN_0043dfd0 C1 (drift-OK leaf-exemption — pure leaf no callees)
2026-05-17  00409930  LoadingState3Enter  C2->C3  c3-batch-h-s5; Util/UtilBatch_h5.cpp; RH_ScopedInstall(LoadingState3Enter,0x00409930); 30b pure 3-global setter (DAT_008a9584=3 DAT_008a9590=1 DAT_008a95b0=0); Frida path1 GREEN 10/10 log/diff_loading_state3_enter.csv; void_write_observe on DAT_008a9584; callers FUN_0043dfd0+FUN_0040ab40 both C1 (drift-OK leaf-exemption)
2026-05-17  00409900  LoadingState2Enter  C2->C3  c3-batch-h-s5; Util/UtilBatch_h5.cpp; RH_ScopedInstall(LoadingState2Enter,0x00409900); 43b: printf("Load start\n") + DAT_008a9584=2 DAT_008a958c=1 DAT_008a95b0=0; Frida path1 GREEN 10/10 log/diff_loading_state2_enter.csv; void_write_observe on DAT_008a9584; ANALYSIS-NOTE-CORRECTION: re/analysis/util_c0_promote/0x00409900.md described callee as FID_conflict__wprintf but string bytes at 0x005ccb28 are ASCII (verified raw bytes); actual call is direct CALL to 0x00442cbd (statically-linked CRT printf); caller FUN_0043dfd0 C1 (drift-OK callee is external CRT)
2026-05-17  00413f90  TimerGetBasePtr  C2->C3  c3-batch-h-s5 (drift-fix); Util/TimerState.cpp; RH_ScopedInstall already lived in tree from prior batch; 5b address-of getter returning &DAT_005f2b10; Frida path1 GREEN 5/5 log/diff_timer_get_base_ptr.csv; pure leaf (no callees); callers 0x0043d7c0/0x0043dfd0 C1 (drift-OK leaf-exemption); U-3468 retained (DAT_005f2b10 semantics still opaque)
2026-05-17  00426c10  TimerDispatch10  C2->C3  c3-batch-h-s5; Util/UtilBatch_h5.cpp; RH_ScopedInstall(TimerDispatch10,0x00426c10); 27b conditional dispatch: if (FUN_0041ea40()!=0) FUN_0041e920(0x00646e58)+FUN_00480100 (tail-call); Frida path1 GREEN 10/10 crash_equal_ok arg_type=none log/diff_timer_dispatch_10.csv (gate returns 0 at quiescent menu — both paths no-op identically); caller FUN_004111c0 C1 (drift-OK); callees C0/C1 invoked via fn-ptr trampoline
2026-05-17  00426c30  TimerDispatch30  C2->C3  c3-batch-h-s5; Util/UtilBatch_h5.cpp; RH_ScopedInstall(TimerDispatch30,0x00426c30); 22b conditional dispatch: if (FUN_0041ea30()!=0) tail-JMP FUN_0041e910(0x00646e58); Frida path1 GREEN 10/10 crash_equal_ok log/diff_timer_dispatch_30.csv; ANALYSIS-NOTE-CORRECTION: re/analysis/timer_d3_cont1_b/0x00426c30.md said FUN_0041e910 called with no args; actual asm writes [ESP+4]=0x00646e58 before JMP (8-byte MOV [ESP+4],0x646e58); caller FUN_004111c0 C1
2026-05-17  00426c70  TimerDispatch70  C2->C3  c3-batch-h-s5; Util/UtilBatch_h5.cpp; RH_ScopedInstall(TimerDispatch70,0x00426c70); 22b conditional dispatch: if (FUN_0041ea50()!=0) tail-JMP FUN_0041e930(0x00646e58); Frida path1 GREEN 10/10 crash_equal_ok log/diff_timer_dispatch_70.csv; ANALYSIS-NOTE-CORRECTION (same as 0x00426c30): FUN_0041e930 takes arg 0x00646e58; caller FUN_004111c0 C1
2026-05-17  c3-batch-h-s5  refused-promote-batch  rvas=004af31a;0041f1e0;0041cb80;00422120  reasons=CRT-init-orphan-no-function-entity(004af31a);event-matrix-copy-game-state-dep-table-harness-gap(0041f1e0);register-passed-implicit-pointer-ESI-to-callee-harness-gap(0041cb80,00422120)  note=8/12 util C2->C3 promoted; 4 refused. 00422120 had stale impl in TimerInit.cpp from prior batch (same ESI register-ABI bug) — left in place at C2 (impl present but not bit-identical; do not promote until inline-asm wrapper exists). 0041cb80 same ESI impedance. 004af31a is a CRT init code block referenced from 0x005ea054 data table, not a callable function entity — cannot install via RH_ScopedInstall. 0041f1e0 needs game-state table prep (DAT_0063d9e0 at (player*0xab+event)*4) beyond current harness.
2026-05-17  0041db80  Sub0041db80_HudThresholdDispatch  C2->C3  c3-batch-h-s4; HUD/HudBatch_h4.cpp; RH_ScopedInstall; short-thunk MOV EAX,0x63d55c JMP 0x41dab0 — body at 0x41dab0 reads float counter at base+0x2c vs 5 float thresholds (0x5cc35c,0x5ccdf4,0x5cd230,0x5cd234,0x5cd238); dispatch is direct fn-ptr at obj+0x48 (NOT vtable indirection); 6 dispatch slots at base+{0x00,0x04,0x08,0x0c,0x10,0x14}; corrected from analysis note (which had int-vs-float and added vtable indirection). Frida A/B 10/10 GREEN log/diff_sub_0041db80_hud_threshold_dispatch.csv (quiescent main menu, no crash, both sides take no-dispatch branch). Leaf-exemption: no named callees (dispatch via direct fn ptr).
2026-05-17  00403160  Sub00403160_SubMode0BViewport  C2->C3  c3-batch-h-s4; HUD/HudBatch_h4.cpp; RH_ScopedInstall; 0.8f viewport sub-mode 0xb handler; callees FUN_004671a0 (C2 cam fetch), FUN_004c19f0 (C3 cam lock), FUN_004c1c80 (C2 setviewport), FUN_004c1a00 (C1 cam begin), FUN_00402fb0 (C1 hud body), FUN_00428760 (C1 cond draw). Caller HudIngameDispatch C3. C2+ callee gate satisfied via 004c19f0/004671a0/004c1c80. Frida A/B 10/10 GREEN log/diff_sub_00403160_submode0b_viewport.csv (quiescent main menu, both sides AV at 0x60 identically — cold camera handle).
2026-05-17  00427780  FontText_StringTableLookup  C2->C3  c3-batch-h-s4; HUD/HudBatch_h4.cpp; RH_ScopedInstall; pure leaf packed-string-table lookup (0x10 bytes); returns &DAT_0066d828 + *(int*)(&DAT_0066d828 + idx*4). Frida A/B 10/10 GREEN log/diff_font_text_string_table_lookup.csv (identical pointer 0x66d828+offset on both sides for indices 0..7). Leaf-exemption: no callees. Also deduplicated stale duplicate C2 row from hud_promote_c2_b.
2026-05-17  00427840  FontText_UTF16WidenCopy  C2->C3  c3-batch-h-s4; HUD/HudBatch_h4.cpp; RH_ScopedInstall; __declspec(naked) __fastcall EAX=src(byte*) ECX=dst(ushort*); vtable+0xf4 byte-length via *0x7d3ff8; byte->ushort widening loop; null-terminate. Frida A/B 10/10 GREEN log/diff_font_text_utf16_widen_copy.csv (quiescent main menu, both sides AV at 0x0 identically on garbage register args — crash_equal_ok). Leaf-exemption: only callee is vtable-indirected (no named direct callees). U-1069 (source object identity at *0x7d3ff8) unresolved but structural/semantic-only. Fixed MSVC inline-asm absolute-memref bug (uses file-static volatile ptr workaround). Also deduplicated stale duplicate C2 row.
2026-05-17  c3-batch-h-s4  refused-promote-batch  rvas=0041d870;0041ded0;00427620;00427680;0041bc50;005554d0;004c57a0  reasons=callee-FUN_0041d410-C1;callee-FUN_0041de80-C1;all-callees-below-C2(00555830/00556e40-missing-from-csv);ESI-implicit-out-ptr+U-2127-EDI-artifact;analysis-note-pair-table-reordered-AND-incomplete-vs-binary;analysis-note-body-summary-inconsistent-with-binary-first-read;allocator-returns-pointer-non-deterministic-across-pair-no-arg_type-fits  note=4/12 GREEN; 1 skip (0x0041c2d0 already hooked); 7 refused; 4 refusals discovered post-diff after authoring (need pre-diff analysis-note-vs-binary validation)
2026-05-17  sweep-20260518-0007  scribe-release  bucket=cluster_004e_first_pass  writes=60  errors=0
2026-05-17  sweep-20260518-0007  scribe-release  bucket=cluster_0051_first_pass  writes=60  errors=0
2026-05-17  sweep-20260518-0007  scribe-release  bucket=cluster_005a_first_pass  writes=60  errors=0
2026-05-17  sweep-20260518-0007  scribe-release  bucket=promote_c2_render_lowrva  writes=31  errors=0
2026-05-17  sweep-20260518-0007  scribe-release  buckets=4 drained  errors=0
2026-05-17  0x00422120  TimerInitLoop  C2->C3  feature/timer-init-naked: __declspec(naked) wrapper fixes silently-broken c3_batch_h-s5 callee() impl (EAX-arg ABI); Frida path1 GREEN 10/10 log/diff_timer_init_loop.csv; U-3877 U-3878 filed
2026-05-17  0x00498f60  VideoDialogInit_i3  C2->C3  c3-batch-i-s3; Save/SettingsAndIO_i3.cpp; RH_ScopedInstall(VideoDialogInit_i3,0x00498f60); 516B WM_INITDIALOG handler; HWND via EAX (__declspec(naked) bridge to __stdcall body); 7x SetControlTextFromResource label localization + 4x BM_SETCHECK (Chk1047/1048/1051==0, Chk1049!=0 inverted) + subsys combo enum stride 0x50 + PopulateModeCombo + CB_SETCURSEL via DAT_00773418[DAT_00773200] + SetFocus; Frida path1 GREEN 10/10 log/diff_video_dialog_init_i3.csv (crash_equal_ok arg_type=none — dialog handler not exercised at quiescent main menu); callees 00499740 0x00498d60 GetDlgItem SendMessageA SetFocus (00499740/00498d60 C2/C3); caller VideoSettingsDlgProc 0x004991f0 C2
2026-05-17  0x00499170  SubsystemSelectionChanged_i3  C2->C3  c3-batch-i-s3; Save/SettingsAndIO_i3.cpp; RH_ScopedInstall(SubsystemSelectionChanged_i3,0x00499170); 128B WM_COMMAND/1000 handler; HWND via EAX (naked bridge); CB_GETCURSEL guard against current DAT_0077340c; on change: CB_GETITEMDATA -> DAT_0077340c + RW_SetSubSystem(0x004c2e70) + CB_RESETCONTENT on mode combo + PopulateModeCombo(0x00498d60) + RW_GetCurrentMode(0x004c2f00) -> DAT_00773200 + CB_SETCURSEL; Frida path1 GREEN 10/10 log/diff_subsystem_selection_changed_i3.csv crash_equal_ok; callees all C2+; caller VideoSettingsDlgProc 0x004991f0 C2
2026-05-17  0x00499740  SetControlTextFromResource_i3  C2->C3  c3-batch-i-s3; Save/SettingsAndIO_i3.cpp; RH_ScopedInstall(SetControlTextFromResource_i3,0x00499740); 100B helper; HWND via EAX + resource_id at [esp+4] (naked bridge to __stdcall body); LoadStringA(DAT_007e9580 id buf 0x100) then SetWindowTextA(hCtrl buf); CHAR[256] local buf; Frida path1 GREEN 10/10 log/diff_set_control_text_from_resource_i3.csv crash_equal_ok arg_type=int_scalar; callees LoadStringA+SetWindowTextA (Win32 imports) + __security_check_cookie (CRT); caller VideoDialogInit_i3 (just promoted) + structural caller VideoSettingsDlgProc C2
2026-05-17  0x004b3b70  FileReadWrapper_i3  C2->C3  c3-batch-i-s3; Save/SettingsAndIO_i3.cpp; RH_ScopedInstall(FileReadWrapper_i3,0x004b3b70); 61B __cdecl(filename buf size); 3-call sequence FUN_004cc230(2 1 filename)->handle->FUN_004cbd30(handle buf size)->FUN_004cc160(handle 0); Frida path1 GREEN 10/10 log/diff_file_read_wrapper_i3.csv (both sides return 0 — FUN_004cc230 returns NULL on garbage filename ptr); callees C2+ (004cc230/004cbd30/004cc160 all C2); caller SaveLoad 0x00404e50 C3
2026-05-17  0x004b3bb0  FileWriteWrapper_i3  C2->C3  c3-batch-i-s3; Save/SettingsAndIO_i3.cpp; RH_ScopedInstall(FileWriteWrapper_i3,0x004b3bb0); 55B __cdecl(filename buf size); 3-call sequence FUN_004cc230(2 2 filename)->handle->FUN_004cbe80(handle buf size)->FUN_004cc160(WRITE_RESULT 0); U-0287 U-0288 acknowledged in source (close receives write_result not handle per binary; bit-identity preserves divergence); Frida path1 GREEN 10/10 log/diff_file_write_wrapper_i3.csv crash_equal_ok; callees C2+ (004cc230/004cbe80/004cc160 all C2); caller SaveWrite 0x00404f50 C3
2026-05-17  0x005ab380  AudioRwsChunkHeaderRead  C2->C3  c3-batch-i-s1; Audio/RwsStream_i1.cpp; RH_ScopedInstall; FUN_004cbd30 stream read + RW version decode; caller=005abfa0(C2); callee=004cbd30(C2 render); Frida path1 GREEN 10/10 log/diff_audio_rws_chunk_header_read.csv (crash_equal_ok — null-stream null-deref identical orig vs reimpl); U-0117 catalog Blocks: none.
2026-05-17  0x005abf80  AudioWaveVtableSlot1cDispatch  C2->C3  c3-batch-i-s1; Audio/RwsStream_i1.cpp; RH_ScopedInstall; vtable+0x1c indirect dispatch (unrecovered jumptable); callers=005abd30(C2) 005abfa0(C2); indirect-vtable-only callee per AudioWaveNodeFree precedent (leaf-equivalent); Frida path1 GREEN 10/10 log/diff_audio_wave_vtable_slot_1c_dispatch.csv (crash_equal_ok); U-0998 catalog Blocks: none.
2026-05-17  0x005aa0c0  AudioTreeWalkPredicateSearch  C2->C3  c3-batch-i-s1; Audio/RwsStream_i1.cpp; RH_ScopedInstall; recursive depth-first predicate tree-walk; NULL→DAT_009146fc root; caller=005ac900(C3 AudioContextLookup); indirect predicate callback + self-recursion (leaf-equivalent); Frida path1 GREEN 10/10 log/diff_audio_tree_walk_predicate_search.csv (crash_equal_ok — uninit audio tree null-deref identical).
2026-05-17  c3-batch-i-s1  refused-promote-batch  rvas=005ab410;005aea00  reasons=only-static-caller-005a7b60-is-C1(needs-caller-at-C2+);U-0125-catalog-Blocks-C3-DAT_007d3ff8-runtime-only-target  note=3/5 promoted to C3; 2 refused with explicit unblock conditions.
2026-05-18  0x0046c570  VehicleDampVec3  C2->C3  c3-batch-j-s3; Vehicle/MiscDamping_j3.cpp; RH_ScopedInstall(VehicleDampVec3,0x0046c570); 74B __cdecl(int idx) pure leaf; scales 3 floats at DAT_00881f50/54/58[idx*0x341*4] by _DAT_005ce264 in place; returns 1; arg_type vec3_global_mul_observe (D-11011 unblock, harness commit 656273b); Frida path1 GREEN 10/10 log/diff_vehicle_damp_vec3.csv crash_equal_ok; U-3879 filed (cosmetic — vector kind unknown, mechanical reimpl exact); caller 0x00424eb0 C2 (game-mode cleanup loop).
2026-05-18  0x00411580  ReplayGetBestTime  C2->C3  c3-batch-j-s3; Vehicle/Replay_j3.cpp; RH_ScopedInstall(ReplayGetBestTime,0x00411580); checks DAT_0063bb10!=0 && best+0x17c+idx*4!=0, delegates to FUN_00411530 (refused this batch — stays at original RVA); arg_type none GREEN 10/10 log/diff_replay_get_best_time.csv; at menu DAT_0063bb10==0 -> returns 0 deterministic.
2026-05-18  0x004115c0  ReplayGetCurrentTime  C2->C3  c3-batch-j-s3; Vehicle/Replay_j3.cpp; RH_ScopedInstall(ReplayGetCurrentTime,0x004115c0); structural mirror of ReplayGetBestTime over DAT_0063bb14; U-1078 catalogued (audio xref); arg_type none GREEN 10/10 log/diff_replay_get_current_time.csv.
2026-05-18  0x004114e0  ReplayCleanup  C2->C3  c3-batch-j-s3; Vehicle/Replay_j3.cpp; RH_ScopedInstall(ReplayCleanup,0x004114e0); frees DAT_0063bb04/08 via FUN_00483a40 (S-1562); zeroes DAT_0063bb10/14/2c; arg_type none GREEN 10/10 log/diff_replay_cleanup.csv; at menu both heap ptrs null so only zero stores.
2026-05-18  0x00411600  ReplayRecordFrame  C2->C3  c3-batch-j-s3; Vehicle/Replay_j3.cpp; RH_ScopedInstall(ReplayRecordFrame,0x00411600); mode-gated (FUN_0042f6a0()==2); body delegates FUN_0046d4a0+FUN_004829d0+FUN_00407a00 to original RVAs; wprintf trios omitted (unreachable in diff regime); arg_type none GREEN 10/10 log/diff_replay_record_frame.csv.
2026-05-18  0x00411750  ReplayStartLap  C2->C3  c3-batch-j-s3; Vehicle/Replay_j3.cpp; RH_ScopedInstall(ReplayStartLap,0x00411750); mode-gated; rewinds DAT_0063bb10 (S-1561) resets DAT_0063bb14 (S-1560); arg_type none GREEN 10/10 log/diff_replay_start_lap.csv.
2026-05-18  0x004117b0  ReplaySave  C2->C3  c3-batch-j-s3; Vehicle/Replay_j3.cpp; RH_ScopedInstall(ReplaySave,0x004117b0); save-once latch DAT_005f29c8 + null best-buf gate; builds c:\toast\Replay%d.rep via vtable+0xc4 on DAT_007d3ff8; serialises DAT_0063bb10 via FUN_00483ca0 (S-1565); arg_type none GREEN 10/10 log/diff_replay_save.csv.
2026-05-18  0x00411170  TimeTrialRecordPlayback  C2->C3  c3-batch-j-s3; Vehicle/Replay_j3.cpp; RH_ScopedInstall(TimeTrialRecordPlayback,0x00411170); per-frame state-6 dispatcher; inner mode-gates short-circuit at menu; DAMAGE_FN early-out; arg_type none GREEN 10/10 log/diff_time_trial_record_playback.csv.
2026-05-18  c3-batch-j-s3  refused-promote-batch  rvas=00411350;00411530  reasons=implicit-ST0-FPU-input-arg(0x00411350-TimeFormat-needs-fpu_st0_input-arg_type);5-arg-(int-int-ptr-ptr-ptr)-shape(0x00411530-GetTimeAtIdx-needs-three_out_ptr-arg_type)  note=8/10 promoted to C3; 2 refused on harness arg_type gaps (filed for next harness-extension batch).
2026-05-18  0x005ac5f0  AudioFmtDescEqual  C2->C3  c3-batch-j-s1; Audio/RwsFmt_j1.cpp; RH_ScopedInstall(AudioFmtDescEqual,0x005ac5f0); 5-field AND equality comparator on fmt-descriptor pairs (+0x00 u32, +0x04 16B fmt-key via FUN_005adf30, +0x0c s8, +0x0d s8, +0x18 u8 masked 0xfd); callee FUN_005adf30(C3 AudioFmtKeyCompare); leaf-equivalent (1 C3 callee); Frida path1 GREEN 10/10 log/diff_audio_fmt_desc_equal.csv (crash_equal_ok on test idx 2 where param_2[1] key-ptr=0xbbbbbbbb deref AVs both sides identically); arg_type=fmt_desc_pair_compare (newly added commit 656273b).
2026-05-18  0x005ac9e0  AudioFmtEntryMatch  C2->C3  c3-batch-j-s1; Audio/RwsFmt_j1.cpp; RH_ScopedInstall(AudioFmtEntryMatch,0x005ac9e0); 6-condition match predicate for fmt-table entries against candidate descriptors (channel byte +0x05/+0x0d, fmt-char +0x04/+0x0c, sample-rate range +0x08/+0x0c vs +0x00, fmt-key compare, flag bit0 +0x10/+0x18); callee FUN_005adf30(C3 AudioFmtKeyCompare); leaf-equivalent (1 C3 callee); Frida path1 GREEN 10/10 log/diff_audio_fmt_entry_match.csv (crash_equal_ok); arg_type=fmt_desc_pair_compare.
2026-05-18  c3-batch-j-s1  refused-promote-batch  rvas=005acaa0;005abcf0;005abfa0;005abd30;005aba20;005ac210;004522d0  reasons=005acaa0:~620B-multi-branch-serialise/deserialise-body-too-dense-for-single-session;005abcf0:callee-005a7e70-C1;005abfa0:STOP-AND-ASK-per-batch-header-12+callees-MULTI;005abd30:callees-005b3b30/60/80-C1-+-U-0997-multi;005aba20:callees-005a7b50/005a7b00/005b3b60-C1-+-multi-vtable;005ac210:complex-multi-page-factory-extensive-flag-dispatch-many-indirect-callees;004522d0:vtable-trampoline-U-0110-signature-unknown-not-statically-resolvable  note=2/10 promoted (the genuinely-tractable fmt comparators); 1 drift-skip(005ab410 already C3 in i-s1); 7 refused with explicit unblock conditions.
2026-05-18  sweep-20260518-0514  scribe-claim  buckets=4 queued, 0 skipped-HOLD
2026-05-18  sweep-20260518-0514  scribe-release  bucket=cluster_005b_first_pass  writes=60  errors=0
2026-05-18  sweep-20260518-0514  scribe-release  bucket=cluster_004f_first_pass  writes=60  errors=0
2026-05-18  sweep-20260518-0514  scribe-release  bucket=cluster_0055_first_pass  writes=60  errors=0  splits=4(rtfs_manager,anim_channel,vector_path,vector_font)
2026-05-18  sweep-20260518-0514  scribe-release  bucket=cluster_0048_first_pass  writes=60  errors=0  renames=1(FUN_00489240->SkySecondaryDispatch drift-reconciled)  splits=5(smplfzx,world_objects,debug_overlay,particle,sky,render_thunk)  hooks-csv-row-update=0x00489240
2026-05-18  sweep-20260518-0514  scribe-release  buckets=4 drained (cluster_005b/004f/0055/0048; total 240 plates + 240 bookmarks + 1 rename) drift-cleanups=2 (FUN_00489240->SkySecondaryDispatch + 0x0040bb50 boot-dup row removed from hooks.csv); sync=partial (slots 1/2/3 refreshed; 0/5/10-15 LOCKED by JVM-orphan-held .lock~ — will sync next /mcp restart); errors=0

2026-05-18  sweep-20260518-1448  scribe-claim  buckets=5 queued, 1 halt (s4 libpng-libpng-zlib), 0 skipped-HOLD
2026-05-18  sweep-20260518-1448  scribe-release  bucket=bucket_0048ad50  writes=80  errors=0
2026-05-18  sweep-20260518-1448  scribe-release  bucket=bucket_005c7070  writes=80  renames=3 (RwV3dTransformPoints/Vectors/Point at 0x005cb000/07f/0ef)  errors=0
2026-05-18  sweep-20260518-1448  scribe-release  bucket=bucket_0051e6f0 (HALT-libpng-zlib)  writes=0  errors=0  note=halt-row (level=halt, 0 per-RVA plates; only _BUCKET_HALT.md in bucket dir; library-residue libpng 2002.x + zlib, recommend FidDB bulk-name + DEFERRED library-tag drain)
2026-05-18  sweep-20260518-1448  scribe-release  bucket=bucket_00405400  writes=80  errors=0
2026-05-18  sweep-20260518-1448  scribe-release  bucket=bucket_004e1ce0  writes=81  renames=26 (RW RpWorld/RpAtomic/RpClump/RpLight/RwFrame catalog matches)  errors=0
2026-05-18  sweep-20260518-1448  scribe-release  bucket=bucket_00583f10  writes=80  renames=0 (library-residue qhull 2002.1; rename deferred to library-tag drain)  errors=0
2026-05-18  sweep-20260518-1448  scribe-release  buckets=6 drained (5 productive + 1 halt-row; 401 plates total)  errors=0  renames=29 (3 RW V3d in s6 + 26 RW scenegraph in s3)  sync=partial (slots 0/2/3/5/6/10-15 LOCKED by JVM-orphan-held .lock~ from prior session; slot 1 attempted but busy; will sync on next /mcp restart)
2026-05-18  sweep-20260518-1729  scribe-claim  buckets=5 queued, 0 skipped-HOLD, 1 HALT-row-drain
2026-05-18  sweep-20260518-1729  scribe-release  bucket=bucket_0057bf30  writes=8  renames=0 (RW-style game-code; rename TBD pending caller-side analysis per session note)  errors=0
2026-05-18  sweep-20260518-1729  scribe-release  bucket=bucket_00516bb0  writes=0  renames=0 (HALT-row; library residue libpng+zlib — no per-RVA plates)  errors=0
2026-05-18  sweep-20260518-1729  scribe-release  bucket=bucket_00489940  writes=80  renames=0 (22 VFX game-code + 49 DirectShow strmbase candidates not FidDB-attested + 9 drift-skip; rename deferred to library-tag drain)  errors=0
2026-05-18  sweep-20260518-1729  scribe-release  bucket=bucket_00412130  writes=80  renames=0 (HUD weapons-VFX + ParticleEmitters game-code; no FidDB attestation)  errors=0
2026-05-18  sweep-20260518-1729  scribe-release  bucket=bucket_004ddfb0  writes=80  renames=0 (RenderWare D3D9 driver; no FidDB attestation present in batch)  errors=0
2026-05-18  sweep-20260518-1729  scribe-release  bucket=bucket_005c1d63  writes=80  renames=0 (33 FidDB-attested MSVC CRT entries pre-renamed by Ghidra analyzer; 23 CRT-internal + 24 game-code remain FUN_xxx)  errors=0
2026-05-18  sweep-20260518-1729  scribe-release  buckets=6 drained (5 productive + 1 HALT-row; 328 plates, 328 bookmarks; 0 renames — FidDB-attested CRT entries already pre-renamed by Ghidra analyzer)  errors=0  sync=partial (slots 0/1/2/10-15 LOCKED by JVM-orphan .lock from prior sessions; slot 4 partial-refresh blocked on busy db.53.gbf/tmp*.ps; will sync on next /mcp restart)
2026-05-19  sweep-20260519-0330  scribe-claim  buckets=6 queued, 0 skipped-HOLD
2026-05-19  sweep-20260519-0330  scribe-release  bucket=bucket_0041e140  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-0330  scribe-release  bucket=bucket_00474d80  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-0330  scribe-release  bucket=bucket_004bdcc0  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-0330  scribe-release  bucket=bucket_00557fb0  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-0330  scribe-release  bucket=bucket_005ae170  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-0330  scribe-release  bucket=bucket_004fcb51  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-0330  scribe-release  buckets=6 drained (6 productive; 480 plates, 480 bookmarks, 0 renames — no FidDB Library Function attestations matched the single-line pattern in these 6 buckets)  errors=0  sync=partial (slots 0/1/2/3/10-15 LOCKED by JVM-orphan .lock from prior sessions; slot 4 partial-refresh blocked on busy db.53.gbf/tmp*.ps; pool sync will retry on next /mcp restart)
2026-05-19  sweep-20260519-1404  scribe-claim  buckets=4 queued + 2 misfiled-undrained (s1 005a6f30, s5 00452ec0 — under ## Drained but no drained-by= tag; will be properly drained this sweep), 0 skipped-HOLD
2026-05-19  sweep-20260519-1404  scribe-release  bucket=bucket_00452ec0  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-1404  scribe-release  bucket=bucket_004b4a80  writes=80  renames=0  errors=0
2026-05-19  sweep-20260519-1404  scribe-release  bucket=bucket_004c4270  writes=80  renames=0  errors=0

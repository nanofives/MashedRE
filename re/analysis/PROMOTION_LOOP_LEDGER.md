# Promotion loop ledger

State file for `/promote-round` (run under `/loop /promote-round`). This file
is the ONLY state that survives between rounds — every round reads it first
and appends to it last. Initialized 2026-06-12.

Goal (user, 2026-06-12): promote everything promotable. The loop ends after
two consecutive dry rounds, leaving the final gated-remainder report below.

## Counters

- rounds_run: 248
- total_green: 415
- dry_counter: 0
- RESUMED 2026-06-15 (round 239) via /loop /promote-round. Near-leaf lane active
  (scripts/near_leaf_frontier.py -> 112 candidates). HARNESS LIMIT: pure-jmp thunks (b0==0xE9)
  abort the NO_AUTO_HOOK safety check -> SKIP near_leaf rows named thunk_FUN_* ; adjustor thunks
  (b0 normal) are fine.
- NEAR-LEAF LANE RE-OPENED (round 237, 2026-06-15): the zero-callee frontier drained to 2 hard
  candidates, but an inline scan (sys.path scripts; promote_frontier.analyze() graph) found
  **112 NEAR-LEAF candidates** = C2 first-party, size 5-260, callees ALL C3, has C2+ caller.
  This pool REFRESHES as leaves promote. Promote via VERBATIM naked (rel32 call -> mov eax,abs;
  call eax; both sides hit the same real C3 callee at suspended-spawn). Smallest are adjustor
  thunks (add/imul arg; jmp callee) + fixed-arg C3 wrappers (memset etc). Recipe proven r237
  (ZeroTwoRegions 0x477b40). CAVEAT: callees that READ .bss state (e.g. 0x45c330->table 0x88fc88
  all-zero) are degenerate at suspended-spawn -> prefer callees that write observable mem / are
  pure given seeded args. TODO: promote the inline scan to scripts/near_leaf_frontier.py.
- NEAR-LEAF LANE OPENED (round 186, 2026-06-15): pure-leaf suspended-spawn pool drained, but
  107 NEAR-LEAF candidates found (C2 first-party, clean, small, ALL callees already C3) ->
  re/analysis/plans/near_leaf_candidates.tsv. Reimpl pattern = verbatim naked port with each
  `call rel32` to the callee replaced by `mov eax,<callee_abs>; call eax` (callee is hooked at
  suspended-spawn, so BOTH orig+reimpl hit the same reimpl-callee -> consistent). CAVEAT: the
  callee must be PURE or write OBSERVABLE memory; callees that READ .bss state are degenerate at
  suspended-spawn (e.g. 0x45c330->0x45bff0 reads .data table 0x88fc88 = all-zero -> degenerate).
  Prefer near-leaves whose callee is a state-writer (observe the written global) or pure-arith.
- LIBRARY-SKIP CORRECTION (round 184, 2026-06-15): user ruled the statically-linked libpng/zlib
  band (0x516000-0x529fff) is library-skip (NOT first-party promotable). Reverted ALL 4 of MY
  this-session in-band C3s -> C2: r135 0x528e30, r149 0x517200, r179 0x5172f0, r183 0x520990
  (357->353). Band now excluded in promote_frontier.py LIBRARY_BANDS. FLAGGED FOR USER REVIEW
  (prior-session in-band C3s, NOT reverted — user decision): 0x5209d0 Set5209d0, 0x523110
  LoadBE523110, 0x5173d0 Set5173d0, 0x518570 Rmw518570, 0x51ca60 StoreBE51ca60.
- last_round: 2026-06-13 round 82 — Ghidra-decompiled STATE leaves (3 GREEN: CmdBuild5b0dc0Set deref_struct_set + ClearDesc5bde50 ptr_fields_clear 5-field + Table69318cSet indexed_table_set) (212->215)
- BOOT FIXED 2026-06-13 (patch_mashed_fix_camera_res.py): run_diff lane OPEN on any display (validated get_771e78 10/10 GREEN on booted game). The +500 grind is now mechanical — see resume recipe + BOOT BLOCKER note below.
- NEW GOAL (user, 2026-06-13): +500 -> ~705. Feasibility read below.
- TOOLING ADDED THIS SESSION (the efficiency-memo deliverables):
  - scripts/promote_frontier.py — capstone call-graph over MASHED.exe.unpatched;
    emits re/analysis/plans/promote_frontier.tsv = C2 ∩ first-party ∩ zero-callee
    ∩ 5<=body<260 ∩ >=1 C2+ caller. Re-run after every round. (Handles the
    early-return-then-continue false positive; excludes <5B bodies = inline-JMP
    install-crashers, and DEMOTED-crash notes.) Current frontier: 232.
  - scripts/promote_classify.py — disasm-shape auto-classifier; reads the frontier,
    decodes each body, tags AUTO (display-independent, emittable: 5) / STATE (needs
    booted game: 204) / MANUAL (27). Emits reimpl C++ + registry fragment with --emit.
- FEASIBILITY (data-grounded 2026-06-13): first-party C2 pool = 2926 rows
  (render 854, audio 500, util 358, gameplay 279, boot 248, particle 174, ...).
  +500 = ~17% of it -> physically plausible. BUT the display-INDEPENDENT leaf vein
  is now PROVEN ~drained (frontier=232 leaves; only ~3-5 are display-independent
  pure shapes the early_window handlers cover; the rest read live state). The
  204 STATE rows + the broader pool REQUIRE run_diff against a BOOTED game.
- BOOT BLOCKER RE-DIAGNOSED 2026-06-13 (CORRECTS the "display wedge" framing):
  MASHED AVs 0xC0000005 ~4s into boot, BUT D3D9 CreateDevice SUCCEEDS (probe
  re/frida/d3d9_createdev_probe.py: adapter=0 HAL 640x480 windowed hr=0, non-null
  device, adapterCount=1). The real fault is a NULL RW-object deref at
  MASHED.exe+0xc7785 (FUN_004c7760, `mov cx,[edi+0x1c]`, edi=NULL): caller
  FUN_0042d4a0 passes arg2 = (FUN_004671a0(-1))->[0x60] which is NULL — a
  per-screen camera/raster sub-object, null with 1 monitor (onset correlated with
  3->1 topology, round 35). This blocks run_diff (engine LUT root only populates
  after this init). NOT a reboot/no-display problem. Probes: boot_crash_probe.py,
  d3d9_createdev_probe.py, raster_create_probe.py.
- **FIXED 2026-06-13** (root cause: camera frameBuffer raster created at the
  auto-selected desktop video mode 2560x1440, fails against the shim's forced
  640x480 backbuffer). scripts/patch_mashed_fix_camera_res.py forces the screen-dim
  getters FUN_00498bc0/bd0 to return 640x480 (= the shim backbuffer) so the camera
  raster always matches the device on ANY display. VERIFIED: RwRasterCreate(640,480,
  flags=2)->non-null; boot survives 15s to menu; run_diff get_771e78 LUT-ready 10/10
  GREEN. **run_diff lane is OPEN regardless of monitor config.** Full writeup:
  re/analysis/BOOT_CRASH_ROOTCAUSE_2026-06-13.md. NEW 5th boot patch (CLAUDE.md).
- RESUME RECIPE (run_diff lane now open): (1) ensure the 5 boot patches applied
  (incl. patch_mashed_fix_camera_res.py). (2) re-run promote_frontier.py +
  promote_classify.py. (3) the 204 STATE rows are the worklist: promote_classify
  --emit gives reimpl+registry; author per PromoLoop cluster, ONE build per batch,
  run_diff (booted, sequential) each. (4) re-classify GREENs. This is the fast route to 705.
  (3) the 204 STATE rows are the worklist: for each, promote_classify --emit gives
  the reimpl+registry; author in a PromoLoop cluster, build ONCE per batch, run_diff
  (booted) sequentially. (4) classify GREENs via re-classify. This is the fast,
  high-volume route to 705.
- WORKLIST: re/analysis/plans/promote_worklist.tsv; ~39 candidates remain
  (done so far via worklist: rounds 26-28 = 15). Byte-verify each before
  authoring (the auto-classifier over-permits accumulators/dispatchers as
  int_scalar — confirm a pure mov/store+ret body in the disasm dump).
- GOAL (user, 2026-06-13): reach 200 C2->C3. Strategy: batch 5/round from
  re/analysis/plans/promote_worklist.tsv (54 handler-fit candidates pre-curated);
  when that drains, re-curate with the 11-handler inventory over the broader C2
  pool + build handlers for new shape classes (thiscall/dispatcher/COM).

## CARRY-OVER CLEARED: Vec3Lerp deferred (x87 bit-identity, commit 6414f413)
The vec3_lerp diff ran 3x (env recovered, user closed their session): all RED
by 3 ulp on out[0] at t=0.9. Full disasm read: the original keeps t*diff0 in
80-bit FPU precision for the add while truncating diff0 to f32 (asymmetric:
out[2] also truncates t*diff2). MSVC /O2 /fp:precise compiles every C variant
to the same 80-bit-diff0 sequence. RH_ScopedInstall commented out. To finish:
inline __asm replicating the FPU op order, OR split the build to compile this
file /fp:strict. Handler 'vec3_lerp' (SWEEP-CRITICAL) stays banked.
LESSON: x87 float leaves whose original store-and-reloads intermediates are
NOT reliably reproducible in /fp:precise C — budget inline-asm for them, or
skip (the formula recovery is the C2 value; sub-ulp x87 parity is low-ROI).

## Lane queues

### L0 — c3_batch_race1 leftovers
DRAINED (session-1 set promoted round 1; session-2 set promoted round 2;
backfill 0042fe70 moved to L1 head — its config is pre-confirmed)

### L1 — race-lane viable (unlocked ∩ v4-passed, arg_type TO CONFIRM per note)
00411ae0, 00413b80, 00413bb0, 00413cb0, 00413f50, 004150e0, 00417180,
004177b0, 0041ad00, 0041c010, 0041d870, 0041d930, 0041da90, 0041e850,
0041ea80, 0041f030, 00423480, 0042c510, 0042fe70, 00443d10, 00469aa0,
0046b1c0, 004715a0, 0047a020, 0047c160, 00484c70, 00485ef0, 00486460,
00489290, 0048a830, 0048ade0, 0048d540
(0042fe70 pre-confirmed: arg_type none, uint32() returns DAT_0067ea80.
Pre-screened deferrals with reasons — see c3_batch_race1.txt DEFERRED block:
0048a830/0048ade0/00484c70/00489290/00413bb0/00413b80/00413f50/0041e850/0041d870)

### L2 — cheap re-earns (demoted-needs-reimpl; analysis exists, needs reimpl + diff)
00402750, 00492370, 004926c0, 00493480, 00493710, 00493900, 00494f20,
004950b0, 00495120, 00495270, 00498c00, 00499730, 00499ba0, 004a4bb7,
004a774d, 004a8a04, 004aa3e4, 004aa3fe, 004ac04a, 004b6540, 004b6560,
004b6610, 004c2c90, 004c2d90, 004c2fb0, 004c9eb0, 004c9f50, 004c9f60,
004cbc60, 004cbc70, 004cbc80, 004cc7f0, 004cc820
(refresh with `py -3.12 scripts/c2_gate_audit.py` if stale; CAUTION: several
are boot/CRT-band — check each row's gate notes; skip abi-limited)

### L2b — mass-disabled leaf re-verify (NEW r213, 2026-06-15) — HIGH YIELD
46 `MASS-DISABLED RH_ScopedInstall` reimpls exist in src; 29 are C2 + first-party.
These have a COMPLETE reimpl already written — promote = re-enable the install +
a focused early_window "seed-the-state" diff (no new reimpl code). Many disable
reasons ("AV/AV", "crash_equal", "needs-canonical-X-state") are just UN-SEEDED
state (null indirect ptr / zero divisor) — seed it and re-verify. Recipe proven
on FpsDiscretise (r211, seed QPF freq), MusicGroupVolumeSet (r212, build list),
FontCtx_ResetTransform (r213, seed indirect ctx ptr).
PROMOTED: 00493480(r211), 00552750(r213).
GATED — DO NOT (callee<C2 / live-state / library / entry / timer-valued-return):
  00552e40 FontCtx_FlushMatrix (callees RwMatrixMultiply/Invert/cam-accessor C1 + live cam),
  004a4bb7 WinMainEntry (process entry, not a leaf),
  004950b0 QpcTimeScaledTo3Mhz (timer-VALUED return -> nondeterministic across calls),
  004b302f StricmpThunk (CRT library), 00428590 ViewportInit (RW cam ops, faults),
  004c5a00/5ae0/5b50 RwTexture* + 004cc7f0/cc820 RwFreeList* (RW engine state),
  00495280/004b6710/6770/67e0 Piz*Open/Read/Close (file I/O).
CANDIDATES — VETTED r214 (most fail the bar; do NOT re-investigate via early_window):
  GATE-FAIL (C1/untracked callee): 0042f8d0 MenuMenusBC (0x427f00 C1), 00552b60
    FontSys_InitSeq (0x4c57a0 C1), 0041c2d0 (0x41d410/0x41de80 C1), 0041d730
    PlayerSlotConfigInit (0x41cdb0 untracked + terminus loop), 00412cf0
    LabelTrailRecordAppend (0x4726f0 untracked + 0x4a2c48 C1 QPC byte).
  TIMER-NONDET: 004c57a0 FontCtxMatrix_AllocInit (QPC sampler), 00492d30
    GameTickStateMachine7 (QpcTimeScaledTo3Mhz), 004926c0 AudioTickAndAvg.
  REFUSED (note inconsistent w/ binary — needs from-scratch re-decode): 005554d0
    FontText_StringWidthAccumulator (binary 1st read [ESI+0x134]+CALL, not the
    leaf the note claims; reimpl is in #if 0).
  DEEP-CALLEE-SIDEFX (callee gate OK but transitive callees have global side
    effects -> sequential Orig/Reim diverge): 00419760 VehicleEliminationSlotInit
    + 00418de0 ...PostInit (-> FUN_004661a0 x4 + FUN_00413c70; EAX-implicit).
  STILL-UNVETTED (assess if revisiting): 00410860 ScoreThresholdStateCheck (591b
    SM, 3 callees), 0041bc50 HudRender29Dispatcher (29-call dispatcher), 00442440
    TransformMatrixUpdate (matrix, some C1 callees), 00495350 IntroSplashOrchestrator.
  => L2b is effectively drained for the early_window solo lane. These live-state /
     deep-callee candidates need the BOOTED run_diff lane (scenario-attach) or the
     promote-c3-batch parallel fanout, not suspended-spawn.
NEW early_window handlers this session (SWEEP-CRITICAL): near_leaf_memset2,
struct_list_float_set, seed_indirect_ctx_obs.

### L3 — broad confirmed-shape pool
Generated per round via c3_filter_v4.py over all first-party subsystems;
do not pre-list here. Done/deferred rows accumulate below.

### L4 — degenerate-GREEN triage (evidence repair; only when L0–L3 empty)
184 residuals in re/analysis/DEGENERATE_GREEN_AUDIT_2026-06-12.md /
DEGENERATE_GREEN_AUDIT_raw.txt. Done rows accumulate below.

## Done (promoted to C3, with round + evidence)

- 00408af0 AiVehicleFieldPtrGet — round 1, log/diff_ai_vehicle_field_ptr_get.csv 10/10 GREEN
- 00442cc0 AiVehicleFloat4Get — round 1, log/diff_ai_vehicle_float4_get.csv 10/10 GREEN
- 00414030 AiSplineBankTimerReset — round 1, log/diff_ai_spline_bank_timer_reset.csv 10/10 GREEN (-1 fill path untested)
- 0040e350 GetRenderSubMode — round 1, log/diff_get_render_sub_mode.csv 10/10 GREEN
- 00429300 HudOverlayFloatGet — round 1, log/diff_hud_overlay_float_get.csv 10/10 GREEN (after FILD root-cause: body is FILD int32->float, bytes DB 05 B8 91 89 00 C3 @0x29300; first attempt as float-load REDed)
- 00442410 CameraSlotFieldPtrGet — round 2, log/diff_camera_slot_field_ptr_get.csv 10/10 GREEN
- 00442420 CameraSlotFloatGet — round 2, log/diff_camera_slot_float_get.csv 10/10 GREEN (FLD byte-verified)
- 00423b20 CarSnapshotDwordGet — round 2, log/diff_car_snapshot_dword_get.csv 10/10 GREEN
- 00426cc0 VehicleTable4cPtrGet — round 2, log/diff_vehicle_table_4c_ptr_get.csv 10/10 GREEN
- 00442df0 RaceFloat898980Get — round 2, log/diff_race_float_898980_get.csv 10/10 GREEN (FLD byte-verified)
- 0042fe70 VehicleDword67ea80Get — round 5, log/diff_vehicle_dword_67ea80_get.csv 10/10 GREEN via read_global (none+race was exit-5: global genuinely 0 in Quick Battle)
- 004cbc60 RwGlobal7d4598Set — round 5 L2 RE-EARN, log/diff_rw_global_7d4598_set.csv 10/10 GREEN
- 004cbc70 RwGlobal7d4598Get — round 5 L2 RE-EARN, log/diff_rw_global_7d4598_get.csv 10/10 GREEN
- 004cbc80 RwGlobal7d459cSet — round 5 L2 RE-EARN, log/diff_rw_global_7d459c_set.csv 10/10 GREEN
- 004c9f50 RwGlobal7d4134Set — round 6 L2 RE-EARN, log/diff_rw_global_7d4134_set.csv 10/10 GREEN
- 004b6610 BootGlobalPairSet — round 6 L2 RE-EARN, log/diff_boot_global_pair_set.csv 10/10 GREEN (multi_arg_global_write, guard-inside-window trick)
- 004b6560 BootGlobalPairSetThunk — round 6 L2 RE-EARN, log/diff_boot_global_pair_set_thunk.csv 10/10 GREEN (call-through reimpl)
- 00499730 BootPtr773818Get — round 7 L2 RE-EARN, log/diff_boot_ptr_773818_get.csv 10/10 GREEN (constant-VA return)
- 00495120 TimerQpfStore — round 7 L2 RE-EARN, log/diff_timer_qpf_store.csv 10/10 GREEN (scalars_to_scattered_globals)
- 004430a0 Util897fe0Set — round 8 L3, log/diff_util_897fe0_set.csv 10/10 GREEN
- 004430b0 Util897fe0Get — round 8 L3, log/diff_util_897fe0_get.csv 10/10 GREEN
- 0042f790 GhostMode::IsActive — round 8 L3, log/diff_ghost_mode_is_active.csv 10/10 GREEN
- 00431d70 Course::GetLeaderIndex — round 8 L3, log/diff_course_get_leader_index.csv 10/10 GREEN
- 00498be0 RenderBitDepthGet — round 9 L3, log/diff_render_bit_depth_get.csv 10/10 GREEN
- 0040dc80 UtilFloat63b910Get — round 9 L3, log/diff_util_float_63b910_get.csv 10/10 GREEN (FLD byte-verified)
- 0040dc90 UtilSlotIndexCondGet — round 9 L3, log/diff_util_slot_index_cond_get.csv 10/10 GREEN (5/10 path untested)
- 00429860 RaceStateFlagGet — round 9 L3, log/diff_race_state_flag_get.csv 10/10 GREEN
- 00429840 RaceStateLatchSet — round 9 L3, log/diff_race_state_latch_set.csv 10/10 GREEN (latch branch via fill=0xFF)
- 0040e360 RaceMode::Set — round 10 L3, log/diff_race_mode_set.csv 10/10 GREEN (restricted-valid vectors)
- 0040b410 RaceScoreTimerGet — round 10 L3, log/diff_race_score_timer_get.csv 10/10 GREEN (race lane)
- 004cae90 RwCapsBlockPtrGet — round 11 L3, log/diff_rw_caps_block_ptr_get.csv 10/10 GREEN
- 0042f520 ViewportBlockPtrGet — round 11 L3, log/diff_viewport_block_ptr_get.csv 10/10 GREEN
- 00485370 DynamicObjectListGetBase — round 11 L3, log/diff_dynamic_object_list_get_base.csv 10/10 GREEN
- 00405420 ReplayCursorReset — round 11 L3, log/diff_replay_cursor_reset.csv 10/10 GREEN (fill=0xFF zero-store)
- 0041b510 HudCounterReset — round 13, log/diff_hud_counter_reset.csv 10/10 GREEN
- 00431b10 BootParamSet2 — round 13, log/diff_boot_param_set2.csv 10/10 GREEN
- 004d6e60 TexStageCacheGet — round 13, log/diff_tex_stage_cache_get.csv 10/10 GREEN (race lane after menu exit-5)
- 004cc7e0 RwGlobal6182b0Set — round 14 CLASSIFY-ONLY (U-5102 resolved via Ghidra xref; round-8 GREEN evidence)
- 00402f40 Util636ad8Get — round 15, log/diff_util_636ad8_get.csv 10/10 GREEN (caller gate filled round-14 Ghidra pass)
- 004c9eb0 DeviceModeBestBelowSet — round 16, log/diff_device_mode_best_below_set.csv 10/10 GREEN non-degenerate (58-instr vtable scanner; disassembly-pinned __stdcall + buffer+8)
- 00485360 DynObjListGetCount — round 17, log/diff_dyn_obj_list_get_count.csv 10/10 GREEN
- 00550790 FsManager7dc76cSet — round 17, log/diff_fs_manager_7dc76c_set.csv 10/10 GREEN
- 00496920 TimerTable772ffcGet — round 17, log/diff_timer_table_772ffc_get.csv 10/10 GREEN (race lane)
- 0041efc0 Car::GetLapProgress — round 18, log/diff_car_get_lap_progress.csv 10/10 GREEN (double-deref, race lane)
- 0040b620 SlotSortByModeScore — round 19, log/diff_slot_sort_by_mode_score.csv 10/10 GREEN (NEW outbuf_only handler validated here)
- 0040e3a0 PlayerColorTableGet — round 19, log/diff_player_color_table_get.csv 12/12 GREEN (int_outbuf4; D-10807 caller gate now satisfied)
- 00495270 HWNDGet — round 20, log/diff_hwnd_get.csv 10/10 GREEN (outbuf_only, call-through)
- 00484c70 WorldObjectsBaseGet — round 20, log/diff_world_objects_base_get.csv 10/10 GREEN (outbuf_only+fold_ret)
- 0041da90 DeltaTimeOutGet — round 20, log/diff_delta_time_out_get.csv 10/10 GREEN (outbuf_only, race lane)
- 0041f030 TriggerStructRead — round 21, log/diff_trigger_struct_read.csv 10/10 GREEN (int_copy_outbuf 16, race)
- 0041f260 WorldMatrixCopy — round 21, log/diff_world_matrix_copy.csv 10/10 GREEN (int_copy_outbuf 64, double-deref, race)
- 00496900 SlotActiveThunk — round 24, log/diff_slot_active_thunk.csv 10/10 GREEN (int_scalar, race, call-through thunk)
- 00415860 InteractionCooldownSet — round 24, log/diff_interaction_cooldown_set.csv 10/10 GREEN (slot_block_zero)
- 0046cbb0 CarStatePairGet — round 25, log/diff_car_state_pair_get.csv 10/10 GREEN (NEW int2out handler validated here)

## Deferred (with reason — a future round or lane may reclaim)

- 0048a830 particle — void_write_observe covers DAT_0071fa34 but not the
  0x200-byte buffer effect; decide coverage policy
- 0048ade0 particle — only effect is the buffer call; no handler observes it
- 00484c70 ai — fn(out_ptr) single-out-pointer shape; no matching handler
- 00489290 particle — 104-byte-record range fill; needs multi-record observe
- 00413b80/00413bb0/00413f50 hud — VehicleIcons object ops; needs
  struct-observe curation against the object layout
- 0041e850/0041d870 hud — effects entirely inside callees
- 0041f030 ai (round-3 triage) — fn(int, out4*) writes 16B; int_with_out_ptr
  does NOT fit (allocs 8B + compares return only; fn is void) — would be
  vacuous + heap overflow. Needs out-buffer-compare handler w/ size config.
- 0041da90 ai (round-3 triage) — fn(out*) single-out-pointer (same class as
  00484c70); *out = DAT_0063d588
- 00443d10/004150e0 ai (round-3 triage) — x87-FPU-stack arg convention;
  exact param list unresolved (U-1427 / D-1127); no handler class exists
- 00423480 ai (round-3 triage) — returns constant &DAT_00644110 every call
  (degenerate); real effect = sprintf into path buffer, unobserved
- 00486460 particle (round-3 triage) — calls FUN_00472650 (random in range)
  mid-body → bit-identity impossible without RNG seeding; also (in_ptr3,int)
  14-parallel-array write shape has no handler
- 0046b1c0 vehicle (round-3 triage) — fn(uint slot, float in6*) writes 48
  dwords into 0x00881630+slot*0xd04 block; no (int,in_buf)+block-readback
  handler
- 0041d930 hud (round-3 triage, CANDIDATE-WITH-CAVEATS) — void() slide
  animator; could run via void_write_observe on DAT_0063d588 (handler
  saves/restores) IF RW callees 004c51a0/004c1480/004c13e0 are C2+ and
  calling them off-frame is safe; revisit when L1/L2 thin out
- 0041ea80 ai TrackDescField40Get (round-5) — reimpl authored + registered
  (PromoLoop_round3.cpp), but exit-5 in Quick Battle: DAT_0063d7e4 IS
  populated (no AV) yet the +0x40 lap-line-gate field is genuinely 0 in
  ARENA mode (no lap lines). Needs a lap-based-race drive recipe OR a
  pointer-indirect seed handler. Stays C2; registry entry kept (race)
- 004c9f60 render (round-6) — 66B conditional setter: DAT_007d413c guard +
  live-vtable call (*DAT_007d4110+0x50) on the DAT_007d4120!=0 path; A/B
  side effects into a live RW object — needs a bespoke save/restore design
- 004b6540 boot thunk->FUN_004b6640 (round-6) — target ALLOCATES a 0x58
  block via vtable freelist and stores the fresh pointer into 3 globals:
  nondeterministic across A/B calls. Possible fit: allocator_nonnull
  (compare nonnull-ness only) — design-first, weak evidence
- 004c9eb0 render (round-7) — 146B setter + device-list scan; analysis-note
  PROSE AMBIGUOUS on inner-loop variable roles (uVar1/uVar3 assignment
  ordering, break semantics) — verbatim reimpl needs a Ghidra decomp
  transcript first (NO-GUESSING); queue for a scribe/pool pass
- 00493900 boot (round-7) — 448B cmdline tokenizer; live cmdline carries no
  -vs/-cs/-l tokens -> all global writes skipped -> degenerate; needs
  arbitrary-string seeding of the 0x00773818 buffer per vector (handler ext)
- 004926c0/00493480 boot/util (round-7) — QPC time-varying outputs; synthetic
  bit-diff inapplicable; needs a behavioral/tolerance evidence lane
- CRT-band L2 rows 004a4bb7 (PE entry) / 004a774d (critsec init) / 004a8a04
  (TLS init) / 004aa3e4 (2-global predicate, one live path only) / 004aa3fe
  (HeapCreate) / 004ac04a (file-handle table) — live one-shot CRT init side
  effects or single-path coverage; not promotable via menu-attach re-call
- 0046c750/0046c730 ai EntityVelocityCounterGet/EntityDamageStateGet
  (round-10) — reimpls authored + registered (PromoLoop_round10.cpp) but
  exit-5: the exposure counter at 0x00882194+i*0xd04 is genuinely 0 across
  all 16 slots in a clean ~20s drive (likely accumulates only under
  damage/exposure, per FUN_0046d7f0 accumulate+drain). Needs a
  damage-inducing scenario (drive recipe ext) — stays C2, entries kept
- 00496930 util TimerTable773030Get (round-17) — reimpl authored + registered
  (PromoLoop_round17.cpp), exit-5 at BOTH menu-attach AND Quick-Battle race:
  the 0x00773030 table is zero in the available scenarios (parent gated
  DAT_006147b8 — likely time-trial/lap mode, not arena). Needs a different
  drive recipe; stays C2, entry kept (scenario:race)
- 004b68e0 render (round-17) — 5B getter DAT_007d3e4c (archive-loaded flag);
  ZERO direct callers, reached only via JMP-thunk 0x004b65a0 (itself C2 with
  C2 callers). Gate NOT stretched: deferred until a direct C2+ caller or a
  policy decision on thunk-only caller gates
- 0046cbb0 vehicle (round-18) — 47B two-out-pointer (fn(i, out_state*,
  out_secondary*) from per-car struct DAT_00881f90 stride 0xd04, guard
  i<=0xf). Needs the out-buffer-compare handler (L5 wishlist) — same class as
  0041f030/0041da90/00484c70/00495270. Count of this shape now >=5 confirmed
- 0041c010 util (round-18) — 116B float-block writer: copies 24B of
  (+/-0.45,0.33,1.0) to &DAT_005f334c, calls FUN_0041b770 (C2) 2x over a
  0x16c-stride record table, zeros DAT_0063cda0. Callee FUN_0041b770 advances
  an UNOBSERVABLE global cursor (S-3682) -> A/B state hazard for save/restore
  diff. Needs the cursor address pinned (Ghidra) + a multi-region observe
  ([0x005f334c len 24] + [0x0063cda0 len 4] + cursor save/restore) before it
  can diff cleanly. U-3652
- 004cc7e0 render (round-8) — GREEN EVIDENCE IN HAND (void_setter_observe
  10/10, log/diff_rw_global_6182b0_set.csv; reimpl in PromoLoop_round8.cpp)
  but U-5102 carries an EXPLICIT Blocks=C2->C3 — promotion refused per the
  re-classify rails. Unblock: trace reads of 0x006182b0 (Ghidra
  reference_to), resolve/downgrade U-5102, then classify-only (no re-diff)

## Harness-extension wishlist (lane L5: implement when one entry unlocks ≥10 rows)

- DONE (rounds 19-20): outbuf_only handler (SWEEP-CRITICAL, diff_template.js)
  with out_buf_size + round-20 fold_ret + seed_global. Promoted 0040b620,
  0041da90, 00484c70, 00495270. int_outbuf4 (pre-existing) promoted 0040e3a0.
  CAVEAT BANKED: seed_global does NOT work for globals the game thread writes
  every frame (it overwrites the seed) -> use the race lane for those.
  REMAINING out-ptr rows to survey/promote (the 20-row count minus 5 done):
  the larger fn(...,out*) bodies (0041f030 16B 4-dword out, 0041f1e0/0041f260
  matrix-copy 64B out, 00425fa0 4-word export, 004224d0, 004b4650 Vec3Lerp,
  etc.) — most fit outbuf_only with a bigger out_buf_size + scalar args; survey
  next rounds.
- STILL NEEDED (separate handler): multi-out-ptr fn(i, out_a*, out_b*) for
  0046cbb0 + 00430b30 (3 out-params, thiscall) — a 2nd ext when those cluster.
  ALSO: scalar-arg + outbuf (fn(int i, T* out) with out>4B) — int_copy_outbuf
  exists for this; survey which out-ptr rows take a leading scalar.
- multi-record buffer observe (base, record_size, count → fingerprint):
  unlocks 00489290, 0048ade0-class
- struct-observe with field map for global objects: VehicleIcons trio +
  TBD count
- (int, in_buf) + global-block readback (seed input buffer, fingerprint
  target block): unlocks 0046b1c0-class
- COM/DirectShow lifecycle harness: 94 rows (audit) — large, design-first

## Round log + session summaries (ARCHIVED)

The per-round narration (`## Round log`, `## STRATEGIC CHECKPOINT 2026-06-13`,
`## SESSION SUMMARY 2026-06-13`, `## Final report`) moved to
`re/analysis/archive/PROMOTION_LOOP_LEDGER_history_2026-06.md` (~260 KB) on
2026-07-01. Live round state (counters, lanes, done, deferred, wishlist) stays
HERE — /promote-round keeps appending new round-log entries below this line.


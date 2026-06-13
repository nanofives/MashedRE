# Promotion loop ledger

State file for `/promote-round` (run under `/loop /promote-round`). This file
is the ONLY state that survives between rounds — every round reads it first
and appends to it last. Initialized 2026-06-12.

Goal (user, 2026-06-12): promote everything promotable. The loop ends after
two consecutive dry rounds, leaving the final gated-remainder report below.

## Counters

- rounds_run: 35
- total_green: 96
- dry_counter: 0
- last_round: 2026-06-13 round 35 BLOCKED-ENV (3 float getters authored+built+wired, pending diff; D3D9 boot wedge)
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

## Round log

(append one row per round: date | lanes used | attempted | GREEN | deferred | exit-5/6 | dry_counter)

2026-06-12 | round 1 | L0 | attempted 5 (race1 session-1 set) | GREEN 5 | deferred 0 | exit-5/6: none (one legit RED on 00429300 float-load, root-caused to FILD same round) | dry_counter 0. Housekeeping: removed stale frida-sweep-20260520-1800 WIP flag (released same day per CHANGELOG, claim flag never deleted — commit 0b6bbbf1); baseline build flaked once ([ERROR] exe build failed) then passed twice consecutively unchanged — transient (suspect file lock on freshly-linked exe); watch for recurrence. Nav note: every race drive logs "[nav] timeout: d3 depth=2 phase=3" then still reaches the race — cosmetic but consistent.

2026-06-12 | round 2 | L0 | attempted 5 (race1 session-2 set) | GREEN 5 | deferred 0 | exit-5/6: none; zero REDs (all 5 bodies byte-verified against MASHED.exe.unpatched BEFORE authoring — adopting this as standing round practice after round 1's FILD lesson) | dry_counter 0. L0 drained; U-8986/U-8987 filed for the camera notes' unfiled markers. Next round: L1 (note-read + arg_type confirmation per candidate; 0042fe70 pre-confirmed config goes first; honor the pre-screened deferral list).

2026-06-12 | round 16 | Ghidra disassembly pass (Mashed_pool2 read-only) | attempted 1 | GREEN 1 (004c9eb0 DeviceModeBestBelowSet — the last identified candidate) | deferred 0 | exit-5/6: none | dry_counter 0. The disassembly pinned the two unknowns the decomp left open: (1) both vtable calls are __stdcall — verified by the ABSENCE of a caller-side `add esp` after each CALL (callee pops 12 / 20 bytes), object pushed as explicit first arg so NOT __thiscall; (2) uStack_8 = buffer+8 — LEA EDX,[ESP+0xc] at ESP=E-0x1c gives buffer=E-0x10, and MOV EAX,[ESP+0x14] reads E-0x8. Faithful 58-instr reimpl GREEN non-degenerate at menu-attach (device object live post-RW-init). LESSON BANKED: when a decomp tags calling_convention `unknown`, the __stdcall-vs-__cdecl question is answered by whether a caller-side `add esp,N` follows the CALL — one listing_disassemble_function call settles it; this unblocks the whole class of indirect-vtable-dispatch C2 functions. POOL: pool2 read-only program_close clean; pool0/pool1 still poisoned.

2026-06-13 | rounds 27-28 | worklist batches (continuous; goal 200) | attempted 10 | GREEN 10 | exit-5/6: none | dry_counter 0. R27: 5 global getters (Global67eca4/67ed6c/771968 uint + Float67eaa8/PowerupRange float; read_global). R28: 5 setters+getter (Set67eaa8/77396c/773978 void_setter; GhostMode::Clear scalars_to_scattered fill; PowerupTargetPtrGet read_global on *(0x00684dac)+0x30). All callers C2 (batched reference_to). 69 total; ~39 worklist candidates remain. CADENCE: working continuously (not waiting on cron), 5/round, ~1 build + 5 diffs per round.

2026-06-13 | round 26 | worklist batch (NEW: re/analysis/plans/promote_worklist.tsv, 54 handler-fit pre-curated; goal=200) | attempted 5 | GREEN 5 (RwGlobal7d459cGet, Flag63b908Get/Set, ElapsedTimeGet, AiTargetEnableGet — all single-global read_global/void_setter_observe leaves) | deferred 0 | exit-5/6: none | dry_counter 0. THROUGHPUT SHIFT: batching 5/round via the worklist + a single Ghidra reference_to pass for all callers + one build + 5 diffs. ~49 worklist candidates remain (will run ~10 rounds to ~108). Then: re-curate broader C2 (filter_v4 rejected callee/vtable/thiscall shapes that some of the 11 handlers may now cover) + author handlers for the remaining classes (thiscall-3out for 00430b30, dispatcher for 004516d0/004d4f00, COM/DirectShow band ~94 rows). 200 is reachable but the back half needs new handlers + possibly the COM harness.

2026-06-13 | round 25 | L5 int2out (two-out-ptr getter) | attempted 1 | GREEN 1 (CarStatePairGet 0x0046cbb0) | deferred 0 | exit-5/6: none | dry_counter 0. Authored the int2out handler (SWEEP-CRITICAL): T fn(int idx, U* out_a, U* out_b), compares both outs + the return. Validated on 0046cbb0. HANDLER INVENTORY now: int_scalar, read_global, none, void_setter_observe, slot_block_zero, scalars_to_scattered_globals, int_outbuf4, int_copy_outbuf, outbuf_only (+fold_ret/seed_global), int2out, multi_arg_global_write — covers most getter/setter/out-ptr shapes. REMAINING out-ptr: 00430b30 (thiscall, 3 out-params — needs thiscall+3out handler), 004516d0 (dispatcher switch on *param_2), 004d4f00 (pipeline linear search), 0041f1e0 (2int+outbuf+RW-iterator callee). Plus Vec3Lerp x87 (inline-asm TODO). Next round: survey these or re-run widened getter curation. Yield holding ~1-2/round.

2026-06-13 | round 24 | resume after Vec3Lerp deferral; broad scan for clean shapes with EXISTING handlers | attempted 2 | GREEN 2 (SlotActiveThunk call-through int_scalar race; InteractionCooldownSet slot_block_zero menu) | deferred 0 new | exit-5/6: none | dry_counter 0 (RESET). Both fit existing handlers (no new code). The simple-getter/setter scan now returns ~3 (00496900+00415860 promoted; 0040d040 Course::ValidateCarsFinished has callees = complex). REMAINING author-able with existing handlers is thinning again; next rounds need either the multi-out-ptr handler (0046cbb0 fn(i,out_a,out_b) — U-rows all Blocks:none, caller 00410d10/0040e180 C2; ~clean) or more complex out-ptr bodies (004516d0 dispatcher, 004d4f00 pipeline search). 0046cbb0 is the next clean L5 (multi_out2 handler). Vec3Lerp x87 remains the one inline-asm/fp-strict TODO.

2026-06-13 | round 23 FINAL | round-22 carry-over | attempted 1 (vec3_lerp) | GREEN 0 | deferred 1 (Vec3Lerp x87 bit-identity — see CARRY-OVER CLEARED) | exit-1 RED x3 (all 3-ulp out[0] t=0.9; 3 reimpl variants identical -> /fp:precise artifact) | dry_counter 1 (0 GREEN; pool NOT dry). User closed mashed_re mid-round -> build unblocked -> diagnosed + deferred cleanly. Next round resumes the out-ptr survey / widened curation (expected GREEN -> reset dry_counter). NOTE on the earlier exit-1 line below: superseded by this FINAL row. The diff finally ran (env recovered) and gave a clean RED -> the non-volatile reimpl kept 80-bit precision. Authored the volatile-float fix but CANNOT verify: the user's mashed_re.exe (pid 2660) locks build\mashed_re.exe -> LNK1104 (killed a zombie MASHED 38072 first; waited 5 min, user session persists). LESSON BANKED: x87 float reimpls of functions that store-and-reload intermediates need `volatile` (or /fp:strict) to match the original's f32 truncation — a plain `float` local under /O2 keeps 80-bit precision and REDs by 1-3 ulp. Reschedule ~25 min for the user's session to close.

2026-06-13 | round 22 | L5 vec3_lerp (clean-leaf-with-existing-handler scan = 0 -> otherwise-dry -> L5 warranted) | attempted 1 (Vec3Lerp 0x004b4650) | GREEN 0 — BLOCKED-ENV (not dry) | deferred 1 (0041f1e0: 2-int+outbuf sig + RW-iterator callee FUN_004c0ed0 possible side-effects) | exit-3 TIMEOUT x4 (incl. a known-good hook -> isolated to env, not handler) | dry_counter 0. The D3D9-init outage from rounds 3-5 RECURRED between the (passing) baseline build and the first diff: boot-probe exits 0xFFFFFFFF after RwEngineOpen even after killing all MASHED/mashed_re procs; 3 displays enumerated. Same class as 2026-06-12 ~19:15. Likely display sleep / GPU wedge — needs the machine to recover (user reboot or display wake). Code is committed WIP. Reschedule ~25 min.

2026-06-12 | round 21 | out-ptr survey: int_copy_outbuf (fn(int i, T* out)) | attempted 2 | GREEN 2 (TriggerStructRead 4-dword; WorldMatrixCopy 16-dword double-deref) | deferred 1 (00425fa0: callees FUN_004b3f90 x3 + conditional FUN_004e5fc0 side-effect U-5769 — not a clean callee gate, and a guarded-empty-at-menu write) | exit-5/6: none | dry_counter 0. The int_copy_outbuf handler (pre-existing, fn(int idx, T* dst)) covers the scalar-arg+outbuf shapes cleanly — no new handler needed. 50-promotion milestone crossed (51). Out-ptr remaining: 0041f1e0 (callee FUN_004c0ed0 — check gate), 004b4650 Vec3Lerp (multi-in-vec3+float, specific shape), 004224d0 (157B, FUN_004b6520 zero-fill + copy + fixed floats — bigger), 00425fa0 (callee/guard, deferred), plus the multi-out-ptr class (0046cbb0, 00430b30 thiscall) still needs a 2nd handler. Sustainable: ~2-3/round from the out-ptr + widened-getter veins.

2026-06-12 | round 20 | out-ptr class (outbuf_only) | attempted 3 | GREEN 3 (HWNDGet call-through; WorldObjectsBaseGet fold_ret captures out+return; DeltaTimeOutGet race lane) | deferred 0 new | exit-5 x1 root-caused: seed_global degenerate because the game thread overwrites 0x0063d588 -> flipped to race lane -> GREEN | dry_counter 0. outbuf_only extended with fold_ret (XOR return into fingerprint — needed for getters that write *out AND return) + seed_global (works ONLY for non-game-written globals). The out-ptr vein continues to yield 3/round; ~13 out-ptr rows remain (larger out buffers + scalar-arg variants). The loop is healthily productive again (rounds 17-20: 3+1+2+3 = 9 GREEN over 4 rounds). Sustainable cadence: out-ptr survey + widened getter curation alternating.

2026-06-12 | round 19 | L5 out-pointer (counted 20 C2 out-ptr rows -> >=10 bar MET) | attempted 2 | GREEN 2 | deferred 0 new | exit-5/6: none | dry_counter 0. TWO discoveries: (1) the int_outbuf4 handler for fn(int idx, byte* out4) ALREADY EXISTED in diff_template.js (built ma3-frida-s5) with a live registry entry for PlayerColorTableGet — it was never run because the caller gate (D-10807) was unmet; FUN_00434720 has since reached C2, so it promoted with zero new code (re-run -> 12/12 GREEN). LESSON: before authoring an L5 handler, GREP diff_template.js for an existing one — handlers get built and orphaned when their first candidate is gated. (2) Authored the genuinely-new outbuf_only handler (SWEEP-CRITICAL) for single-out-ptr fn(T* out), validated on SlotSortByModeScore (re-enabled its mass-disabled RH_ScopedInstall) -> 10/10 GREEN. Both callers = FUN_00434720 frontend C2. The out-pointer class is now OPEN: next rounds promote 0041da90/00495270/00484c70 (single out*) via outbuf_only + survey the arg-shape bucket (321 rows) for more idx+outbuf shapes via int_outbuf4. POOL RE-OPENED: the loop is NOT near-dry — the out-ptr vein (20 rows) is freshly unlocked.

2026-06-12 | round 18 | widened curation (third-shape <=15B = 0 hits; widened to 16-60B) | attempted 1 | GREEN 1 (0041efc0 Car::GetLapProgress, double-deref race-lane getter) | deferred 2 (0046cbb0 two-out-ptr -> L5; 0041c010 cursor hazard -> needs Ghidra cursor-pin + multi-region observe) | exit-5/6: none | dry_counter 0. SIGNAL: the trivial single-global-leaf vein is now genuinely EXHAUSTED (third-shape <=15B found 0). The 16-60B net yields ~1 author-able per pass + accumulating out-ptr deferrals. The out-buffer-compare handler (L5) is now at >=5 confirmed unlocks and rising — it is the clear next high-leverage move. RECOMMENDATION TO USER: rounds 19+ should either (a) authorize the out-buffer-compare L5 extension (one handler unlocks 5+ now, likely 10+ after an arg-shape-bucket curation), or (b) accept ~1 promotion/round from progressively-wider getter curation until that too dries. Note: user's mashed_re.exe standalone session is running concurrently (pid 29596) — builds have not been blocked so far this round, but a future rebuild may hit LNK1104 (wait it out).

2026-06-12 | round 17 | fresh discovery sweep (broadened single-global-leaf curation over loop_round_8_passed, excluding all 40 prior touches) | attempted 4 (00485360, 00550790, 00496920, 00496930) + 1 dropped pre-author (004b68e0: thunk-only caller) | GREEN 3 | deferred 2 (00496930 exit-5 in menu AND race — table zero in arena, needs time-trial/lap mode; 004b68e0 gate) | exit-5 x3 root-caused (2 fixed via menu->race flip, 1 genuinely unreachable scenario) | dry_counter 0. The "POOL EMPTY" call from round 16 was PREMATURE — a broadened curation regex (returns/writes DAT_ + leaf/trivial/stub + size<=35, minus all priors) surfaced 6 fresh single-global leaves the round-8/11 passes missed (their regexes required the literal word "getter"/"setter"). 4 were author-able, 3 GREEN. LESSON: "pool dry" should mean "broadened curation finds nothing", not "my last regex found nothing" — vary the curation shape before declaring dryness. Remaining from this pass: 0041c010 (116B float-block writer — multi_arg/scattered, larger; a real candidate for a careful round). Next round: re-curate once more with a THIRD regex shape (arithmetic leaves, 2-arg ops, fn-ptr-return) + attempt 0041c010; if that yields <1 author-able, THAT is dry round 1.

--- ROUND-16 "POOL EMPTY" CALL (superseded by round 17) ---
After 16 rounds (40 promotions; 14 productive, 2 env/contention skips fully
recovered) every candidate the curation/triage passes surfaced is
promoted-or-deferred-with-reason. The next round has NO pre-identified
candidate to author. Lanes status:
  L0 race1-leftovers   : DRAINED (rounds 1-2)
  L1 race-lane         : promoted what was viable; residue is deferred
                         (callee-internal effects / out-ptr shapes / lap-mode)
  L2 demoted-needs-reimpl: all 33 promoted or triaged-out (CRT one-shot init,
                         QPC time-varying, COM — none menu-attach-promotable)
  L3 curated leaves    : both curation passes (strict + loose) exhausted
  L4 degenerate-repair : 184 residuals but yields NO new C3s (evidence repair)
  L5 harness-ext       : out-buffer-compare unlocks ~4 (under the >=10 bar)
The honest call: a 17th round must either (a) author an L5 harness extension
(out-buffer-compare; ~4 unlocks; SWEEP-CRITICAL) which the >=10 rule currently
forbids, or (b) run a fresh c2_gate_audit / c3_filter sweep hunting a NEW vein
(diminishing returns — the obvious shapes are gone), or (c) declare the
demand-driven promotable pool DRAINED. Per the loop's own dry-round rule, the
next round will run the discovery sweep once more; if it surfaces nothing
author-able, that is dry round 1, and a second dry round ends the loop with the
final gated-remainder report.

2026-06-12 | round 15 | round-14 carry-over (build unblocked: user fixed their exe_main.cpp WIP; baseline GREEN) | attempted 1 | GREEN 1 (00402f40 read_global) | deferred 1 (004c9eb0 -> moved to Deferred: needs a Ghidra DISASSEMBLY pass for the vtable calling convention, not just decomp) | exit-5/6: none | dry_counter 0. Loop now at 39 promotions over 15 rounds (13 productive, 2 env/contention skips fully recovered). REMAINING PROMOTABLE POOL is thin and Ghidra-gated: (a) 004c9eb0 (disassembly pass — convention + buf offset); (b) L4 evidence-repair (184 degenerate residuals, NO new C3s — would not grow the count); (c) harness-ext wishlist (out-buffer-compare unlocks ~4, under the >=10 bar). Honest assessment for the user: the cheap-leaf + cheap-re-earn veins are EXHAUSTED; further C2->C3 growth now costs either a Ghidra disassembly session (1 hook) or a harness extension (~4 hooks). The loop will go dry within 1-2 rounds unless L5 is authorized or a new candidate vein is identified. Continuing one more round to confirm dryness.

2026-06-12 | round 14 | Ghidra pass (read-only, Mashed_pool2) | attempted 1 classify-only | GREEN/promoted 1 (004cc7e0 — U-5102 resolved: reference_to 0x006182b0 = 2 refs, both documented) | banked 2 unlocks (00402f40 caller=FUN_0043dfd0 C2; 004c9eb0 transcript appended to note) | — | dry_counter 0. BLOCKED-BUILD context: user's uncommitted exe_main.cpp WIP (112 lines, C2601 mid-edit) fails the exe phase -> no .asi rebuild possible -> no new diffs this round; Ghidra work was read-only so unaffected. POOL HYGIENE: acquire handed Mashed_pool1 but its on-disk lock + JVM-held .lock~ make it POISONED (like pool0) — used pool2 read-only instead, program_close clean; release script removed what it could. Pre-assign pool2+ in future prompts; pool0/pool1 dead until the shared MCP restarts. Next round: boot-probe the build (user WIP may persist — if still broken, ledger the skip), then run the two carry-over authorings.

2026-06-12 | round 13 | L3 (round-12 carry-over) | attempted 3 | GREEN 3 (tex_stage_cache_get via menu->race scenario flip after exit-5: the 2D menu path never populates the per-stage texture cache; in-race 3D bindings do) | deferred 0 new | exit-5 x1 root-caused + fixed in-round | dry_counter 0. POOL STATUS after 13 rounds: looser-curation list exhausted (promoted/deferred all 12); the remaining identified pools are (a) the Ghidra-pass shortlist — now FOUR items: 00402f40 caller-xref, 004c9eb0 decomp transcript, 004cc7e0 U-5102 read-trace, 0041ea80 lap-mode scenario alternative — one shared pool session unlocks up to 4 classify-only/cheap promotions; (b) L4 evidence repair (184 degenerate-GREEN residuals, no NEW C3s); (c) harness-ext wishlist (out-buffer-compare: 4 confirmed unlocks incl. 00495270, under the >=10 bar); (d) deeper L3 re-curation with yet-looser shapes (diminishing returns expected). Recommendation for the user: the per-round marginal cost is rising — the highest-leverage next step is the ONE Ghidra pool session covering shortlist (a), which this loop cannot do cheaply mid-round (shared-MCP solo policy + pool hygiene).

2026-06-12 | round 12 | L3 (looser remainder) | attempted 3 (authored only) | GREEN 0 — BLOCKED-CONTENTION | deferred 1 (004d71f0: all 3 callers third-party-library[renderware] C1 by policy — caller gate unfillable unless a first-party caller surfaces) | exit-2 x3 VOID (stale .asi, export-not-found — NOT verdicts; lesson: ABORT the diff batch when the build fails, the chained command ran them anyway) | dry_counter 0 (contention, not dry). User's mashed_re.exe (22:15:51, still alive >5 min) locks build\mashed_re.exe -> LNK1104 -> .asi cannot rebuild. Carry-over above. Reschedule ~25 min.

2026-06-12 | round 11 | L3 (looser re-curation of loop_round_8_passed: 12 hits) | attempted 4 (00402f40 dropped pre-diff: NO identified caller at C2+ — plate says "xrefs via Ghidra"; caller gate unfillable without a pool pass) | GREEN 4 | deferred 0 new | exit-5/6: none | dry_counter 0. Looser-curation remainder for round 12: 0041b510 (10b zero-setter DAT_0063cab0 — same shape as ReplayCursorReset), 00431b10 (10b setter DAT_007f0f10=2), 004d71f0 (read_global getter DAT_007d6b10), 004d6e60 (indexed getter DAT_007d6b30[i*6] — check stride bytes + bounds), 0046d510 (matrix-transform vec3 — needs vec3-out handler, likely defer), 0046cbb0 (two-out-ptr — wishlist class), 004102f0 (171b coordinator — too big). Plus pending-from-curation: 00402f40 (needs a Ghidra xref to fill the caller gate — queue for a pool pass alongside 004c9eb0's decomp transcript and 004cc7e0's U-5102 read-trace; THREE items now want one shared Ghidra session).

2026-06-12 | round 10 | L3 (round-9 leads) | attempted 4 | GREEN 2 (race_mode_set restricted-valid-vector technique on a live state-machine global; race_score_timer_get race lane) | deferred 2 (entity getter pair — exit-5, exposure counter genuinely 0 in clean drive; needs damage-inducing scenario) | exit-5 x1 root-caused | dry_counter 0. The curated cheap-leaf vein from loop_round_8 is now EXHAUSTED (all 14 curated rows promoted/deferred). Round-11 options: (a) re-curate loop_round_8_passed.tsv with looser shape patterns (indexed getters w/o "getter" keyword, 2-arg setters, ptr-return helpers — expect lower hit rate), (b) L4 degenerate-GREEN evidence repair (184 residuals), (c) L5 wishlist review (out-buffer-compare at 4 confirmed unlocks — under the >=10 bar but the bar is ledger-amendable if the loop would otherwise go dry).

2026-06-12 | round 9 | L3 (round-8 curation remainder) | attempted 5 | GREEN 5 | deferred 0 new | exit-5/6: none | dry_counter 0. New technique: latch-branch coverage via scalars_to_scattered_globals fill=0xFF (forces current!=0 inside the restored bracket → no-store branch exercised). L3 curated remainder for round 10: 0046c730/0046c750 (0xd04-stride physics getters — race lane, in-bounds 0..15), 0040b410 (11b indexed getter — read note for stride), 0040e360 RaceMode::Set (9b setter on the LIVE race-phase global 0x0063ba8c — void_setter_observe saves/restores, but a mid-write phase glitch could perturb the menu; vector with menu-mode values; CAUTION). After those: re-run the curation regexes with looser patterns (indexed getters, 2-global conditionals) or move to L4 evidence repair.

2026-06-12 | round 8 | L3 (c3_filter_v4 sweep loop_round_8: 1685 passed, curated to 14 small getter/setter shapes, picked 5) | attempted 5 | GREEN 5 diffs / PROMOTED 4 | refused 1 (004cc7e0: U-5102 explicit Blocks=C2->C3 — first blocking U-row the loop has hit; honored) | exit-4 attach flake x1 (rw_global_6182b0_set, GREEN on retry — injection failure, not a verdict) | dry_counter 0. L3 curated pool remainder: 9 more from this curation pass (00498be0 5b getter; 0040dc80 6b float getter FILD-CHECK; 0040dc90 23b conditional getter; 00429840/00429860 latch pair; 0046c730/0046c750 0xd04-stride physics getters race-lane; 0040b410 11b indexed getter; 0040e360 RaceMode::Set 9b setter CAUTION live race-phase global) — next round continues here.

2026-06-12 | round 7 | L2 | attempted 2 (00499730, 00495120) | GREEN 2 | deferred 9 this round (004c9eb0 needs-decomp-transcript; 00493900 needs buffer-seeding ext; 004926c0/00493480 QPC time-varying; 004a4bb7/004a774d/004a8a04/004aa3e4/004aa3fe/004ac04a CRT one-shot-init class) | exit-5/6: none | dry_counter 0. L2 effectively TRIAGED OUT: remaining un-attempted L2 rows are 00402750/00492370 (large multi-phase loaders), 004c2c90/004c2d90/004c2fb0 (RW plugin/driver dispatchers — live registry mutation), 004cc7f0/004cc820 (freelist allocators — nondeterministic ptrs), 00495270 (single-out-ptr — wishlist), 00498c00 (live D3D mode-table alloc), 00499ba0 (CoInitialize+CreateWindow), 00494f20 (callee C1), 00493540-dup n/a. NEXT ROUND: L3 (c3_filter_v4 sweep) or evaluate L5 — wishlist out-buffer-compare now unlocks 0041f030+0041da90+00484c70+00495270 = 4 confirmed (<10 bar). If L3 yields <3, consider relaxing the L5 bar or running an L4 evidence-repair round. (004c9f50, 004b6610, 004b6560 thunk) | GREEN 3 | deferred 2 (004c9f60 guard+live-vtable; 004b6540 allocating thunk target) | exit-5/6: none. Baseline build LNK1104 once: the USER's own mashed_re.exe instance (started 21:04) locked the exe output — waited out per the MASHED-running rail (exited 21:07), build then OK. RETROACTIVE root-cause for the round-1/round-3 transient exe-link flakes: same class (a running mashed_re.exe locks build\mashed_re.exe). multi_arg_global_write guard-inside-restored-window trick validated for guard-less multi-param setters (time_display_set_entry precedent). dry_counter 0. L2 remaining: ~24 rows (boot-band 00402750-depth cluster + render dispatchers + CRT-band odd-address rows — expect rising cost; several look abi-limited/oversize-adjacent).

2026-06-12 | round 5 RESUMED ~21:35 (user present; env recovered — boot probe GREEN on re-check) | L1+L2 round-3 carry-over | attempted 5 | GREEN 4 (0042fe70 via read_global switch after exit-5; 004cbc60/70/80 L2 re-earns) | deferred 1 (0041ea80: lap-line gate field genuinely 0 in arena mode — needs lap-race recipe or pointer-indirect seed) | exit-5 x2, both root-caused (genuinely-zero domain values, NOT wrong-scenario): 0042fe70 fixed by read_global seeding; 0041ea80 deferred | dry_counter 0. LESSON: probe-unlocked != discriminating — a populated SCENARIO can still read a legitimately-zero GLOBAL; prefer read_global/seeded handlers for plain getters wherever the address is fixed.

2026-06-12 | round 5 | (boot probe only) | attempted 0 (round-3 five still pending) | GREEN 0 — BLOCKED-ENV persists (~70 min; same clean exit -1 after RwEngineOpen; no user intervention yet) | deferred 0 | — | dry_counter 0. Backing off to 1 h retries. NOTE: a user reboot (the likely fix) kills this session and its /loop — if a fresh session picks this up, this ledger is the full state: run the round-3 five (vehicle_dword_67ea80_get + track_desc_field40_get race-lane; rw_global_7d4598_set/get + rw_global_7d459c_set menu-attach) after one boot probe, then classify the GREEN set.

2026-06-12 | round 4 | (env diagnosis only) | attempted 0 new (round-3 five still pending) | GREEN 0 — BLOCKED-ENV persists ≥40 min | deferred 0 | — | dry_counter 0 (unchanged). DIAGNOSIS MATRIX (all ~19:45-20:00): boot probe FAILS same way (exit -1 = clean exit(-1) after "Calling RwEngineOpen", NOT a crash); round-2-content .asi rebuilt+deployed → ALSO fails (round-3 .asi fully exonerated; build.bat restored after bisect); standalone mashed_re.exe from build\ → ALSO fails (exit 0x3; caveat: cwd-sensitivity of its asset paths unverified); monitors 3/3 Active=True (WMI power state); zero TDR/display/PnP events since 18:30; GPU RTX 5070 Ti PnP status OK; disk 718 GB free (round-3 "1 GB" reading was a display artifact — corrected); videocfg.bin still canonical; session console+unlocked+idle-none; TeamViewer running but NO established connection (phantom virtual monitor adapter present but inactive). CONCLUSION: D3D9 device-init broken system-wide for NEW processes since ~19:15, cause unidentified from inside the session — needs hands-on (reboot likely). Pushed a notification to the user. Next round: boot-probe FIRST (18s aliveness); if it boots, run the round-3 five immediately.

2026-06-12 | round 3 | L1+L2 | attempted 5 (authored: 0042fe70 + 0041ea80 race-lane, 004cbc60/70/80 L2 re-earns) | GREEN 0 — BLOCKED-ENV, not dry | deferred +8 (L1 triage: 0041f030, 0041da90, 00443d10, 004150e0, 00423480, 00486460, 0046b1c0, +0041d930 caveat-candidate) | run_diff died 2x in boot phase (frida InvalidOperationError "script has been destroyed", surfaces as exit 1 NOT 6) | dry_counter 0 (unchanged — environment outage, pool not drained). ROOT CAUSE BISECTED: MASHED stopped booting ~19:15 local — dies after "Calling RwEngineOpen" with zero hooks (MASHED_HOOK_HI=0) AND with the .asi renamed away → fully environmental, round-3 code exonerated. videocfg.bin = canonical (hash match); 3 displays enumerated (power state unknown); ~14 rapid device create/destroy cycles + 2 force-killed boots preceded the failure. Suspect monitors asleep or GPU driver wedge. ACTION: reschedule ~20 min; next round MUST first confirm one manual boot reaches the menu before burning diff attempts. KEY LEARNINGS: (1) int_with_out_ptr allocs 8B + compares return ONLY — read diff_template.js handler before trusting an arg_type name; (2) L2 demoted-needs-reimpl rows are real cheap wins (render trio authored in minutes); (3) gate-check Glob on **WIP** false-positives on dead worktree copies — check repo root only.

2026-06-13 | round 29 | worklist batch (const global setters) | attempted 5 | GREEN 5 (Set77196c_1 0x00493570, Set771970_1 0x00493580, Set603868_0 0x00462510, Set603868_1 0x00462500, Set703058_0 0x00487df0 — all pure `C7 05 <addr> <imm32> C3` single-store leaves; scalars_to_scattered_globals fill=0xFF; menu-attach) | deferred 0 | exit-5/6: none; zero REDs (all 5 byte-verified before authoring) | dry_counter 0. total_green 69->74. SCREEN-1 PIN: user asked to "test on screen 1" — added a SetWindowPos(64,64) pin to the d3d9 shim's ApplyWindowBorders (opt-out env MASHED_RE_NO_SCREEN1_PIN=1), rebuilt+redeployed; verified the MASHED window lands at (51,51) inside DISPLAY1 (primary 0,0 2048x1152). The shim only proxies MASHED.exe, so the standalone is unaffected. 0040d270 (Course::Finish) is the shared caller for both 00462500/00462510 (set/clear pair over 0x00603868). ~34 worklist candidates remain.

2026-06-13 | round 30 | worklist batch (global + table getters) | attempted 5 | GREEN 3 (Global7d3e4cGet 0x004b68e0 + Global7d3e4cGetThunk 0x004b65a0 read_global menu-attach; PowerupTable6885e0Get 0x00455b40 int_scalar scenario:race non-degenerate) | deferred 2 (table_773030_get 0x00496930 + joint_ptr_6ce81c_get 0x004840d0 — both exit-5 INCONCLUSIVE-DEGENERATE: their tables 0x00773030 / 0x006ce81c read all-zero at the race-attach window, unlike sibling 0x00772ffc which populates; need a deeper-into-race / different scenario where those tables are written) | exit-5: 2 (table_773030_get x2, joint_ptr_6ce81c_get x1) | dry_counter 0. total_green 74->77. DROPPED pre-author: 004d71f0 (read_global DAT_007d6b10) — ALL 3 callers are in the RenderWare library band (FUN_005405c0 C1 third-party), fails the C3 "one caller at C2+" gate; requires a first-party caller promotion first. Also deferred-from-curation: 0045a0f0 (bounds-checked table getter, needs full-body read of the v<0/v>=4 branch targets before authoring). LEARNING: table getters that index live arrays are degenerate-GREEN traps even at scenario:race unless that specific table is populated in the attach window — byte-verify the table is non-zero, or expect exit-5 (the powerup table 0x006885e0 populated; the timer 0x00773030 and joint 0x006ce81c did not).

2026-06-13 | round 31 | byte-scan replenished pool (single-global getters) | attempted 5 | GREEN 5 (Global67f19cGet 0x0042f760, Global67f1a0Get 0x0042f770, Global67f1a4Get 0x0042f780 audio trigger-flag trio; CameraPath::GetCount 0x00426bb0; Bezier::GetLocate 0x0045bfe0 — all `A1 <addr> C3` read_global menu-attach) | deferred 0 | exit-5/6: none | dry_counter 0. total_green 77->82. POOL REPLENISHMENT (KEY): the hand-curated worklist + c3_filter_v4 were thinning on trivial leaves, so I byte-scanned original/MASHED.exe.unpatched for the 4 exact opcode shapes my handlers cover (`A1 <a4> C3` u32 getter, `D9 05 <a4> C3` f32 getter, `C7 05 <a4> <imm4> C3` const setter, `8B 44 24 04 A3 <a4> C3` param setter) and intersected with still-C2 rows -> 25 promotable trivial-shape leaves (get_u32×15, get_f32×3, set_const×3, set_u32×4). Scanner output at /tmp/trivial_leaf_hits.csv (PowerShell temp); regenerate any time via the inline scan. This is ~4-5 more rounds of clean leaves. After they drain, the next lever is L5 harness-ext for the deref-param predicate/getter class (e.g. 0x004cc4f0 RW chunk-type validator does `mov eax,[eax]` on its arg -> needs a ptr-to-int-in handler; many leaves share this shape). DROPPED again: 004d71f0 still caller-gated (RW-library-only callers).

2026-06-13 | round 32 | byte-scan pool (getters + param setters) | attempted 5 | GREEN 5 (GetRaceStateField 0x0042d390, Global7e9584Get 0x00499710 HWND, Global7dcabcGet 0x005a7b50 audioctx read_global; Set86ecc8 0x00472640, Set63ba7c 0x0040e170 void_setter_observe) | deferred 0 | exit-5/6: none | dry_counter 0. total_green 82->87. All menu-attach, all callers C2+ (00492e90 boot / 00495150 input / 005a7b60 audio / 004030d0 util / 0043df00 frontend). ~15 trivial-shape leaves remain in the byte-scan pool (get_u32×~7 incl. caller-gated 004d71f0, get_f32×3, set_const×3, set_u32×~2).

2026-06-13 | round 33 | byte-scan pool (getters + const setter) | attempted 5 | GREEN 5 (Global63a5d0Get 0x004075a0, Global63d7e0Get 0x0041e140, Global6c6eb0Get 0x0047ce70, Global772ffcGet 0x00496910 read_global; Set7f1a0c_1000 0x0042b950 scalars_to_scattered_globals) | deferred 0 | exit-5/6: none | dry_counter 0. total_green 87->92. Note 0x00496910 reads the same 0x00772ffc table base that exit-5'd round-16 via int_scalar — read_global SEEDS the global so it's non-degenerate even though the int_scalar table-index path was degenerate. ~10 trivial-shape leaves remain (get_f32×3 untried + a few get_u32/set_const/set_u32 + caller-gated 004d71f0). NEXT: validate the float-getter (D9 05 <a4> C3) path on 0x004039e0 — needs the existing float read_global arg_type confirmed in diff_template.js.

2026-06-13 | round 34 | byte-scan pool (getter/setters + bounds getter) | attempted 4 | GREEN 4 (Global772facGet 0x00495790 read_global; Set684b34 0x0044d6e0 void_setter_observe; Set771968_1 0x00493590 scalars_to_scattered_globals; VehPwrState68ba00Get 0x0045a0f0 int_scalar bounds getter) | deferred 0 | dropped 2 on caller-gate (0x00496d00 caller 00448951 in UNDEFINED region; 0x004b6700 reached by JUMP not CALL; also 0x00495520 caller FUN_004960a0 not in hooks.csv) | exit-5/6: none | dry_counter 0. total_green 92->96. KEY WIN: 0x0045a0f0 is a bounds-checked getter (signed v<0||v>=4 -> -1) -> the out-of-range -1 returns make int_scalar NON-degenerate even at menu-attach (no race needed), unlike the plain table getters that exit-5'd in round 30. hooks.csv note for 0045a0f0 was WRONG ('no bounds check') -> corrected in the C3 row. TRIVIAL-SHAPE POOL NOW NEARLY DRY: remaining are get_f32×3 (need a float-return read_global handler -> L5, only 3 rows so below the >=10 threshold) + caller-gated/jump-target leftovers (004d71f0, 004b6700, 00496d00, 00495520). NEXT LEVER for sustained yield: L5 handler for the deref-param class (e.g. 0x004cc4f0 RW chunk validator does `mov eax,[eax]` on arg) OR re-curate the broader C2 pool for non-leaf shapes the existing handlers can reach.

## STRATEGIC CHECKPOINT 2026-06-13 (after round 34, total_green=96)

The cheap/trivial-leaf C2 vein is DRAINED. Proven by 5 exhaustive byte-scans of
original/MASHED.exe.unpatched intersected with still-C2 hooks.csv rows:
  - u32 getter `A1 <a4> C3`, f32 getter `D9 05 <a4> C3`, const setter
    `C7 05 <a4> <imm4> C3`, param setter `8B 44 24 04 A3 <a4> C3`  -> 25 found,
    ALL promoted across rounds 31-34 except: f32×3 (need a float-return
    read_global handler = L5, only 3 rows < threshold), 004d71f0 (RW-library-only
    callers), 004b6700 (jump-target), 00496d00/00495520 (caller not a tracked fn).
  - single-arg arithmetic leaf `8B 44 24 04 ... C3` (<=12B) -> 28 hits, but all
    are either deref-param (need ptr handler), bounds-getters returning 0
    (degenerate-prone), live-state table getters (exit-5), or tail-call non-leaves.
  - cdecl field getter `8B 44 24 04 8B 40 <off> C3` -> 1 (lua lib).
  - thiscall field getter `8B 41 <off> C3` -> 1 (d3dx9 lib).

CONCLUSION: 69->96 this session (rounds 29-34, all real 10/10 GREEN diffs, full
gate). The remaining first-party C2 inventory (~thousands of rows) is NON-trivial.
Reaching 200 requires a PHASE SHIFT from pattern-scan grinding to bespoke harness
engineering. The path forward, in yield order:

  L5-A. Float-return read_global handler (read_global_f32): seed the target global
        with a float bit-pattern, call the getter, read ST0 as the return. Unlocks
        the 3 D9-05 f32 getters now + every future float getter. ~Small but reusable.
  L5-B. deref-param / bounds-table family via scenario:race: the `83 F8 XX 72 03
        33 C0` bounds getters + `8B 04 85 <tbl>` table getters are promotable IF run
        at a race state where the table is populated. Expect a mix of GREEN + exit-5;
        triage per-candidate. Medium yield, medium cost.
  L5-C. thiscall/cdecl multi-statement field getters & small methods: thiscall_field_get
        + ptr_scratch_field already exist; the trivial single-load shapes are gone but
        2-3 statement field-deref methods (`mov eax,[ecx+off]; <op>; ret`) remain in the
        broader C2 pool. Needs a curation pass (not a byte-scan) over C2 method-shaped rows.
  L5-D. COM/DirectShow band (~94 rows) — needs the COM harness (largest single bucket,
        highest cost). Deferred until A-C are exhausted.

NEXT ROUND should start at L5-A (build read_global_f32, validate on 0x004039e0),
then sweep the 3 f32 getters, then move to L5-B race-state table getters.
This is the durable plan; the ledger counters + this block are the full state.

2026-06-13 | round 35 | L5-A float-getter vein (read_global + ret:float) | attempted 3 | GREEN 0 — BLOCKED-ENV (NOT dry, NOT promoted) | deferred 3 (pending diff) | dry_counter 0 (unchanged). The 3 `D9 05 <a4> C3` float getters (Float5ea0a8Get 0x004039e0, Float89a360Get 0x004173a0, Float61313cGet 0x0046dd80) are authored, byte-verified, built into the .asi, and registry-wired with the VALIDATED method: arg_type 'read_global' + signature {ret:'float'} + clean float-bit-pattern seeds (no NaN/Inf) — NO new handler needed (the generic compare captures the ST0 float JS-number and `===` works). They could NOT be diffed: MASHED began exiting 0xC0000005 (ACCESS_VIOLATION) at boot ~3s. ROOT CAUSE = environmental D3D9 boot wedge correlated with the display topology dropping 3 monitors -> 1 (two displays slept mid-session; primary screen-1 still active but the device-init still AVs — same class as the [[feedback_d3d9_shim_wedges_gpu_driver]] / round-3/4/5 outage). Rounds 29-34 (20+ diffs) booted fine earlier this session with the SAME pinned shim, so this is NOT the screen-1 pin and NOT the float code. RESUME: when the display is restored (user wakes monitors / restores the 3-mon config; a boot probe reaching the menu confirms), run the 3 float diffs FIRST (they are the cheapest pending GREENs and validate the float-getter method), then continue L5-A/B per the strategic checkpoint. The round-35 .cpp/registry/build wiring is committed; hooks.csv stays C2 for the trio until GREEN.

## Final gated-remainder report

(written by the round that ends the loop)

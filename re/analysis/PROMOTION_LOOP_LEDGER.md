# Promotion loop ledger

State file for `/promote-round` (run under `/loop /promote-round`). This file
is the ONLY state that survives between rounds — every round reads it first
and appends to it last. Initialized 2026-06-12.

Goal (user, 2026-06-12): promote everything promotable. The loop ends after
two consecutive dry rounds, leaving the final gated-remainder report below.

## Counters

- rounds_run: 9
- total_green: 28
- dry_counter: 0
- last_round: 2026-06-12 round 9 (5 GREEN, L3 curated)

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
- 004cc7e0 render (round-8) — GREEN EVIDENCE IN HAND (void_setter_observe
  10/10, log/diff_rw_global_6182b0_set.csv; reimpl in PromoLoop_round8.cpp)
  but U-5102 carries an EXPLICIT Blocks=C2->C3 — promotion refused per the
  re-classify rails. Unblock: trace reads of 0x006182b0 (Ghidra
  reference_to), resolve/downgrade U-5102, then classify-only (no re-diff)

## Harness-extension wishlist (lane L5: implement when one entry unlocks ≥10 rows)

- out-buffer-compare handler (fn(args..., out*) with configurable out size,
  compare buffer + return): unlocks 0041f030 (16B), 0041da90 + 00484c70
  (single-out-ptr as the 0-int special case) — 3 confirmed, count more
  in the arg-shape audit bucket (321 rows) when curating
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

2026-06-12 | round 9 | L3 (round-8 curation remainder) | attempted 5 | GREEN 5 | deferred 0 new | exit-5/6: none | dry_counter 0. New technique: latch-branch coverage via scalars_to_scattered_globals fill=0xFF (forces current!=0 inside the restored bracket → no-store branch exercised). L3 curated remainder for round 10: 0046c730/0046c750 (0xd04-stride physics getters — race lane, in-bounds 0..15), 0040b410 (11b indexed getter — read note for stride), 0040e360 RaceMode::Set (9b setter on the LIVE race-phase global 0x0063ba8c — void_setter_observe saves/restores, but a mid-write phase glitch could perturb the menu; vector with menu-mode values; CAUTION). After those: re-run the curation regexes with looser patterns (indexed getters, 2-global conditionals) or move to L4 evidence repair.

2026-06-12 | round 8 | L3 (c3_filter_v4 sweep loop_round_8: 1685 passed, curated to 14 small getter/setter shapes, picked 5) | attempted 5 | GREEN 5 diffs / PROMOTED 4 | refused 1 (004cc7e0: U-5102 explicit Blocks=C2->C3 — first blocking U-row the loop has hit; honored) | exit-4 attach flake x1 (rw_global_6182b0_set, GREEN on retry — injection failure, not a verdict) | dry_counter 0. L3 curated pool remainder: 9 more from this curation pass (00498be0 5b getter; 0040dc80 6b float getter FILD-CHECK; 0040dc90 23b conditional getter; 00429840/00429860 latch pair; 0046c730/0046c750 0xd04-stride physics getters race-lane; 0040b410 11b indexed getter; 0040e360 RaceMode::Set 9b setter CAUTION live race-phase global) — next round continues here.

2026-06-12 | round 7 | L2 | attempted 2 (00499730, 00495120) | GREEN 2 | deferred 9 this round (004c9eb0 needs-decomp-transcript; 00493900 needs buffer-seeding ext; 004926c0/00493480 QPC time-varying; 004a4bb7/004a774d/004a8a04/004aa3e4/004aa3fe/004ac04a CRT one-shot-init class) | exit-5/6: none | dry_counter 0. L2 effectively TRIAGED OUT: remaining un-attempted L2 rows are 00402750/00492370 (large multi-phase loaders), 004c2c90/004c2d90/004c2fb0 (RW plugin/driver dispatchers — live registry mutation), 004cc7f0/004cc820 (freelist allocators — nondeterministic ptrs), 00495270 (single-out-ptr — wishlist), 00498c00 (live D3D mode-table alloc), 00499ba0 (CoInitialize+CreateWindow), 00494f20 (callee C1), 00493540-dup n/a. NEXT ROUND: L3 (c3_filter_v4 sweep) or evaluate L5 — wishlist out-buffer-compare now unlocks 0041f030+0041da90+00484c70+00495270 = 4 confirmed (<10 bar). If L3 yields <3, consider relaxing the L5 bar or running an L4 evidence-repair round. (004c9f50, 004b6610, 004b6560 thunk) | GREEN 3 | deferred 2 (004c9f60 guard+live-vtable; 004b6540 allocating thunk target) | exit-5/6: none. Baseline build LNK1104 once: the USER's own mashed_re.exe instance (started 21:04) locked the exe output — waited out per the MASHED-running rail (exited 21:07), build then OK. RETROACTIVE root-cause for the round-1/round-3 transient exe-link flakes: same class (a running mashed_re.exe locks build\mashed_re.exe). multi_arg_global_write guard-inside-restored-window trick validated for guard-less multi-param setters (time_display_set_entry precedent). dry_counter 0. L2 remaining: ~24 rows (boot-band 00402750-depth cluster + render dispatchers + CRT-band odd-address rows — expect rising cost; several look abi-limited/oversize-adjacent).

2026-06-12 | round 5 RESUMED ~21:35 (user present; env recovered — boot probe GREEN on re-check) | L1+L2 round-3 carry-over | attempted 5 | GREEN 4 (0042fe70 via read_global switch after exit-5; 004cbc60/70/80 L2 re-earns) | deferred 1 (0041ea80: lap-line gate field genuinely 0 in arena mode — needs lap-race recipe or pointer-indirect seed) | exit-5 x2, both root-caused (genuinely-zero domain values, NOT wrong-scenario): 0042fe70 fixed by read_global seeding; 0041ea80 deferred | dry_counter 0. LESSON: probe-unlocked != discriminating — a populated SCENARIO can still read a legitimately-zero GLOBAL; prefer read_global/seeded handlers for plain getters wherever the address is fixed.

2026-06-12 | round 5 | (boot probe only) | attempted 0 (round-3 five still pending) | GREEN 0 — BLOCKED-ENV persists (~70 min; same clean exit -1 after RwEngineOpen; no user intervention yet) | deferred 0 | — | dry_counter 0. Backing off to 1 h retries. NOTE: a user reboot (the likely fix) kills this session and its /loop — if a fresh session picks this up, this ledger is the full state: run the round-3 five (vehicle_dword_67ea80_get + track_desc_field40_get race-lane; rw_global_7d4598_set/get + rw_global_7d459c_set menu-attach) after one boot probe, then classify the GREEN set.

2026-06-12 | round 4 | (env diagnosis only) | attempted 0 new (round-3 five still pending) | GREEN 0 — BLOCKED-ENV persists ≥40 min | deferred 0 | — | dry_counter 0 (unchanged). DIAGNOSIS MATRIX (all ~19:45-20:00): boot probe FAILS same way (exit -1 = clean exit(-1) after "Calling RwEngineOpen", NOT a crash); round-2-content .asi rebuilt+deployed → ALSO fails (round-3 .asi fully exonerated; build.bat restored after bisect); standalone mashed_re.exe from build\ → ALSO fails (exit 0x3; caveat: cwd-sensitivity of its asset paths unverified); monitors 3/3 Active=True (WMI power state); zero TDR/display/PnP events since 18:30; GPU RTX 5070 Ti PnP status OK; disk 718 GB free (round-3 "1 GB" reading was a display artifact — corrected); videocfg.bin still canonical; session console+unlocked+idle-none; TeamViewer running but NO established connection (phantom virtual monitor adapter present but inactive). CONCLUSION: D3D9 device-init broken system-wide for NEW processes since ~19:15, cause unidentified from inside the session — needs hands-on (reboot likely). Pushed a notification to the user. Next round: boot-probe FIRST (18s aliveness); if it boots, run the round-3 five immediately.

2026-06-12 | round 3 | L1+L2 | attempted 5 (authored: 0042fe70 + 0041ea80 race-lane, 004cbc60/70/80 L2 re-earns) | GREEN 0 — BLOCKED-ENV, not dry | deferred +8 (L1 triage: 0041f030, 0041da90, 00443d10, 004150e0, 00423480, 00486460, 0046b1c0, +0041d930 caveat-candidate) | run_diff died 2x in boot phase (frida InvalidOperationError "script has been destroyed", surfaces as exit 1 NOT 6) | dry_counter 0 (unchanged — environment outage, pool not drained). ROOT CAUSE BISECTED: MASHED stopped booting ~19:15 local — dies after "Calling RwEngineOpen" with zero hooks (MASHED_HOOK_HI=0) AND with the .asi renamed away → fully environmental, round-3 code exonerated. videocfg.bin = canonical (hash match); 3 displays enumerated (power state unknown); ~14 rapid device create/destroy cycles + 2 force-killed boots preceded the failure. Suspect monitors asleep or GPU driver wedge. ACTION: reschedule ~20 min; next round MUST first confirm one manual boot reaches the menu before burning diff attempts. KEY LEARNINGS: (1) int_with_out_ptr allocs 8B + compares return ONLY — read diff_template.js handler before trusting an arg_type name; (2) L2 demoted-needs-reimpl rows are real cheap wins (render trio authored in minutes); (3) gate-check Glob on **WIP** false-positives on dead worktree copies — check repo root only.

## Final gated-remainder report

(written by the round that ends the loop)

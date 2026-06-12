# Promotion loop ledger

State file for `/promote-round` (run under `/loop /promote-round`). This file
is the ONLY state that survives between rounds — every round reads it first
and appends to it last. Initialized 2026-06-12.

Goal (user, 2026-06-12): promote everything promotable. The loop ends after
two consecutive dry rounds, leaving the final gated-remainder report below.

## Counters

- rounds_run: 3
- total_green: 10
- dry_counter: 0  (round 3 was BLOCKED-ENV, not dry — pool not drained)
- last_round: 2026-06-12 round 3 (skipped: environment)

## UNFINISHED — round 3 carry-over (finish or defer FIRST next round)

Five reimpls authored + built + registered (commit c9998f07) but ZERO diffs
ran (game cannot boot — see round-3 log row). Next round: verify boot works
(spawn one manual MASHED, expect menu), then run these five sequentially:
- vehicle_dword_67ea80_get (race), track_desc_field40_get (race),
- rw_global_7d4598_set, rw_global_7d4598_get, rw_global_7d459c_set (menu-attach)
Then classify the GREEN set as usual. Registry entries exist but are NOT
evidence yet.

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

2026-06-12 | round 3 | L1+L2 | attempted 5 (authored: 0042fe70 + 0041ea80 race-lane, 004cbc60/70/80 L2 re-earns) | GREEN 0 — BLOCKED-ENV, not dry | deferred +8 (L1 triage: 0041f030, 0041da90, 00443d10, 004150e0, 00423480, 00486460, 0046b1c0, +0041d930 caveat-candidate) | run_diff died 2x in boot phase (frida InvalidOperationError "script has been destroyed", surfaces as exit 1 NOT 6) | dry_counter 0 (unchanged — environment outage, pool not drained). ROOT CAUSE BISECTED: MASHED stopped booting ~19:15 local — dies after "Calling RwEngineOpen" with zero hooks (MASHED_HOOK_HI=0) AND with the .asi renamed away → fully environmental, round-3 code exonerated. videocfg.bin = canonical (hash match); 3 displays enumerated (power state unknown); ~14 rapid device create/destroy cycles + 2 force-killed boots preceded the failure. Suspect monitors asleep or GPU driver wedge. ACTION: reschedule ~20 min; next round MUST first confirm one manual boot reaches the menu before burning diff attempts. KEY LEARNINGS: (1) int_with_out_ptr allocs 8B + compares return ONLY — read diff_template.js handler before trusting an arg_type name; (2) L2 demoted-needs-reimpl rows are real cheap wins (render trio authored in minutes); (3) gate-check Glob on **WIP** false-positives on dead worktree copies — check repo root only.

## Final gated-remainder report

(written by the round that ends the loop)

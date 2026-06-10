# SCRIBE_QUEUE fragment — batch_ak session 4 (ak_s4)

Author-only promote-c2 pass (ai low cluster). Bucket plates are the C2
deliverable; central finalize (ghidra-sweep) writes hooks.csv / trackers and
commits. This session did NOT touch hooks.csv / STUBS / UNCERTAINTIES / DEFERRED,
did NOT mint U-/S-IDs, did NOT mutate the master Ghidra project.

## Queued

2026-06-02  ak_s4  bucket=re/analysis/bucket_ai_00407a40_00415880  confidence=C1->C2  rvas=00407a40,00408af0,0040e4a0,00413fe0,00414030,00414300,00414490,00414570,004148b0,00414a70,00414c30,00414f00,00415020,004150e0,00415190,00415200,00415220,00415860,00415880  note=subsystem_observed=ai (all 19 CONFIRMED, ZERO library-band hits, no reclassifications); needs_function_create=none (all 19 are real Ghidra function objects, function_at+decomp_function non-null for every RVA)

## Notes for the sweep

- **Count**: 19 RVAs, 19 plates authored in the bucket dir. None drift-skipped
  (all 19 were C1 in hooks.csv at session start; hooks.csv shows them as
  subsystem `ai`). `00415860` is status `new` in hooks.csv (still C1) — plated.
- **Slot**: opened the pre-assigned `Mashed_pool3` read-only (session
  139bb5cb…, project_name=Mashed_pool3 confirmed via open identity). No leak,
  no fallback needed. `program_close` ran clean at end. Marker `.pool_slot_ak_s4`.
- **Subsystem confirmation**: all 19 CONFIRMED `ai` (CPU-racer behaviour:
  targeting modes, powerup-activation decisions, last-place/frustration timers,
  per-vehicle AI-state resets, track-grid surface query). They cluster as the
  AI control-step callee set: every non-getter is called by `FUN_00416250` /
  `FUN_00416a30` / `FUN_00417da0` (the three AI control-step variants) and/or
  `FUN_00418560`. Matches the batch-header "ai: ZERO library-band hits, clean
  application code." NO third-party-library reclass-OUTs in this bucket.

- **Three plate corrections vs the stale C1 notes** (reported per current decomp,
  binary is source of truth):
  1. `0x004150e0` — track-grid query returns 1 iff the fine sub-cell byte
     `== 0x02` (value 2). The prior C1 plate said "type 0 or 3"; the current
     decompilation tests `== '\x02'`. Corrected.
  2. `0x00415020` — frustration timer: the `_DAT_005cc320 <= leaderProgress`
     branch returns 0 WITHOUT resetting the timer; only the fall-through `else`
     resets to 0. The prior plate claimed both branches reset. Corrected.
  3. `0x00414300` — `_DAT_005cc320` is **+1.0f**, not the "-1.0" the old d3 plate
     claimed. Verified by memory_read at 0x005cc320 → `00 00 80 3f`. With +1.0 the
     perpendicular slope `-(1.0/m)` and the `1.0/sqrt(dist²)` normalize are
     geometrically correct.

- **Constants verified by memory_read this session** (cite in re-classify if
  promoting data-semantics): `0x005cc320 = 0x3f800000 = +1.0f`;
  `0x005d757c = 0x00000000 = 0.0f` (the pervasive "last-place / zero" sentinel).
  Other `_DAT_005c*` / `_DAT_005d*` thresholds were cited by address only (not
  read); their float values remain data-semantic [UNCERTAIN], non-blocking for C2.

- **DEFERRED touchpoint to clear** (do NOT resolve here — flag for re-classify):
  `0x00414300`'s sqrt callee `FUN_004c3b30` is the project's mapped `FastSqrt`
  (C3). The old DEFERRED **D-5560** describes it as an "unmapped sqrt callee";
  it is now mapped, so D-5560 is resolvable at finalize.

- **Pre-existing tracker IDs carried** (referenced, NOT re-minted): U-0410
  (00407a40 field role), U-0411/U-0412 (00413fe0 zeroed fields / 008032d4 table),
  U-1427 + D-1127 (004150e0 void-signature / coord args), U-1429 + D-1128
  (00415220 5-arg signature read from stack), U-1431 (004148b0 005f2dd8 limit
  table), U-1889 (00415880 FUN_0046cc10 state-code 4).

- **New holes**: marked as bare `[UNCERTAIN]` inline in each plate's
  `## Uncertainties` section (data-semantic float values; the unused
  `FUN_0046d6d0`/`FUN_0046d570` scratch calls in `0x00414f00`; the `5 - rank`
  lap-wrap basis shared by 00414570/00414a70/00415880). Central re-classify mints
  IDs from the reserved range U-7500..U-7699 / S-6200..S-6399 as needed.

- **No new S-IDs**, no function_create, no master writes. Files left as the
  bucket dir + this fragment + `.pool_slot_ak_s4` for the single end-of-session
  commit.

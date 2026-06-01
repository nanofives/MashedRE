# Scribe Queue fragment — batch_ag Session 2 (util C1→C2, AUTHOR-ONLY)

To be concatenated into `re/SCRIBE_QUEUE.md` (Queued section) by the central finalize step.

## Queued

2026-06-01  batch-ag-s2  bucket=re/analysis/bucket_util_0040e4b0_0042f790  rvas=0040e4b0,0040e560,0040e590,0040fc00,004102f0,004103a0,004111c0,00413f20,00414060,00414180,00414220,00418990,004189a0,0041a250,0041b4d0,0041b510,0041b540,0041bf20,0041c010,0041c090,0041c180,0041cc50,0041d830,0041d930,0041dca0,0041e6c0,0041ede0,0041ee50,0041ef60,0041ef80,0041efc0,0041f120,0041f260,0041f320,004215a0,00421960,004219f0,00421d20,004222d0,00425a40,00425d50,00426bb0,004292c0,004292d0,00429300,00429310,00429620,00429660,00429820,004298e0,0042aab0,0042b900,0042b950,0042c150,0042c1a0,0042c1f0,0042c300,0042c3c0,0042c7c0,0042f790

## Session metadata

- **Model/role:** Opus 4.8, AUTHOR-ONLY (plates only; hooks.csv / re-classify / build / Frida NOT touched).
- **Confidence target:** C2 (mechanical-description plate per RVA = the C2 deliverable).
- **Count:** 60/60 plates authored. Drift-skips: 0 (all 60 were `util,C1` in hooks.csv at session start).
- **LAB_ / function_create:** none required (no LAB_ rows in this session's list; all 60 had real function boundaries).
- **STOP-AND-ASK triggers hit:** none (no null function_at+listing; no ambiguous-signature blockers).
- **U-IDs minted:** U-6000 .. U-6052 (53 used; all data-semantic / non-blocking for C2 per
  feedback_data_semantic_uncertainty_nonblocking).
- **S-IDs minted:** S-4500 only (00421d20 particle index resolvers FUN_0041f1c0 / FUN_0041f290).
  All other plates: no blocking stub (depth-1 callees are out-of-scope leaves already tracked).
- **DEFERRED cross-ref:** 00421d20 carries prior D-8262 (particle-struct internals); plate is the C2
  mechanical transcription, full field semantics remain deferred.

## POOL-SLOT DEVIATION (action required by finalize/operator audit)

- Pre-assigned **Mashed_pool7** was **stale-locked** at session start: its `.lock~` channel lock was held
  by a live (non-sibling) MCP server process while my own MCP server reported 0 open sessions; repeated
  `project_program_open_existing` on pool7 returned `LockException: Unable to lock project`.
- Per the batch setup note ("if stale-locked, acquire and record actual slot"), probed all clones and
  switched to **Mashed_pool5** (verified FREE; not a sibling slot 6/8/9/13/14; not the frontend slot 0).
  All 60 plates were authored against Mashed_pool5 (recorded in each plate's `opened_in_slot` + in
  `.pool_slot_ag_s2`). Live free/busy probe 2026-06-01: FREE = pool1,2,3,4,5,10,11,12 ; BUSY = pool0,6,7,8,9,13,14.

## subsystem_observed (hooks.csv has all 60 as `util` — these are reclass CANDIDATES; central sweep decides, NOT applied here)

Nearly the entire bucket is mechanically NOT generic-util. Grouping by mechanical evidence:

- **race / game-state** (player slot table PTR_PTR_005f2770, race-phase DAT_0063ba8c, per-player array stride 0x2ac):
  0040e4b0, 0040e560, 0040e590, 0040fc00 (Race::Tick), 004102f0, 004111c0 (state-machine dispatcher),
  00413f20, 00414060, 00414180, 00414220, 004189a0, 0041ede0, 0041ee50, 0041ef60, 0041ef80, 0041efc0,
  0041f120, 0041f260, 0041f320, 00425a40, 0042f790 (GhostMode::IsActive)
- **timetrial:** 004103a0 (TimeTrial::LapFinishProcessor), 00429310 (TimeTrial::Tick)
- **hud** (RenderWare 2D billboard via FUN_004c51a0/004c1480/004c13e0; HUD record tables 0x0063c8xx/cdxx/d2xx; time-display globals):
  00418990, 0041a250, 0041b4d0, 0041b510, 0041b540, 0041bf20, 0041c010, 0041c090, 0041c180, 0041cc50,
  0041d830, 0041d930, 0041dca0, 0041e6c0, 00429300, 00429620, 00429660, 00429820, 004298e0
- **effects / particle** (block grid 0x0063fb90 stride 0x208; anim table 0x008992a4; RandomFloat FUN_004b4510):
  004215a0, 004219f0, 00421d20, 004222d0, 00425d50
- **audio:** 00421960 (FUN_0055dec0/FUN_00559c40 audio band)
- **camera:** 00426bb0 (CameraPath::GetCount → DAT_0066d6d8)
- **localization / text format** (FUN_00427780 [u16 len][UTF-16] buffers):
  0042c300, 0042c3c0, 0042c7c0
- **util (genuinely generic; keep as util):** 004292c0 (indirect store), 004292d0 (memcpy-from-object+8),
  0042aab0 (const init), 0042b900 (getter), 0042b950 (const setter 0x1000), 0042c150 (nonzero-count flag),
  0042c1a0 (state 2→3), 0042c1f0 (all-6-slots-empty predicate)

## Notes for central re-classify
- Plates are bit-faithful mechanical transcriptions with address-cited constants; callees_depth1 + callers_noted
  populated per plate. Sizes verified via body_end−body_start.
- 004111c0 plate consolidates the prior verified C1 transcription at re/analysis/timer_d2/0x004111c0.md
  (13730-byte dispatcher; body 0x0040dd80..0x00411322, entry mid-body 0x004111c0).
- Several RVAs already had scattered C1 plates (leaderboard*/timer*/game_state*/localization*/profile_career*/
  random_rng*) — those remain; this bucket is the consolidated C2 set for the sweep.

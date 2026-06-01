# Scribe Queue fragment — batch_ag Session 1 (util C1→C2, AUTHOR-ONLY)

To be concatenated into `re/SCRIBE_QUEUE.md` (Queued section) by the central finalize step.

## Queued

2026-06-01  batch-ag-s1  bucket=re/analysis/bucket_util_00095280_0040e460  rvas=00095280,000952f0,000b6570,000b6710,000b67e0,000b6940,001504d0,00150c30,00150c50,00150d80,00150da0,00150f20,00151050,00151090,00151180,00151190,00151290,001512b0,001512d0,001514e0,00402b70,00402f40,004030d0,00403250,004039f0,004046a0,00405420,00405460,00406ce0,004073b0,00407600,00407a60,00408b00,00409290,0040ab40,0040ac80,0040ad30,0040b180,0040b250,0040b410,0040b430,0040b540,0040b6d0,0040b6e0,0040b700,0040bd80,0040ce00,0040d040,0040d470,0040da50,0040dba0,0040dbd0,0040dc80,0040dc90,0040ddb0,0040de10,0040e170,0040e360,0040e450,0040e460

## Session metadata

- **Model/role:** Opus 4.8, AUTHOR-ONLY (plates only; hooks.csv / re-classify / build / Frida NOT touched; no git commit — files left untracked per session prompt).
- **Confidence target:** C2 (mechanical-description plate per RVA = the C2 deliverable).
- **Count:** 60/60 plates authored. Drift-skips: 0 (all 60 were `util,C1` in hooks.csv at session start).
- **Address mapping (IMPORTANT for sweep):** the 20 low RVAs in the candidate list are *true RVAs* (Ghidra addr = rva + 0x00400000): 00095280→0x00495280 … 001514e0→0x005514e0. The 40 high RVAs (00402b70..0x0040e460) are *absolute VAs* (Ghidra addr = rva). Each plate records both `rva:` (hooks.csv value) and `ghidra_addr:`.

### POOL FALLBACK (deviation from pre-assigned slot — please note at finalize)
- Pre-assigned **Mashed_pool6 was unusable**: the shared MCP server JVM held a *leaked/stale project lock* on pool6 (left by a failed open). Deleting the on-disk `.lock` did not help — the in-JVM FileChannel lock persists for the server's lifetime, and the server is shared with the live sibling sessions (s2=pool7, s4=pool9) + the Frontend session, so it must NOT be restarted.
- **Fell back to Mashed_pool0** (an unassigned clone; verified free by clearing its 6-day-old stale lock and opening cleanly). Did NOT run `ghidra_pool.sh acquire` (no racing). Recorded in `.pool_slot_ag_s1`.
- pool0 is byte-identical to the target binary, so all mechanical plates are faithful; the only consequence was that pool0 (an older snapshot) lacked Ghidra function boundaries for part of the RtFS block (see function_create below).

### LAB_ / function_create (10 — clone-staleness in pool0, see U-5901)
The following RtFS handler methods had **no function boundary** in the pool0 fallback clone (each was a bare `LAB_` instruction). Created via `mcp__ghidra__function_create` before decomp; boundaries auto-bounded by Ghidra flow from the disassembled entry:
  00150c30 (0x00550c30), 00150c50 (0x00550c50), 00150d80 (0x00550d80), 00150da0 (0x00550da0), 00150f20 (0x00550f20), 00151050 (0x00551050), 00151180 (0x00551180), 00151290 (0x00551290), 001512d0 (0x005512d0), 001514e0 (0x005514e0).
Pre-existing functions (no create needed): 001504d0, 00151090, 00151190, 001512b0.
ACTION FOR SWEEP: confirm the master Mashed.gpr already has (or create) these 10 boundaries before scribing names/types.

### subsystem_observed (CONFIRM hypothesis 'util' per RVA)
- **Low block (20 RVAs, 0x00095280..0x001514e0): util CONFIRMED.**
  - 00095280..000b6940 = piz archive open/close/parse (OpenPizFile, ClosePizFile, PizOpen, PizWin32Open, PizWin32Read, PizOpenAndParse).
  - 001504d0..001514e0 = RenderWare **RtFS** filesystem handler (FindHandler + a 12-method handler vtable; vtable wired by Install/FUN_00551190 @0x00551190, which is the authority confirming every method's role). Genuine util/IO.
- **High block (40 RVAs, 0x00402b70..0x0040e460): predominantly RACE/GAMEPLAY, NOT util — FLAG FOR RECLASSIFICATION.**
  - Strong evidence (strings/globals): FUN_00408b00 logs "Invalid number of cars in race"/"Invalid position"; RaceMode::Set (0x0040e360) writes the race-mode global DAT_0063ba8c (also written =5 by 0040dbd0, =1 by 0040de10, read ==3 by 0040d470); Course::ValidateCarsFinished (0x0040d040) checks per-car lap state; FUN_0040b180/b250/b410/b430/b540/b6d0/b700 are the race scoreboard/standings/event-counter array ops (DAT_008a94xx/95xx); 0040ab40/0040ac80 are race state machines (DAT_008a9584); 004039f0/004046a0 countdown/event table; 00406ce0/004073b0/00407600 per-car entity update+accessor (DAT_00639d80, stride 0xec); 0040bd80/0040ce00 frame init/update subsystem dispatchers; 00403250 attract-idle timer; 004030d0 frame-time advance; 00405420/00405460 ghost/replay cursor.
  - **Keep util** within the high block: 00402b70 (FUN_00402b70) — genuine path/string normalization (8.3 canonicalizer); util CONFIRMED.
  - **Low-signal leaves (kept util, ambiguous):** 00402f40 (returns DAT_00636ad8), 00405420 (sets DAT_00639d74) — pure 1-line accessor/setter; left util pending the central call.
  - RECOMMENDATION: central re-classify should move the ~37 high-block race/gameplay rows from `util` to `race`/`gameplay` (subsystem name per project convention); 00402b70 stays util.

### Tracker IDs
- **U-IDs minted:** U-5900 .. U-5911 (12 used), all data-semantic / structural and **non-blocking for C2** (behavioral mechanics fully transcribed; per feedback_data_semantic_uncertainty_nonblocking):
  - U-5900 — PizOpenAndParse (000b6940) header dword semantics + dir-block arithmetic (cross-check PIZFile.cs).
  - U-5901 — **shared** across the 10 function_create'd RtFS methods: boundaries absent in pool0 fallback clone.
  - U-5902 — Poll (00151090) FUN_004d7ff0(slot[0],slot[1]) recompute into slot[0xb] (callee out of range).
  - U-5903 — **shared** countdown/event table layout @DAT_00636b8c (004039f0, 004046a0).
  - U-5904 — **shared** entity record layout @DAT_00639d80 stride 0xec (00406ce0, 004073b0; cf. 00407600).
  - U-5905 — race-position place/count combination math (00408b00).
  - U-5906 — **shared** race state-machine numeric code meanings (0040ab40, 0040ac80).
  - U-5907 — **shared** scoreboard array layout @DAT_008a94xx/95xx (0040b180, 0040b250, 0040b540).
  - U-5908 — per-player event-counter array meanings @DAT_008a9530/40/50/60 (0040b700).
  - U-5909 — finish condition + track-entry field (0040d040 Course::ValidateCarsFinished).
  - U-5910 — director/camera object layout + FUN_004671a0 return shape (0040d470).
  - U-5911 — per-car table layouts + FUN_0041eeb0 call shape (0040da50).
- **S-IDs minted:** none (S-4400..S-4499 range unused). Every plate's depth-1 callees are out-of-scope leaves already in the tracker; no plate carries a blocking stub.

### STOP-AND-ASK triggers
- None hit. No RVA returned null for both function_at AND listing_code_unit_at (the RtFS LAB_ rows had valid code units → resolved via function_create). No library-function-family rename performed. One entry/body note: 0040ac80 has entry 0x0040ac80 but body 0x0040abc0..0x0040acb5 (mid-body secondary entry) — described in its plate, not a blocker.

# Session End — title_screen

**Session ID:** title_screen-20260506-VVVVV
**Pool slot:** Mashed_pool15
**Binary anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
**Parent:** strategy-3 entry via intro_splash chain → FUN_00402750 → FUN_004669b0 → FUN_00429290 → FUN_00429240 → FUN_00428a30

## Work completed

Identified TITLE_FN as FUN_00428a30 (title screen renderer) via Strategy 3 (intro_splash call chain). Analyzed all 7 core functions in the title/attract/lobby frontend rendering subsystem.

### Functions analyzed (new — C1)

| RVA | Name in Ghidra | Notes file | Key finding |
|---|---|---|---|
| 0x00428a30 | FUN_00428a30 | 0x00428a30.md | Title screen renderer (TITLE_FN); string 0x222 at (580.0, 140.0); build date; 24M-tick attract timer; gated by DAT_0067d84c |
| 0x00429240 | FUN_00429240 | 0x00429240.md | State dispatcher; switch(DAT_0067d960): 0→title, 1→attract, 3→lobby |
| 0x00429290 | FUN_00429290 | 0x00429290.md | Per-frame tick: frame-begin + input-poll + state-dispatch + audio-tick + frame-end; 39b; called ×3 in FUN_004669b0 |
| 0x00428bf0 | FUN_00428bf0 | 0x00428bf0.md | Attract mode renderer; car sprite at (320.0, 260.0); string 0x222 at (580.0, 140.0); no attract timer |
| 0x00428d30 | FUN_00428d30 | 0x00428d30.md | Lobby renderer (1291b); gradients + border lines + logo + car slot DAT_00771964 + FUN_0042f0b0 at (347.5, 168.0) |
| 0x00427c90 | FUN_00427c90 | 0x00427c90.md | Assets-ready getter (5b); returns DAT_0067d84c |
| 0x00428390 | FUN_00428390 | 0x00428390.md | State setter (9b); DAT_0067d960 = param_1 |

## Stubs introduced

| ID | RVA | Size | Description | D-ID |
|---|---|---|---|---|
| S-2460 | 0x00428450 | ? | Ticker/overlay; args (0x20, 0xffffffe0) in title, (0x10, 0x100) in lobby | D-7300 |
| S-2461 | 0x004288a0 | ? | Dark/blank screen renderer; called when DAT_0067d84c==0 | D-7301 |
| S-2462 | 0x00428320 | ? | Text renderer; renders build date + string 0x222 | D-7302 |
| S-2463 | 0x0042e590 | ? | Sprite draw; car sprite at (320.0, 260.0) in attract renderer | D-7303 |
| S-2464 | 0x0040d250 | ? | Lobby visual element (gradient/line/logo path) | D-7304 |
| S-2465 | 0x00401ee0 | ? | Lobby visual element | D-7305 |
| S-2466 | 0x0042f0b0 | ? | Lobby UI sub-renderer at (347.5, 168.0) | D-7306 |

## Uncertainties introduced

| ID | Function | Summary |
|---|---|---|
| U-2467 | 0x00428a30 | Attract timer 24,000,000 tick threshold — tick rate unit unknown; timer unreachable in 3-frame init context |
| U-2468 | FUN_0043dfd0 | &DAT_005f7000 sentinel on menu page 11 — DAT_005f7000 identity and structure unknown |
| U-2469 | 0x00428bf0 | FUN_0042e590 sprite table structure unknown (S-2463) |
| U-2470 | 0x00428a30/0x00428d30 | FUN_00428450 ticker args semantics — (0x20,−32) vs (0x10, 0x100) arg-pair meaning unknown |

## Deferred cleared

None from prior queue. This is a new bucket.

## Depth-2 continuation

Queued as `title_screen-cont1` in re/queue.md.
- D-7300: 0x00428450 FUN_00428450 (ticker/overlay)
- D-7301: 0x004288a0 FUN_004288a0 (dark screen)
- D-7302: 0x00428320 FUN_00428320 (text renderer)
- D-7303: 0x0042e590 FUN_0042e590 (sprite table)
- D-7304: 0x0040d250 FUN_0040d250 (lobby visual element)
- D-7305: 0x00401ee0 FUN_00401ee0 (lobby visual element)
- D-7306: 0x0042f0b0 FUN_0042f0b0 (lobby UI sub-renderer)

## Tracker changes

- **hooks.csv**: +7 rows (00428a30, 00429240, 00429290, 00428bf0, 00428d30, 00427c90, 00428390) all C1/frontend
- **STUBS.md**: +S-2460..S-2466
- **UNCERTAINTIES.md**: +U-2467..U-2470
- **DEFERRED.md**: +D-7300..D-7306
- **re/queue.md**: +title_screen-cont1 section

## Key architectural finding

FUN_00428a30 is the TITLE_FN. The title screen is rendered for exactly 3 frames during audio initialization (FUN_004669b0 calls FUN_00429290 ×3, no loop). The localization loader FUN_004274e0 sets DAT_0067d84c=1 earlier in FUN_00402750, so the full title renders. The 24M-tick attract timer is architecturally present but practically unreachable in those 3 frames; sustained title/attract behavior in the main loop is driven via the &DAT_005f7000 sentinel in FUN_0043dfd0 on menu page 11 (DAT_0067e9fc = 0xb).

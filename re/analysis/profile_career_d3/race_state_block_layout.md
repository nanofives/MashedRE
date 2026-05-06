---
type: data-layout
range: 0x0063b900..0x0063bb00
size_bytes: 512
session_date: 2026-05-06
opened_in_slot: Mashed_pool3
---

# Race State Block — 0x0063b900..0x0063bb00

Runtime data block. Reads as all-zero in static Ghidra view (BSS segment, runtime-initialized).
Known offsets derived from functions that reference this range this session and prior sessions.

## Known offsets

| Offset | Absolute VA | Type | Value / Description | Source RVA |
|--------|-------------|------|---------------------|------------|
| +0x00  | 0x0063b900  | ?    | [UNCERTAIN U-2198] — not referenced this session | — |
| +0x0C  | 0x0063b90c  | int  | Race-concluded flag. `1` = race is done; `0` = in progress. Set by Race::EvaluateResult (0x00410510). Read by Race::GuardConcludedAndP1Won (0x0040dd60). | 0x00410510, 0x0040dd60 |
| +0xEC  | 0x0063b8ec  | int  | Zeroed by FUN_0040b810 (util, 21-global scatter-zero). Part of a cluster also reset at init/round-start. | 0x0040b810 |
| +0x18C | 0x0063ba8c  | int  | Race phase/state byte-sized field. Written `1` by FUN_0040de10 (util) at lap-start path. Written `0xb` by Race::EvaluateResult on race conclusion. | 0x00410510, 0x0040de10 |

## Layout uncertainty

- [UNCERTAIN U-2198] The full layout of this 512-byte block is not known. Only +0x0C and +0x18C have confirmed semantics. The block likely contains per-race state (player positions, lap counts, timer, etc.) based on context but content is not read in detail this session.
- Note: 0x0063b8ec is at offset −0x14 from the block base (0x0063b900). It is included here because prior session (timer_d2_cont1) groups it with the race-state region in FUN_0040b810's zero-init sweep.

## Cross-references observed this session
- FUN_00441990 reads `_DAT_007f1a58` and `_DAT_007f1a5c` (not in this block — in 0x007f1xxx range).
- FUN_00446520 reads `DAT_007f0fd0` (game mode), `DAT_007f0f38`, `param_1[0x265..0x269]` (player-pair/zoom state stored in the camera struct, not in this block).
- This block is the race-conclusion state; camera-follow state lives in a separate camera struct.

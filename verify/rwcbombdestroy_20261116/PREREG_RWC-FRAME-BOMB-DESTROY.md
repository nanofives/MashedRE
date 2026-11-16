# PREREG RWC-FRAME-BOMB-DESTROY (run 52, 2026-11-16)

Committed before any arm of this row runs.

## Static trace (MASHED.exe.unpatched, absolute-dword scan with verify/rwcframeclump_20261113/memrefs.py)

References to DAT_00636c00 in .text:

| site | access | function |
|---|---|---|
| 0x004039d1 `mov [0x636c00], eax` | W | 0x004039c0 (stores the return of `0x0042a5d0(0x005cc718, 0, 0)` at 0x004039c9) |
| 0x00404659, 0x00404681, 0x0040468f | R | 0x00404650 |
| 0x00404820 `mov eax, [0x636c00]` | R | 0x00404820 |
| 0x00404601 | no memory-form decode (not a reference) | inside 0x00404320 |

0x00404820: `mov eax,[0x00636c00]` / `push eax` (0x00404825) / `call 0x004e6e00` (0x00404826, RpClumpDestroy) / `pop ecx` /
`ret`. Its call site: `call 0x404820` at 0x0045b945 in 0x0045b930 (after 0x00412010, 0x0045b350, 0x00458880, 0x004593b0,
before the +0x18 slot loop at 0x0045b94a). re/match/s01/raw/00404820.c and re/match/s09/0045b930.c are C4 by match.

Exe: `Match/PortTop_0045b930.cpp` bound `FUN_00404820` to `PortTop_StandIn04C_00404820` (logs once, returns 0), so
RpClumpDestroy is never called on DAT_00636c00.

## Knobs (both default OFF, top-down path only)

- `MASHED_RWC_BOMB_DESTROY=1`: the 0x0045b945 call site runs the 0x00404820 body through the exe's RpClumpDestroy binding
  (`MashedRw_ClumpDestroy_004e6e00`, core route under MASHED_RWC_FRAME). Off: the stand-in, as before.
- `MASHED_RWC_SLOTS18_OFF=1`: control only. 0x0045bae0's call site skips the run-46 +0x18 teardown-slot store on the frame
  route.

## Arms (vs = VS2022 build, ddk = MASHED_DDKOBJ=1 build; frameT = run_arms.sh frameT env, MASHED_RWC_FRAME_N=100000,
MASHED_OT_TIMER=1)

| arm | env added | predicted `live clumps at RwEngineStop` | predicted end |
|---|---|---|---|
| vs_frameT / ddk_frameT | none | 1 (Bomb.dff, as run 46/49) | rc 0 |
| vs_frameT_bomb / ddk_frameT_bomb | BOMB_DESTROY=1 | 0 | rc 0 |
| vs_frameT_s18off | SLOTS18_OFF=1 | no prediction (the measurement) | no prediction |
| vs_frameT_s18off_bomb | both | no prediction | no prediction |
| vs_frame / ddk_frame | none, BOMB_DESTROY=1 | self-test PASS | 0x5257F001 |

GREEN for deliverable (1): both `_bomb` arms report 0 live clumps, log the RWC-FRAME-BOMB-DESTROY line with the clump
pointer equal to the 0x0045bae0 `DAT_00636c00 (Bomb.dff)` line, and end rc 0. RED otherwise, reported with the census.
Deliverable (2) reports the s18off count against the baseline: (103 - s18off) / 102 of the drop is attributed to the merged
IMG-5 bodies and the rest to the store, with the caveat that the run-45 103 was measured on an older tree.

No gate (G-CTRL, B, 12 captures): top-down row, both knobs are off on the default path.

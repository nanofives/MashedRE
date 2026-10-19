# PREREG u9152: the +0x928 / +0x968 double buffer (2026-10-18)

Branch `work/u9152` from `race/first-frame-parity` `a5a77d17c` (default G-CTRL `b7534619 / fb690fb1 / d3a20f4c`, default B
dumps `c7b40cf2 / 5b9905d8 / f680a529`). Model claude-opus-5-5, account2. Exe `b5efa344` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9756_root` (reused, its lane is closed). Written before any knob arm.

## 0. Decoded before this PREREG (disassembly of the anchored binary, `re/tools/disasm_va.py`)

- `FUN_004709a0` `0x00470a6f..0x00470a8f`: `ECX = rec + [+0x9a8]*0x40 + 0x928` (stored `[esp+0x20]`, passed as EDI = src),
  `EAX = rec + [+0x9ac]*0x40 + 0x928` (`[esp+0x18]`, EBX = dst). `0x00470ac1 / 0x00470ac5 / 0x00470ad3`: A9 `FUN_0046e9e0` is
  called with EBX = dst, EDI = src. A9 writes dst = src + step (rows and translation, `0x0046ea6c` etc.), then
  `FUN_004c4680(dst, dst)` (`push ebx` x2 at `0x0046ef11 / 0x0046ef12`, call `0x0046ef5e`).
- `FUN_0046f6c0` `0x0046f6d8`: the solver's matrix is `rec + [+0x9ac]*0x40 + 0x928`, i.e. dst. It reads no other half.
- `0x00470c31..0x00470c41`: after a substep that settles, `+0x9ac = +0x9a8`, `+0x9a8 = (+0x9a8 - 1) & 1`.
  `0x00470ab0 / 0x00470ab3`: retry count >= 2 jumps to `0x00470c47`, past the swap (also `0x00470aa4`, `+0x9f0 == 0xff`).
- `FUN_00470c70` `0x00471195..0x004711af`: frame end, if `+0x9a8 != 0`: `+0x9ac = +0x9a8`, `+0x9a8 = (+0x9a8 - 1) & 1`, and
  `rep movsd` 0x10 dwords from `+0x928` (ESI = `ebx - 0x80`) to `+0x968` (EDI = `ebx - 0x40`).
- So with 2 substeps per frame, at frame end `+0x9a8 = 0` holds the newest pose and `+0x968` the pose after substep 1
  (what u9756 measured: `+0x99c` 0.468632549 against `+0x95c` 0.468502462 at window call 0). During a substep the `+0x9a8`
  half is the pre-substep pose. Readers inside a substep: the car-car proximity test (`0x00470b44..`, car i's `+0x9a8`
  half translation, car j's `+0x9a8` half), and `FUN_00469df0` (`CarCarContacts.cpp:57-58` read both `+0x9a8` halves,
  `:216-218` `Rw_MatrixDerive` both `+0x9ac` halves; for a car j not yet stepped in this substep, its `+0x9ac` half is its
  previous-substep pose).
- The port: one pose (`g_bodyBasis` + `io.pos`), published by `SyncContactRingMatrix` into both halves after the wheel
  solver; selectors fixed at 0 / 1. The solver's own dst read is already the new pose (seated in `SolveWheelContacts`).

## 1. Hypothesis

H1: the `+0x968` half differs only because the port does not keep the double buffer; with it, the half equals the
original's at window call 0. H2 (prediction, stated before the run): car 1's pre-contact drift does NOT change, because the
solver never reads the stale half; the only in-substep readers are the car-car test and `FUN_00469df0`. If the drift moves,
the car-car path is the reader to look at (the car-car log says whether it ran).

## 2. Knobs and arms

- `MASHED_RING_DBUF=1` (default OFF, `Vehicle/VehiclePhysicsRun.cpp`): publish the dst half only, swap the selectors after a
  settled substep (`0x00470c31..0x00470c41`), frame-end fixup (`0x00471195..0x004711af`).
- `MASHED_RING_DBUF_DISCARD=1` (needs DBUF): a substep whose retries ran out keeps its src pose (the skipped swap).
Arms: `B_DEF_K0/K1/K26`, `H_DEF` (G-NOOP); `B_DB_K0/K1/K26`, `B_DBX_K0/K1/K26`; `B_SPLSBKNF60DB_K1`,
`B_SPLSBKNF60DBR1_K1` (car-1 record), `B_SPLSBKNF60DBL_K1` / `B_SPLSBKNF60L_K1` (car-car logs); `H_DB`, `H_DBX`.
Known deviation left in place: the port steps each car's whole frame before the next car, so for j > i the car-car test
sees car j's frame-start pose on both substeps (the original: its post-substep-1 pose on substep 2).

## 3. Gates

| gate | rule |
|---|---|
| G-NOOP | knobs off: `B_DEF_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529`, `H_DEF` `b7534619 / fb690fb1 / d3a20f4c` |
| G-HALF | `SPLSBKNF60DBR1` car 1 `+0x968..+0x9a4` at window call 0 = o_wobj2 (`+0x99c` 0.468632549), `recdiff1.py` |
| G-DRIFT | `SPLSBKNF60DB` K1 car 1 x/z gap at call 21 (now 1.2e-4): report; predicted unchanged (H2) |
| G-B | (b) on `DB` and `DBX`: no car that passes on the default turns to FAIL; B contact schedule (`ovon.py`) reported |
| G-D2 / G-13 | only for a default proposal: D2 3/3, 13 tracks 0 RED, G-CTRL reported |

## Amendment 1 (2026-10-18, after the B arms, before these)

Measured: G-NOOP B PASS (`c7b40cf2 / 5b9905d8 / f680a529`). G-HALF PASS on float content (`+0x968..+0x9a0` = o_wobj2 at
call 0; only the pad dwords `+0x954 / +0x964 / +0x984 / +0x9a4` differ). G-DRIFT: identical to `SPLSBKNF60` through call 27
(H2 holds). G-B FAIL: `B_DB_K1` car 1 B PASS -> FAIL. `DBX` = `DB` on K0 / K1 / K26 (the discard never fires). No default
proposal, so no D2 / 13-track arm. The car-1 record's remaining call-0 differences are `+0x9cc` (0.999988973 / 1) and
`+0x9d4` (-8.74227766e-08 / -4.37113883e-08); `+0x9c8..+0x9d0` is the tri-normal `MASHED_UP_TRINORMAL` ports (lk21).
Extra arms, report only: `B_SPLSBKNF60UTR1_K1` and `B_SPLSBKNF60DBUTR1_K1` (+ `MASHED_UP_TRINORMAL`, car-1 record): does
`+0x9cc` at call 0 equal o_wobj2, and does the call-21 gap move. `H_DEF` (G-NOOP) and `H_DB` (reported).

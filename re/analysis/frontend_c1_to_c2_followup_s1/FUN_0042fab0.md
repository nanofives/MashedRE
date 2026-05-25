# FUN_0042fab0 — C1→C2 plate

## Signature
`void FUN_0042fab0(undefined4 param_1)`

## Callers
- 0x004335f0 `FUN_004335f0`
- 0x00434720 `FUN_00434720`
- 0x004368e0 `FUN_004368e0`
- 0x0043a610 `FUN_0043a610`
- 0x0043aa30 `FUN_0043aa30`
- 0x00490020 `FUN_00490020`

## Callees
- 0x0040bb90 `FUN_0040bb90` — called unconditionally for every case

## Body summary
10-case switch dispatcher on param_1 (cases 0..9). Every case calls FUN_0040bb90() with no arguments and returns. Default case returns without action. No state is read or written beyond the dispatch itself.

## Cited constants/offsets
- Cases 0..9: dispatch range is 0x0..0x9 (cited at 0x0042fab0 switch table)
- No globals accessed.

## Uncertainties
- [U-4230] `structural` — FUN_0040bb90 takes no arguments here but the cases 0..9 all call it identically. Without decomp of FUN_0040bb90, it is unknown whether it reads param_1 indirectly (via a global) or whether the switch has been mis-decompiled and each case should pass a different argument. Resolve by decomp FUN_0040bb90. Required for C3.

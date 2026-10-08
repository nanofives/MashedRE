# RESULT — `GF1-CALLWISE` RAN: **0 vs 64**. And the cause is one hardcoded constant.

Date 2026-10-08. Follows `SCOPE_CALLWISE.md` §2, chosen by **USER DECISION (Mariano,
2026-10-08)**. **RAN.** No C-level. `original/` untouched, nothing default-ON.

Raw: `CW_port.step.csv` (port, `NO_ELIM` + wire ON), `verify/d3_elim_20261003/o_t3.msd.aistep.csv`
(original, committed).

## 0. Verdict first

> **In each side's own 220-call window: ORIGINAL 64, PORT 0.**
>
> **But the windows cover different phases of the race**, and the reason is a single line:
> `aib_game_sub_mode()` returns a **hardcoded 6** (`TrackRenderer.cpp:100`). The port has no race
> sub-state machine, therefore no start countdown, therefore its AI accelerates from call 0 while
> the original holds for 683 calls — so `i0` is **0** on the port and **683** on the original.

| car | ORIGINAL `i0` | window fires | PORT `i0` | window fires | port, whole run |
|---|---:|---:|---:|---:|---:|
| v1 | 683 | **31** | 0 | **0** | 0 |
| v2 | 683 | **4** | 0 | **0** | **18** |
| v3 | 683 | **29** | 0 | **0** | 0 |
| **total** | | **64** | | **0** | **18** |

## 1. The instrument has full coverage — the zero is real

`b2_ret` over all port rows: **42,636 zeros, 18 ones, and no `-1`s**. `-1` encodes "the wired block
did not evaluate"; its complete absence means the mode-6 gate held on **every** logged call. So
"0 in window" is a measured zero, not a non-evaluation (memory
`absent-log-proves-nothing-run-a-control`).

The port's 18 firings are real but sit at calls **≈643-660**, outside its own window (0-220) and
just *before* the original's window opens at 683.

## 2. Why the anchors differ — measured, and it is the root

`i0` = first call with `c4 != 0`. Over each side's first 683 calls for v1:

| | `c4` | `substate` |
|---|---|---|
| ORIGINAL | **`0` on all 683** | **`3`** on 682, `4` on 1 |
| PORT | **`255` on 649**, `0` on 34 | **`6` on all 683** |

The original holds accel off through a start countdown in sub-state 3. The port is in **sub-state
6 from the first frame** and accelerates immediately.

That is not emergent — `TrackRenderer.cpp:100` is literally:

```cpp
int aib_game_sub_mode() { return 6; }   // race (FUN_0040e350)
```

## 3. This is the same root as two earlier findings this session

| finding | surface symptom | root |
|---|---|---|
| `RESULT_M5.md` | `bias374` ramps monotonically; the original's sawtooths | sub-mode never **5** → `FUN_00418560` Branch A never runs → `0x007f0ff8` never re-zeroed |
| `RESULT_M5.md` | the ported mode-5 branch is INERT | sub-mode never **5** |
| **this leg** | `GF1-CALLWISE` windows don't overlap | sub-mode never **3** → no countdown → `i0` 0 vs 683 |

**All three are `aib_game_sub_mode()` returning a constant.** The port does not model the race
sub-state machine, and that single stand-in is upstream of the `bias374` divergence, the inert
mode-5 port, and the CALLWISE anchor mismatch alike.

## 4. What is NOT claimed

- **Not that branch 2 is wrong.** It fires 18 times when reachable, LOS passing on all of them, and
  the firing logic was never shown defective. The comparison is confounded by phase, not by logic.
- **Not that 0 vs 64 is a verdict on the port.** It is a verdict on *comparability*: the two
  windows measure different parts of the race.
- Not that fixing the sub-state machine would make the counts match. Necessary, not sufficient —
  the firing condition also needs `Prog(v) == 0` and `6.5 < Prog(0)` to co-occur.
- No C-level. A call-count comparison is not a behavioural diff.
- The port ran with `MASHED_NO_ELIM`, a measurement control that suppresses a real mechanic
  (`RESULT_NOELIM.md` §5). The original did not.

## 5. Next

1. **The race sub-state machine is now the single highest-value item in this area.** It is upstream
   of three measured symptoms and it is what `aib_game_sub_mode`'s constant stands in for. It
   deserves a `DEFERRED.md` row of its own, scoped as a real subsystem rather than a knob.
2. `GF1-CALLWISE` can be re-run meaningfully once the port has a countdown — the anchors would then
   land on comparable phases.
3. `MASHED_WIRE_B2`, `MASHED_SLOT_PLAYER` stay default-OFF.

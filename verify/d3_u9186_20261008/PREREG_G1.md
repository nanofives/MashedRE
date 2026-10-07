# PRE-REGISTRATION (UNRUN) — U-9186 leg G1: are `FUN_00442a60`'s gates live standalone?

Date 2026-10-08. **Status at commit time: UNRUN.** Measurement only. No port is written by this leg.
No C-level, nothing default-ON, `original/` untouched.

## 0. Why this leg exists instead of the port

`RESULT_E2.md` closed the rule engine as a route to D3 criterion (b) and left U-9186 —
the three unported `FUN_00416250` branches — as the named carrier, with `FUN_00442a60`
(the producer of their missing input) as the thing to port first.

Decompiling `FUN_00442a60` first (headless, slot `Mashed_pool0`, read-only) shows **porting it
blind would be a fourth null in this lane.** What it actually does:

```c
void FUN_00442a60(void)            // 0x00442a60, 531 bytes
  _DAT_008989b0 = 0; _DAT_008989b4 = 0; _DAT_008989b8 = 0; _DAT_008989bc = 0;
  local_3c = 4;  FUN_0040e180(&local_3c,&local_38);          // the two most-separated cars
  local_34 = FUN_00408ad0(local_3c);                          // race_pct of each
  local_30 = FUN_00408ad0(local_38);
  ... lap-wrap adjust with _DAT_005cc730 / _DAT_005ccd6c / _DAT_005cc568 ...
  FUN_0046cbb0(local_3c,&local_24,local_20);                  // per-car state field
  FUN_0046cbb0(local_38,&local_28,local_1c);
  if (local_30 < local_34) { swap }                           // rearmost becomes reference
  if ( ((local_28 == 0) || (..., local_24 == 0))
       && (_DAT_008989a8 = iVar4, DAT_008989c8 = iVar2,
           FUN_0040e370(iVar4) != 0)                          // GATE 1: slot-state
       && (FUN_0046c7b0(iVar4) == 1) )                        // GATE 2: vehicle type
  {
      FUN_0046d4a0(&local_2c,iVar4);
      local_18 = *(float*)(local_2c+0x30); local_10 = *(float*)(local_2c+0x38);
      for (local_3c = 0; local_3c < 4; ++local_3c)
          if (FUN_0040e370(local_3c) && FUN_0046c7b0(local_3c)==1) {
              FUN_0046d4a0(&local_2c,local_3c);
              local_c = { local_18 - rec[0x30], 0.0, local_10 - rec[0x38] };
              *(float*)(&DAT_008989b0 + local_3c*4) =
                  FUN_004c3ac0(local_c) * _DAT_005cc9bc;      // planar XZ distance, scaled
          }
  }
```

The entire body past the entry zeroing is gated on `FUN_0040e370(v) != 0`:

```c
bool FUN_0040e370(int param_1)      // 0x0040e370
{ if (3 < param_1) return false;
  return *(int *)(PTR_PTR_005f2770 + param_1 * 4 + 0x34) != 0; }
```

**`*(u32*)0x005f2770` is 0 in the standalone.** It is a load-time `.data` constant whose value is
`0x005f2728`; the standalone does not load the original's `.data` and its image-pad owns the RVA
zero-filled (`AiStandalone.cpp:1388-1393`, two independent witnesses recorded there, and
`TrackRenderer.cpp:366-377`). `CarSlotStateSet` already early-returns on it
(`AiStandalone.cpp:1404-1409`).

So a byte-faithful port of `FUN_00442a60` would: zero the four floats, call `FUN_0040e180` (whose
own double loop is gated on the *same* predicate and would return `(0,0)`), fail the guard chain,
and return — writing nothing but zeros, which is what `0x008989b0` already holds. **Predicted
inert, for a reason that has nothing to do with `FUN_00442a60` itself.**

Memory `three-nulls-mean-run-an-experiment` applies directly: this lane has now produced three
inert results (leg C's slot-state seed, the `FUN_00416250` branch wiring, leg E2's metric swap).
The registered response is to supply a missing trigger and measure, not to transcribe a fourth
function.

## 1. What G1 measures

A default-OFF probe `MASHED_U9186_GATES=<path>` dumps, once per `UpdateCar` tick, one row per
`v = 0..3` carrying **every input `FUN_00442a60` reads**, so the gate chain can be evaluated
offline without porting anything:

| column | expression | why |
|---|---|---|
| `p5f2770` | `U32(0x005f2770)` | the pointer `FUN_0040e370` dereferences; 0 means gate 1 is unreachable |
| `slot_state` | `p5f2770 ? I32(p5f2770 + 0x34 + v*4) : -1` | **gate 1**, `FUN_0040e370` |
| `veh_type` | `I32(0x008815a4 + v*0xd04)` | **gate 2**, `FUN_0046c7b0`; passes iff `== 1` |
| `state0` | `I32(0x00881f90 + v*0xd04)` | `FUN_0046cbb0`'s first out; the `== 0` test |
| `rec_t` | `U32(0x00881f48 + v*0xd04)` | `FUN_0046d4a0`'s table index |
| `rec_x`, `rec_z` | `F32(rec + 0x30)`, `F32(rec + 0x38)` where `rec = 0x00881ec8 + v*0xd04 + rec_t*0x40` | the positions the distance is built from. Stride decode per `PromoLoop_round58.cpp:21`, NOT the decompiler's dword-scaled `*0x341`/`*0x10` |
| `racepct_ec` | `F32(0x008a96ec + v*0x30c)` | `FUN_00408ad0`; the bridge's slot |
| `refdist` | `F32(0x008989b0 + v*4)` | the output, to confirm it is 0 today |

All reads. The probe writes nothing and computes nothing the game uses.

## 2. Arms

Both under the F2 determinism knobs (`MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400`) and E1's
scenario (`MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1
MASHED_ROUND_RULE=4`):

| run | adds |
|---|---|
| `G1base` | — |
| `G1seed` | `MASHED_SLOTSTATE_SEED=1` (makes `0x005f2770` point at `0x005f2728`) |
| `G1both` | `MASHED_SLOTSTATE_SEED=1 MASHED_RACEPCT_BRIDGE=1` |

## 3. Gates

| gate | threshold |
|---|---|
| `G1-PTR` | `p5f2770` is **0** on every row of `G1base` and **0x005f2728** on every row of `G1seed`/`G1both`. Confirms the knob is the only thing that moves it |
| `G1-GATE1` | count of `(frame, v)` where `slot_state != 0`, per arm. **The prediction on `G1base` is 0 of all rows** |
| `G1-GATE2` | count where `veh_type == 1`, per arm. Independent of the seed knob |
| `G1-GATE3` | count where `state0 == 0`, per arm |
| `G1-ALL` | count where all three pass — i.e. rows on which a ported `FUN_00442a60` would write a non-zero distance |
| `G1-OUT` | `refdist` distinct values per arm. Predicted `{0.0}` everywhere today |
| `G1-POS` (**control that can fail**) | `rec_x`/`rec_z` must be non-constant and must differ between cars on `G1base`. If the position source is itself dead, the whole substrate is absent and gate counts say nothing about `FUN_00442a60` specifically |

## 4. Decision rules, fixed now

- **`G1-ALL` > 0 on `G1base`** → the gates are already live, the inertness prediction is WRONG, and
  porting `FUN_00442a60` is the next leg. Register it separately.
- **`G1-ALL` == 0 on `G1base` but > 0 on `G1seed`** → the slot-state seed is the one missing
  trigger. The port becomes worth writing *behind that knob*, and the knob's own default-ON case
  becomes the real question.
- **`G1-ALL` == 0 on every arm** → `FUN_00442a60` is unportable-to-effect today and U-9186's
  blocker is **the vehicle-table substrate** (`0x008815a4` / `0x00881f90` / `0x00881ec8`), not any
  single function. Report that and do NOT write the port. Which of gates 2/3 fails tells which
  table is missing.
- **`G1-POS` fails** → report the measurement as uninformative about `FUN_00442a60` and name the
  dead position source instead.

## 5. Non-goals

Porting `FUN_00442a60`. Porting any `FUN_00416250` branch. Any default-ON change. Any edit to
`original/`. Any C-level movement. Any claim about the original's values — this leg measures the
**standalone** only, and whether the original agrees is a separate question.

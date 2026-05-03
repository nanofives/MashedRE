# Session DDDD — vehicle_damage — SESSION END

**Outcome: SUCCESS — DAMAGE_FN identified**  
**Dates:** 2026-05-03 (two-part; resumed after context compaction)  
**Pool slot used:** Mashed_pool0  
**SHA-256 anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓  

---

## Primary result

**DAMAGE_FN = `FUN_00410d10`** @ `0x00410d10`–`0x00411148`.

This is the per-step collision damage dispatcher. It reads a Rwp37-written collision event flag, identifies the loser car, triggers the destruction effect, and delegates game-side reset and scoring. It is called once per 50ms game step while the race is running.

---

## Full call chain

```
FUN_00492290  game loop           (0x00492290)
  └─ FUN_00492d30  sim driver     (0x00492d30)
       └─ FUN_004111c0  race SM   (0x004111c0, entry; body 0x0040dd80)  ← 50ms step
            └─ FUN_00411170       (0x00411170)
                 ├─ FUN_00411600(DAT_007f0ff4)   TimeTrial_HitCheckpoint
                 ├─ FUN_00411ae0(param_1, DAT_007f0ff4)  lap-timing update
                 └─ FUN_00410d10()               ← DAMAGE_FN
                      ├─ FUN_00442df0()          collision-event reader (→ DAT_00898980)
                      ├─ FUN_0040e180(&a, &b)    collision pair finder
                      ├─ FUN_00408ad0(car)        race-position comparator
                      ├─ FUN_0046cbb0(car, &s, …) per-car damage-state reader
                      ├─ FUN_004922e0(loser,3,10,0x80) hit effect / sound
                      ├─ FUN_00422fd0(loser)      disable car + explosion
                      └─ FUN_0040eee0(loser, 1)  game-side reset + scoring
                           ├─ FUN_00422fd0(loser) (again, conditional)
                           ├─ FUN_0040b290(loser, -1) score write → DAT_008a9520
                           ├─ FUN_0040b290(winner, +1)
                           └─ FUN_0040d590(a,b,c,unit)  sandwich-kill arbiter
```

After `FUN_00410d10` returns 1 (only 1 car left active), `FUN_00411170` checks `FUN_00410510()`:
- If `FUN_00410510() != 0`: calls `FUN_00430290()` and returns (non-elimination end condition).
- Else: sets `DAT_0063ba8c = 7` (race-over sub-state) and calls `thunk_FUN_004194f0()`.

---

## `FUN_00410d10` — annotated decompile summary

```c
uint FUN_00410d10(void) {
  iVar3 = FUN_00443080();
  if (iVar3 == 1) return 0;          // race not started / already finished

  switch (DAT_007f0fd0) {            // game mode checks (modes 4, 5 bypass some logic)
    ...
  }

  fVar10 = FUN_00442df0();           // read DAT_00898980 (collision impact float)
  if (fVar10 == 10.0f) {             // DAT_005cc55c = 0x41200000 = 10.0f (collision sentinel)
    FUN_0040e180(&local_24, &local_c); // find pair: loser = local_24 (farthest-displaced)
    // compare race positions with FUN_00408ad0
    FUN_0046cbb0(local_24, &local_10, local_8);  // read loser damage state
    FUN_0046cbb0(local_c,  &local_14, local_4);  // read winner damage state
    if ((local_14 != 0) && (iVar3 != 0)) return 0; // already-destroyed guard
    FUN_004922e0(fVar7, 3, 10, 0x80); // hit sound/particle
    FUN_00422fd0(fVar7);              // destroy car (disable + explosion)
    iVar3 = FUN_0040eee0(fVar7, 1);  // game-side reset + scoring
    if (iVar3 == 1) return 1;        // race over: only 1 car left
  }

  // count remaining alive cars → DAT_00803320
  // return 1 if exactly 1 active car, 0 otherwise
}
```

---

## Collision event mechanism

| Symbol | Address | Value / role |
|---|---|---|
| `DAT_00898980` | `0x00898980` | Per-frame float written by Rwp37 physics callback. Read by `FUN_00442df0`. No static write xref found — writer uses a computed pointer (likely registered Rwp37 collision callback). |
| `DAT_005cc55c` | `0x005cc55c` | `0x41200000` = **10.0f**. Sentinel: `DAT_00898980 == 10.0f` → collision event this step. |

`FUN_00442df0` (0x00442df0–0x00442df5) is a 6-byte stub: `return (float10)DAT_00898980;`

**The writer of `DAT_00898980` is unresolved** — no static xref exists. It is set from a computed-pointer path, most likely a Rwp37 collision callback registered during physics scenario setup. Finding the writer requires either a Frida write-watchpoint on `0x00898980` at runtime or tracing the Rwp37 callback table.

---

## `FUN_0040e180` — collision pair finder

`FUN_0040e180(int *param_1, int *param_2)` @ `0x0040e180`–`0x0040e330`:

Finds the pair of active, non-destroyed cars with maximum 3D Euclidean separation:
- Outer loop: car `iVar2` = 0..3, inner loop: car `iVar1` = 0..3 (skip self).
- Per-car checks: `PTR_PTR_005f2770 + (0x34 + car*4) != 0` (slot active), `FUN_0046c7b0(car) == 1` (car valid), `FUN_0046cbb0(car, &state, ...) → state == 0` (not destroyed).
- Position read via `FUN_0046d4a0(&ptr, car)` → `[ptr+0x30..0x38]` = X, Y, Z.
- Distance = `FUN_004c3ac0(&delta_vec)` (3D magnitude).
- Returns the pair `(iVar3, iVar5)` with maximum distance. Falls back to (0, 0) if no valid pair.

Pointer table base: `PTR_PTR_005f2770` + offsets `0x34, 0x38, 0x3c, 0x40` for cars 0–3.

---

## `FUN_00422fd0` — car destruction effect

`FUN_00422fd0(int param_1)` @ `0x00422fd0`–`0x00423031`:

```c
undefined4 FUN_00422fd0(int param_1) {
  if (param_1 < 0x10) {
    FUN_0046c5c0(param_1);                   // deactivate car physics/control
    FUN_0046c790(param_1, 1);                // set car state = 1 (destroyed)
    FUN_004215c0(param_1, 50.0f, 0);         // 0x42480000 = 50.0f (explosion param)
    FUN_004215c0(param_1, 50.0f, 1);
    FUN_0045ba00(param_1, 2);                // trigger effect type 2 (explosion)
    if (DAT_007f0fd0 != 7) {
      iVar1 = FUN_0040e470(param_1);
      if (iVar1 == 1) thunk_FUN_00419760(param_1, 1);
    }
  }
  return 0;
}
```

Already has a Ghidra comment `[C1 2026-05-03]` from prior work.

---

## `FUN_00411ae0` — lap-timing update (NOT physics)

`FUN_00411ae0(undefined4 param_1, uint param_2)` @ `0x00411ae0`–`0x00411ccf`:

Updates `DAT_0063bb1c` (current race time / lap delta). If game mode 2 (`FUN_0042f6a0() == 2`):
sets up two 48-byte transform structures, calls `FUN_00482c10(DAT_0063bb0c, &mat, time)` and `FUN_00482c10(DAT_0063bb10, &mat, time)`. `FUN_00482c10` is a RenderWare animation/physics driver step — **candidate for the Rwp37 simulation tick** that may write `DAT_00898980` via internal callback. Requires further tracing.

---

## Functions discovered this session

| RVA | Provisional name | Notes |
|---|---|---|
| `0x00410d10` | `CollisionDamage_Step` | DAMAGE_FN. Per-step collision damage dispatcher. |
| `0x00411170` | `RaceStep_DamageAndEnd` | Wraps TimeTrial checkpoint + lap timing + DAMAGE_FN; handles race-end transition to state 7. |
| `0x00411ae0` | `LapTiming_Update` | Updates `DAT_0063bb1c` lap delta; drives `FUN_00482c10` animation/physics tick (mode 2 only). |
| `0x00422fd0` | `Car_Destroy` | Deactivates car, sets destroyed state, triggers explosion effect type 2. |
| `0x00442df0` | `CollisionEvent_Read` | 6-byte stub: returns `DAT_00898980` as float. |
| `0x0040e180` | `CollisionPair_FindMaxDist` | Finds pair of active non-destroyed cars with maximum 3D separation. |
| `0x00482c10` | `(RW physics/anim tick?)` | Called with object ptr + transform + time delta; candidate for Rwp37 simulation step. |
| `0x0046c7b0` | `(car valid check?)` | Returns 1 if car slot is valid/active. |
| `0x0046cbb0` | `(car damage state read)` | Returns current per-car damage/hit state via out-param. |
| `0x0046d4a0` | `(car position ptr)` | Returns pointer to car transform; `[ptr+0x30..0x38]` = X, Y, Z. |
| `0x004c3ac0` | `Vec3_Magnitude` | Takes &float[3] delta vector, returns Euclidean length. |
| `0x00410510` | `(race-end mode check?)` | Non-zero return suppresses race-over state when 1 car left; game-mode specific. |
| `0x00430290` | `(non-elimination end)` | Called when `FUN_00410510` non-zero after all eliminations. |

---

## Prior session inventory (retained)

See Strategy 1 results and vehicle array confirmation in git history. All string-search strategies were exhausted; the winning approach was top-down game-loop tracing.

---

## Vehicle array (confirmed prior session)

- Base: `0x00684ea8`, stride `0x110` (272 bytes/slot), 32 slots → end `0x006870a8`
- Iterator/reset: `FUN_00452f30` (0 Ghidra callers — called via function pointer)
- Per-slot reset: `FUN_00452f00` (zeros +0x4, +0x44, +0x4c, +0x54; checks +0x48)

---

## Recommended next steps

1. **Resolve `DAT_00898980` writer.** Run a Frida write-watchpoint on address `0x00898980` during a live race collision. The function that fires is the Rwp37 collision callback — this is the bridge between physics and game damage.

2. **Trace `FUN_00482c10`.** This is the `FUN_00411ae0` callee that drives the Rwp37 physics tick (mode 2). Its internals likely contain the collision callback dispatch that writes `DAT_00898980`.

3. **Decompile `FUN_0046c7b0` and `FUN_0046cbb0`.** These are used in both `FUN_00410d10` and `FUN_0040e180` to gate per-car processing. Understanding their data layout will clarify the per-car damage/active state struct.

4. **Decompile `FUN_00482c10`.** Identify whether it is a generic RenderWare animation step or a Rwp37-specific `RwpWorldStep`-equivalent, and confirm whether it dispatches collision callbacks.

5. **Commit vehicle_damage analysis.** No hooks.csv rows added yet — `FUN_00410d10` is C1 confidence (call chain confirmed, game behavior understood, no Frida diff yet). Use `re-classify` when promoting.

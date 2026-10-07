# PRE-REGISTRATION (UNRUN) — U-9186 leg H3: port `FUN_00442a60`

Date 2026-10-08. **UNRUN at commit time.** Scope: `re/analysis/VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md` §4.
This is DEFERRED leg B, unblocked by the **USER DECISION (Mariano, 2026-10-08)** recorded in
`DEFERRED.md` D-11072 and the CHANGELOG.

No C-level follows — leg B is a bridge and inherits the 2026-10-06 decision (knob-gated,
non-bit-identical substrate acceptable, no C-level from any bridge). `original/` untouched.

## 0. What ships

A standalone body for `FUN_00442a60` writing `0x008989b0 + v*4`, **behind a default-OFF knob
`MASHED_REFDIST=1`**. Default behaviour is unchanged; nothing is promoted; the `.asi` is untouched.

Verbatim structure from the headless decompilation (slot `Mashed_pool0`, read-only):

```
  _DAT_008989b0..bc = 0                                  // zero all four first
  local_3c = 4; FUN_0040e180(&local_3c, &local_38)       // the two most-separated cars
  local_34 = FUN_00408ad0(local_3c)                      // race_pct of each
  local_30 = FUN_00408ad0(local_38)
  if ((local_34 <= _DAT_005cc730) || (_DAT_005ccd6c <= local_30)) {
      if ((_DAT_005cc730 < local_30) && (local_34 < _DAT_005ccd6c))
          local_30 -= _DAT_005cc568;                     // lap wrap
  } else local_34 -= _DAT_005cc568;
  FUN_0046cbb0(local_3c,&local_24,local_20)
  FUN_0046cbb0(local_38,&local_28,local_1c)
  if (local_30 < local_34) swap                          // rearmost becomes reference
  if (((local_28==0) || (local_24==0))
      && (_DAT_008989a8 = iVar4, DAT_008989c8 = iVar2, FUN_0040e370(iVar4) != 0)
      && (FUN_0046c7b0(iVar4) == 1)) {
      FUN_0046d4a0(&local_2c, iVar4);
      local_18 = *(float*)(local_2c+0x30); local_10 = *(float*)(local_2c+0x38);
      for (local_3c = 0; local_3c < 4; ++local_3c)
          if (FUN_0040e370(local_3c) && FUN_0046c7b0(local_3c)==1) {
              FUN_0046d4a0(&local_2c, local_3c);
              local_c = { local_18 - rec[0x30], 0.0, local_10 - rec[0x38] };
              *(float*)(&DAT_008989b0 + local_3c*4) = FUN_004c3ac0(local_c) * _DAT_005cc9bc;
          }
  }
```

Callees already available to the standalone: `FUN_0046c7b0`, `FUN_0046cbb0` (H1a),
`FUN_0046d4a0` (H1b), `FUN_00408ad0` (reads `0x008a96ec`, supplied by `MASHED_RACEPCT_BRIDGE`),
`FUN_0040e370`. `FUN_0040e180` has an exe copy — **see §3's hazard**.

## 1. The four tuning constants — resolve before writing, do not guess

`_DAT_005cc730`, `_DAT_005ccd6c`, `_DAT_005cc568` (lap-wrap thresholds/offset) and
`_DAT_005cc9bc` (distance scale) are in the original's `.data`, which the standalone does not
load — the same class of problem as `0x005f2770`. Each must be read out of
`original/MASHED.exe.unpatched` at its file offset and written into the port as a named literal
with the address cited, exactly as `AiStandalone.cpp:1388-1390` did for the slot-state pointer.

**If any of the four reads back as `0`**, that is a finding, not a value: stop and report, because
a zero scale makes every distance `0` and a zero wrap offset disables the wrap branch. Recorded as
gate `H3-CONSTS`.

## 2. Gates

All runs under `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400` with the E1 scenario, three
repeats per arm, OFF arm = knob unset.

| gate | threshold |
|---|---|
| `H3-CONSTS` | all four constants read non-zero from the unpatched image, each reported with its file offset and hex bits |
| `H3-WROTE` | with the knob ON, `0x008989b0 + v*4` is non-zero for at least the three AI cars on ≥1 frame, and equals the probe's independently recomputed `|Δxz| * scale` on **100%** of rows |
| `H3-GATEFIRE` (**the one that matters**) | a default-OFF counter on U-9186's two branches: `FUN_00414a70 == 2` and `FUN_004148b0 != 0 && FUN_00416060 != 0`. CONFIRM each fires at a rate approaching the original's **36 / 64** calls over the 220-call window, BEFORE any `ctrl` write is enabled |
| `H3-INERT` (**can fail, and failing is the good outcome**) | OFF vs ON arm, cell-for-cell. If identical, the port is INERT and no behaviour claim follows — report it as such, exactly as E2 did |
| `H3-KNOBOFF` | OFF arm identical to a freshly-taken reference under the same knobs |
| `H3-NOREG-E` / `-B` | criterion (e) six digits and (b) bands unchanged on the OFF arm |
| `H3-DET` | 3 repeats identical within each arm |

`H3-WROTE`'s "independently recomputed" means the probe must compute the expected distance from
`VehiclePhysics_RecordF32` positions **without** calling the ported function — otherwise the gate
compares the port against itself (memory `gate-instrument-must-touch-the-changed-path`, earned
twice this session).

## 3. Registered hazards

- **`FUN_0040e180`'s exe copy is DEMOTED C4→C2 (2026-09-29)**: `RaceCamera::MostSeparatedPair`
  uses `std::sqrt` and disagrees with the `.asi` copy by 7.8% (`hooks.csv:877`). H3 depends on its
  *index* output, not its distance, so a 7.8% magnitude difference may or may not change which
  pair is selected. **Gate `H3-PAIR`:** log both selected indices per frame and report how often
  they differ from the `.asi` formulation; if they ever differ, the dependency is real and must be
  resolved before `H3-WROTE` is read.
- **`sel_9a8` reads 0 while `VehicleStruct.h:101` says `kWheelSetSel` init 1** — recorded
  `[UNCERTAIN]` in the scope §6 and still unresolved. `FUN_0046d4a0` multiplies it by `0x40`, so a
  wrong value selects the wrong matrix. `H3-WROTE`'s independent recomputation must use the same
  `t` the port uses, and the value must be *reported*, not assumed.
- **`0x008989a8` / `0x008989c8`** are written by the original before the guard chain. The port
  writes them too, or it is not faithful; both are in blank-mapped space standalone, so writing
  them is inert but must still happen.

## 4. Decision rules

- `H3-CONSTS` fails → stop. Nothing else is read.
- `H3-PAIR` shows any disagreement → resolve the `FUN_0040e180` demotion first; `H3-WROTE` is not
  interpreted until then.
- `H3-WROTE` passes, `H3-GATEFIRE` approaches 36/64 → the lane's premise is confirmed and wiring
  the branches becomes the next registered leg.
- `H3-WROTE` passes, `H3-GATEFIRE` ≈ 0 → the producer works and the branches still do not fire;
  report the next blocker rather than widening scope.
- `H3-INERT` identical → report INERT. **Do not** chase an effect by enabling anything further in
  the same leg.

## 5. Non-goals

Any default-ON change. Wiring the `FUN_00416250` branches to `ctrl` (a separate leg, gated on
`H3-GATEFIRE`). Any C-level promotion. Any edit to `original/` or to the `.asi` code path.

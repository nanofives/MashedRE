# RESULT D4-WSJ-WIRE: race SFX wired by data behind MASHED_AUDIO_WIRE (default OFF)

**Verdict: HANDOFF (code, PREREG, runner and scorer committed; build, gates and arms not run: solo window open).**
Branch `orch/D4-WSJ-WIRE` off `orch/integration` @ `992687ebc`, with `orch/D4-WSJ-SFXMAP` @ `05d4dd024`,
`orch/D4-WSJ-ENGCLASS` @ `5473f09c9` and `orch/D4-WSJ-MUSIC` @ `05339c191` merged in (`git merge-base --is-ancestor`:
none was an ancestor of `992687ebc`, so all three were merged, `merge.autoStash=false`, no conflict).

PREREG `PREREG_D4-WSJ-WIRE.md` committed alone as `3ff8f93f3`, before any code or arm. L-24 = (a) (Mariano 2026-10-26).

## What the knob does (code `6a0443a8f`, `6607c59ab`)

One knob, `MASHED_AUDIO_WIRE=1`, read once. Unset, every changed site takes its pre-change path (the only knob-OFF
additions are early returns on a cached bool). Set:

| # | wiring | where |
|---|---|---|
| W1 | permdict wave table in code, 43 rows (name, count `+0x84`, flag `+0x88`), bases = running sum (139) | `Audio/AudioWire.cpp` `kPerm` |
| W2 | 139-voice model: `FUN_00465ca0` first-free (22050 / 1.0, pod counter), `FUN_00465940` explosion scan (voices base+4..7, pitch k*2000+22050), fixed slot `FUN_00465c10`/`FUN_00465e80`, stop `FUN_004661a0`. Busy = one-shot length at the start frequency (QPC) or a running loop, so a muted arm takes the same decisions | `AudioWire.cpp` `FirstFree` / `ExplosionScan` / `FixedSlot` / `StopSlot` |
| W3 | voice-budget guard, `DAT_0068f4c8` pinned to 1 (SFXMAP's recommendation; every REF capture had 1), count = `Fi_ParticipantCount()` (`DAT_008a94d0`, which `FUN_0040e340` returns). Closes `fire` in the scan and every impact | `GuardClosed()` |
| W4 | powerup waves by event: GUN `machineg`, DRUM `rocket`, MISSILE `rocket` + `missile exhaust`, P_MINE `drop mine`, SHOTGUN `shotgun`, FLASH `flash`, MORTAR `rocket` + `mortarexhaust`, OIL `oilslick` (slot = owner), R_FLAME arm `flame thrower` (slot = owner) and deact stop. The three WRONG-WAVE rows (GUN, DRUM, MORTAR) are fixed under the knob | `PowerupEffects.cpp` -> `PowerupBackend::SfxEvent` -> `WirePowerup` |
| W5 | pickup collect starts `collision with pod` (the port's `PK_Sfx` was a counted no-op), at PC_53 like `PK_Grant` | `Gameplay/PickupTick.cpp` |
| W6 | audio-side copy of the `0x007e9de0` ring: type 3 when `VehicleContactHistoryUpdate` returns 1 (the branch that pushes at `0x00469cc8`), type 4 at the existing car-car push site with the same mode test. Consumer = `FUN_004648b0` (newest first) + `FUN_00464670` (both voices free, max-speed volume law, kind-1 scale, clamp 1.0, pitch 22050 + [0, 2205)) | `VehiclePhysicsRun.cpp`, `AudioWire.cpp` `Impact` / `RingDrain` |
| W7 | engine: per car 0..3 the class from `EngineClassTable.h` (port DFF base name -> original vehicle index), voice base+car on `eng<k>` and base+4+car on `eng<k>d`, looped, started when not busy; pitch rpm*3.5 + car*200 + row `+0x90`; volume rpm*1e-5 + bias. `EngineStart` takes the player's vehicle (`RaceSession.cpp`), and under the knob the legacy single voice is not created | `AudioWire.cpp` `WireRaceTick`, `AudioEngine.cpp` `EngineStart` |
| W8 | music: no race `cdaudio.rws` stream and so no 120 s cap (the D4-WSJ-MUSIC branch, taken when either knob is set) | `AudioEngine.cpp` `MusicSetState` |

Self-test: `MASHED_AUDIO_WIRE_SELFTEST=1` runs 20 checks at boot when the permdict bank loads (table sums, first-free order,
full-wave rejection, `fire` guard at 4 and 1 participants, explosion pitches, ring type 3 with the guard closed / open /
busy, fixed slot, engine class by name).

## Findings carried from the siblings, and how each was handled

- **Guard (`DAT_0068f4c8 && live cars > 1`)**: pinned to 1 as SFXMAP recommends. Consequence: in every multi-car race
  impacts, `fire`, skid and tyre stay silent, as in the original on the reference hardware. Rejections are counted
  (`guard_closed` first row + `guard_closed_total` per wave at race end).
- **Event-ring pushes**: wired as an audio-only copy (W6). The ring has no sim reader in this copy. Types 8 and 0xb have
  no port producer (`FUN_0047def0` RWP and `FUN_0044ba30`/`FUN_0044c380` are not ported), so they are not pushed.
- **GUN / DRUM / MORTAR wrong waves**: fixed under the knob (W4); knob OFF still plays the legacy names.
- **`EngineStart` vehicle argument**: added (`EngineStart(float, int vehicleIdx = -1)`), resolved through `EngineClassTable.h`
  plus a new `kVehicleNames` row list in that header (record `+0x20` names, ENGCLASS tsv).
- **Music**: no race cdaudio and no 120 s cap (W8). The `musicloop1` fade-out stays `[UNCERTAIN]` and is **not modelled**:
  the knob stops the menu loop at race entry. The original fades `_DAT_0068fc8c` by 0.005 per `FUN_00466b50` call in
  state 3/7 (`0x0045db50` case 3) from an unmeasured start level, then stops voice 138 from `0x00462415`; REF saw that
  stop on almost every race frame (5560 / 2721 / 6472 calls), so the audible fade is at most 200 ticks. Evidence needed
  to model it: a per-call trace of `0x0045db50` across the countdown.

## Not wired (residuals, not predicted)

Course-bank waves (skid, tyre roll, 3-state impact: all behind the W3 guard), `carlands` (`FUN_004644a0`, needs
`FUN_0046cc10` count history), helicopter proximity (3 starts per REF capture at tick 0, needs the `FUN_00425ef0`
entity list), noise burst / character streams (G4), debris lane (4 per REF capture, `FUN_00422160` not ported), splash,
copter, mode-6 launchers, beat-the-bomb, timing beam, per-car state-2 explosion (`FUN_004645c0`), missile in-flight
update and terminal stop (so `missile exhaust` plays one-shot), R_FLAME volume update, the rpm state machine
(`DAT_0069054c`; car 0 uses the stand-in's speed norm, cars 1..3 hold min rpm), 3D position (`FUN_0045daf0`).

## [UNCERTAIN] items introduced here

- `+0x88` = 1 treated as "loops" (engines, flame thrower, fire, helicopter rows). The 80E RESULT names the field
  (`FUN_005a6dc0(v,8,1,flag)`), not its meaning.
- Fixed-slot starts (`FUN_00465c10` / `FUN_00465e80`) use `FUN_00465ca0`'s 22050 / 1.0: their own frequency and volume
  were not read.
- MISSILE slot: the original indexes `DAT_006883ec[record]`; the port has no record index at the fire site, the owner is used.
- Type-4 ring car field (`0x007ea5e4`): the pair's vehA (= other car j) is used. Unreachable under the pin (needs 2+ cars).
- d-layer pitch / volume law (`0x00463b39`) not read: the main voice's is reused.
- Speed for the impact volume = record `+0x9e4` (`FUN_0046d6d0`, as `Ai/AiPostStepBrake.cpp`); memory notes that
  `+0x9e4` is not comparable port vs original, so impact volumes are not claimed equal.

## Not interpreted, for the lead

The original's audio draws the shared RNG `FUN_00472650`: per car per audio tick in `FUN_00463640` (`0x004636ed`
min rpm, and rpm case 0) and per impact in `FUN_00464670` (`0x00464777`, `0x004647a4`). The knob uses an audio-private
RNG so it cannot move sim state. Whether the port's sim RNG stream (`Ai_RandRange`) should carry those draws to match
the original's draw index is a separate question, not acted on.

## Gates

Not run (solo window). `VehiclePhysicsRun.cpp` is touched (two knob-gated calls), so the merge train runs its full set
(`full_set_paths` includes `Vehicle/`).

## HANDOFF (resume here)

1. Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` from the worktree. Fix any compile error (nothing was
   compiled in the solo window).
2. Private root: `robocopy <repo>\original D:\MashedWT\D4-WSJ-WIRE_root\original /E` (the root contains `original\`).
3. G0: `A4_ROOT=D:/MashedWT/D4-WSJ-WIRE_root A4_OUT=D:/MashedWT/D4-WSJ-WIRE_data/gates_off py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`
   under `mashed_lock.py run --label D4-WSJ-WIRE`. Expect G-CTRL `5943c5f1/b493262b/09551e6a`, B `3a03670c/2516d903/53778664`.
4. G1: same with `A4_OUT=.../gates_on` and `py -3.12 verify/wsjwire_20261026/gates_knob.py ...` (sets `MASHED_AUDIO_WIRE=1`). Expect the same six.
5. Arms: `run_port.py A menu`; `ARM_KNOB=1 run_port.py B menu`; `ARM_KNOB=1 ARM_SELFTEST=1 run_port.py C menu 30`.
6. Score: `py -3.12 -I verify/wsjwire_20261026/score_wire.py <data>/port/A.audio_events.csv <data>/port/B.audio_events.csv <data>/port/C.audio_events.csv verify/wsjmusic_20261026/A_off.audio_events.csv verify/wsjwire_20261026/score.txt`.
7. Fill the verdict, the gate hashes, and the raw-data SHA-256 table here; `roadmap.ps1 set D4-WSJ-WIRE review -Evidence verify/wsjwire_20261026/RESULT_D4-WSJ-WIRE.md`.

## Raw data (not committed), SHA-256

None yet. Root `D:/MashedWT/D4-WSJ-WIRE_data/`.

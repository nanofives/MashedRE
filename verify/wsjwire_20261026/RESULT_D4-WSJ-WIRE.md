# RESULT D4-WSJ-WIRE: race SFX wired by data behind MASHED_AUDIO_WIRE (default OFF)

**Verdict: DECISION. Predictions 1, 2, 3 and 5 hold. Prediction 4 holds except the per-car engine class for cars 1..3,
which falls back to class 0 because the port gives no vehicle to cars 1..3 (cause and detail in §Scores). The knob stays
default OFF. The flip is the open question.**
Resumed 2026-10-26 (session `cmvpe71q0bb0go115rxub6230`): `orch/integration` @ `e11287674` (MASHED_COLL_LUT default ON)
merged in as `5b361ea4d` (`merge.autoStash=false`, no conflict, `--is-ancestor` true after). Build: first compile clean,
`MASHED_NO_DEPLOY=1`, `mashed_re.exe` SHA-256 `48916bb8c281...eeaea`.
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

`VehiclePhysicsRun.cpp` is touched (two knob-gated calls), so the merge train runs its full set (`full_set_paths`
includes `Vehicle/`). Every boot below ran under `mashed_lock.py run --label D4-WSJ-WIRE` on the private root
`D:/MashedWT/D4-WSJ-WIRE_root`, exe `48916bb8`. Refs from `re/orchestrator/merge_train.json` on this base: G-CTRL
`5943c5f1/b493262b/09551e6a`, B `3a03670c/e85ea6fb/53778664` (the B middle ref moved from the PREREG's `2516d903` with the
MASHED_COLL_LUT flip in `e11287674`; the json wins).

| run | knob | G-CTRL (H_DEF step / gates / ptrace) | B (K0 / K1 / K26) | result |
|---|---|---|---|---|
| G0 | OFF | `5943c5f1 / b493262b / 09551e6a` | `3a03670c / e85ea6fb / 53778664` | exact, all six |
| G1 | `MASHED_AUDIO_WIRE=1` (`gates_knob.py`) | `5943c5f1 / b493262b / 09551e6a` | `3a03670c / e85ea6fb / 53778664` | exact, all six |
| G1b (not preregistered) | ON + `MASHED_AUDIO_EVENT_LOG` | `5943c5f1 / b493262b / 09551e6a` | not run | exact |

G1b exists because the gate arms write no audio event log, so G1 alone does not show that the wire ran inside the gate
scenario. G1b's event log has 12 `vstart`, 2 `pickup`, 1 `guard_closed` rows: the wire ran on H_DEF and the three
G-CTRL hashes did not move.

## Scores (`score.txt`, `score_wire.py`)

| # | prediction | result |
|---|---|---|
| 1 | G0 = refs | PASS (6/6) |
| 2 | G1 = refs | PASS (6/6) |
| 3 | A event log = MUSIC arm A (`A_off`) | PASS, 13 rows = 13, no diff |
| 4a | no race `music_start` | PASS, 0 |
| 4b | `threetwoone` once on v65, `go` once on v95 | PASS, n=1 each |
| 4c | per live car: `eng<k>` on base+c, `eng<k>d` on base+4+c, k = class of that car's vehicle + 1 | **PARTIAL** (below) |
| 4d | no `fire` / impact starts, rejections counted | PASS, 0 starts, `guard_closed_total` barrier 5, other vehicle 1 |
| 4e | powerup / pickup waves | PASS, 0 `pu_event` (menu recipe drives none), 2 `collision with pod` on v100 |
| 5 | self-test | PASS, 20/20 |

**4c detail.** B's race has 4 participants (`guard_closed` rows carry participants 4, and the tick starts 4 cars) but
`RaceSession` passes `m_cfg.numCars = 1` to `WireRaceBegin` (`Race/RaceSession.cpp:215`). The port sets only
`cfg.cars[0].carIndex` (`exe_main.cpp:2111`) and loads one car DFF (`RaceSession.cpp:135`), so cars 1..3 have no vehicle.
Rows: car 0 `engine_class ADVANTAGE -> eng2`, `eng2` v8 + `eng2d` v12 (correct by ENGCLASS). Cars 1..3: no
`engine_class` row, `eng1` v1/v2/v3 + `eng1d` v5/v6/v7, i.e. `WireEngineClass(-1)` = class 0 (the JA unknown-case
branch). Voice indices and the base+c / base+4+c layout hold. The class for cars 1..3 is not the original's, because the
port has no AI vehicle selection to read. The scorer checked car 0 only, which is why it printed PASS. Not a RED by the
verdict mapping (4 is not 1..3). Fix path: an AI vehicle choice in the port (out of this step's scope), then
`WireRaceBegin` with the live count.

## Not run

No D2 arms: the PREREG registers none (G0, G1, A, B, C only), so the solo lock was not taken. Every boot used the
shared `mashed_lock.py run` lock. The audio wire touches no sim value (G1 and G1b are bit-identical to G0).

## Decision for the lead (flip question)

Flip `MASHED_AUDIO_WIRE` to default ON? Options: (a) flip now, with cars 1..3 on class 0 until the port picks AI
vehicles, (b) hold until an AI vehicle choice lands, then flip. Evidence: this RESULT, `score.txt`. Recommendation: (a).
Default sim gates do not move with the knob ON (G1, G1b), and the 4c residue is a missing port input, not a wire error.
Knob-OFF behaviour is unchanged (G0, prediction 3).

## Raw data (not committed), SHA-256

Root `D:/MashedWT/D4-WSJ-WIRE_data/`.

| file | SHA-256 |
|---|---|
| `port/A.audio_events.csv` | `0974688e2493f3f5116f38a6dc2acd28b25965d412e579d3e9a9eb9c4cfc20c5` |
| `port/B.audio_events.csv` | `e24a9281974ff8c8753aacd5969f9862cd8e67a686e75385c073256006b559f3` |
| `port/C.audio_events.csv` | `9eaeeec6e3e3426959a606be4fa52cc68367688b94d3b8cfde619ca8c9195850` |
| `port/A.log` | `26f03b7d08b3fee70300fdf4c400b2b834f2e187916d1a6037145162aed34906` |
| `port/B.log` | `1be538c29afbe843479a4ef094394511a25b2987d8fc294825588ebaa3996f69` |
| `port/C.log` | `53032becb620cfa186a2ff36c371dd9de86ad4e2a3f072d78d28a7a591b40600` |
| `gates_off/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates_off/H_DEF.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `gates_off/H_DEF.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `gates_off/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates_off/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates_off/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates_on/*` | same six SHA-256 as `gates_off` (byte-identical) |
| `gates_on_evlog/H_DEF.audio_events.csv` | `351bf364cd4ec81768998a8b363a6b9e6ae9a5ebd20cfeb98cb9e166679fadeb` |
| `gates_on_evlog/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `build.log` | `9cd3204dc140f74b80512c42a60175727aca10c4ed1aff8eef8852ab92d729f1` |
| `mashedmod/build/mashed_re.exe` | `48916bb8c281672a0996c127056991b46a553bb1f91a2c378b3db95edfbeaeea` |

G1b runner: a scratch copy of `gates_knob.py` that also sets `MASHED_AUDIO_EVENT_LOG` (not committed, one extra env line).

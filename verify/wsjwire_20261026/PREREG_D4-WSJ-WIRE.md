# PREREG D4-WSJ-WIRE (committed alone, before any code change or arm)

Branch `orch/D4-WSJ-WIRE` off `orch/integration` @ `992687ebc` (contains D4-WSJ-80E `dec04fb72`), with
`orch/D4-WSJ-SFXMAP` @ `05d4dd024`, `orch/D4-WSJ-ENGCLASS` @ `5473f09c9`, `orch/D4-WSJ-MUSIC` @ `05339c191` merged in
(none was an ancestor of `992687ebc`; checked with `git merge-base --is-ancestor`).

L-24 = (a) (Mariano 2026-10-26): make the AudioEngine stand-in faithful by data. This step wires the sibling tables into
the race behind ONE default-OFF knob. No default is flipped (the flip is a decision row).

## Knob

`MASHED_AUDIO_WIRE=1`, default OFF, read once (`GetEnvironmentVariableA`). With it unset every changed call site takes
its pre-change path. With it set (all of W1..W8):

| # | what | source |
|---|---|---|
| W1 | permdict wave table in code: 43 rows, name, voice count `+0x84`, flag `+0x88`; voice base = running sum of the counts (bases are cumulative at load) | `verify/wsj80e_20261026/wave_tables.tsv`, 80E RESULT §check (sum 139 = `DAT_0068fc84`) |
| W2 | virtual-voice model for the 139 permdict voices. A voice is busy while its one-shot plays (PCM length / start frequency, QPC clock) or while a loop runs. `FUN_00465ca0` first-free scan (base..base+count-1, first not busy, freq 22050 `0x46ac4400`, vol 1.0 `0x3f800000`, `INC [0x008aa2d0]` for wave 0x1c), `FUN_00465940` explosion scan (voices base+4..7, pitch k*2000 `0x005cd0b8` + 22050 `0x005ce6b8`, k = 4,3,2,1), fixed-slot start `FUN_00465c10` / `FUN_00465e80` (voice base+slot, only if free) and stop `FUN_004661a0` | `dis_465ca0.txt` (SFXMAP raw), `decomp/00465940.c`, SFXMAP CSV |
| W3 | voice-budget guard, `DAT_0068f4c8` PINNED to 1 (every REF capture had 1), `DAT_008aa254` = the port's participant count (`FUN_0040e340` = `DAT_008a94d0`). Closed (pin != 0 and count > 1): `fire` (wave 0x20) is rejected in W2's scan (`0x00465ca0..0x00465cb7`) and every impact one-shot is skipped (`0x00464671..0x00464695`) | SFXMAP RESULT §2 |
| W4 | powerup waves as the SFXMAP table: GUN `machineg`; DRUM `rocket`; MISSILE `rocket` + `missile exhaust` (fixed slot); P_MINE `drop mine`; SHOTGUN `shotgun`; FLASH `flash`; MORTAR `rocket` + `mortarexhaust`; OIL `oilslick` (fixed slot = owner); R_FLAME arm `flame thrower` (fixed slot = owner), deact stop | SFXMAP CSV rows `powerup,*` |
| W5 | pickup collect plays `collision with pod` through W2 (the port's `PK_Sfx` no-op, `Gameplay/PickupTick.cpp`) | `0x00458a26` |
| W6 | audio-side copy of the `0x007e9de0` event ring: type 3 pushed when `VehicleContactHistoryUpdate` (`0x00469aa0`) returns 1, type 4 at the existing car-car push site (`VehiclePhysicsRun.cpp`, same mode test). Consumer = `FUN_004648b0` + `FUN_00464670` (both voices free, vol = max(`FUN_0046d6d0` car, other) or override + 500.0 (`0x005ccd04`), times 1.8181818e-4 (`0x005ce9bc`) for kind 1 else 2.5e-4 (`0x005ce9b8`), clamped to 1.0, pitch 22050 + random [0, 2205)). The ring is audio-only: no sim reader | `decomp/004648b0.c`, `dis_464670.txt`, consts.txt |
| W7 | engine: per car 0..3 (`FUN_00463640`), class from `EngineClassTable.h` via the car's vehicle, voice base+car on `eng<class+1>` and the d-layer voice base+4+car on `eng<class+1>d`, both looped, started when not busy; pitch = rpm * 3.5 (`0x005ce26c`) + car * 200.0 (`0x005ce194`) + row `+0x90`; vol = rpm * 1.0e-5 (`0x005cc990`) + `DAT_00613020[class]`. `RaceSession.cpp:183`'s `EngineStart` gets the player's vehicle | ENGCLASS RESULT |
| W8 | music: no race `cdaudio.rws` stream and no 120 s cap (implies the D4-WSJ-MUSIC knob's branch) | MUSIC RESULT |

Not wired (residuals, reported, not predicted): course-bank waves (skid, tyre roll, impact 3-state: all behind the W3
guard), `carlands`, helicopter proximity, noise burst / character streams, debris lane, splash, copter, mode-6 launchers,
beat-the-bomb, timing beam, per-car state-2 explosion, missile in-flight update / terminal stop, R_FLAME volume update,
the rpm state machine (W7 uses the existing speed norm for car 0 and min rpm for cars 1..3), 3D position (`FUN_0045daf0`).

Not interpreted: the original draws `FUN_00472650` (the shared RNG) in the engine tick (`0x004636ed`, case 0 of the rpm
switch) and in `FUN_00464670` (`0x00464777`, `0x004647a4`). The knob uses an audio-private RNG so it cannot move sim state.

## Arms (standalone `mashed_re.exe` built from this branch, `MASHED_NO_DEPLOY=1`, private root `D:/MashedWT/D4-WSJ-WIRE_root`, every boot under `mashed_lock.py run --label D4-WSJ-WIRE`, explicit PIDs, `MASHED_MUTE=1`, `MASHED_AUDIO_EVENT_LOG`)

- **G0 gates, knob OFF:** `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **G1 gates, knob ON:** the same four arms with `MASHED_AUDIO_WIRE=1` in the environment.
- **A** menu path (the D4-WSJ-MUSIC `run_port.py menu` recipe), knob OFF.
- **B** same, knob ON.
- **C** knob ON, `MASHED_AUDIO_WIRE_SELFTEST=1`: a boot-time self-test of W2/W3/W6 on the loaded permdict (no race needed), one log row per check.

## Predictions (scored in this order)

1. **G0:** G-CTRL `5943c5f1 / b493262b / 09551e6a` and B `3a03670c / 2516d903 / 53778664` (`re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` on this base).
2. **G1:** the same six hashes. The knob reads no sim state into sim code and the ring copy has no sim reader.
3. **A:** the event-log rows equal the D4-WSJ-MUSIC arm A (`verify/wsjmusic_20261026/A_off.audio_events.csv`) in kind / arg / resolved / val order, ms column ignored, with no new row kind.
4. **B:** in the race segment (between `music_state` 2 and the next `music_state` 1):
   - no `music_start` row (no `cdaudio.rws`);
   - one `vstart` row for `threetwoone` on voice 65 and one for `go` on voice 95;
   - per live car c (0..3): one `vstart` on `eng<k>` voice base+c and one on `eng<k>d` voice base+4+c at race start, k = class of that car's vehicle + 1, every start logged with its car and class;
   - zero `vstart` rows on `fire` (voices 116..119) and on any impact wave (perm 9..13, voices 40..59), with `guard_closed` rows counting the rejected requests when any occur;
   - every powerup / pickup start that occurs names the W4 / W5 wave (no `shotgun` from GUN, no `drop mine` from DRUM, no `explosion1` from MORTAR). Counts are not predicted: the menu recipe drives no powerup in MUSIC arm A.
5. **C:** every self-test row PASS: first-free picks base, base+1, ... then rejects when all `count` voices are busy; `fire` rejected with participants 4 and accepted with participants 1; the explosion scan assigns pitches 30050 / 28050 / 26050 / 24050; a type-3 ring event plays `impact with barrier` on voice 40+car only when the guard is open.

## Verdict mapping

- GREEN: 1..5 hold.
- RED: 1 or 2 fails (a default gate moved), or 3 fails (knob OFF changed the log).
- Any miss in 4 or 5 is reported per row with its cause; a miss caused by a residual listed above is not a RED.

Raw data: `D:/MashedWT/D4-WSJ-WIRE_data/` (event logs, gate step files, self-test log), SHA-256 in the RESULT.

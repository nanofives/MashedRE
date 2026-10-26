# RESULT D4-WSJ-REF: original-side audio event log vs the AudioEngine stand-in

**Verdict: GREEN (measurement committed, one decision filed).** Branch `orch/D4-WSJ-REF` off `orch/integration` @ `d55d89c48`.
Decision L-21 (Mariano, 2026-10-26) said measure first. This is the measurement, and the follow-up architecture decision
(stand-in by data vs RWS pool / voice scheduler port, G8) is filed on the new row `D4-WSJ-ARCH`.

Scoping source: `verify/d4scope_20261026/SCOPING_D4-WSJ.md` (gaps G3 G5 G6 G7, `AudioEngine.h:84-85`).
Full per-wave tables: `verify/wsjref_20261026/ref_tables.md`.

## What was run

| Run | Side | Recipe | Result |
|---|---|---|---|
| o_train1 | original | `scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --hold 110 --audio-probe` | 12026 rows, 0 dropped, 3 race segments |
| o_train2 | original | same, probe with `.rws` open/read hooks | 10546 rows, 0 dropped |
| o_pu1 | original | the d3close `m1r` argv (`--statediff-drive --statediff-puhook --puhook-contacts --puhook-aim --pu-plan 7,9,11 --pu-warm 60 --hold 110`) + `--audio-probe` | 19228 rows, 0 dropped |
| p_train1 / p_train2 | port | `run_port.py` track-view: base_env Training, rule 0, mode 10, deterministic, 6600 frames | 6 rows each |
| p_menu1 | port | `run_port.py menu`: D2BASE menu path (RACE_DEMO, GOTO=6, TRACK_SEL=12, CAR_SEL=0, DRIVE_HOLD), all cars, 110 s wall | 13 rows |

All original boots ran on the private root `D:/MashedWT/D4-WSJ-REF_root` (robocopy of `original\`), stock original, no hooks
installed (`MASHED_RE_NO_AUTO_HOOK=1`), every boot under `mashed_lock.py run --label D4-WSJ-REF`. Port runs are `MASHED_MUTE=1`
like every arm.

## Tooling (logging only, committed in 13733510e)

- `re/frida/scenario_launch.py --audio-probe <prefix>`: a second Frida script (AUDIO_AGENT) on the same session. It logs:
  - every call into `FUN_005a66d0(voice, sel)`, the voice start/stop. sel != 0 is start per `re/analysis/bucket_audio_00465c10_005a7b50/005a66d0.md`. The data agrees: the music voice 138 is only ever called with 0 in the race, and starts come in once per event.
  - the gameplay callers `FUN_00465ca0` / `00465c10` / `00465e80` / `004661a0`, `FUN_004656e0` (character voice), `FUN_005a8960` (stream) and `FUN_005b92e0` (physical source).
  - `CreateFileA/W` on `*.rws`, and `ReadFile` on those handles.
  - Each row carries the race tick `DAT_007f101c`, phase `DAT_00771968`, state `DAT_0063ba8c`, the voice index in `DAT_0068f640` and the return address.
- The wave descriptor tables are dumped raw and decoded from bytes:
  - permdict at `0x006044f0`, stride 0x94, 43 rows (`DAT_0068fc88` = 43).
  - course at `*0x0088e670` = `0x006085c0`, stride 0x90, 12 rows for TRAINING (`DAT_00690478` = 12).
  - Name = C string at +0, voice base = s32 at +0x80, voice count = s32 at +0x84. Confirmed by the data: the bases are contiguous 0..138 for permdict (`DAT_0068fc84` = 139), then 139..173 for the course bank.
- `mashedmod/src/mashed_re/Audio/AudioEngine.cpp` `MASHED_AUDIO_EVENT_LOG=<path>`, **default OFF**. It writes one CSV row per `SfxPlay` / `PlayLoop` / `PlayOnce` / `EngineStart` / `EngineStop` / `MusicStart` / `MusicSetState` / `MusicBankLoop` / `StopAll`, at the API entry and before the device check. Logging only, nothing played changes.
- `re/tools/audio_ref_summary.py`: the per-wave and compare tables. `verify/wsjref_20261026/run_port.py`: the port arms.

## Gates on this exe (knob OFF), unchanged

exe `ac7033d8` (`mashedmod/build/mashed_re.exe`, built `MASHED_NO_DEPLOY=1`), `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`:

- G-CTRL: `5943c5f1 / b493262b / 09551e6a`.
- B: `3a03670c / 2516d903 / 53778664`.
- Both equal `merge_train.json` `gctrl.ref` / `b.ref`.

## REF numbers: how far apart they are

Race-phase (phase 3) voice starts per wave, original vs the port's requests:

| wave (bank idx) | voices | o_train1 | o_train2 | o_pu1 | original caller (ret into the trigger) | port |
|---|---|---|---|---|---|---|
| eng2 (perm 2) | 8..11 | 4 | 4 | 4 | `0x0046375c` (FUN_00463640) | **eng1** ×1, player only (`RaceSession.cpp:183`, skipped when muted) |
| eng2d (perm 3) | 12..15 | 4 | 4 | 4 | `0x00463777` (FUN_00463640) | none |
| helicopter (perm 14) | 60..62 | 3 | 3 | 3 | `0x00464b6b`, tick 0 | none |
| threetwoone (perm 17) | 65 | 1 | 1 | 1 | `0x00463019` | 1 |
| go (perm 26) | 95 | 1 | 1 | 1 | `0x004630d0` | 1 |
| noise burst (perm 39) | 132 | 1 | 1 | 1 | `0x0046316c` | none |
| collision with pod (perm 28) | 100..103 | 2 | 4 | 2 | `FUN_00465ca0` from `0x00458a2b` | none |
| debris (perm 33) | 120..123 | 4 | 4 | 3 | `FUN_00465c10` from `0x00422239` / `0x0042227a` | none |
| rocket (perm 18) | 66..69 | 0 | 0 | 3 | `FUN_00465ca0` from `0x004535f2` | no `rocket` call site |
| mortarexhaust (perm 30) | 108..111 | 0 | 0 | 3 | `FUN_00465ca0` from `0x004535fa` | no call site |
| machineg (perm 19) | 70..73 | 0 | 0 | 4 | `FUN_00465ca0` from `0x004566bd` | no call site |
| explosion1 (perm 8) | 32..39 | 0 | 0 | 4 | voice starts from `0x004659c1` ×3, `0x00464619` ×1 | call sites exist (`TrackRenderer.cpp:5936`, `PowerupEffects.cpp:731`), 0 fired in the port runs |
| **total race starts** | | **20** | **22** | **33** | | **2** logged (threetwoone, go) + eng1 when unmuted |

Other measured facts:

- **Race music (G3).** The original opens no music bank in any capture.
  - The only `.rws` files it opens are `PERMDICT.RWS` (boot, phase 0), `training.rws` (load, phase 2) and `english\RED.rws` ×4 (race start, phase 3, state 2, the 4 character streams).
  - `cdaudio.rws` is never opened.
  - The menu loop `musicloop1` (voice 138) is started in phases 0 and 2. It is then stopped every race frame from `0x00462415` (5560 / 2721 / 6472 calls).
  - The port starts a `cdaudio.rws` stream (120 s cap) on entering the race (p_menu1 row 6, `MusicSetState(Race)`, `GameFlow.cpp:285`).
  - So in this scenario the port plays music the original does not play. [UNCERTAIN] whether other modes or tracks open `cdaudio.rws`: only TRAINING (track 0, mode 10) was captured.
- **Countdown timing.** In every capture, `threetwoone` starts at tick T and `go` at T+91 (58→149, 51→142, 707→798). The port logs 1601.1 / 1654.3 ms (track-view, 2 reps) and 3019.3 ms (menu path) between them. Those are QPC ms under `MASHED_MUTE`, not frames, so they are not directly comparable with ticks.
- **Character voice (G4).** Four `RED.rws` streams are opened at race start, with `FUN_004656e0` taunts at ticks 172 / 475 / 635 (o_train1), 1 (o_train2) and 1 (o_pu1), sent through `FUN_005a8960(s, 2)` from `0x00465781`. RED for all 4 slots is this warp scenario's colour state. The port plays no character voice (`CharacterBankPath` log only).
- **Collision FX (G6).** There are 0 starts of any impact wave (perm 9..12), skid or tyre wave (course 0..6), `carlands` or `bullethitscar` in all three captures. So these captures cannot score G6: they contain no contact event that reaches audio. [UNCERTAIN] whether contacts occurred and were filtered, or did not occur. A contact scenario (`--puhook-contacts` rows exist in `o_pu1.msd.pucontact.csv`) must be paired with these rows to attribute it.
- **Ambience.** The TRAINING course bank `ambience` (course 7) and the other course loops start 0 times in the original.
  - The port's 7 ambience candidates (`RaceSession.cpp:68-69`: windy, wind, sea, heavyrainloop, ambient, rain, loop) match none of the 12 TRAINING course wave names read from the bytes, so the port also plays none.
  - Equal on this track, by different routes.
- **`fire` (perm 32).**
  - Requested 5068 / 3720 / 12520 times through `FUN_00465ca0` from `0x00486f01`. Voices 116..119 are never started in any capture.
  - Voices 116..119 are never started, so they cannot be busy.
  - [UNCERTAIN] which guard in `FUN_00465ca0` rejects it: the game-state gate `FUN_00492d10()==3` or the free-voice test. A one-capture follow-up for SFXMAP.
  - The port plays nothing there either.
- **Win/lose (G7).** Not resolved here: the captures contain no win-vs-lose pair. `menu selection` (voice 64) starts 6 times in phase 1 between rounds (o_train1), from `0x0046322f`.
- **Device state.** The original's audio device is live under the boot patches:
  - `DAT_0069049c` (device) is non-zero.
  - `DAT_006904a0` = 28 voices.
  - `DAT_0068f4c8` = 1.
  - `FUN_005b92e0` physical-source requests: 56 / 43 / 81.

## What this says about G8 (the decision)

- **Every difference measured is data.** The differences are:
  - which gameplay events trigger a start;
  - which wave they use (engine class `eng2` vs the port's `eng1`; no `cdaudio` in the race);
  - how many voices per wave (one engine voice for the player vs 2 per car).
- **None depends on the pool or the scheduler.** Every recorded start lands on a fixed per-wave voice slice whose base and count are in the descriptor bytes (43 permdict + 12 course rows). The peak is 33 starts per race, against 174 virtual voices and 28 physical voices.
- **The only request that never starts is `fire`.** It never starts in any capture, so voice contention cannot explain it.
- **So the stand-in can be made faithful by data:**
  - the wave table with its voice counts;
  - the first-free scan of `FUN_00465ca0` (a leaf);
  - the caller table above for the trigger sites;
  - engine class and per-car engine voices;
  - no race `cdaudio` on this mode.
- **Porting the RWS pool and voice scheduler (G8) would change nothing measured here.**

Recommendation: **(a) stand-in made faithful by data.** Revisit G8 only if a later diff shows a difference that depends on
voice allocation, for example a capture where `FUN_00465ca0` finds every voice of a wave busy.

## Raw data (not committed), SHA-256

Root `D:/MashedWT/D4-WSJ-REF_data/`:

| file | SHA-256 |
|---|---|
| o_train1.audio.csv | ee34c6144418116f2c627813873e57da526a8706b2557e8eb2a0b45c1c26d99a |
| o_train1.audiosnap.json | d8f27afff6d6b422c575e517bff532238313731151d7b868e7cbd118f90fe815 |
| o_train2.audio.csv | 1a516d9c32cda55727e73e05ad1cac642f7080bcb8371f60d46c6a89997230bd |
| o_train2.audiosnap.json | f7b3e65d0d322354a8c6c7c9c597249346184d8857b5973898455fa0335c0c22 |
| o_pu1.audio.csv | da0112e6d297cce3149d29cedf10c55d0a5cb19b9f8f015d14f0f6aa208c2391 |
| o_pu1.audiosnap.json | 6cf0c732857d8da79728113f358a1787ccea9ab6860464007e4a052cab55ab80 |
| o_pu1.msd | ff04686a9a9aa8a576af09a972009ac1d2cdfa12a2556664fb6b746f265f01c1 |
| o_pu1.msd.puhook.csv | 3d6e1f08a03143a0ce30dceb38472260ecbf25cfbce96159a9c0e25041362d37 |
| port/p_train1.audio_events.csv | 02b8175c665a25e7a750c6238834b8de94cb64072441127fef47a6afd809a79d |
| port/p_train2.audio_events.csv | dc8af93d8f5117f9f23c089c0def669039911b91f8c6cc5fd7da707212777799 |
| port/p_menu1.audio_events.csv | e12cb0537c625a858d262b2a2ef55d2db94ed13c5e4ee2cc0554968d87251d4d |
| gates/H_DEF.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| gates/H_DEF.gates.csv | b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f |
| gates/H_DEF.ptrace.log | 09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 |
| gates/B_DEF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| gates/B_DEF_K1.step.csv | 2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6 |
| gates/B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| mashedmod/build/mashed_re.exe (worktree) | ac7033d8e27ed9ffb87a13a836dd43db78f53ec00ef1be4766c1bf86741d6d30 |

Notes:

- o_train1 ran before the probe gained its `.rws` open/read hooks and the `first_race_event` snap fix. Its per-wave rows use the `end` snap, the same tables as the later runs.
- p_train1 and p_train2 differ in SHA-256 only in their QPC ms column.
- The port arms ran muted, so `EngineStart` and the ambience are cited from source (`RaceSession.cpp:165-183`, guarded by `Audio::Available()`), not measured.

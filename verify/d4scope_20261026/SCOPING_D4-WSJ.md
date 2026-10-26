# SCOPING D4-WSJ: Audio remainder (behaviour beyond TU linking)

Text-only. Base `orch/integration` @ `c2bf772f8`. Source line: `ROADMAP.md:2999` (WS-J, "Audio remainder"), dep D4-AUDIO-REST
(merged, `verify/audiorest_20261024/RESULT_D4-AUDIO-REST.md`).

## Boundary with the D4-AUDIO-* rows

The merged rows (D4-AUDIO-RWS/RWS3, -BANKPATH, -VOLTODS(-FLIP), -REST) are all about linking original-shaped bodies into the exe
with no MASHED address in an exe code path. Per the AUDIO-REST survey the five remaining `.asi`-only files (AudioPoolFreeGuards,
TimerGetters_ah5, AudioVoiceQueueSet, AudioListSearch_wfb0f, MixedC3Sweep) are dead code in the exe if linked: nothing in the exe
calls them. So TU linking does not change what the player hears. WSJ rows below do NOT duplicate any of that: none touches a file
in `asi_sources.rsp` that AUDIO-REST ranked, and none re-links a body.

## What the standalone plays today (the real gap)

The exe does not run the original's RenderWare/DSound path. It runs a small stand-in, `Audio/AudioEngine.{h,cpp}`
(header lines 1-10: "minimal real DirectSound playback"). Call sites found in `mashedmod/src/mashed_re`:
11 `SfxPlay` calls (7 menu navigation/selection in `exe_main.cpp`, `go` x2 `TrackRenderer.cpp:5016,5090`, `explosion1`
`TrackRenderer.cpp:5936`, `threetwoone` `TrackRenderer.cpp:7301`), `MusicSetState` x4, `EngineStart/SetRpm/Stop`, one `PlayLoop`
(ambience), `CharacterBankPath` x2 (path builder only). Gaps, each cited:

| # | Gap | Cited source |
|---|---|---|
| G1 | Engine class: `eng1` is used for every vehicle. The vehicle to eng1..eng4 assignment is `[UNCERTAIN]` (not an exe string, not in the vehicle piz, vehicle stat data not RE'd) | `AudioEngine.h:47-50`; `re/analysis/audio_character_banks_REmap_20260616.md:126-128, 159-163` |
| G2 | 0x80E sub-sound directory record format unreversed (~53 B/record, count at +0x20, bank name +0x50). Blocks real per-clip character taunts AND per-track cdaudio music selection | REmap note lines 157-160 (Open item 1) |
| G3 | Race music is one cdaudio decode capped at 120 s, not per track | `AudioEngine.cpp:339-350` (`MusicStart(kCdaudio, 0.45f, 120)`), REmap note lines 140-150 (section 5) |
| G4 | Character voice banks: only the path builder is called (`CharacterBankPath`), no playback of taunts or commentary | `AudioEngine.h:55-70`; REmap note section 2 |
| G5 | Race SFX coverage: permdict holds `rocket`, `drop mine`, `explosion1`, ... but the race only fires `go`, `threetwoone`, `explosion1`. Powerup and mine SFX are not wired to the D3 powerup dispatch | `AudioEngine.h:84-85`; call-site scan above |
| G6 | Collision FX (skid/impact one-shots on real contact) was "blocked on WS-B (real collision events)". The ported collision chain has been the default since D2 closed (`ROADMAP.md:3001` table, WS-A/WS-B rows), so the stated blocker is gone | REmap note lines 164-165 |
| G7 | Win/lose audio treatment unconfirmed in the original: no call site distinguishing them found in `FUN_00466b50` dispatch | `UNCERTAINTIES.md:3638` (U-9017); `re/analysis/audio_music_state_dispatch_20260711.md:78-82` |
| G8 | Full RWS sound-pool dispatch (`FUN_005a7b60` bank loader) and the original's voice/pool scheduling are not what the exe runs | `AudioEngine.h:8-9` |

Not claimed: whether any of G5/G6 matter for fidelity as the user hears it needs a recorded original-side reference, which is
what the first row below produces. Nothing here is a behaviour change until a PREREG and a default-OFF knob exist.

## Steps emitted

| Row | Kind | Gap | Source line |
|---|---|---|---|
| D4-WSJ-REF | frida-boot, solo-free: capture an original-side audio event log (which wave, which bank, at which race tick) on the canonical TRAINING and one powerup scenario, committed as a small table. This is the reference every later row diffs against | G3 G5 G6 G7 | this note, gaps table |
| D4-WSJ-SFXMAP | text-only: map each powerup, mine, collision and countdown event in the default race path to the original's SFX call site (RVA) and wave name, output a wiring table | G5 G6 | `AudioEngine.h:84-85`; REmap lines 164-165 |
| D4-WSJ-80E | Ghidra read (account3 only) + text: decode the 0x80E sub-sound directory record, from bytes, with each field cited | G2 | REmap lines 157-160 |
| D4-WSJ-ENGCLASS | Ghidra read + text: find the vehicle to eng1..eng4 assignment in vehicle stat data, close the `[UNCERTAIN]` | G1 | `AudioEngine.h:47-50`; REmap lines 126-128 |
| D4-WSJ-WIRE | build + game run: default-OFF knob wiring the SFXMAP table into the race, PREREG first, then G-CTRL and B unchanged | G5 G6 | depends on SFXMAP |
| D4-WSJ-MUSIC | build + game run: per-track cdaudio selection and removal of the 120 s cap behind a knob, after 80E | G3 | depends on 80E |

Dependencies: SFXMAP and 80E and ENGCLASS and REF are independent of each other. WIRE needs SFXMAP and REF. MUSIC needs 80E and REF.
G4 (taunt playback) is folded into 80E's follow-up, not a separate row, because it has the same blocker. G7 and G8 are not given
rows: G7 is a recorded absence with no regression (U-9017 says so), G8 would be an architecture choice (replace the stand-in
with the original's pool) and belongs to a decision, filed once below.

## Decision filed (one)

Scope cut: keep the AudioEngine stand-in as the shipping audio path (rows above make it faithful by data), or port the
original's RWS pool/voice scheduler (G8) so audio matches the original structurally. The stand-in route costs six rows, the port
route is open-ended and has no reference capture yet. Recommendation: stand-in, revisit after D4-WSJ-REF shows how far apart they are.

## Notes for the lead

- `roadmap.ps1 add` cannot set `deps`, `needs`, `solo`, `next`. They sit in each row's `note`. Only the Frida/game-run rows
  would need `solo` if they were D2 arms: none of these are D2 arms, so `solo` is false throughout.
- Ghidra-bound rows (80E, ENGCLASS) cannot run on account2 (`re/ACCOUNT2_CAPABILITIES.md`).

# PREREG D4-WSJ-MUSIC (committed before any arm)

Branch `orch/D4-WSJ-MUSIC` off `orch/integration` @ `86bdd5121`.

## Question
Does the original play race music from `cdaudio.rws`, and does the stand-in's 120 s cap (`AudioEngine.cpp` `MusicStart(kCdaudio, 0.45f, 120)`) have an original counterpart?

## Static evidence plan (original, `MASHED.exe.unpatched`, Ghidra headless read-only)
1. Every string in the image that builds a `toastaudio\pc\audio\pcdics\` path, and every reader of each.
2. Every code reference to the `CDAUDIO` row string at `0x00604170`.
3. The per-frame music path in `FUN_00466b50` (race = dispatch case 3/7) and `FUN_004623e0`.
Prediction: no reader of `0x00604170`, no path builder that can produce `cdaudio.rws`, and the race branch never starts a music voice.

## Knob
`MASHED_MUSIC_NO_RACE_CDAUDIO=1`, default OFF. When ON, `MusicSetState(Race)` stops the music voice and starts no `cdaudio.rws` stream, and `MusicSetState(Results)` starts none either. OFF path is unchanged code.

## Arms (standalone `mashed_re.exe`, `MASHED_AUDIO_EVENT_LOG`, one Menu -> Race entry)
- A (knob OFF): expect a `music_state` row with val 2 (Race) followed by a `music_start` row for `cdaudio.rws`.
- B (knob ON): expect the same `music_state` row and no `cdaudio.rws` start. `MusicStart` is never reached.
Pass = A shows the stream and B does not. No gate is changed by a default-OFF audio knob. The `gctrl.ref` / `b.ref` runs are reported as not run if the OFF path is byte-identical in the diff.

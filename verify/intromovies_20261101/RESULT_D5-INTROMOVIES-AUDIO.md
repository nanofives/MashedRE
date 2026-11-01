# RESULT D5-INTROMOVIES-AUDIO: the boot movies' audio plays in the standalone as in the original

Verdict: **GREEN. U-9799 resolved.** With an audio endpoint present (VB-CABLE 4.5), two original boots and two
standalone boots with the real boot movies were recorded off the loopback. The standalone's movie audio is present,
at the same level (-13.69 dBFS against -13.68 / -13.69 over the 85 s movie span) and with the same envelope
(log-RMS Pearson r 0.996 / 0.997 against the original, 0.988 original against original). Every standalone movie
logs `audio_hr=0x00000000` except renderware, which logs `0x00000001` and is silent in the original too.

Branch `orch/D5-INTROMOVIES-AUDIO` off `orch/integration` @ `0e3d71a75`, LIVING-PC, 2026-11-01. Measurement only:
no game source changed, no knob added, no PREREG needed. Prior result: `verify/intromovies_20261030/RESULT_D5-INTROMOVIES-FLIP.md`
(it filed U-9799 because LIVING-PC had no endpoint and every run logged `audio_hr=0x80040256`).

## 1. Setup

- Exe `mashed_re.exe` built from `orch/integration` `0e3d71a75` with `MASHED_NO_DEPLOY=1` (built in the
  HB-ARCTIC-NOLIGHT-CAP worktree, same commit), SHA-256 `19e494fe89cea5116850c6420fd2eadee808661f02313c8f258c5fe7cdfe5915`.
  No `MASHED_*` variable set: `MASHED_INTRO_MOVIES` is ON by default (D5-INTROMOVIES-FLIP), `MASHED_MUTE` not set
  (it gates `Audio/AudioEngine.cpp:215` only, not the movie graph).
- Original `<root>/original/MASHED.exe` SHA-256 `b9977dab...79fe87` (the patched install, equal to the main
  `original/MASHED.exe`), launched with cwd `<root>/original`.
- Private root `D:/MashedWT/D5-INTROMOVIES-AUDIO_root` (robocopy of `original\`). `run_audio.py` copies the five real
  movies from `movies\backup\` over the 167,414-byte skip files in the root only (sha16 empire `F89F7D92D51A4184`,
  supersonic `33A6DE9296B65F87`, renderware `177DDB4D65223B09`, intro `DBB08D35C970109C`, small `E709EF15FDED167C`).
- Recorder `rec_wave.py`: standard library only (ctypes + winmm MCI, no package). It records the default capture
  device, `CABLE Output (VB-Audio Virtual Cable)`, the loopback of the only render endpoint
  `Altavoces (VB-Audio Virtual Cable)`. Self-test: a 2 s 1 kHz tone played through `winsound` reads 21 of 40
  100 ms windows above 0.01 RMS (PASS, `selftest/`).
- Each launch: `Get-PnpDevice -Class AudioEndpoint -PresentOnly` immediately before it (all four OK, VB-Audio listed,
  `run_audio_log.txt`), recording from 3.3 to 5.5 s before the launch to 115 s after it, then the spawned PID killed
  (only that PID, all four alive at the wall). The whole script ran inside one `mashed_lock.py run` hold, so no other
  game booted during a recording. Pre-launch noise floor 0.00001 RMS on all four.

## 2. Results (`audio_score.txt`, `analyze.py`)

| arm | first sound after launch | sounding segments, s after launch (>= 1 s) | movie-span RMS |
|---|---|---|---|
| O1 original | 4.6 s | 4.6-11.9, 12.4-21.4, 30.1-93.4, 99.1-115.1 | 0.2068 (-13.69 dBFS) |
| O2 original | 4.3 s | 4.3-21.0, 29.7-92.9, 98.4-115.1 | 0.2069 (-13.68 dBFS) |
| S1 standalone | 5.7 s | 5.7-22.5, 31.2-94.5, 95.2-115.1 | 0.2068 (-13.69 dBFS) |
| S2 standalone | 2.5 s | 2.5-19.3, 28.1-91.3, 92.0-115.0 | 0.2067 (-13.69 dBFS) |

Envelope correlation (100 ms log-RMS, 85 s from each arm's first sound, best lag in +-10 s):

| pair | r | onset-aligned residual lag |
|---|---|---|
| O1 vs S1 | 0.997 | +0.0 s |
| O1 vs S2 | 0.996 | +0.0 s |
| O2 vs S2 | 0.988 | +0.1 s |
| S1 vs O2 | 0.990 | -0.1 s |
| O1 vs O2 (control) | 0.988 | -0.1 s |
| S1 vs S2 (control) | 0.997 | +0.0 s |

Standalone log, `INTRO movie` lines (PREREG bands 0.85..1.35 x `DUR` of `verify/intromovies_20261028/run_intro.py`):

| movie | S1 ms | S2 ms | band ms | audio_hr S1 / S2 |
|---|---|---|---|---|
| 1 empire | 8000 | 8266 | 6613-10503 | 0x00000000 / 0x00000000 |
| 2 supersonic | 9703 | 9703 | 8058-12798 | 0x00000000 / 0x00000000 |
| 3 renderware | 8187 | 8219 | 6834-10854 | 0x00000001 / 0x00000001 |
| 4 intro | 63922 | 64016 | 54170-86036 | 0x00000000 / 0x00000000 |

All 4 complete, in band, then `INTRO small.mpg: BUILT (graph built, not run)`.

Reading:
- The audio plays. Both sides sound for the same spans: about 17 s (empire + supersonic), a silent gap of about 8.7 s,
  then about 63 s (intro). The gap is renderware: `audio_hr=0x00000001` is `S_FALSE` from
  `MpegVideoTexture.cpp:184` (no unconnected output pin left after the video, `ntodo == 0`), and the original is silent
  over the same span (O1 21.4-30.1, O2 21.0-29.7).
- In step: the envelopes correlate at r 0.996-0.997 against the original with zero residual lag once the first sound is
  aligned, at or above the original's own repeat (0.988). Level is equal to 0.01 dB.
- Onset differs by run (2.5 to 5.7 s after launch on the standalone, 4.3 / 4.6 s on the original). That is boot time
  before the first movie, not movie audio.

## 3. Not in scope, measured anyway: frontend sound start after the intro

After the intro audio ends, the original is silent for 5.7 s (O1 93.4 -> 99.1) and 5.5 s (O2 92.9 -> 98.4) before the
next sound. The standalone is silent for 0.7 s (S1 94.5 -> 95.2, S2 91.3 -> 92.0). This is the frontend path after
`FUN_00495350`, not boot-movie audio, and this step does not attribute it. [UNCERTAIN] what the original does in those
~5 s (load time or a wait) is not measured. Proposed row for the lead: measure the time from the end of `intro.mpg` to
the first frontend sound and to the first frontend frame on both sides.

## 4. Trackers (re-classify skill)

- **U-9799** resolved with `re/tools/uncertainty_resolve.py resolve` (CHANGELOG line written by the helper). Its path
  to resolution (an I1-style run with the real movies on a machine with an endpoint: `audio_hr` per movie, each movie in
  its band, a loopback capture) is met by section 2.
- No `hooks.csv`, `STUBS.md`, `DEFERRED.md` change.

## 5. Gates

Not run: no game source changed on this branch (scripts and evidence only), so the default exe is the `orch/integration`
exe and its gates are the merge-train references.

## 6. Raw data (not committed), `D:/MashedWT/D5-INTROMOVIES-AUDIO_data`

```
84fcb8f58d0f5c78ac0acd8d25894a3d9666188254d6136871512790fd702587 *arms/O1.wav
40afb3bc602344b639c78d527c65c7bd2fb91ce5c9a295a597e48fb6e6783f4e *arms/O1.meta
beadac8797fd038730edc78e60f762980e5bd1aa7e5a881f7922601f31341d7b *arms/O2.wav
c7e5b0d8ed5c44f085c6b10e88c3aea1df97d78c00728534a46b99e8cd698dd8 *arms/O2.meta
a1a19ddf8a6b9e7c9fd483f8ea2ea1fb87c0445e569b265f5a360e075e3065fe *arms/S1.wav
ace75c15ba83fbe98e8b278d978e0143022afa914e2ed0b6cd76e0a1e14d3c62 *arms/S1.meta
cb8c3ec1d04deb3c586b911b24159244f9963463ad261bd12be95530f7ee0c3a *arms/S1.log
bd68e622bf9ce272a8c3451e351fba4f2496bfa585c3214c1830dee04541e864 *arms/S2.wav
98f6f9ceb382662d4d3838e4b8a312d289efafca92013b8270b667fd63863fe4 *arms/S2.meta
398ae042f5fad3e326126069a2a587f1c2fbc41e58ce2936a8ae1ee63eaaabe2 *arms/S2.log
4dcbc70af79da83dc979e1fd1e56c6d4890c91fcb90d5cef83a8fefa151f2a3c *analyze.txt
ed1f3d9da35aced846d347253e597816ab737405e765e3ea7b77173d8949b910 *run_audio.log
e3a4a9342d41cb5bdfcbb71dbc08d2222313f6a2eb3b0b86414029dde5a8df69 *selftest/selftest_rec.wav
66a2e32b9f95051448856d159d1a8805511a895d3787ec5ece5db5dc7ac90d31 *selftest/tone_1k.wav
```

`audio_score.txt` here is `analyze.txt` plus the movie-span RMS lines. Scripts: `rec_wave.py`, `run_audio.py`, `analyze.py`.

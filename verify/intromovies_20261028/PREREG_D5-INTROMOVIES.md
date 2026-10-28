# PREREG D5-INTROMOVIES (D-11062, L-45): boot movies through DirectShow

Committed alone, before any arm runs. Branch `orch/D5-INTROMOVIES`, code commit `9c07e7b08`
(exe built from it: `mashed_re.exe` sha256 prefix `265732d019c9cbc9`).

## Change under test

`MASHED_INTRO_MOVIES=1` (default OFF) runs `RunBootIntroMovies()` in `exe_main.cpp` after
`InitDirectInput` and before the menu music starts. It ports:

- `FUN_00495350`: movies 1..4 of the path table `0x006147c4` (empire, supersonic, renderware,
  intro), each played to `EC_COMPLETE` (`FUN_004942b0`) or skipped on a rising edge of the
  action-4 byte of any cooked slot (`0x007f1042` / `0x007f1502` scan, 4 slots). Teal clear
  `{0x00,0x80,0x80,0xff}` (`FUN_004c1bb0`), one full-backbuffer quad, UV extent
  video/texture size (`0x0049545a`..`0x0049546f`), `Sleep(5)` per frame.
- `FUN_00494c80`: the small.mpg graph, built, never run, released at exit.

Knob OFF executes none of it. The frontend.mpg backdrop keeps `MpegOpenOptions` defaults, so its
graph, texture and loop logic are unchanged.

## Where the test runs

Private root `D:\MashedWT\D5-INTROMOVIES_root` (robocopy of `original\`, root contains `original\`).
The real movies are copied into the root's `original\toastart\pc\movies\` from the shared
install's own `original\toastart\pc\movies\backup\` (the skip_intro backup). The shared
`original\` is read only: `patch_mashed_skip_intro.py --restore` is NOT run. The RESULT records the
SHA-256 of the five shared `.mpg` files before and after the arms (expected: all five
167,414 bytes, `A0E085D46D222763...`, unchanged).

Real movie facts (read from the files, not the exe):

| file | bytes | sha256 prefix | container | duration |
|---|---|---|---|---|
| empire.mpg | 4,751,364 | F89F7D92D51A4184 | MPEG-1 PS | 7.78 s (SCR span) |
| supersonic.mpg | 6,445,060 | 33A6DE9296B65F87 | MPEG-1 PS | 9.48 s (SCR span) |
| renderware.mpg | 4,463,728 | 177DDB4D65223B09 | MPEG-1 video ES, 640x480 | 241 pictures = 8.04 s at 29.97 |
| intro.mpg | 37,072,900 | DBB08D35C970109C | MPEG-1 PS | 63.73 s (SCR span) |
| small.mpg | 24,580 | E709EF15FDED167C | MPEG-1 PS | 1.83 s (SCR span) |

## Arms and pass criteria (all must hold for GREEN)

| arm | env | pass |
|---|---|---|
| G | knob OFF: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL and B hashes equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref` |
| I0 | knob OFF, boot 25 s wall | no `INTRO` line in `mashed_re.log`, `F1 frontend.mpg: RUNNING` present |
| I1 | `MASHED_INTRO_MOVIES=1 MASHED_INTRO_SHOT=90`, real movies, 140 s wall | 4 `INTRO movie` lines in order idx 1,2,3,4, each `complete`, each `ms` within [0.85, 1.35] x the duration above; `INTRO small.mpg: BUILT`; `F1 frontend.mpg: RUNNING` logged after the 4 lines; 4 shots written, each with under 50% of pixels equal to the teal clear and a per-channel stddev above 0 |
| I2 | `MASHED_INTRO_MOVIES=1 MASHED_INTRO_PRESS=30`, real movies, 60 s wall | 4 lines, each `skip-test frames=30`; small.mpg BUILT; frontend RUNNING |
| I3 | `MASHED_INTRO_MOVIES=1`, the 167,414-byte skip files in the private root | 4 lines `complete`, each `ms` under 3000; small.mpg BUILT; frontend RUNNING |

Not claimed by any arm: audio output of the movies (this machine has no audio endpoint, which is
also why the original does not boot now). `audio_hr` is logged and reported as is. No
original-side capture is made or needed: the comparison is static (decomp + disassembly cites in
the code comment) plus the file facts above. A RED on any row is reported, not re-scored.

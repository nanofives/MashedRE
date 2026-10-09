# RESULT: U-9201 (the phase-3 camera target)

Date 2026-10-09. Pre-registration: `PREREG_U9201.md`. §0-§2 were committed at `4f9f4f2f` before
the captures. §3 (step-1 result) and §4 (step-2 predictions) were committed at `7a9d8a8e` before the
replay knob existed. **No gate-path code changed.** The replay knob and the twin trace are diagnostic,
and G-CTRL is byte-identical on this build (`c741a4c5` / `642af0ac` / `05984e30`).

## 0. Verdict first

> **The phase-3 length is set by the system clock, and the port reproduces it once given the same
> clock. The camera residual that remains is exactly the cars' grid placement.**
>
> - **H1 holds (step 1).** The camera's sway input `DAT_007f1030` is `QueryPerformanceCounter` rescaled
>   to 3 MHz, low 32 bits, written each frame by the main-loop timer `FUN_00493390` (`0x00493459`, via
>   `FUN_004950b0` -> import `0x005cc0c4`). Two original runs entered phase 3 at different clock values
>   (1,689,010,897 vs 1,925,953,538), with first camera targets 0.25 apart, and phase 3 lasted 690 vs
>   740 game frames (o_cadence_nopress: 652). **The original's phase-3 length is not reproducible across
>   runs by construction.** The validated entry model reproduces all three runs from their own targets
>   (650/651, 689/689, 738/739).
> - **H2 as registered FAILS on Q1/Q2, and PASSES on Q3.** With the run's own tick series replayed into
>   `RaceCamera`:
>   - Q3: phase 3 lasts **693 vs 691 +-3** (r1) and **742 vs 741 +-3** (r2). PASS.
>   - Q1/Q2: the camera target is within 0.05 from frame ~60 on, but is 0.089 / 0.098 off at frame 0,
>     a decaying transient. FAIL as worded.
> - **G-CURVE, re-scored against the run being replayed, PASSES within 1 %** for both runs
>   (ratios 1.002..1.009 at f60..f600). The leg-2 single-run reference was ill-posed.
> - **The steady residual is the cars' grid placement.** From frame 60 on, port - orig = (+0.0118,
>   +0.0403, -0.0004) at every checked frame, in both runs. That is the midpoint shift of the camera's
>   car pair (0, 3) to 4 decimals: (+0.0118, +0.0404). Every port car sits **+0.040 higher** on the grid
>   than the original, and x shifts by +0.008 per car index (U-9202).

## 1. The original's time base, from the bytes

| site | instruction | effect |
|---|---|---|
| `0x00493392` | `call 0x004950b0` | `QueryPerformanceCounter` (`0x005cc0c4`) x 3,000,000 / frequency (`0x00771e70/74`), low 32 bits in EDX |
| `0x00493459` | `mov [0x007f1030], edx` | the camera sway input (main block reads it / 60000) |
| `0x0049344b` | `fstp [0x007f1010]` | x 3.333e-7 (`0x005ccd68`), the same clock in seconds |
| `0x004933d5` | `mov [0x007f1000], 0x32` | frame units 50 |
| `0x0040fdde..0x0040fe24` | `FUN_0040fc00` | adds to both only when `DAT_007f1000 > 0x3c` (not on a 50-unit frame) |

The counter wraps every 1431.7 s. The port's `cam_ticks_` starts at 0 per race and advances 50,000
per frame, which matches the rate at exactly 60 fps but not the phase.

## 2. What refuted the other named contributors (RESULT_LEG2 §2)

- **"The original runs the camera in states 0-2."** It does not. `cam+4..+0xc` is 0 there in the
  original too, and `cam[7]` is 0 throughout.
- **`FUN_00442a20`.** It is render-only: during phase 3 the rendered view is entry 0's matrix, copied
  into the RW camera frame. It never writes `cam+0x40`.
- **dt = 0 on the first two port calls.** This is not separated here. It is a candidate for the
  frame-0 transient (U-9203).

## 3. Open (filed)

| id | item | next step |
|---|---|---|
| U-9202 | port grid placement: all four cars +0.040 in y, and x +0.008 x car index, vs the original (`o_leg1b`) | a D2-domain spawn / rest-height question; locate the y writer before the first physics frame |
| U-9203 | camera transient over the first ~60 phase-3 frames (0.09 at f0; port runs about one frame ahead early) | compare the original's two first-frame camera calls with the port's (dt 0, reset path) |
| knob default flip | unchanged: user decision. The phase-3 length cannot match "the" original by design | (b) scoring question first |

## 4. Raw

`PREREG_U9201.md`, `u9201_decomp.txt`, `o_u9201_r{1,2}.{csv,log}` (`orig_rampwatch.py --cam --u9201
--hz 60`), `u9201_replay_r{1,2}.txt`, `U9201_RUN.txt`, `L2_R{1,2}.*`, `L2_CTRL.*`, `U9201_SCORE.txt`,
`U9201_CURVE.txt`, `score_u9201.py`, `leg2_model.py` (now game-frame indexed). Every PID was its own
and exited on its own; the capture tool adopts only the PID it launched.

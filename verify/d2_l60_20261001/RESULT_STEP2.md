# D2 attempt 13, STEP 2 — the upstream trace. **Every cross-side band above 100 speed in this lane is OFF-REGIME, and four reported "defects" are one artefact.**

Step 1 left `l_60` diverging 2.15x at 100-150 with a verified attribution, which
is what unlocks step 2: trace `l_60`'s inputs upstream to the first diverging
term by RVA, and **test the hypothesis on the running original before coding**.
The first candidate came from step 0 and the test refuted it — then refuted
three more things with it. Read-only throughout; **no game was launched for any
number in this section**.

## 2.1 The candidate, and the disassembly that made it testable

Step 0's one outside-scope row was record **`+0x1a8`**, the steer angle, written
by A4 `0x00470670` (and never by A6a). Disassembling A4 — 431 instructions,
`0x00470670..0x00470c6f` — gives **two** `+0x1a8` stores, `0x00470835` (branch A,
`in[0]`) and `0x004708fa` (branch B, `in[1]`, with `fchs` at `0x004708f8`), plus
the entry zeroing at `0x004706cd`/`0x004706d3`. Both branches compute the same
shape. Note `0x00470841 fst [edi + ecx*4 + 0x1ac]` — a dword-index store off a
computed base, which an offset grep cannot see (memory
`offset-grep-misses-dword-index`).

Branch A, by RVA, with every constant read out of `MASHED.exe.unpatched`
(hex first, memory `plate-hex-gloss-authoritative`):

```
0x004707b3  movzx eax, byte [ebp]            ; in0, the steer byte 0..255
0x004707bb  fild  [esp+0x10]
0x004707bf  fmul  [edi+0x190]                ; per-vehicle steer scale
0x004707c5  fmul  [0x5ceaa8]  0000803b       ; 0.00390625 = 1/256
0x004707cb  jne   0x4707d5                   ; esi == 7 -> ST0 = 0 (0x5d757c)
0x004707db  fmul  [0x5cc32c]  0000003f       ; 0.5
0x004707e5  fmul  [0x5cc950]  0000403f       ; 0.75, only if [edi+0xbf0] != 0
0x004707eb  movzx eax, byte [ebp+5]          ; in5
0x004707f7  fcomp [0x5cc9d0]  00000043       ; 128
0x00470804  fmul  [0x5cc348]  0000c03f       ; 1.5          <- in5 > 128 branch
      else
0x0047080c  fild  [edi+0xb24]                ; the RAMP COUNTER
0x00470812  fcom  [0x5ceaa4]  0080bb45       ; 6000, clamp the counter to it
0x00470827  fadd  [0x5ceaa4]                 ; + 6000
0x0047082f  fmul  [0x5cea58]  3ec32e39       ; 1/6000
0x00470835  fst   [edi+0x1a8]
```

> **`ramp = (min(+0xb24, 6000) + 6000) / 6000`, which is 1.0 at counter 0 and
> saturates at exactly 2.0 at counter >= 6000.** With `in0 = 255` that makes the
> steer angle `255/256 * 0.5 * p190 * ramp`, i.e. it **doubles** over the
> counter's first 6000 units. Branch B is identical through `+0xb28`.

So `+0x1a8` is a quantity that ramps **in TIME**, not in speed.

## 2.2 The test on the running original — and the candidate is REFUTED

Both sides' `+0x1a8`, read from the captures that already exist (the original
from `orig_fp2.msd`, the port from `a6a_dump.log`'s `snap.steer`, which
`VehiclePhysicsRun.cpp:1304` prints as `F(r,0x1a8)` — the same offset):

| | first steering frame | value there | increment / frame | saturates at | saturated value |
|---|---:|---:|---:|---:|---:|
| **ORIGINAL** | 884 | **17.07471** | **+0.141113** | frame **1003** (the 120th) | **33.86719** |
| **PORT** | 2 | **17.07471** | **+0.141113** | frame **121** (the 120th) | **33.86719** |

The original's `+0xb24` steps **50 per frame** (50, 100, 150, 200, …) and
crosses 6000 on the 120th steering frame; `+0xb28` is 0 throughout. The
original has **120 distinct** `+0x1a8` values and the port's ramp reproduces
every one of them.

> **WITHDRAWN: step 0's `+0x1a8` row.** The two sides' steer ramps are identical
> in start value, rate, length and saturated value, to every printed digit. The
> band medians differed because the two cars are at **different times** when
> they are at a given speed.

## 2.3 The same artefact kills §21.10's front-axis defect

§21.10 reported the PORT's front wheel-axis deflection as **10.0% / 15.4% /
20.8% short and speed-DEPENDENT** against the ORIGINAL's flat `-33.867`, called
it "a genuine fidelity defect worth its own fix" worth **10.9%** of the `ld4`
gap, and left **[U-9156]**'s "its writer is not located" open. It scored the
port's deflection against the **original's saturated 33.867**, because its port
steer column was `motion_diag`'s `steer=` **input byte** (+1.000), not an angle.

Scored instead against the **port's own `+0x1a8` on the same frame**, the
invariant `(front − rear) + steer == 0` holds on both sides:

| side | n | per-frame residual, median | p05 | p95 |
|---|---:|---:|---:|---:|
| ORIGINAL | 1448 | **+0.000002** | −0.000000 | +0.000004 |
| PORT | 1626 | **+0.000002** | −0.000000 | +0.000004 |

And the mechanism is visible in the median frame index per band — the port is
at frame **22** in the 260-500 band (steer 19.897, still ramping) where the
original is at frame **1102** (steer 33.867, saturated):

| band | ORIG medFrm / steer / front−rear | PORT medFrm / steer / front−rear |
|---|---|---|
| 100-150 | 1017 / 33.867 / **−33.867** | 104 / 31.468 / **−31.468** |
| 260-500 | 1102 / 33.867 / **−33.867** | 22 / 19.897 / **−19.756** |
| 1500-2000 | 1556 / 33.867 / **−33.867** | 72 / 26.953 / **−26.812** |

> **WITHDRAWN: §21.10's "the port's front-axis deflection is 10.0/15.4/20.8%
> short and speed-dependent", and the 10.9% of the `ld4` gap it was credited
> with. The port's wheel axes follow its own steer angle EXACTLY.** U-9156's
> "[UNCERTAIN] its writer is not located" is moot: there is no defect to locate.

## 2.4 The general result — there is NO like-for-like data above 100 on this arm

Regime = steer saturated (`+0x1a8 >= 33.8`) **and** speed `>= 100` **and**
grounded:

| | frames | steer saturates at | frames at/after saturation | of those, speed >= 100 | median speed after saturation | max |
|---|---:|---:|---:|---|---:|---:|
| **ORIGINAL** | 2332 | 1003 | 1329 | **1329 (100.0%)** | **1847.0** | 2562.5 |
| **PORT** | 1627 | 121 | 1507 | **5 (0.3%)** | **22.2** | 118.6 |

> **The two sides share exactly FIVE frames of common regime, at median speed
> 103.9 on the port against 1847.0 on the original.**

Therefore **every cross-side band comparison above 100 speed in §21.9, §21.10,
§25.3 and attempt 13 step 1 compares the ORIGINAL at full lock and high speed
against the PORT during its 120-frame steer ramp.** That includes:

- §21.9's `ld4` **2.71x**, `le4` 1.20x, `grip*speed` **1.84x**;
- §21.10's front-axis 10-20.8% and the `+0x9e8` ratios;
- §25.3's per-band net table;
- **this attempt's own step 1 `l_60` 2.15x..1.11x table.**

None of them is a measurement of an implementation difference. They are
measurements of two different moments that happen to share a speed. Memory
`a-band-scored-off-regime-is-not-a-measurement` and
`band-scored-off-regime-is-not-a-measurement`, and this is the largest instance
of it in the project.

**This does NOT retract step 1's two hard results**, which do not depend on
banding: the **call-site attribution** (frame −96, four accesses, the two sites
`a8_l60.py` used, known-answer 1424/1424) and the **arm selection** (the
original's `grip × |vel|` is below the 32768 knee at 100-150, so it is on the
LOW arm where `k` can never be 0 — §25.3's `l_60 >= 79 240` stays withdrawn).
What is retracted is the *cross-side ratio*, which cannot be read off this arm
at all.

## 2.5 What D2's defect actually is, restated with numbers

The port reaches **1831.5** peak speed during its ramp — the original reaches
**1815.4** — and then collapses to a median of **22.2** and never again exceeds
100 for more than 5 of 1507 frames. The original reaches **1847.0** median and
stays there for 1329 of 1329 frames.

> **D2's defect is the collapse, and it happens inside the port's first ~121
> frames.** Clamp #6, `l_60`, `ld4`, the wheel axes and the steer ramp are all
> downstream scenery: the port's steer ramp is bit-identical, its wheel axes are
> exact, and its clamp arithmetic was proven byte-faithful in §21.5 and re-read
> instruction by instruction in this attempt's `PREREG_STEP1.md`.

This is §21.5's residency finding and §20.14's "the sides separate over the ~5
frames after the first bounce", now with a hard count attached.

## 2.6 The instrument fix, committed

`re/tools/statediff/collateral.py --mode banded` now prints the **median frame
index per arm per band** beside the median speed, flags any band whose two
median frames differ by more than 50% with `!!`, and prints an explicit
OFF-REGIME warning block. Re-running step 0's cross-side table with it flags
**every band from 100-150 to 1500-2000** — which is the whole table. A band
comparison is only a measurement for quantities that are functions of the
banding variable; for anything that ramps in time it is not.

## Artefacts

`collateral_step2_offregime.{txt,csv}` (the same step-0 table with the guard
live). Scratch analysis scripts were not committed: every number above is
reproducible from `collateral.py`, `a13_l60.py` and the disassembly RVAs cited.

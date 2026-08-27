# D1 — the verbatim race camera, measured for the first time (2026-08-26)

`RaceCamera` (RVAs 0x00446520 / 0x00441820 / 0x0040e180 / 0x00410d10) has run every
frame in the standalone since 2026-06-10 with its pose discarded — `race_cam_.pos()`
and `.target()` have zero call sites in the tree. It had never been compared against
the original. This is that comparison, and it found two defects.

## Method

Offline unit diff. No renderer, no wiring, no game run on the standalone side:

1. `re/frida/camera_probe.py` hooks `FUN_00446520` in the live ORIGINAL and records,
   per frame, the camera's own outputs AND the inputs it was given.
2. `re/tools/camera/cam_driver.cpp` links **only** `RaceCamera.cpp` + `PizReader.cpp`
   (MSVC x86, to preserve x87 behaviour) and drives the port with those same inputs.
3. `re/tools/camera/cam_diff.py` compares them.

This isolates "is the verbatim port faithful?" from "is its output the world camera?",
which is separate and still open (see `verify/d1_carproj/RESULT.md`, and the caveat
below).

## Artifacts

| file | what |
|---|---|
| `camera_trace_v2.csv` | 826 frames of the live original: pose, aim, elev/azim, zoom, pair, plus per-car position/velocity/flags and `track_type`/`overhead`/`mode`/`ticks`/`reset` |
| `camera_probe_static.json` | full 30-node ribbon + the 30-entry LED override table |
| `cam_nodes.txt` | the ribbon, flattened for the driver |
| `port_out_fixed.csv` | the port's outputs after both fixes |

Capture regime, all uniform and inside what the port models: `track_type` 30,
`overhead` 0, `mode` 0 (the port implements the standard path only), every row
`alive == 1` and `dead == 0`, velocity gate `+0x14` zero throughout.

## Result

| metric, 766 comparable frames | before | after |
|---|---:|---:|
| eye position, median / max | 4.4343 / 4.4713 | **0.0001 / 0.0021** |
| look-at point, median / max | 0.0000 / 2.2036 | **0.0000 / 0.0005** |
| aim angle, median / max | 37.27° / 66.56° | **0.0007° / 0.0058°** |
| zoom, median | 0.0001 | 0.0001 |
| most-separated pair, exact | 0.0% | **92.2%** |

## Defect 1 — `0x004a2c48` is `__ftol`, not banker's rounding

`RaceCamera.cpp` implemented it as `std::nearbyint`. `hooks.csv` and the verbatim port
at `Math/FPURound.cpp` both identify `0x004a2c48` as `__ftol`, whose documented
algorithm (CW round, then a residual correction subtracting 1 for positive `x` that
rounded up) is **truncation toward zero**.

Consequence: at `path_prog >= 29.5` on a 30-node ribbon, `nearbyint` yields `node = 30`
— one past the end — and a negative `frac`. The original wraps only `next`, never
`node`, and that asymmetry is correct precisely because truncation guarantees
`node <= count-1`.

Located by a controlled `path_prog` sweep: fine at 29.30, broken at 29.50, exactly
where the two conversions diverge. It hit **591 of 766 frames** because the pre-race
grid sits at `path_prog` 29.18..30.00, right at the wrap.

`Math/FPURound.cpp` had already listed `RaceCamera.cpp BankersRound` as a known-wrong
approximation. The warning was written and the defect shipped anyway.

## Defect 2 — `MostSeparatedPair` out-params swapped

Source-verified against the PE (`re/analysis/race_camera/FUN_0040e180.asm`, dumped
with `re/tools/pedisasm.py`): `0x0040e2b9 mov ebp,esi` (INNER), `0x0040e2bb mov
[esp+0x10],ebx` (OUTER), tail `0x0040e325 mov [eax],ebp` / `0x0040e327 mov [ecx],edi`
— **first out-param is INNER, second is OUTER**. The port had them backwards.

Not cosmetic: `Update` leads only the A side (`mid = (posA+posB)/2 + velA*k`, the `sep`
leads cancelling), so a swap moves the target by `k*(velA - velB)` — invisible while
the cars are stationary, 0.22..0.45 at racing speed. Measured tail residual was 0.231.

Note `0x0040e180` remains correctly C4: that row is the HOOKED copy in
`CameraClusterHooks.cpp`, which had the order right and whose own comment (line 16)
documents the divergence from the standalone. Two live implementations of one RVA,
one verified and one wrong, with the disagreement written down and shipped.

## One wrong turn, recorded

An attempt to add a `- velB*k` term to `sep` improved the look-at and broke zoom — two
compensating errors. Reverted after reading the ASM properly: `0x00446b76` and
`0x00446bff` BOTH load `[ebp-0x28]` (car A), so the leads cancel and `sep = posA - posB`.
`0x005ccd18` was checked against the PE and is genuinely `0.00015` (`0x391d4952`), not
a gloss error.

## Also corrected here

The pre-existing `re/analysis/race_camera/camera_trace.csv` (2026-06-10) has **invalid
car-position columns** — the probe read offsets that are not a position triple, giving
denormal Y (`|y|` max 1.0e-07 / 3.5e-05 against a real ground height of ~0.425). It sat
in `re/analysis/` for eleven weeks looking like ground truth. The rewritten probe now
fails loudly on such a capture instead of emitting it silently.

`+0x4c..+0x54` is the aim DIRECTION vector, not a look-target. Deriving `elev`/`azim`
from it reproduces the recorded `+0x34`/`+0x38` to **0.0000°** (azim) and 0.002° (elev)
over 286 frames; deriving them from `(tgt - pos)` is off by up to 60° / 22°. This
resolves the tension between `race_camera.md:28` ("look-target") and `UNCERTAINTIES.md`
U-9040's Xbox-twin reading, in favour of the twin.

## NOT established

- **This is not C4 evidence.** It is an offline diff driven by captured inputs, not a
  `diff-original` canonical-scenario run with the inline JMP live. Per the anti-overclaim
  rule in `CLAUDE.md`, that is C3-grade at best.
- **7.8% of frames (60 of 766) pick a genuinely different pair.** Untouched. Leading
  suspect, untested: the driver derives `active` as `(alive != -1)` rather than
  replicating `IsCarSlotActive` (`0x0040e370`).
- **Whether these fields are the world camera.** `verify/d1_carproj/RESULT.md` says no,
  but its Candidate A test fed the aim DIRECTION field as a look-at point, so that
  conclusion does not follow from that test. Its Candidate B result is unaffected.

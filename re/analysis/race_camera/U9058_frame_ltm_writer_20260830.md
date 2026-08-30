# U-9058 RESOLVED — what writes the RwCamera frame LTM in a TRAINING race

Branch `race/camframe`, worktree `.worktrees/race-camframe`, based on 56df8d2f.
All measurements on TRAINING (Quick Battle, always TRAINING), original MASHED.exe
(anchored build), 2026-08-30. Method: live Frida x86 hardware write-watchpoint
(DR0) + headless Ghidra decomp of pool slot Mashed_pool0 via analyzeHeadless +
DecompPC.java (the Ghidra MCP was down this session; analyzeHeadless is the same
PC program, read-only, not the Xbox twin).

## Verdict (one line)

The rendered camera basis is a **wholesale copy of a per-frame-composed record
matrix at `0x0089650c`**, built by `FUN_00445aa0` (0x00445aa0); the ~24-29 deg of
roll is that function's **THIRD axis-angle rotation, about the camera's own
forward/`at` axis, by a time-sinusoidal angle** (line 338 below). It is NOT the
euler roll field `DAT_00897fe0+0x3c` (which is 0) and NOT `Camera::Apply`.

## The writers, measured live (verify/camframe/writers.json, writers2.json)

Camera object resolved live: `cam = *(DAT_00897fe0+0x84)`, `frame = *(cam+4)`,
modelling matrix `frame+0x10`, LTM `frame+0x50` (RW 3.x: right+0x00/up+0x10/
at+0x20/pos+0x30). DR0 armed on the resolved slots, 2.5 s window (~151 frames).

**frame+0x10 (modelling) writers:**
| PC | in | count | value written | via (bt) |
|---|---|---|---|---|
| 0x004c15d2 | FUN_004c15c0 (identity reset) | 151 | 0x3f800000 (1.0) | 0x441785 = **Camera::Apply 0x00441760** |
| 0x004c4c1a / 0x004c4cf1 | FUN_004c4a50 (rotate core) | 151/302 | euler-rotated | Apply's 3 FUN_004c4d20 |
| **0x00442a4e** | **Camera::InitWithMatrix 0x00442a20** | 151 | **0xbf64826b (-0.8926, ROLLED)** | 0x44820d in FUN_00446520 |

**frame+0x50 (LTM) writers:** `0x004d836f` and `0x004d8120` (CRT memcpy), reached
via `0x004c0ee7` / `0x004c2bc1` (RW frame-sync). The LTM is a **verbatim memcpy of
the modelling matrix** — the frame is a root frame (modelling == LTM in every
capture), so nothing composes the LTM independently.

Both Camera::Apply AND Camera::InitWithMatrix write frame+0x10 every frame.
The live/rendered value (`right.x ≈ -0.89`, carrying roll) matches
**InitWithMatrix's copy**, not Apply's identity+euler output. Apply's euler path
(roll field +0x3c = 0) is overwritten. Confirmed: the reference `at` vector is
bit-identical `(0, -0.18674041, -0.98241)` across three captures taken seconds
apart, while roll = 28.76 / 26.71 / 23.76 deg and pos translates — a fixed forward
that ROLLS and follows the pack.

**record matrix `0x0089650c` writers (writers2.json):** watching the FIXED global
`0x0089650c` (= record[0]+0x4c, the source InitWithMatrix copies from):
| PC | in | count | value | via (bt) |
|---|---|---|---|---|
| 0x0044643a | **FUN_00445aa0 0x00445aa0** | 151 | 0x3f800000 (1.0) | 0x4464e5 (FUN_004464c0) <- 0x4468fe (FUN_00446520) |
| 0x004c4c1a / 0x004c4cf1 | FUN_004c4a50 (rotate core) | 151/302 | 0xbf7c316e (rolled) | (rotate of the record matrix) |

## The composition, mechanically (dec: verify/camframe/decomp/)

Call chain (all cited from decomp):
`FUN_00446520` (race director; caller FUN_00448220)
 → `FUN_004464c0` (0x004464c0, entry-type dispatcher; on `entry[1]==0` calls FUN_00445aa0)
 → **`FUN_00445aa0`** (0x00445aa0, the active TRAINING camera-node handler)

`FUN_00445aa0` builds the record matrix at `puVar2 = iVar10 + 0x4c` (= `0x0089650c`
for record[0]; `iVar10` = camera-node record base, stride 0xd8):
- lines 323-335: reset to identity — `*(iVar10+0x4c)=1.0` (right.x; **this is the
  0x0044643a write**), `*(iVar10+0x60)=1.0` (up.y), `*(iVar10+0x74)=1.0` (at.z),
  off-diagonals 0, `*(iVar10+0x58) |= 0x20003` (RW matrix flags).
- line 336: `FUN_004c4d20(puVar2, &DAT_006146fc, *(iVar10+0xb0), 2)` — rotate about
  world axis `DAT_006146fc` by angle `iVar10+0xb0`.
- line 337: `FUN_004c4d20(puVar2, puVar2, *(iVar10+0xac), 2)` — rotate about the
  matrix's own (post-step-1) right column by angle `iVar10+0xac`.
- **line 338: `FUN_004c4d20(puVar2, (iVar10+0x6c), *(iVar10+0xb4), 2)` — rotate
  about the matrix's own forward/`at` column (`iVar10+0x6c` = matrix+0x20) by angle
  `iVar10+0xb4`. THIS IS THE ROLL.**
- line 339: `FUN_004c51a0(puVar2, pfVar1, 2)` — set the position column from `pfVar1`.

(`FUN_004c4d20` @ 0x004c4d20 → `FUN_004c4a50` @ 0x004c4a50 is the rotate core; its
element stores are the watchpoint PCs 0x4c4c1a/0x4c4cf1.)

The roll angle `iVar10+0xb4` is computed at lines 312-321:
```
fVar14 = fsin((float10)(DAT_007f101c & 0x3ff) * (float10)_DAT_005ce17c);  // sine of a phase counter
fVar14 = fVar14 * (float10)_DAT_005cc72c;                                  // * amplitude
if (*piVar11 != 0 && fVar3 < _DAT_005cc35c) {                              // conditional extra scale
    fVar15 = fVar3 - _DAT_005cc320; if (fVar15 < DAT_005d757c) fVar15 = DAT_005d757c;
    fVar14 = fVar14 * fVar15 * _DAT_005ccac8;
}
*(float *)(iVar10 + 0xb4) = (float)fVar14;                                 // stored roll angle
```
i.e. a **sinusoid of the global phase counter `DAT_007f101c` (masked 0x3ff),
scaled by `_DAT_005cc72c`** — a time-driven camera sway. Because it is a rotation
about the forward axis, it changes roll while leaving `at` fixed — exactly the
measured behaviour (constant `at`, roll 28.76→26.71→23.76).

Then the record matrix is copied into the RwCamera frame by:
- `Camera::InitWithMatrix` (0x00442a20, line 604 of the director): 16-dword copy
  from `&DAT_0089650c` (record[0]) into `*(*(param_1+0x84)+4)+0x10` (frame+0x10).
  Confirmed by plate re/analysis/bucket_util_0042f7a0_004764e0/00442a20.md and the
  live watchpoint (0x00442a4e).
- `Spectator::SelectCamera` (0x004427c0, line 608): the same copy from
  `&DAT_0089650c + sel*0xd8` for the hysteresis-selected node `sel`.
- `Camera::Apply` (0x00441760, line 596): writes frame+0x10 from the euler fields
  (`+0x38` azim / `+0x34` elev / `+0x3c` roll) — **overwritten**, roll +0x3c = 0.

Finally RW frame-sync memcpys modelling (frame+0x10) → LTM (frame+0x50).

## The three U-9058 contradictions, resolved

1. **ROLL** — from FUN_00445aa0 line 338 (sinusoidal rotation about the forward
   axis), baked into the record matrix at 0x0089650c, copied to the frame. NOT
   from euler `+0x3c` (that field belongs to the discarded Camera::Apply path).
2. **PITCH** — from FUN_00445aa0 lines 336-337 (rotations by `iVar10+0xb0/+0xac`),
   baked into the record matrix. NOT from euler elev `+0x34`.
3. **POSITION** — from FUN_00445aa0 line 339 (`FUN_004c51a0(puVar2, pfVar1)`) into
   the record matrix pos column. NOT the controller eye `+0x40`.

The controller struct `DAT_00897fe0` euler/eye fields feed only Camera::Apply,
whose output never reaches the screen in this mode.

## Refutes the prior write-up

`re/analysis/race_camera/render_camera_child_c_20260830.md` attributed the roll to
Camera::Apply's third euler rotation about the forward axis by `cam[+0x3c]`, and
proposed transcribing that field. The live watchpoint disproves it: `+0x3c` is 0 on
the rolled frame, Apply's output is overwritten, and the roll is composed in
FUN_00445aa0's record matrix instead. (This is the hypothesis U-9058 pre-flagged as
already refuted.)

## Port status (step 4) — NOT done; here is the faithful recipe

A faithful port is a **verbatim transcription of FUN_00445aa0** (2579 bytes) into
the standalone's RaceCamera, because the roll angle and the two direction angles are
computed there from the camera-node record fields (`iVar10+0xac/+0xb0/+0xb4`, the
path point `pfVar1`, and globals `_DAT_005ccad0/_005cd09c/_005ccae0/_005ce17c/
_005cc72c/_005ccac8/…`), then applied as three `FUN_004c4d20` axis-angle rotations
about `DAT_006146fc`, the right column, and the forward column. The standalone must:
(a) build the node matrix by identity + those three rotations (not a Y-up LookAt),
(b) publish the full basis (right/up/at) through RaceSceneState, (c) let the render
build the view from the basis. I did NOT hand-fit a sin() roll onto the existing
level LookAt — that is the exact "fitted formula" this uncertainty exists to
prevent, and it would also mis-model the two direction rotations and the position.

Deliverable is the confirmed mechanism above; the port is a scoped follow-up
(hook-author FUN_00445aa0 + FUN_004c4d20/FUN_004c4a50 + the camera-node struct).

## What should be filed (parent session, via re-classify — NOT filed here)

- U-9058: RESOLVE. Writer = FUN_00445aa0 (record matrix 0x0089650c) copied to the
  frame by Camera::InitWithMatrix 0x00442a20 / Spectator::SelectCamera 0x004427c0;
  roll = FUN_00445aa0 line 338 sinusoidal rotation about the forward axis.
- FUN_00445aa0 (0x00445aa0): promote toward C2 — camera-node matrix composer
  (identity + 3 axis-angle rotations + position); evidence in this note + decomp.
- FUN_004c4a50 (0x004c4a50): note as the rotate core behind FUN_004c4d20.
- Correct render_camera_child_c_20260830.md's roll attribution.

## Repro

```
py -3.12 re/frida/cam_frame_writer_watch.py --window 2.5 \
  --out <wt>/verify/camframe/writers.json          # DR0 watch: frame+0x10, +0x50, 0x0089650c
py -3.12 re/frida/race_draw_burst.py --out <wt>/verify/camframe/orig.bmp  # basis + lens
# headless decomp (MCP-free), read-only against pool slot:
analyzeHeadless.bat mashed_pool Mashed_pool0 -process MASHED.exe -readOnly \
  -scriptPath re/tools/ghidra_scripts -postScript DecompPC.java <manifest> <out.json> decomp callees callers
```

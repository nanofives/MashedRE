# A8 — POSITION half of FUN_0046e9e0 (0x0046e9e0)

Scope: the per-tick body-rotation + position writer. This note decodes ONLY the
position half and its surroundings (Q1–Q5). The orientation half is decoded in
`A8_body_heading_law_20260825.md` and is not repeated here.

NO-GUESSING: every constant and store below cites its exact instruction address.
Register roles (given, confirmed by decomp): ESI = vehicle record (base
`DAT_008815a0`, stride 0xd04), EDI = source 4x4 matrix, EBX = destination 4x4
matrix. Signature recovered by Ghidra: `void FUN_0046e9e0(float param_1, byte *param_2)`
where `param_1` = dt, `param_2` = EBP = the per-vehicle input/descriptor block.

Verified against slot `Mashed_pool14`, image base 0x00400000.

---

## Q1 — The position store, exact

### Annotated disassembly

Setup of the per-axis position increment (velocity * scaled dt):

```
0046e9e3  FLD  float ptr [ESP+0x30]     ; ST0 = param_1  (dt)
0046e9e8  FMUL float ptr [0x005cc948]   ; ST0 = param_1 * _DAT_005cc948          <-- const #1
0046e9f2  FST  float ptr [ESP+0x38]     ; save fVar1 = param_1*_DAT_005cc948  (SHARED with omega seed)
0046e9f6  FMUL float ptr [0x005cea80]   ; ST0 = fVar1 * _DAT_005cea80             <-- const #2  (= local_1c)
0046e9fc  FLD  ST0
0046e9fe  FMUL float ptr [ESI+0x9b0]    ; * linVel.x  (record +0x9b0)
0046ea04  FSTP float ptr [ESP+0xc]      ; local_24 = incX  ([ESP+0xc])
0046ea08  FLD  ST0
0046ea0a  FMUL float ptr [ESI+0x9b4]    ; * linVel.y  (record +0x9b4)
0046ea10  FSTP float ptr [ESP+0x10]     ; local_20 = incY  ([ESP+0x10])
0046ea14  FMUL float ptr [ESI+0x9b8]    ; * linVel.z  (record +0x9b8)
0046ea1a  FSTP float ptr [ESP+0x14]     ; local_1c = incZ  ([ESP+0x14])
```

Mode gate (zeroes the increment when mode not in {6,0xb,0xa}) — see Q1d:

```
0046ea1e  CALL 0x0040e350               ; mode = FUN_0040e350()
0046ea23  CMP  EAX,0x6      / JZ 0046ea54
0046ea28  CALL 0x0040e350  / CMP EAX,0xb / JZ 0046ea54
0046ea32  CALL 0x0040e350  / CMP EAX,0xa / JZ 0046ea54
0046ea3c  MOV  [ESP+0x14],0x0           ; incZ = 0
0046ea44  MOV  [ESP+0x10],0x0           ; incY = 0
0046ea4c  MOV  [ESP+0xc],0x0            ; incX = 0
```

The three translation stores (matrix translation floats at EBX+0x30 / +0x34 / +0x38):

```
0046ea54  FLD  float ptr [ESP+0xc]      ; incX
0046ea5c  FADD float ptr [EDI+0x30]     ; + srcMat.tx
0046ea5f  FSTP float ptr [EBX+0x30]     ; dstMat.tx = srcMat.tx + incX
0046ea62  FLD  float ptr [EDI+0x34]     ; srcMat.ty
0046ea65  FADD float ptr [ESP+0x10]     ; + incY
0046ea69  FSTP float ptr [EBX+0x34]     ; dstMat.ty = srcMat.ty + incY
0046ea6c  FLD  float ptr [EDI+0x38]     ; srcMat.tz
0046ea6f  FADD float ptr [ESP+0x14]     ; + incZ
0046ea73  FSTP float ptr [EBX+0x38]     ; dstMat.tz = srcMat.tz + incZ
```

### (a) Velocity component source offsets
- incX reads `ESI+0x9b0`  (0046e9fe)  = record +0x9b0  (linVel.x)
- incY reads `ESI+0x9b4`  (0046ea0a)  = record +0x9b4  (linVel.y)
- incZ reads `ESI+0x9b8`  (0046ea14)  = record +0x9b8  (linVel.z)

### (b) Every multiplicative constant in the increment chain, in FMUL order
1. `_DAT_005cc948 = 0x39aec33e = 3.33320e-4`  (FMUL at 0046e9e8)
2. `_DAT_005cea80 = 0x3b360bc0 = 2.77804e-3`  (FMUL at 0046e9f6)

So per axis:  `inc = param_1 * _DAT_005cc948 * _DAT_005cea80 * linVel`  and
`dstMat.t = srcMat.t + inc`. The product of the two constants is
`3.33320e-4 * 2.77804e-3 = 9.2598e-7`.

### (c) Is the dt here the SAME param_1 as the omega seed?
YES — literally the same value. The position path computes
`fVar1 = param_1 * _DAT_005cc948` and stores it to `[ESP+0x38]` at 0046e9f2.
The omega seed re-loads that exact slot: `FLD [ESP+0x38]` at 0046ea76 then
`FMUL _DAT_005cc32c` (0x3f000000 = 0.5) at 0046ea7c. Both halves therefore share
one `param_1` and one `_DAT_005cc948` factor; they differ only in the second
constant (position = `_DAT_005cea80`, omega seed = `_DAT_005cc32c = 0.5`).

### (d) Clamp / gate / conditional around the position store
- The only conditional is the mode gate at 0046ea1e–0046ea3a. When
  `FUN_0040e350()` is NOT 6, 0xb, or 10, the increment is zeroed
  (0046ea3c/0046ea44/0046ea4c), so the store degenerates to a pure copy
  `dstMat.t = srcMat.t`.
- The FSTP stores at 0046ea5f/0046ea69/0046ea73 themselves are UNCONDITIONAL
  (no branch skips them). There is NO numeric clamp on the position delta.

---

## Q2 — Is the position store inside or outside the ESI[4] (record +0x10) gate?

- Position store: 0046ea54 – 0046ea73.
- ESI[4] gate compare: `MOV EAX,[ESI+0x10]` (0046eaab) / `TEST EAX,EAX` (0046eab1) /
  `JNZ 0x0046ed62` (0046eab3).

The position store executes BEFORE the gate compare, so it is **OUTSIDE** the
ESI[4] gate — the ESI[4]==0 body (the long force/torque block) does not contain it.

- mode-7 branch: `CMP EAX,0x7` (0046edaa) → `CMP [ESI+0x4],0x1` (0046edaf) →
  `JNZ 0046edef`. This whole block is far after the position store, so the store is
  also **OUTSIDE** (i.e. before, and unrelated to) the mode-7 / FUN_0040e350==7
  branch at 0046edaf.

Net: the position store is gated ONLY by its own mode∈{6,0xb,0xa} check at 0046ea1e.

---

## Q3 — Who rebases the matrix translation

The block at record+0x928 (and its twin +0x968) is a full 0x40-byte 4x4 matrix;
the active one is `iVar12 = *(ESI+0x9ac)*0x40 + 0x928 + ESI`. Translation lives at
block+0x30/+0x34/+0x38 (= record +0x958.. for selector 0, +0x998.. for selector 1).

Confirmed WRITERS of the block translation (both per substep, in order, from the
substep loop in `FUN_004709a0`: `FUN_0046e9e0(param_1,local_1c); FUN_0046f6c0(1);`):

1. **FUN_0046e9e0 @ 0046ea5f/0046ea69/0046ea73** — integrates velocity:
   `dstMat.t = srcMat.t + gated(velocity*dt)` (the ping-pong write, dst = EBX).

2. **FUN_0046f6c0 @ ~0046ff.. (decomp line `pfVar15=(float*)(iVar12+0x30); *pfVar15 -= fVar3*fVar2; iVar12+0x34 -= fVar3*local_f0; iVar12+0x38 -= fVar3*local_110`)**
   — the wheel-contact position correction. `fVar3 = _DAT_005cc320 / bVar16`
   (contact count). This SUBTRACTS a penetration term from the block translation
   every substep. **This is the rebaser** that keeps the block translation bounded:
   the integrate step pushes it, the contact solver pulls it back to the wheel
   contact set the same substep.
   - FUN_0046f6c0 also calls `FUN_004c52f0(iVar12, auStack_40, 2)` which rewrites
     the block MATRIX (rotation); translation is explicitly saved to
     local_ec/local_e8/fStack_e4 and RESTORED around that call, so FUN_004c52f0
     does not change translation.
   - FUN_0046f6c0 reads the block translation at 0046df72-region and 00470304
     (`FLD [EDI+0x958]`) for the a-frame velocity term.

Other functions that REFERENCE the block (LEA/FSUB the +0x958 translation) but whose
write status was NOT confirmed here (read sites only observed): FUN_00469df0
(0046e9e1f/0046e9e32), FUN_0046bfc0 (0046bff9), FUN_0046ddb0 (0046df72 FSUB = read).
FUN_00469df0 is the car↔car collision (LEAs BOTH source+dest blocks). [UNCERTAIN]
whether these three write the translation — only reads were verified.

Because during a normal forward drive the mode is generally not in {6,0xb,0xa}, the
FUN_0046e9e0 increment is zeroed (Q1d) and the block translation is dominated by the
FUN_0046f6c0 contact correction — a small, bounded, per-substep suspension frame.
That matches the archived capture: block translation stays in x∈[-2.5,2.5],
y∈[0.35,0.51], z∈[-5,0.5] with ~0.08 per-frame deltas. **The block translation is a
contact-corrected LOCAL frame, not the absolute world position.**

---

## Q4 — Where the world position actually lives  [UNCERTAIN]

I could NOT establish the absolute-world-position holder from static analysis in this
session. What is established:

- The block translation (+0x958 / +0x998) is bounded because FUN_0046f6c0 rebases it
  to the wheel contacts every substep (Q3). It is not the world position.
- The angular-velocity (omega) accumulators are record +0x144/+0x148/+0x14c
  (`ESI[0x51]/[0x52]/[0x53]`), written by FUN_0046e9e0 (0046ee0f-0046ee39, also
  0046ed75-0046ed9f) AND by FUN_0046f6c0 (the `unaff_EDI+0x144..` adds). These are
  omega, NOT position.
- The drifting float accumulators the capture saw at record +0x620/+0x6a0/+0x720/
  +0x7a0/+0x820 (stride 0x80): a whole-image search for a store with literal
  displacement `0x620` finds ZERO float stores — only nine integer `MOV
  reg,[base+0x620]` READS (0045f0f2, 0045f900, 0045fde6, 00460661, 004612a4,
  00461810, 00461a97, 00464d5b, 0046631b), and those base registers are loaded from
  objects that are NOT demonstrably the 0xd04 vehicle record. This means the
  vehicle-record +0x620 accumulators are written via folded base offsets (a base
  register already advanced past +0x600, displacement < 0x620) that a literal-
  displacement search does not catch. I did not resolve those writers.
- Candidate path (i) — the record's block matrix being copied into a RenderWare
  frame / atomic LTM — was not traced to a concrete copy site in this session.

Verdict: world-position field + its writer = **[UNCERTAIN]**. Evidence still missing:
(1) the folded-offset writer of the +0x620-family accumulators; (2) the site that
copies the +0x928 block matrix into a RW frame LTM. Both are the resolution path.

---

## Q5 — Unit scale between internal linear velocity (+0x9b0) and world/render position

Within FUN_0046e9e0 the ONLY scaling applied to the internal linear velocity
(+0x9b0..+0x9b8) on the way into the block translation is the timestep-integration
product:

- `_DAT_005cc948 = 0x39aec33e = 3.33320e-4`  (FMUL 0046e9e8)
- `_DAT_005cea80 = 0x3b360bc0 = 2.77804e-3`  (FMUL 0046e9f6)

i.e. `positionIncrement = linVel * (_DAT_005cc948 * _DAT_005cea80) * param_1`,
combined factor `9.2598e-7 * param_1`. There is NO separate standalone unit-scale
multiplier applied to +0x9b0 elsewhere in this function on the position path.

Whether internal units ARE world units cannot be closed here because the world-
position holder itself is [UNCERTAIN] (Q4). What can be stated with evidence: the
velocity→block-translation conversion uses exactly the two constants above and dt;
no additional render-space rescale of +0x9b0 appears in FUN_0046e9e0.

---

## Constant reference (raw dword -> float)

| Symbol | Address | Raw dword | Float | Used at |
|---|---|---|---|---|
| `_DAT_005cc948` | 0x005cc948 | 0x39aec33e | 3.33320e-4 | 0046e9e8 (pos + omega shared) |
| `_DAT_005cea80` | 0x005cea80 | 0x3b360bc0 | 2.77804e-3 | 0046e9f6 (position #2) |
| `_DAT_005cc32c` | 0x005cc32c | 0x3f000000 | 0.5 | 0046ea7c (omega seed #2) |
| `_DAT_005ce018` | 0x005ce018 | 0x3b03126f | 2.0001e-3 | 0046ee4a (later omega/coupling) |

# A8 — FUN_004a2c48 is `_ftol2`; the gearbox/boost timers and steer counters it feeds

Date: 2026-08-25. Program: MASHED.exe (image base 0x00400000), read-only pool slot Mashed_pool14.
NO-GUESSING: every constant/offset below cites the exact address it was read from.

Bottom line: **the `_ftol` hypothesis is CONFIRMED.** FUN_004a2c48 is the MSVC CRT
`_ftol2` helper (float on x87 ST0 → 64-bit signed int in EDX:EAX, **truncated toward
zero**). Ghidra renders it as a no-arg call because its operand arrives on the x87 stack,
which is invisible to the decompiler. Our `return 0` stub therefore silently zeroes eight
different computed float→int casts in the vehicle physics.

---

## Q1 — What is FUN_004a2c48? (CONFIRMED: `_ftol2`)

Full body, 0x004a2c48..0x004a2cbc (verbatim):

```
004a2c48 55                PUSH EBP
004a2c49 8bec              MOV  EBP,ESP
004a2c4b 83ec20            SUB  ESP,0x20
004a2c4e 83e4f0            AND  ESP,0xfffffff0          ; align scratch to 16
004a2c51 d9c0              FLD  ST0                     ; duplicate input x  -> ST0=x, ST1=x
004a2c53 d9542418          FST  float ptr [ESP+0x18]    ; store x as f32 (sign+bits), no pop
004a2c57 df7c2410          FISTP qword ptr [ESP+0x10]   ; round(x) -> int64 temp (default RN mode)
004a2c5b df6c2410          FILD  qword ptr [ESP+0x10]    ; reload round(x) as float -> ST0=round, ST1=x
004a2c5f 8b542418          MOV  EDX,[ESP+0x18]          ; EDX = f32 bits of x (for sign test)
004a2c63 8b442410          MOV  EAX,[ESP+0x10]          ; EAX = low32 of round(x)
004a2c67 85c0              TEST EAX,EAX
004a2c69 743c              JZ   0x004a2ca7              ; low32==0 special-case (int/QNaN/zero)
004a2c6b dee9              FSUBP                        ; ST0 = x - round(x)   (remainder), pop
004a2c6d 85d2              TEST EDX,EDX
004a2c6f 791e              JNS  0x004a2c8f              ; branch on sign of x
  ; --- x < 0 ---
004a2c71 d91c24            FSTP float ptr [ESP]         ; store remainder
004a2c74 8b0c24            MOV  ECX,[ESP]
004a2c77 81f100000080      XOR  ECX,0x80000000          ; flip remainder sign
004a2c7d 81c1ffffff7f      ADD  ECX,0x7fffffff          ; CF = (remainder != 0)
004a2c83 83d000            ADC  EAX,0x0                 ; +1 toward zero if needed
004a2c86 8b542414          MOV  EDX,[ESP+0x14]          ; high32 of round(x)
004a2c8a 83d200            ADC  EDX,0x0
004a2c8d eb2c              JMP  0x004a2cbb
  ; --- x >= 0 (0x004a2c8f) ---
004a2c8f d91c24            FSTP float ptr [ESP]
004a2c92 8b0c24            MOV  ECX,[ESP]
004a2c95 81c1ffffff7f      ADD  ECX,0x7fffffff          ; CF = (remainder != 0)
004a2c9b 83d800            SBB  EAX,0x0                 ; -1 toward zero if needed
004a2c9e 8b542414          MOV  EDX,[ESP+0x14]
004a2ca2 83da00            SBB  EDX,0x0
004a2ca5 eb14              JMP  0x004a2cbb
  ; --- low32(round)==0 (0x004a2ca7) ---
004a2ca7 8b542414          MOV  EDX,[ESP+0x14]
004a2cab f7c2ffffff7f      TEST EDX,0x7fffffff          ; any nonzero magnitude in high32?
004a2cb1 75b8              JNZ  0x004a2c6b              ; yes -> normal remainder path
004a2cb3 d95c2418          FSTP float ptr [ESP+0x18]    ; else pop the two fp regs and fall through
004a2cb7 d95c2418          FSTP float ptr [ESP+0x18]
004a2cbb c9                LEAVE
004a2cbc c3                RET
```

Literal reading:
- **Reads**: one implicit argument x on the x87 stack (ST0). No stack/memory args, no registers in.
- **Returns**: signed 64-bit integer in **EDX:EAX** (the 32-bit truncation is EAX). All eight
  vehicle call sites use only EAX.
- **Rounding**: FISTP rounds-to-nearest, then the FSUBP remainder + XOR/ADD/ADC/SBB carry trick
  corrects that by ±1 so the net result is **truncation toward zero** — identical to a C `(int)`
  cast. It does **not** modify the FPU control word (the whole point of the `_ftol2` variant).
- **Pattern match**: byte-for-byte the canonical MSVC `_ftol2` (a.k.a. `__ftol2`): prologue
  `55/8bec/83ec20/83e4f0`, `FLD ST0`, `FST`, `FISTP qword`, `FILD qword`, the sign-test on the
  stored f32, and the two carry-adjust tails. **Hypothesis CONFIRMED.**

---

## Q2 — The +0x494 gearbox-shift-timer call sites (structure & x87 build-up)

The timer is armed a few instructions earlier (raw, so both arming constants are cited):
```
00467978 c78694040000b80b0000  MOV [ESI+0x494],0xbb8       ; +3000 on shift-up
00467990 c7869404000048f4ffff  MOV [ESI+0x494],0xfffff448  ; -3000 (=-3000) on shift-down
```
`0xbb8 = 3000`, `0xfffff448 = -3000`. ESI = the 0xd04 vehicle record. Stack note for this
function (SUB ESP,0xe4 + 3 pushes = 0xf0 below return frame): **`[ESP+0xf8] = param_2`** (the
float dt), `[ESP+0x100] = param_4`. So the operand subtracted/added below is dt.

### Site 1 — decrement when +0x494 > 0  (call at 0x004679ca)
```
004679a6 8b8694040000  MOV  EAX,[ESI+0x494]       ; EAX = timer
004679b3 3bc3          CMP  EAX,EBX               ; EBX = 0
004679b9 89442434      MOV  [ESP+0x34],EAX
004679bd 7e20          JLE  0x004679df            ; guard: run only if timer > 0
004679bf db442434      FILD dword ptr [ESP+0x34]  ; ST0 = (float)(int)timer
004679c3 d8a424f8000000 FSUB float ptr [ESP+0xf8] ; ST0 = timer - param_2(dt)
004679ca e879b20300    CALL 0x004a2c48            ; EAX = (int)(timer - dt)   [trunc toward 0]
004679cf 3bc3          CMP  EAX,EBX
004679d1 898694040000  MOV  [ESI+0x494],EAX       ; store back
004679d7 7d06          JGE  0x004679df
004679d9 899e94040000  MOV  [ESI+0x494],EBX       ; if result < 0 -> clamp to 0
```

### Site 2 — increment when +0x494 < 0  (call at 0x004679f8)
```
004679df 8b8694040000  MOV  EAX,[ESI+0x494]
004679e5 3bc3          CMP  EAX,EBX
004679e7 89442434      MOV  [ESP+0x34],EAX
004679eb 7d20          JGE  0x00467a0d            ; guard: run only if timer < 0
004679ed db442434      FILD dword ptr [ESP+0x34]  ; ST0 = (float)(int)timer
004679f1 d88424f8000000 FADD float ptr [ESP+0xf8] ; ST0 = timer + param_2(dt)
004679f8 e84bb20300    CALL 0x004a2c48            ; EAX = (int)(timer + dt)
004679fd 3bc3          CMP  EAX,EBX
004679ff 898694040000  MOV  [ESI+0x494],EAX
00467a05 7e06          JLE  0x00467a0d
00467a07 899e94040000  MOV  [ESI+0x494],EBX       ; if result > 0 -> clamp to 0
```

**The port's assumed structure is CORRECT.** Real clamp confirmed:
```c
if ((int)v0x494 > 0) { r = (int)((float)v0x494 - dt); v0x494 = r; if (r < 0) v0x494 = 0; }
if ((int)v0x494 < 0) { r = (int)((float)v0x494 + dt); v0x494 = r; if (r > 0) v0x494 = 0; }
```
Note it is `> 0` / `< 0` (strict) with the store-then-clamp order shown. `dt = param_2`.

**Why the stub reads 0 on every frame:** with FUN_004a2c48 forced to `return 0`, site 1 gives
`r=0` so `v0x494=0`; site 2 gives `r=0` so `v0x494=0`. The countdown collapses to 0 immediately,
exactly matching the measured standalone behaviour (0 on all 1096 frames) vs the original's
-2950..+2950.

---

## Q3 — The actual per-call decrement of +0x494

Formula, verbatim from the two sites above:
```
timer > 0:  v0x494 <- trunc_toward_zero( (float)v0x494 - param_2 ),  floored at 0
timer < 0:  v0x494 <- trunc_toward_zero( (float)v0x494 + param_2 ),  capped at 0
```
The decrement magnitude per call is **exactly `param_2`** (the function's float dt argument),
applied via `(int)((float)timer ∓ param_2)`. There is **no literal 50 in this function** — the
step is whatever dt the caller passes. The user's "50 units → 3000/50 = 60 frames" is consistent
with a runtime `param_2 == 50.0` (armed ±3000, first step lands at ±2950, matching the observed
range), but that value lives in the caller, not here. `[UNCERTAIN]` — the numeric value of
param_2 at runtime is not decidable inside FUN_00467650; only the `timer ∓ param_2` law is.

---

## Q4 — The other six FUN_004a2c48 sites feeding vehicle state

All are real computed float→int casts; each is zeroed by the `return 0` stub.
`ESI` = 0xd04 record; `EDI/piVar12` = a 0xc4-stride per-wheel sub-block (base ESI+0x1a4);
`param_2 = [ESP+0xf8]` = dt; `param_4 = [ESP+0x100]` = the input descriptor.

| call RVA | receives | float expression fed to _ftol2 |
|---|---|---|
| 0x00467cfe | `[ESI+0xbf4]` (boost timer, mode==6 branch) | `(int)((float)[ESI+0xbf4] − param_2)`, then clamp to `[0, 0xbb8=3000]` |
| 0x00467dc6 | `[ESI+0xbf4]` (bf8==1 branch) | `(int)((float)[ESI+0xbf4] − param_2)`, reset bf8/bf4 to 0 if ≤0 |
| 0x00467e25 | `[ESI+0xbf4]` (bf8==2 branch) | `(int)((float)[ESI+0xbf4] − param_2)`, reset bf8/bf4 to 0 if ≤0 |
| 0x00467e7e | `piVar12[0]` (per-wheel spin int) | `(int)( (dirX·vx + dirY·vy + dirZ·vz) / (piVar12[-0xa] · _DAT_005cea24) )` where dir = piVar12[0x1f..0x21], v = [ESI+0x9b0/0x9b4/0x9b8], `_DAT_005cea24 = 0x3fdf6715 ≈ 1.745329` |
| 0x00467f93 | `piVar12[-1]` (per-wheel, brake branch) | `(int)( (float)param_4[5] + _DAT_005cea20 )`, `_DAT_005cea20 = 0x43800000 = 256.0` |

The +0xbf4 boost timer decrements by `param_2` per frame exactly like +0x494 (same
`FILD; FSUB [ESP+0xf8]; CALL` shape at 0x00467cf1/0x00467db9), clamped to [0,3000].

**Count note:** the task expected "6 call sites" in FUN_00467650; the binary has **7**
(gearbox ×2, boost ×3, per-wheel ×2). The extra one is the third +0xbf4 call. All seven are
real values collapsed to 0 by the stub.

### The two steer sites in FUN_00470670 (+0xb24 / +0xb28)
Stack here: 6 pushes → `[ESP+0x1c] = param_2`, and FUN_00470670 forwards that same param_2 into
FUN_00467650 (`FUN_00467650(param_1, param_2, iVar1, param_3)` at 0x0047090x). So it is the same dt.
`EDI = in_EAX` = the 0xd04 record; `EBP = param_3` = the input byte pair.

```
; +0xb24  (left/analog-0 hold counter) — runs while param_3[0] != 0
00470732 385d00        CMP  byte ptr [EBP],BL         ; param_3[0] == 0 ?
00470735 7417          JZ   0x0047074e                ; ==0 -> [EDI+0xb24] = 0
00470737 db87240b0000  FILD dword ptr [EDI+0xb24]     ; ST0 = (float)(int)[EDI+0xb24]
0047073d d844241c      FADD float ptr [ESP+0x1c]      ; + param_2(dt)
00470741 e802250300    CALL 0x004a2c48                ; (int)([EDI+0xb24] + dt)
00470746 8987240b0000  MOV  [EDI+0xb24],EAX

; +0xb28  (right/analog-1 hold counter) — runs while param_3[1] != 0
00470754 385d01        CMP  byte ptr [EBP+0x1],BL     ; param_3[1] == 0 ?
00470757 7417          JZ   0x00470770                ; ==0 -> [EDI+0xb28] = 0
00470759 db87280b0000  FILD dword ptr [EDI+0xb28]     ; ST0 = (float)(int)[EDI+0xb28]
0047075f d844241c      FADD float ptr [ESP+0x1c]      ; + param_2(dt)
00470763 e8e0240300    CALL 0x004a2c48                ; (int)([EDI+0xb28] + dt)
00470768 8987280b0000  MOV  [EDI+0xb28],EAX
```

So `+0xb24 / +0xb28` are **integer dt-accumulators** ("how long this steer input has been held"),
reset to 0 when the corresponding input byte is 0, otherwise `(int)(old + dt)`. They are later
read back as `(float)*(int*)(in_EAX+0xb24/0xb28)`, clamped by `_DAT_005ceaa4`, and used to scale
the steer force (FUN_00470670 body). These are **real accumulators, zeroed by the stub** — not
raw analog bytes: the analog byte is param_3[0]/param_3[1]; +0xb24/+0xb28 hold the *held-duration*.

---

## Q5 — Is FUN_004a2c48 a generic CRT helper? (YES)

`function_callers(0x004a2c48)` returns **154 callers** spanning the entire image, e.g.:
- `FUN_004011f0`, `FUN_00402fb0` — startup / early 0x401xxx-0x402xxx code (nothing to do with vehicles)
- `FUN_0043c5b0`, `FUN_00443dc0` — frontend/menu region (0x43xxxx-0x44xxxx)
- `FUN_00490500`, `FUN_00496530`, `FUN_00497310` — device/input region (0x49xxxx)

Vehicle physics (FUN_00467650, FUN_00470670) is just two of 154. The uniform prologue, the
truncate-toward-zero semantics, and the 154-way spread across unrelated subsystems together
establish this is the statically-linked MSVC CRT `_ftol2`, emitted at every `(int)floatExpr`
cast — **not** a game routine. It should be modelled as the C cast operator, never stubbed.

---

## Recommendation (context, not part of the decode)

Replace both stubs with a real truncating float→int cast. The eight sites reduce to:
`Vc_RoundST0(x) == (int)x` and `Vc_InputFilter(x) == (int)x` (truncation toward zero). The
"argument" is the x87 expression built immediately before each call, transcribed above.

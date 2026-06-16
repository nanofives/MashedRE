# FUN_00416250 — steering-byte `__ftol(ST0)` writes, recovered from ASM (WS-C2, 2026-06-16)

The Ghidra decomp of `FUN_00416250` (primary AI control step) loses the x87 FPU
stack at its six `FUN_004a2c48` (`__ftol`) call sites (`extraout_ST0`), so the
steer-byte VALUES are not in the decomp. This note recovers them from the
instruction listing (pool5, this session) so the verbatim port can fill them in
without guessing. **The port is NOT yet committed** — it also needs the
`[ESP+0x10]/[0x18]` frame-slot identities confirmed (see "Remaining").

Anchored to MASHED.exe SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`.
`FUN_004a2c48` = MSVC `__ftol` (reads ST0, returns rounded long). Build is x87
([[project_wsa2_rwmath_bitident]]) so `(int)floatExpr` reproduces it.

## Frame

`PUSH EBP; MOV EBP,ESP; AND ESP,~7; SUB ESP,0x44; PUSH EBX,ESI,EDI` — aligned-ESP
FPO frame with **deferred bulk arg-cleanup** (e.g. `ADD ESP,0x2c` @0x004162bb,
`ADD ESP,0xc` @0x004165a2). EBX = param_3 (ctrl block, `[EBP+0x10]`); ESI =
param_2 (vehicle); EDI = param_1 (spline). Call ESP-base after the 3 reg-pushes
"S0"; the steering block runs at S0 (all call args cleaned).

Confirmed frame slots (at S0):
- `[ESP+0x14]` = **fVar1** = the FUN_00415e20 signed angle error (`FST [ESP+0x20]`
  @0x0041659e, then `ADD ESP,0xc` → becomes `[ESP+0x14]`).
- `[ESP+0x1c]` = **local_3c** = rate (FUN_0046d6d0 out; `LEA ECX,[ESP+0x24]`
  @0x00416273 at ESP=S0-8 → S0+0x1c).
- `[ESP+0x20]` = local_38 = speed (FUN_0046d6a0); `[ESP+0x24]` = local_34 = game
  mode (FUN_0040e350).

## The six `__ftol` sites

`fVar1` (`[ESP+0x14]`) selects a band; per band there is a **fresh-steer** path
and a **decay/replay** path. `a4ec` = `(&DAT_0089a4ec)[v*0x1d]`, `a4f0` =
`…a4f0`, `a4f4` = `…a4f4` (the 0x74-stride state record).

### Band 1 — `fVar1 < _DAT_005cd09c`, gated by `_DAT_005cc320(1.0) < fVar1`
- **Fresh** (`a4ec==1` OR `DAT_007f0ff4 - a4f0 >= 0xc8`), @0x00416648:
  ```
  steerVal = fVar1 * [ESP+0x1c] * _DAT_005cd0e8           ; FLD;FMUL [ESP+0x1c];FMUL [0x5cd0e8]
  if ([ESP+0x10] == 0 && [ESP+0x18] <= _DAT_005ccd6c):
      steerVal *= [ESP+0x18] * _DAT_005cc9a0              ; conditional extra factor
  steerVal = max(steerVal, _DAT_005cd04c)                 ; FCOM [0x5cd04c] clamp-up
  param_3[0] = (u8)__ftol(steerVal)                       ; 0x00416692 MOV [EBX],AL
  a4f4       = (s32)__ftol(steerVal)                      ; 0x00416699 (FLD ST0 dup)
  a4ec = 1; a4f0 = DAT_007f0ff4
  ```
- **Decay** (`a4ec!=1 && elapsed<0xc8`), @0x00416623 → 0x0041663e:
  ```
  local_2c   = 0xc8 - (DAT_007f0ff4 - a4f0)
  param_3[1] = (u8)__ftol((float)local_2c * _DAT_005cd0ec * (s32)a4f4)
  ```

### Band 2 — `_DAT_005cd09c < fVar1`, gated by `fVar1 < _DAT_005cd0e4`
Mirror of band 1 with `[0]`/`[1]` swapped and the fresh value built from the
`2π` complement:
- **Fresh** (`a4ec==2` OR elapsed>=0xc8), @0x0041675c:
  ```
  steerVal = (_DAT_005ccac4 - fVar1) * [ESP+0x1c] * _DAT_005cd0e8   ; FLD [0x5ccac4];FSUB [ESP+0x14]
  ... same [ESP+0x10]==0 extra factor + max(_DAT_005cd04c) clamp ...
  param_3[1] = (u8)__ftol(steerVal)                       ; 0x004167b1 MOV [EBX+1],AL
  a4f4       = (s32)__ftol(steerVal)                      ; 0x004167b4 ; a4ec=2;a4f0=now
  ```
- **Decay**, @0x00416738 → 0x00416753:
  ```
  param_3[0] = (u8)__ftol((float)local_2c * _DAT_005cd0ec * (s32)a4f4)
  ```

### Debug remap @0x004169fb
`if (DAT_0089a368 == 1) param_3[4] = (u8)__ftol((float)(u8)param_3[4] ... )` —
the decomp's `local_2c = (uint)(byte)param_3[4]; param_3[4] = FUN_004a2c48()`.

## Constants (all `[0x005cXXXX]`, harvest for the standalone — WS-A7)
`_DAT_005cd09c` (band split), `_DAT_005cc320`=1.0, `_DAT_005cd0e4` (band-2 hi),
`_DAT_005cd0e8` (steer scale), `_DAT_005cd0ec` (decay scale), `_DAT_005cd04c`
(steer floor), `_DAT_005cc9a0` (extra factor), `_DAT_005ccd6c` (extra gate),
`_DAT_005ccac4`=2π.

## Remaining to finish the port (NO-GUESSING gate)
- `[ESP+0x18]` and `[ESP+0x10]` frame-slot → decomp-local identity: needs ESP
  tracking through the uncleaned push groups (0x004162xx setup). `[ESP+0x1c]` =
  rate is confirmed; `[ESP+0x18]` is likely `local_40` (FUN_00443440 progress,
  written via `FSTP [ESP+0x18]` @0x004162d5 at a shifted ESP) and `[ESP+0x10]`
  the mode/flag tested `TEST EAX,EAX` — but **confirm before porting** (a wrong
  map silently steers wrong; [[feedback-diff-reimpl-asm-vs-original]]).
- Then transcribe with the explicit decomp body (targeting chain, mode commit
  DAT_0089a52c[v]=local_48, the param_3[4]/[5] mode-overrides + final game-mode
  gate are all unambiguous in the decomp).

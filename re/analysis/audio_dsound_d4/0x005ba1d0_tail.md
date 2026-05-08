---
rva: 0x005ba1d0 (tail: 0x005ba4a0–0x005ba716)
name_in_ghidra: FUN_005ba1d0
size_bytes: 0x547 (body_start=005ba1d0 body_end=005ba716)
confidence: C1
callees_depth1_tail:
  - 0x005bbf30: FUN_005bbf30 (Release x3; already mapped)
  - 0x005bbdb0: FUN_005bbdb0 (buffer create wrapper; already mapped)
  - 0x005ba760: FUN_005ba760 (cleanup on fail; already mapped)
  - 0x005c7990: FUN_005c7990 (thin wrapper to FUN_005aa560; NEW — S-3190)
  - 0x005bbd50: FUN_005bbd50 (CreateSoundBuffer wrapper; already mapped)
  - 0x005bc400: FUN_005bc400 (double-QI chain; NEW)
  - 0x005bb000: FUN_005bb000 (buffer init+teardown; already mapped)
  - 0x005bbfc0: FUN_005bbfc0 (QI + vtable+0x14 secondary init; NEW)
  - 0x005baf00: FUN_005baf00 (music group volume setter; already mapped)
  - 0x005baf60: FUN_005baf60 (mode-flag setter; NEW)
  - 0x005baf90: FUN_005baf90 (vtable+0x20 caller; NEW)
opened_in_slot: Mashed_pool1
session_date: 2026-05-08
resolves: D-0940
---

## Context

This file supplements `re/analysis/audio_dsound/0x005ba1d0.md` (session audio_dsound-20260502-1942, d1).
The d1 session capped the listing at 250 instructions (~0x005ba49f). This note covers
the tail `0x005ba4a0`–`0x005ba716` (211 code units) obtained via
`listing_code_units_list(start=0x005ba4a0, end=0x005ba716)`.

## Mechanical description — tail blocks

### Entry join (0x005ba4a0–0x005ba4aa)
- `JNZ 0x005ba4ab` (0x005ba4a1): from earlier conditional, if ECX (bit-probe result) non-zero, skip OR.
- `MOV EAX,[EBP+0x78]`; `OR AL,0x20`; `MOV [EBP+0x78],EAX` (0x005ba4a3–0x005ba4a8): sets bit 0x20 in flag word if ECX was zero.

### Third buffer creation (LAB_005ba4ab, 0x005ba4ab)
- `PUSH EDI`; `CALL 0x005bbf30` (0x005ba4ab–0x005ba4ac): calls FUN_005bbf30 (Release x3) with 1 arg.
- `MOV ECX,[EBP+0x78]` (0x005ba4b1); `MOV EAX,[EBP+0x7c]` (0x005ba4b4).
- `NOT ECX`; `SHR ECX,0x6`; `AND ECX,0x1` (0x005ba4b7–0x005ba4be): ECX = (NOT flags >> 6) & 1 — extracts inverted bit-6 of the flag word.
- `PUSH EDI`, `PUSH EBX`, `PUSH 0x1`, `LEA EDX,[ESP+0x2c]`; `PUSH ECX`; `PUSH EDX`; `PUSH EAX` (0x005ba4bc–0x005ba4c9): pushes 6 args.
- `CALL 0x005bbdb0` (0x005ba4ca): FUN_005bbdb0 (buffer create wrapper). `ADD ESP,0x1c` (0x005ba4cf).
- `TEST EAX,EAX`; `JNZ 0x005ba4f5` (0x005ba4d2–0x005ba4d4): if nonzero → LAB_005ba4f5.
- Failure path: vtable+0x8 Release on [EBP+0x7c]; `CALL 0x005ba760`; `XOR EAX,EAX`; epilogue; `RET` (0x005ba4d6–0x005ba4f4).

### Flag conditional / bit-0x8 test (LAB_005ba4f5, 0x005ba4f5)
- `MOV EAX,[ESI+0xc]`; `TEST EAX,EAX`; `JZ 0x005ba50b` (0x005ba4f5–0x005ba4fa).
- If nonzero: `TEST byte [EBP+0x78],0x20`; `JZ 0x005ba50b` (0x005ba4fc–0x005ba500).
- If bit 0x20 set: `OR EDX,0x8`; `MOV [EBP+0x78],EDX` (0x005ba502–0x005ba50a): sets bit 0x8 in flags.
- LAB_005ba50b: `TEST byte [EBP+0x78],0x8`; `JZ 0x005ba574` (0x005ba50b–0x005ba50f).

### Audio format dispatch (0x005ba511, bit-0x8 set path)
- `MOV ECX,[ESI+0x1c]` (0x005ba511); `MOV EDX,[ESI+0x14]` (0x005ba514); `XOR EAX,EAX`; `MOV AX,word [ESI+0x1a]` (0x005ba51b).
- `SHR ECX,0x1` (0x005ba529): half of [ESI+0x1c].
- `PUSH 0x4`, stack build for FUN_005c7990 call.
- `CALL 0x005c7990` (0x005ba53a): passes (EBP, [ESI+0x14], AX=[ESI+0x1a], 4, 0, 0, [ESI+0x1c]/2, EBP) to FUN_005c7990.
- `ADD ESP,0x10` (0x005ba53f).
- `MOV [EBP+0x11c],EAX` (0x005ba542): stores return into [EBP+0x11c].
- `TEST EAX,EAX`; `JNZ 0x005ba57e` (0x005ba548–0x005ba54a): success → LAB_005ba57e.
- Failure: vtable+0x8 Release; `CALL 0x005ba760`; `XOR EAX,EAX`; epilogue; `RET` (0x005ba54c–0x005ba573).
- LAB_005ba574: `MOV dword [EBP+0x11c],0x0` (0x005ba574): zero path for bit-0x8 clear.

### 3D buffer acquisition (LAB_005ba57e, 0x005ba57e)
- `TEST byte [EBP+0x78],0x40`; `JNZ 0x005ba62f` (0x005ba57e–0x005ba582).
- Non-0x40 path: `PUSH 0x1`; `PUSH [EBP+0x7c]`; `CALL 0x005bbd50` (0x005ba58e): CreateSoundBuffer-style.
- `MOV EBX,EAX`; `TEST EBX,EBX`; `JZ 0x005ba5be` (0x005ba593–0x005ba59a).
- If EBX nonzero: `LEA EDI,[EBP+0x8c]`; `PUSH EDI`; `PUSH EBX`; `CALL 0x005bc400` (0x005ba5a4): double-QI chain on buffer object.
- `MOV EAX,[EBX]`; `PUSH EBX`; `CALL [EAX+0x8]` (0x005ba5b0–0x005ba5b5): vtable+0x8 Release on EBX.
- `TEST EAX [ESP+0x14]`; `JNZ 0x005ba5f4` (0x005ba5ba–0x005ba5bc): if 005bc400 returned 1 → LAB_005ba5f4.
- Zero path (LAB_005ba5be): `PUSH 0x0`; `PUSH EBP`; `CALL 0x005bb000` (0x005ba5c1); `LEA EAX,[EBP+0x80]`; `CALL 0x005bbf30`; vtable+0x8 Release; `CALL 0x005ba760`; `XOR EAX,EAX`; `RET` (0x005ba5be–0x005ba5f3).

### 3D buffer vtable calls (LAB_005ba5f4, 0x005ba5f4)
- `MOV EAX,[EDI]` (EDI = [EBP+0x8c]) (0x005ba5f4).
- `PUSH 0x0`; `PUSH 0x3f800000` (IEEE 754 1.0); `PUSH EAX`; `MOV EDX,[EAX]`; `CALL [EDX+0x30]` (0x005ba5f6–0x005ba602): calls vtable+0x30 with (iface, 1.0, 0).
- `MOV EAX,[EDI]`; pushes 0x0 ×4; `PUSH EAX`; `MOV ECX,[EAX]`; `CALL [ECX+0x40]` (0x005ba603–0x005ba612): calls vtable+0x40 with (iface, 0, 0, 0, 0).
- `MOV EDI,[EDI]`; pushes 0x0 ×2; `PUSH 0x3f800000`; `MOV EDX,[EDI]`; `PUSH 0x0`; `PUSH 0x3f800000`; `PUSH 0x0 ×2`; `PUSH EDI`; `CALL [EDX+0x34]` (0x005ba613–0x005ba62e): calls vtable+0x34 with (iface, 0, 1.0, 0, 0, 1.0, 0, 0).

### Secondary init path (LAB_005ba62f, 0x005ba62f)
- `LEA EAX,[EBP+0x80]`; `PUSH EAX`; `CALL 0x005bbfc0` (0x005ba636): QI+vtable+0x14 init on [EBP+0x80].
- `TEST EAX,EAX`; `JZ 0x005ba64a` (0x005ba640): if nonzero: `OR AL,0x10`; writes to [EBP+0x78] (sets bit 0x10).

### Volume init (LAB_005ba64a, 0x005ba64a)
- `PUSH 0x3f800000`; `PUSH EBP`; `CALL 0x005baf00` (0x005ba650): calls FUN_005baf00 (music group volume setter) with (EBP, 1.0).
- `ADD ESP,0x8`; `MOV EAX,[ESI+0x24]`; `CMP EAX,0x1`; `JNZ 0x005ba66d` (0x005ba655–0x005ba65e).
- If [ESI+0x24]==1: `PUSH 0x1`; `PUSH EBP`; `CALL 0x005baf60` (0x005ba663): FUN_005baf60 with (EBP, 1).
- Else: `PUSH EBP`; `CALL 0x005baf90` (0x005ba66e): FUN_005baf90 with (EBP); `AND CH,0xfe` clears bit 9 of [EBP+0x78] (0x005ba679); stores return to [EBP+0x74] (0x005ba67c); stores updated flags (0x005ba67f).

### Low-nibble flag block (LAB_005ba682, 0x005ba682)
- `TEST byte [EBP+0x78],0xf`; `JZ 0x005ba6fc` (0x005ba682–0x005ba686): if no low-nibble flag, skip.
- `MOV EDX,[EBP+0x78]`; `MOV EAX,0xffffd8f0` (signed dec -10000) (0x005ba68b); `LEA EDI,[EBP+0x44]`; `MOV ECX,0xc` (0x005ba693); `LEA ESI,[ESP+0x1c]`.
- `OR DL,0x80` (0x005ba69c): sets bit 0x80 in DL of [EBP+0x78].
- Writes 12 dwords to [ESP+0x1c..+0x48] then `MOVSD.REP` 12 dwords from [ESP+0x1c] → [EBP+0x44..+0x73]:
  - [ESP+0x1c] = [ESP+0x20] = 0xffffd8f0 (-10000; cited at 0x005ba69f, 0x005ba6a3)
  - [ESP+0x24] = 0x0 (cited at 0x005ba6a7)
  - [ESP+0x28] = 0x3fbeb852 (IEEE 754 ~0.373; cited at 0x005ba6af)
  - [ESP+0x2c] = 0x3f547ae1 (IEEE 754 ~0.829; cited at 0x005ba6b7)
  - [ESP+0x30] = 0xfffff5d6 (signed dec -2602; cited at 0x005ba6bf)
  - [ESP+0x34] = 0x3be56042 (IEEE 754 ~0.007; cited at 0x005ba6c7)
  - [ESP+0x38] = 0xc8 (200; cited at 0x005ba6cf)
  - [ESP+0x3c] = 0x3c343958 (IEEE 754 ~0.011; cited at 0x005ba6d7)
  - [ESP+0x40] = [ESP+0x44] = 0x42c80000 (IEEE 754 100.0; cited at 0x005ba6df, 0x005ba6e7)
  - [ESP+0x48] = 0x459c4000 (IEEE 754 5000.0; cited at 0x005ba6ef)
- `MOV [EBP+0x78],EDX` (0x005ba6f9): stores OR'd flags back.

### Epilogue (LAB_005ba6fc, 0x005ba6fc)
- `LEA EAX,[EBP+0x94]` (0x005ba6fc); `POP EDI`.
- `MOV [EBP+0x98],EAX` (0x005ba703): writes ptr to [EBP+0x94] into [EBP+0x98].
- `POP ESI`.
- `MOV [EAX],EAX` (0x005ba70a): writes [EBP+0x94] = EAX = &[EBP+0x94] (self-referential pointer).
- `MOV EAX,EBP` (0x005ba70c): return value = EBP (first param / audio struct ptr).
- `POP EBP`, `POP EBX`; `ADD ESP,0x9c`; `RET` (0x005ba70e–0x005ba716).

## Constants
| RVA cited at | Raw hex | Signed dec | Notes |
|---|---|---|---|
| 0x005ba4a6 | 0x20 | 32 | OR bit into [EBP+0x78] |
| 0x005ba4be | 0x1 | 1 | AND mask on NOT(flags>>6) |
| 0x005ba4c1 | 0x1 | 1 | pushed arg to 005bbdb0 |
| 0x005ba50b | 0x8 | 8 | bit-test on [EBP+0x78] |
| 0x005ba529 | 0x1 | 1 | SHR ECX,1 on [ESI+0x1c] |
| 0x005ba542 | 0x11c | 284 | [EBP+0x11c] stores 005c7990 return |
| 0x005ba574 | 0x0 | 0 | zero-fill of [EBP+0x11c] on 0x8-clear path |
| 0x005ba57e | 0x40 | 64 | bit-test flag → 3D buffer path |
| 0x005ba600 | 0x30 | 48 | vtable+0x30 call on [EDI] |
| 0x005ba610 | 0x40 | 64 | vtable+0x40 call on [EAX] |
| 0x005ba62c | 0x34 | 52 | vtable+0x34 call on [EDI] |
| 0x005ba63b | — | — | 005bbfc0 return stored; bit 0x10 OR'd into [EBP+0x78] |
| 0x005ba645 | 0x10 | 16 | OR bit into [EBP+0x78] |
| 0x005ba650 | 0x3f800000 | 1065353216 | IEEE 754 1.0; arg to 005baf00 |
| 0x005ba68b | 0xffffd8f0 | -10000 | first dword in float-constant block |
| 0x005ba693 | 0xc | 12 | REP count (12 dwords = 48 bytes) |
| 0x005ba69c | 0x80 | 128 | OR bit into DL of [EBP+0x78] |
| 0x005ba6af | 0x3fbeb852 | 1069120594 | IEEE 754 ~0.373 |
| 0x005ba6b7 | 0x3f547ae1 | 1062948577 | IEEE 754 ~0.829 |
| 0x005ba6bf | 0xfffff5d6 | -2602 | negative dword in block |
| 0x005ba6c7 | 0x3be56042 | 1005240386 | IEEE 754 ~0.007 |
| 0x005ba6cf | 0xc8 | 200 | integer in block |
| 0x005ba6d7 | 0x3c343958 | 1010731352 | IEEE 754 ~0.011 |
| 0x005ba6df | 0x42c80000 | 1119879168 | IEEE 754 100.0 |
| 0x005ba6ef | 0x459c4000 | 1168048128 | IEEE 754 5000.0 |
| 0x005ba6fc | 0x94 | 148 | [EBP+0x94] addr written to [EBP+0x98] |
| 0x005ba70a | — | — | self-ref: [EBP+0x94] = &[EBP+0x94] |

## Uncertainties
- [UNCERTAIN U-0363] Float-constant block at 0x005ba6af–0x005ba6ef: 12 dwords written to [EBP+0x44..+0x73] when low-nibble of [EBP+0x78] is nonzero. Values include two pairs of 100.0/5000.0 and fractional floats. Struct layout and semantic meaning unknown. Evidence needed: cross-check with 3D audio API (IDirectSound3DListener::SetAllParameters or similar) struct layout.

## Stubs encountered (new, depth-2 from 005ba1d0)
- [STUB S-3190] 0x005aa560 FUN_005aa560 (called by 005c7990 with &DAT_007de1c0 + 4 params; body 0x005aa560–0x005aa797, 567 bytes)

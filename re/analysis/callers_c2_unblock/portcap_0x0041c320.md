---
rva: 0x0041c320
name: FUN_0041c320
size_bytes: 90
confidence_target: C2
callees_depth1: [FUN_004b3fc0, FUN_004b6520, FUN_004b5190]
callers_noted: [0x0041cb10]
opened_in_slot: Mashed_pool10
session_date: 2026-07-30
---

## Mechanical description

Port-grade disassembly capture of the Class C (24-atomic) ParticleEmitter
constructor. `function_at` confirms body `0041c320..0041c37a` (source:
`mcp__ghidra__function_at`). Identical template to FUN_0041ad60 but with a
24-count loop bound and offsets shifted to `this+0x80` (handle table) /
`this+0x100` (clump) / `this+0x104` (frame). `this` in EBX, never assigned
inside the body. Entry EAX copied to ESI @ 0x0041c324 exactly as in
FUN_0041ad60 (see U-7925 resolution).

## Full listing

| Address | Bytes | Mnemonic/text |
|---|---|---|
| 0x0041c320 | 83ec60 | SUB ESP,0x60 |
| 0x0041c323 | 56 | PUSH ESI |
| 0x0041c324 | 8bf0 | MOV ESI,EAX |
| 0x0041c326 | 57 | PUSH EDI |
| 0x0041c327 | 8d442408 | LEA EAX,[ESP + 0x8] |
| 0x0041c32b | 50 | PUSH EAX |
| 0x0041c32c | 56 | PUSH ESI |
| 0x0041c32d | e88e7c0900 | CALL 0x004b3fc0 |
| 0x0041c332 | 89b300010000 | MOV dword ptr [EBX + 0x100],ESI |
| 0x0041c338 | 8b4e04 | MOV ECX,dword ptr [ESI + 0x4] |
| 0x0041c33b | 8d9380000000 | LEA EDX,[EBX + 0x80] |
| 0x0041c341 | 6880000000 | PUSH 0x80 |
| 0x0041c346 | 52 | PUSH EDX |
| 0x0041c347 | 898b04010000 | MOV dword ptr [EBX + 0x104],ECX |
| 0x0041c34d | e8cea10900 | CALL 0x004b6520 |
| 0x0041c352 | 83c410 | ADD ESP,0x10 |
| 0x0041c355 | 33f6 | XOR ESI,ESI |
| 0x0041c357 | 8b7cb408 | LAB_0041c357: MOV EDI,dword ptr [ESP + ESI*0x4 + 0x8] |
| 0x0041c35b | 6a00 | PUSH 0x0 |
| 0x0041c35d | 6a00 | PUSH 0x0 |
| 0x0041c35f | 57 | PUSH EDI |
| 0x0041c360 | e82b8e0900 | CALL 0x004b5190 |
| 0x0041c365 | 83c40c | ADD ESP,0xc |
| 0x0041c368 | 46 | INC ESI |
| 0x0041c369 | 83fe18 | CMP ESI,0x18 |
| 0x0041c36c | 89bc8380000000 | MOV dword ptr [EBX + EAX*0x4 + 0x80],EDI |
| 0x0041c373 | 7ce2 | JL 0x0041c357 |
| 0x0041c375 | 5f | POP EDI |
| 0x0041c376 | 5e | POP ESI |
| 0x0041c377 | 83c460 | ADD ESP,0x60 |
| 0x0041c37a | c3 | RET |

## Callee ABI

### CALL 0x004b3fc0 @ 0x0041c32d
- Live-in setup: `LEA EAX,[ESP + 0x8]` @ 0x0041c327 (24-entry stack buffer
  address); `PUSH EAX` @ 0x0041c32b (pushed FIRST → deepest arg); `PUSH ESI`
  @ 0x0041c32c (entry-EAX value, pushed SECOND → topmost arg). Identical
  shape/argument order to FUN_0041ad60's call.
- Post-call consumption: `MOV [EBX+0x100],ESI` @ 0x0041c332 — again stores
  ESI (preserved entry-EAX), NOT the post-call EAX return value. See Decomp
  divergences.
- Stack adjust: none immediately; batched with the next call.

### CALL 0x004b6520 @ 0x0041c34d
- Live-in setup: `LEA EDX,[EBX + 0x80]` @ 0x0041c33b (address of the 24-entry
  handle-table region at `this+0x80`); `PUSH 0x80` @ 0x0041c341 (immediate
  0x80 = 128 decimal, pushed FIRST → deepest arg); `PUSH EDX` @ 0x0041c346
  (pushed SECOND → topmost arg = the pointer). `MOV [EBX+0x104],ECX` @
  0x0041c347 (storing `*(clump+4)`, ECX read @ 0x0041c338) is interleaved
  between the pushes and the call.
- Post-call consumption: none.
- Stack adjust: `ADD ESP,0x10` @ 0x0041c352 — 16 bytes, batched cleanup for
  both this call and FUN_004b3fc0's call (same pattern as FUN_0041ad60).

### CALL 0x004b5190 @ 0x0041c360 (loop body, 24 iterations)
- Live-in setup: `MOV EDI,[ESP + ESI*0x4 + 0x8]` @ 0x0041c357 (indexed read
  of the same buffer passed to FUN_004b3fc0); `PUSH 0x0` @ 0x0041c35b; `PUSH
  0x0` @ 0x0041c35d; `PUSH EDI` @ 0x0041c35f (pushed LAST → topmost arg =
  handle value).
- Post-call consumption: EAX preserved across `ADD ESP,0xc` (0x0041c365),
  `INC ESI` (0x0041c368), `CMP ESI,0x18` (0x0041c369), then used at `MOV
  [EBX + EAX*0x4 + 0x80],EDI` @ 0x0041c36c (store handle at
  `this+0x80+index*4`).
- Stack adjust: `ADD ESP,0xc` @ 0x0041c365 — 12 bytes, immediate (not
  batched).

## U-7925 resolution

Same mechanism as FUN_0041ad60 / U-7917. `LEA EAX,[ESP + 0x8]` @ 0x0041c327
computes the address of the 24-entry stack buffer; that address is pushed as
the FIRST (deepest) stack argument to `CALL 0x004b3fc0` @ 0x0041c32d, ahead of
the entry-EAX value (`PUSH ESI` @ 0x0041c32c). FUN_004b3fc0 is passed this
pointer directly — there is no separate copy or global staging area visible
in this caller. The loop @ 0x0041c357 (`MOV EDI,[ESP+ESI*4+0x8]`) reads back
from the exact same `[ESP+0x8]` stack region. Whether FUN_004b3fc0 itself
WRITES through that pointer is not re-derived here beyond its prologue (see
the prologue capture under FUN_0041ad60's plate, identical callee); treat the
internal write step as [UNCERTAIN] pending a full FUN_004b3fc0 body walk.

## Callee prologue reads

Identical callee addresses/prologues to those captured under
`portcap_0x0041ad60.md` (FUN_004b3fc0, FUN_004b6520, FUN_004b5190 — same
functions, not re-disassembled here to avoid duplicate transcription; see
that file's "Callee prologue reads" section for the literal instructions).

## Decomp divergences

- `decomp_function` for 0x0041c320 shows `FUN_004b3fc0();` with no
  arguments (same elision as FUN_0041ad60), while `FUN_004b6520(unaff_EBX +
  0x80,0x80);` is shown WITH both arguments resolved correctly — confirming
  the argument-elision on FUN_004b3fc0 call sites is specific to that callee,
  not a blanket decompiler failure.
- `*(int *)(unaff_EBX + 0x100) = in_EAX;` in the decomp again names the
  ESI-preserved entry-EAX value as `in_EAX`, matching the listing's `MOV
  [EBX+0x100],ESI` @ 0x0041c332 exactly (no divergence in the value itself,
  only in the missing argument list for FUN_004b3fc0's call).

## Stubs encountered

- FUN_004b3fc0, FUN_004b6520, FUN_004b5190 — same three callees as
  FUN_0041ad60, same ABI shapes (buffer-ptr+entry-EAX; ptr+length;
  handle+0+0). Depth-1; not minted.

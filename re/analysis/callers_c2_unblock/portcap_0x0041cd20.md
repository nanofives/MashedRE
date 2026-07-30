---
rva: 0x0041cd20
name: FUN_0041cd20
size_bytes: 138
confidence_target: C2
callees_depth1: [FUN_004b3fc0, FUN_004b6520, FUN_004b5190, FUN_004b5260]
callers_noted: [0x0041d6e0]
opened_in_slot: Mashed_pool10
session_date: 2026-07-30
---

## Mechanical description

Port-grade disassembly capture of the Class D (34-atomic) ParticleEmitter
constructor. `function_at` confirms body `0041cd20..0041cdaa` (source:
`mcp__ghidra__function_at`). Same template as FUN_0041ad60/FUN_0041c320, plus
a 4-byte default-color stack buffer (`0x32,0x32,0x32,0xff`) initialized before
the FUN_004b3fc0 call, and a trailing `FUN_004b5260` call after the loop that
applies that color to the master atomic at `this+0xb8`. `this` in EBX, never
assigned inside the body; entry EAX copied to ESI @ 0x0041cd27 as in the
other two ctors.

Stack-frame layout (all offsets relative to R = ESP immediately after `SUB
ESP,0x8c` @ 0x0041cd20 and `PUSH ESI` @ 0x0041cd26):
- R+4, R+5, R+6, R+7 — the 4-byte color buffer (0x32,0x32,0x32,0xff),
  confirmed contiguous by back-computing the stack-relative address used in
  the later `LEA EAX,[ESP+8]` @ 0x0041cd94 (evaluated when only `PUSH EDI` @
  0x0041cd37 is still live, giving EAX = R+4).
- R+8 onward — the 34-entry handle buffer (address computed by `LEA EAX,[ESP
  + 0xc]` @ 0x0041cd38, evaluated when only `PUSH EDI` is live, giving EAX =
  R+8).

## Full listing

| Address | Bytes | Mnemonic/text |
|---|---|---|
| 0x0041cd20 | 81ec8c000000 | SUB ESP,0x8c |
| 0x0041cd26 | 56 | PUSH ESI |
| 0x0041cd27 | 8bf0 | MOV ESI,EAX |
| 0x0041cd29 | b032 | MOV AL,0x32 |
| 0x0041cd2b | 88442404 | MOV byte ptr [ESP + 0x4],AL |
| 0x0041cd2f | 88442405 | MOV byte ptr [ESP + 0x5],AL |
| 0x0041cd33 | 88442406 | MOV byte ptr [ESP + 0x6],AL |
| 0x0041cd37 | 57 | PUSH EDI |
| 0x0041cd38 | 8d44240c | LEA EAX,[ESP + 0xc] |
| 0x0041cd3c | 50 | PUSH EAX |
| 0x0041cd3d | 56 | PUSH ESI |
| 0x0041cd3e | c6442413ff | MOV byte ptr [ESP + 0x13],0xff |
| 0x0041cd43 | e878720900 | CALL 0x004b3fc0 |
| 0x0041cd48 | 89b350010000 | MOV dword ptr [EBX + 0x150],ESI |
| 0x0041cd4e | 8b4e04 | MOV ECX,dword ptr [ESI + 0x4] |
| 0x0041cd51 | 8d93b0000000 | LEA EDX,[EBX + 0xb0] |
| 0x0041cd57 | 68a0000000 | PUSH 0xa0 |
| 0x0041cd5c | 52 | PUSH EDX |
| 0x0041cd5d | 898b54010000 | MOV dword ptr [EBX + 0x154],ECX |
| 0x0041cd63 | e8b8970900 | CALL 0x004b6520 |
| 0x0041cd68 | 83c410 | ADD ESP,0x10 |
| 0x0041cd6b | 33f6 | XOR ESI,ESI |
| 0x0041cd6d | 8d4900 | LEA ECX,[ECX]  (alignment filler) |
| 0x0041cd70 | 8b7cb40c | LAB_0041cd70: MOV EDI,dword ptr [ESP + ESI*0x4 + 0xc] |
| 0x0041cd74 | 6a00 | PUSH 0x0 |
| 0x0041cd76 | 6a00 | PUSH 0x0 |
| 0x0041cd78 | 57 | PUSH EDI |
| 0x0041cd79 | e812840900 | CALL 0x004b5190 |
| 0x0041cd7e | 83c40c | ADD ESP,0xc |
| 0x0041cd81 | 46 | INC ESI |
| 0x0041cd82 | 83fe22 | CMP ESI,0x22 |
| 0x0041cd85 | 89bc83b0000000 | MOV dword ptr [EBX + EAX*0x4 + 0xb0],EDI |
| 0x0041cd8c | 7ce2 | JL 0x0041cd70 |
| 0x0041cd8e | 8b8bb8000000 | MOV ECX,dword ptr [EBX + 0xb8] |
| 0x0041cd94 | 8d442408 | LEA EAX,[ESP + 0x8] |
| 0x0041cd98 | 50 | PUSH EAX |
| 0x0041cd99 | 51 | PUSH ECX |
| 0x0041cd9a | e8c1840900 | CALL 0x004b5260 |
| 0x0041cd9f | 83c408 | ADD ESP,0x8 |
| 0x0041cda2 | 5f | POP EDI |
| 0x0041cda3 | 5e | POP ESI |
| 0x0041cda4 | 81c48c000000 | ADD ESP,0x8c |
| 0x0041cdaa | c3 | RET |

## Callee ABI

### CALL 0x004b3fc0 @ 0x0041cd43
- Live-in setup: `LEA EAX,[ESP + 0xc]` @ 0x0041cd38 (34-entry buffer address,
  = R+8); `PUSH EAX` @ 0x0041cd3c (pushed FIRST → deepest arg); `PUSH ESI` @
  0x0041cd3d (entry-EAX value, pushed SECOND → topmost arg). `MOV [ESP +
  0x13],0xff` @ 0x0041cd3e (writing the 4th color byte at physical address
  R+7) is interleaved between the pushes and the call, same as the other two
  ctors' pattern of interleaving unrelated stores around the call-arg setup.
- Post-call consumption: `MOV [EBX+0x150],ESI` @ 0x0041cd48 — stores ESI
  (preserved entry-EAX), not the post-call EAX. Same pattern as the other two
  ctors; see Decomp divergences.
- Stack adjust: none immediately; batched with the next call.

### CALL 0x004b6520 @ 0x0041cd63
- Live-in setup: `LEA EDX,[EBX + 0xb0]` @ 0x0041cd51 (handle-table region at
  `this+0xb0`); `PUSH 0xa0` @ 0x0041cd57 (immediate 0xa0 = 160 decimal, pushed
  FIRST → deepest arg); `PUSH EDX` @ 0x0041cd5c (pushed SECOND → topmost arg =
  pointer). `MOV [EBX+0x154],ECX` @ 0x0041cd5d (storing `*(clump+4)`, ECX read
  @ 0x0041cd4e) interleaved between pushes and call.
- Post-call consumption: none.
- Stack adjust: `ADD ESP,0x10` @ 0x0041cd68 — 16 bytes, batched cleanup for
  both this call and FUN_004b3fc0's call.

### CALL 0x004b5190 @ 0x0041cd79 (loop body, 34 iterations)
- Live-in setup: `MOV EDI,[ESP + ESI*0x4 + 0xc]` @ 0x0041cd70 (indexed read
  of the same buffer passed to FUN_004b3fc0, offset +0xc matches R+8 given
  ESP is R-4 throughout the loop since PUSH EDI @ 0x0041cd37 is still live);
  `PUSH 0x0` @ 0x0041cd74; `PUSH 0x0` @ 0x0041cd76; `PUSH EDI` @ 0x0041cd78
  (pushed LAST → topmost arg = handle value).
- Post-call consumption: EAX preserved across `ADD ESP,0xc` (0x0041cd7e),
  `INC ESI` (0x0041cd81), `CMP ESI,0x22` (0x0041cd82), then used at `MOV
  [EBX + EAX*0x4 + 0xb0],EDI` @ 0x0041cd85 (store handle at
  `this+0xb0+index*4`).
- Stack adjust: `ADD ESP,0xc` @ 0x0041cd7e — 12 bytes, immediate.

### CALL 0x004b5260 @ 0x0041cd9a (post-loop, once)
- Live-in setup: `MOV ECX,[EBX + 0xb8]` @ 0x0041cd8e (the master-atomic
  handle, `this+0xb8`); `LEA EAX,[ESP + 0x8]` @ 0x0041cd94 (address of the
  4-byte color buffer, = R+4, evaluated while only `PUSH EDI` is live so
  `[ESP+8]` = `(R-4)+8` = R+4); `PUSH EAX` @ 0x0041cd98 (color-buffer address,
  pushed FIRST → deepest arg); `PUSH ECX` @ 0x0041cd99 (atomic handle, pushed
  SECOND → topmost arg).
- Decomp confirms parameter order as `FUN_004b5260(*(this+0xb8), &local_8c)`
  i.e. (atomicHandle, colorPtr) — matches: ECX (atomic, last-pushed) is
  parameter 1, EAX (color ptr, first-pushed) is parameter 2, consistent with
  `mcp__ghidra__decomp_function` output for this RVA.
- Post-call consumption: none (function returns void after this call).
- Stack adjust: `ADD ESP,0x8` @ 0x0041cd9f — 8 bytes, immediate (this call is
  not batched with any other, unlike the FUN_004b3fc0/FUN_004b6520 pair).

## U-7930 resolution

Same mechanism as U-7917/U-7925. `LEA EAX,[ESP + 0xc]` @ 0x0041cd38 computes
the 34-entry buffer address (R+8); `PUSH EAX` @ 0x0041cd3c pushes it as the
deepest (first-pushed) stack argument to `CALL 0x004b3fc0` @ 0x0041cd43,
ahead of `PUSH ESI` @ 0x0041cd3d (entry-EAX value, topmost arg). The loop @
0x0041cd70 (`MOV EDI,[ESP+ESI*4+0xc]`) reads back from the identical stack
offset. As with the other two ctors, whether FUN_004b3fc0 WRITES through this
pointer is not re-derived here beyond its prologue (identical callee,
prologue captured once under `portcap_0x0041ad60.md`); treat the internal
write as [UNCERTAIN] pending a full FUN_004b3fc0 body walk.

## Callee prologue reads

FUN_004b3fc0, FUN_004b6520, FUN_004b5190 — identical addresses/prologues to
those captured under `portcap_0x0041ad60.md`; not re-disassembled here.

### FUN_004b5260 (first 5 instructions, new callee for this ctor)
| Address | Bytes | Text |
|---|---|---|
| 0x004b5260 | 83ec44 | SUB ESP,0x44 |
| 0x004b5263 | 56 | PUSH ESI |
| 0x004b5264 | b0ff | MOV AL,0xff |
| 0x004b5266 | 57 | PUSH EDI |
| 0x004b5267 | 8b7c2450 | MOV EDI,dword ptr [ESP + 0x50] |

Reads: after `SUB ESP,0x44` + 2 pushes (ESI, EDI = 8 bytes), `[ESP+0x50]`
resolves to entry_ESP+4 — the LAST-pushed caller argument, which in this
caller was `PUSH ECX` @ 0x0041cd99 (the atomic handle). So EDI = atomic
handle, matching the decomp's parameter order `FUN_004b5260(atomicHandle,
colorPtr)`. The color-ptr argument (entry_ESP+8) is not yet read in these 5
instructions. A harness stub for FUN_004b5260 must honor EDI = atomic
handle from the first read.

## Decomp divergences

- `decomp_function` for 0x0041cd20 shows `FUN_004b3fc0();` with no arguments
  (same elision as the other two ctors), while `FUN_004b6520(unaff_EBX +
  0xb0,0xa0);` and `FUN_004b5260(*(undefined4 *)(unaff_EBX + 0xb8),
  &local_8c);` both show fully resolved arguments — again isolating the
  elision to FUN_004b3fc0 call sites specifically.
- Decomp models the 4-byte color buffer as four separate byte locals
  (`local_8c`, `local_8b`, `local_8a`, `local_89`) rather than one array; the
  listing confirms these are in fact contiguous (R+4..R+7) via the
  cross-referenced `LEA EAX,[ESP+8]` @ 0x0041cd94 address computation, so the
  decompiler's split into 4 separate byte locals is a decomp-level naming
  choice, not evidence of non-contiguity.
- Decomp's `iVar3 < 0x22` loop bound (34 decimal) matches the listing's `CMP
  ESI,0x22` @ 0x0041cd82 exactly — no divergence there.

## Stubs encountered

- FUN_004b3fc0, FUN_004b6520, FUN_004b5190 (same ABI as the other two ctors),
  plus FUN_004b5260 (set-atomic-color; args ECX=atomicHandle [read first],
  EAX=&colorBuf [4 bytes, BGRA-order 0x32,0x32,0x32,0xff in this caller]).
  Depth-1; not minted.

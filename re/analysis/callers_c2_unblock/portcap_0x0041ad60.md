---
rva: 0x0041ad60
name: FUN_0041ad60
size_bytes: 71
confidence_target: C2
callees_depth1: [FUN_004b3fc0, FUN_004b6520, FUN_004b5190]
callers_noted: [0x0041b450]
opened_in_slot: Mashed_pool10
session_date: 2026-07-30
---

## Mechanical description

Port-grade disassembly capture of the Class A (17-atomic) ParticleEmitter
constructor, superseding the elided decomp in
`re/analysis/bucket_gameplay_0041a980_0041d910/0041ad60.md` for the purpose of
resolving callee-call ABI (U-7917). `function_at` confirms body
`0041ad60..0041ada7` (source: `mcp__ghidra__function_at`, body_start=0041ad60,
body_end=0041ada7). `this` arrives in EBX, never assigned inside the function
body (matches decomp's `unaff_EBX`). A second incoming value arrives in EAX at
entry and is immediately copied to ESI at 0x0041ad64 — this is the "clump"
value the decomp calls `in_EAX`; it is preserved across the FUN_004b3fc0 call
in ESI and later stored directly, i.e. the clump value is a caller-supplied
argument to this ctor, NOT a return value read out of FUN_004b3fc0 (see
Decomp divergences).

## Full listing

| Address | Bytes | Mnemonic/text |
|---|---|---|
| 0x0041ad60 | 83ec44 | SUB ESP,0x44 |
| 0x0041ad63 | 56 | PUSH ESI |
| 0x0041ad64 | 8bf0 | MOV ESI,EAX |
| 0x0041ad66 | 57 | PUSH EDI |
| 0x0041ad67 | 8d442408 | LEA EAX,[ESP + 0x8] |
| 0x0041ad6b | 50 | PUSH EAX |
| 0x0041ad6c | 56 | PUSH ESI |
| 0x0041ad6d | e84e920900 | CALL 0x004b3fc0 |
| 0x0041ad72 | 89735c | MOV dword ptr [EBX + 0x5c],ESI |
| 0x0041ad75 | 8b4e04 | MOV ECX,dword ptr [ESI + 0x4] |
| 0x0041ad78 | 6a50 | PUSH 0x50 |
| 0x0041ad7a | 53 | PUSH EBX |
| 0x0041ad7b | 894b60 | MOV dword ptr [EBX + 0x60],ECX |
| 0x0041ad7e | e89db70900 | CALL 0x004b6520 |
| 0x0041ad83 | 83c410 | ADD ESP,0x10 |
| 0x0041ad86 | 33f6 | XOR ESI,ESI |
| 0x0041ad88 | 8b7cb408 | LAB_0041ad88: MOV EDI,dword ptr [ESP + ESI*0x4 + 0x8] |
| 0x0041ad8c | 6a00 | PUSH 0x0 |
| 0x0041ad8e | 6a00 | PUSH 0x0 |
| 0x0041ad90 | 57 | PUSH EDI |
| 0x0041ad91 | e8faa30900 | CALL 0x004b5190 |
| 0x0041ad96 | 83c40c | ADD ESP,0xc |
| 0x0041ad99 | 46 | INC ESI |
| 0x0041ad9a | 83fe11 | CMP ESI,0x11 |
| 0x0041ad9d | 893c83 | MOV dword ptr [EBX + EAX*0x4],EDI |
| 0x0041ada0 | 7ce6 | JL 0x0041ad88 |
| 0x0041ada2 | 5f | POP EDI |
| 0x0041ada3 | 5e | POP ESI |
| 0x0041ada4 | 83c444 | ADD ESP,0x44 |
| 0x0041ada7 | c3 | RET |

## Callee ABI

### CALL 0x004b3fc0 @ 0x0041ad6d
- Live-in setup: `LEA EAX,[ESP + 0x8]` @ 0x0041ad67 (address of the 17-entry
  stack buffer, the frame slot immediately above the 0x44-byte SUB); `PUSH EAX`
  @ 0x0041ad6b (pushed FIRST → deepest stack arg); `PUSH ESI` @ 0x0041ad6c
  (ESI = entry-EAX value, pushed SECOND → topmost/closest-to-ESP stack arg at
  call time).
- Callee ABI (from FUN_004b3fc0 prologue, see below): reads the buffer address
  from `[ESP+0x14]` post-SUB, which maps to the deeper/first-pushed arg — i.e.
  the buffer pointer (`LEA EAX,[ESP+0x8]` result) is argument 2 by push order,
  the ESI/entry-EAX value is argument 1.
- Post-call consumption: EAX (FUN_004b3fc0's own return value) is NOT read at
  all after the call — the next instruction `MOV [EBX+0x5c],ESI` @ 0x0041ad72
  stores ESI (preserved entry-EAX), not EAX. See Decomp divergences.
- Stack adjust: none immediately after this call; cleanup is deferred and
  batched with the next call (see 0x0041ad83).

### CALL 0x004b6520 @ 0x0041ad7e
- Live-in setup: `PUSH 0x50` @ 0x0041ad78 (immediate 0x50 = 80 decimal, pushed
  FIRST → deepest arg); `PUSH EBX` @ 0x0041ad7a (`this`, offset +0, pushed
  SECOND → topmost arg). Between the two pushes and the call, `MOV
  [EBX+0x60],ECX` @ 0x0041ad7b stores ECX (= `[ESI+4]`, read at 0x0041ad75,
  i.e. `*(clump+4)`) into `this+0x60` — this store is interleaved with call
  setup, not part of it.
- Post-call consumption: none of EAX/ECX/EDX read after this call.
- Stack adjust: `ADD ESP,0x10` @ 0x0041ad83 — 16 bytes, cleans BOTH this call's
  2 args (8B) AND the FUN_004b3fc0 call's 2 args (8B) in one batched
  cdecl-style cleanup covering both calls.

### CALL 0x004b5190 @ 0x0041ad91 (loop body, 17 iterations)
- Live-in setup: `MOV EDI,[ESP + ESI*0x4 + 0x8]` @ 0x0041ad88 (loop-indexed
  read of the SAME stack buffer passed to FUN_004b3fc0 at `[ESP+0x8]`, ESI =
  loop counter 0..0x10); `PUSH 0x0` @ 0x0041ad8c; `PUSH 0x0` @ 0x0041ad8e;
  `PUSH EDI` @ 0x0041ad90 (pushed LAST → topmost arg = the handle value read
  from the buffer).
- Post-call consumption: EAX (return value, `iVar2`) is preserved across `ADD
  ESP,0xc` (0x0041ad96), `INC ESI` (0x0041ad99), and `CMP ESI,0x11`
  (0x0041ad9a) with no intervening EAX write, then consumed at `MOV [EBX +
  EAX*0x4],EDI` @ 0x0041ad9d (store handle EDI at index EAX into `this+0`).
- Stack adjust: `ADD ESP,0xc` @ 0x0041ad96 — 12 bytes, cleans this call's 3
  args immediately (not batched).

## U-7917 resolution

RESOLVED by the listing. The 17-entry buffer is not filled through an elided
global or hidden write path — its address is computed with `LEA EAX,[ESP +
0x8]` @ 0x0041ad67 and passed as the second (deeper-pushed) stack argument to
`CALL 0x004b3fc0` @ 0x0041ad6d (`PUSH EAX` @ 0x0041ad6b, before `PUSH ESI` @
0x0041ad6c). FUN_004b3fc0's own prologue reads this same address at
`[ESP+0x14]` (`MOV EAX,[ESP+0x14]` @ 0x004b3fc3, see Callee prologue reads) —
i.e. FUN_004b3fc0 receives a pointer to this stack region and (per its
prologue) treats it as an out-parameter it writes into. The ctor then reads
the filled entries directly out of the SAME stack slot at `[ESP+ESI*4+0x8]`
inside the loop @ 0x0041ad88 — there is no separate `local_44` copy step; the
loop reads the buffer FUN_004b3fc0 was handed. Full internal proof that
FUN_004b3fc0 writes through this pointer (rather than merely reading it) is
not walked past its first ~10 instructions here — see Callee prologue reads
below and treat the write itself as [UNCERTAIN] pending a deeper walk of
FUN_004b3fc0's body.

## Callee prologue reads

### FUN_004b3fc0 (first 5 instructions)
| Address | Bytes | Text |
|---|---|---|
| 0x004b3fc0 | 83ec0c | SUB ESP,0xc |
| 0x004b3fc3 | 8b442414 | MOV EAX,dword ptr [ESP + 0x14] |
| 0x004b3fc7 | 8d542404 | LEA EDX,[ESP + 0x4] |
| 0x004b3fcb | 52 | PUSH EDX |
| 0x004b3fcc | 89442408 | MOV dword ptr [ESP + 0x8],EAX |

Reads: `[ESP+0x14]` post-SUB = the buffer-pointer argument (arg2 by push
order from the caller); stores it into a local stack slot at `[ESP+0x8]` and
also takes `LEA EDX,[ESP+4]` (address of another local slot) before pushing
EDX — consistent with forwarding both the buffer pointer and a second local
to a deeper call. Not walked further; the arg-1 (ESI/entry-EAX) value is not
yet read in these 5 instructions.

### FUN_004b6520 (first 5 instructions)
| Address | Bytes | Text |
|---|---|---|
| 0x004b6520 | 8b442408 | MOV EAX,dword ptr [ESP + 0x8] |
| 0x004b6524 | 8b4c2404 | MOV ECX,dword ptr [ESP + 0x4] |
| 0x004b6528 | 50 | PUSH EAX |
| 0x004b6529 | 6a00 | PUSH 0x0 |
| 0x004b652b | 51 | PUSH ECX |

Reads: `[ESP+8]` (the deeper/first-pushed arg = the length immediate, e.g.
0x50 from this caller) into EAX; `[ESP+4]` (the topmost/last-pushed arg = the
pointer, e.g. EBX from this caller) into ECX. Both are read before any
branch/call in the 5 instructions shown; a harness stub for FUN_004b6520 must
honor EAX=length, ECX=pointer.

### FUN_004b5190 (first 5 instructions)
| Address | Bytes | Text |
|---|---|---|
| 0x004b5190 | 8b442404 | MOV EAX,dword ptr [ESP + 0x4] |
| 0x004b5194 | 56 | PUSH ESI |
| 0x004b5195 | 8b7018 | MOV ESI,dword ptr [EAX + 0x18] |
| 0x004b5198 | 56 | PUSH ESI |
| 0x004b5199 | e8a2eb0800 | CALL 0x00543d40 |

Reads: `[ESP+4]` (the topmost/last-pushed arg = the handle value, EDI from
this caller) into EAX at entry; immediately dereferences it as a pointer at
`[EAX+0x18]` into ESI, then calls 0x00543d40 with ESI pushed. A harness stub
for FUN_004b5190 must honor EAX as a pointer valid to dereference at +0x18.

## Decomp divergences

- `decomp_function` for 0x0041ad60 shows `FUN_004b3fc0();` with NO arguments
  and `int in_EAX;` used directly for the clump value (`*(int
  *)(unaff_EBX + 0x5c) = in_EAX;`). The listing shows this is not an artifact
  of a return value — `in_EAX` in the decomp corresponds exactly to the
  ESI-preserved entry-EAX value (never overwritten by the CALL), and the
  decompiler's elision of FUN_004b3fc0's two stack arguments (buffer pointer,
  entry-EAX value) is the literal source of U-7917's ambiguity: the decomp
  gives no hint that the buffer address is passed by reference to
  FUN_004b3fc0 at all.
- Every stack-passed argument to all three callees (FUN_004b3fc0,
  FUN_004b6520 in this function, FUN_004b5190) is omitted from the decomp's
  call expressions in this function's decomp (`FUN_004b3fc0()`,
  `FUN_004b6520()`, `FUN_004b5190(uVar1,0,0)` — the latter DOES show its
  args, so the elision is call-site-specific, not systematic across all
  three).

## Stubs encountered

- FUN_004b3fc0 (clump/handle-buffer builder, args: buffer ptr, entry-EAX
  value), FUN_004b6520 (zero-fill/init, args: ptr, length), FUN_004b5190
  (handle→index, args: handle ptr, 0, 0; dereferences handle at +0x18, calls
  0x00543d40). Depth-1; not minted.

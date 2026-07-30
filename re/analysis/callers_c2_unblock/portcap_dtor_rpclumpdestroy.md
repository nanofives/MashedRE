---
rvas: [0x0041b440, 0x0041beb0, 0x0041cb00]
name: ParticleEmitter RpClumpDestroy destructors
size_bytes: 11 (each)
confidence_target: C2->C3
callees_depth1: [FUN_004e6e00 (RpClumpDestroy)]
opened_in_slot: Mashed_pool10
session_date: 2026-07-30
---

## Mechanical description

Three identical-shape particle-emitter destructors, `this` in **EAX**. Each
reads a clump handle from a fixed `this` offset and tail-calls
`RpClumpDestroy` (FUN_004e6e00). Full disassembly captured live via
`listing_disassemble_function` (Mashed_pool10, orch-iter8).

| RVA | body | clump offset | listing |
|---|---|---|---|
| 0x0041b440 | 0041b440..0041b44a | `this+0x5c`  | `MOV ECX,[EAX+0x5c];  PUSH ECX; CALL 0x004e6e00; POP ECX; RET` |
| 0x0041beb0 | 0041beb0..0041bebd | `this+0x15c` | `MOV ECX,[EAX+0x15c]; PUSH ECX; CALL 0x004e6e00; POP ECX; RET` |
| 0x0041cb00 | 0041cb00..0041cb0d | `this+0x100` | `MOV ECX,[EAX+0x100]; PUSH ECX; CALL 0x004e6e00; POP ECX; RET` |

- `this` arrives in EAX; never written.
- Single argument to RpClumpDestroy = `*(this + off)`, pushed via `PUSH ECX`.
- `POP ECX` after the call is the cdecl caller-side cleanup of the one arg
  (equivalent to `ADD ESP,4`). RpClumpDestroy is cdecl.
- Void return; the ONLY observable effect is the RpClumpDestroy call and its
  argument. Hence verified via the `reg_this_call_observe` A/B handler which
  records the stubbed RpClumpDestroy's first argument and diffs it (a wrong
  offset in the port records a different value → RED).

The clump offsets match the corresponding ctor writes: 0x5c = CtorA's clump
(0x0041ad60), 0x100 = CtorC's clump (0x0041c320); 0x15c is the destructor
counterpart for a 0x15x-family emitter.

## Callee

- FUN_004e6e00 = **RpClumpDestroy** (named RenderWare API; C1 by library-skip
  policy). Satisfies the C3 callee-half via the **identified-callee clause**
  (re/CONFIDENCE.md ruling 2026-07-30) — a named library callee is understood
  context, not an island.

## Callers (E8 byte scan, orch-iter5)

- 0x0041b440 ← FUN_0041b660 (C2) · 0x0041beb0 ← FUN_0041c0e0 (C2) ·
  0x0041cb00 ← FUN_0041ccf0 (C2). Caller-gate satisfied.

## Uncertainties

- (none) — the functions are fully resolved 11-byte tail-call destructors.

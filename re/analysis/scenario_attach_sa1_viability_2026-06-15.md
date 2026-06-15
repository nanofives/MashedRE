# c3-batch-sa1 session 1 — viability audit (2026-06-15)

Session goal: promote 5 HUD leaves C2→C3 via scenario-attach (live-race) Frida
diffs. Outcome: **0 of 5 cleanly promotable with the current harness.** Every
candidate (and the assessed backfills) is blocked by a structural property that
the disassembly reveals — not by being "dirty code." Evidence below is from
Ghidra `Mashed_pool10` (read-only), MASHED.exe anchor
`BDCAE093…EFD3C0E`. NO-GUESSING: every claim cites an RVA + the literal
decompilation/listing.

No `.cpp` was authored, no build/registry change made, no diff forced. Per the
prompt's STOP-AND-ASK rule ("a candidate needs a NEW arg_type … queue it, don't
invent inline") and the no-overclaiming ban (never force a degenerate/false
GREEN), this is a stop-and-report.

## Per-candidate determination

### 1. 0x0041d870 — NON-VIABLE (draw dispatcher, no memory observable)
Listing: `PUSH ESI; MOV ESI,0x63d298; LAB: MOV EAX,ESI; CALL 0x0041d410;
ADD ESI,0x160; CMP ESI,0x63d558; JL LAB; POP ESI; RET`. 2-iteration EAX-thiscall
loop over the array at 0x0063d298 (stride 0x160), calling FUN_0041d410 with
EAX = entry pointer.
Callee FUN_0041d410 (0x0041d410) decompiles to a pure **vtable[0x48] draw
dispatcher**: it reads ~33 enable-flag fields of the entry object (`+0x1c`,
`+0x18`, … `+0x38`) and, for each set flag, calls `(**(code**)(*(entry+0xNN)+0x48))`
on a member sub-object. It writes **no observable global**. Its only effect is
GPU draw commands.
→ `state_machine_observe` has no legitimate `output_global`. Observing an
unwritten-but-nonzero race global would be a **false GREEN** (exactly the
DEGENERATE_GREEN class). Not promotable via run_diff.

### 2. 0x0041e850 — NON-VIABLE (draw dispatcher, no memory observable)
Listing: `MOV EAX,[0x63d7e0]; TEST EAX,EAX; JZ ret; PUSH EDI; MOV EDI,0x63d610;
CALL 0x0041e630; POP EDI; RET`. Guard on `[0x0063d7e0]`; when set, sets
**EDI=0x63d610** (implicit register arg the decompiler hides) and calls
FUN_0041e630.
Callee FUN_0041e630 (0x0041e630) is **also a pure vtable[0x48] draw dispatcher**
(reads `unaff_EDI` struct fields `+0x16c/+0x170/+0x174`, then a 6-iteration loop
dispatching `vtable[0x48]` on sub-objects selected by `+0x188` indices). Writes
**no observable global**.
→ Same as #1. The guard (`[0x63d7e0]`) is a clean input, but there is no clean
output to observe. Not promotable via run_diff.

### 3. 0x00413bb0 — NON-VIABLE (destructive teardown / double-free)
Listing: `PUSH 0x63bd50; CALL 0x004768c0; POP ECX; RET` → FUN_004768c0(&DAT_0063bd50).
Callee FUN_004768c0 (0x004768c0) is a **destructor**: reads `*(param_1+4)`
(a live RW resource handle), calls FUN_004e7e30 / FUN_004c0c20 / FUN_004e6920
(RW resource releases), `vtable[0x10c]` frees on `+0x10..+0x28`, then
`FUN_004b6520(param_1, 0x40)` frees the 0x40-byte descriptor.
→ A/B diffing calls it twice back-to-back on the same `&DAT_0063bd50`. The
second (reimpl) call sees the freed/zeroed descriptor → divergent path / crash.
The harness save/restore covers only declared globals, not the freed heap.
Not promotable via run_diff.

### 4. 0x00413b80 — MARGINAL / WEAK + RISKY (heavy RW initializer)
Listing (cdecl, ESP cleaned 0x20 at end):
`FUN_0040bb30(0x5cd064)` → tex handle; `FUN_004770c0(&DAT_0063bd50,0x37,0x14,tex)`;
`FUN_00476cb0(&DAT_0063bd50,5,6)`. `0x5cd064` = the ASCII string "VehicleIcons".
Callee FUN_004770c0 (0x004770c0) is a heavy RW particle/sprite-batch init:
`FUN_004b6520(param_1,0x40)` zero; `FUN_00534b60(0x14,flags,0)` **allocates** and
stores a **non-deterministic heap handle** in `param_1[1]`; writes ~13 global
RW-state words (DAT_00692564 |= 0x20003, _DAT_00692580 = 0x3f800000, …);
sub-allocates `param_1[4..10]` (more **non-deterministic handles**); vtable/
resource attach via the live handle.
Only `*param_1` (=0x37) and `param_1[2]` (=0x14) are deterministic constants.
→ A `state_machine_observe` on `output_globals=[0x63bd50:u32, 0x63bd58:u32]`
would GREEN, but it verifies only that the two immediates (0x37, 0x14) reach the
installer — a **weak** check that ignores the bulk of the function. Worse, it
calls a heavy RW initializer **twice in a live race** (leaks 2× resources,
re-touches global RW state, derefs freshly-allocated handles) → real
instability/crash risk and live-state pollution the harness can't restore.
Not shipped without an explicit ruling that the weak+risky evidence is acceptable.

### 5. 0x00413bc0 — BLOCKED on a NEW arg_type (genuinely promotable leaf)
PURE, deterministic leaf. Convention from the listing: **index in ESI,
out-pointer in ECX** (writes 4 floats to `[ECX..ECX+0xc]`). cell =
`[0x005cd060]` = `0x3e800000` = **0.25f** (read from .rdata; the next bytes are
"Vehi…"). Semantics:
- `ESI < 0 || ESI >= 5` → `{0.0, 0.0, 1.0, 1.0}` (full-texture quad).
- `ESI == 3` → `{0.255, 0.255, 0.995, 0.995}` (0x3e828f5c / 0x3f7eb852; inset).
- else → 4-wide-atlas UV rect:
  `out[0]=(ESI&3)*0.25; out[1]=(ESI>>2)*0.25; out[2]=((ESI&3)+1)*0.25; out[3]=((ESI>>2)+1)*0.25`
  (SAR with the `+3 & 3` round-toward-zero adjust; ESI≥0 here so it's `/4`).
This diffs GREEN at **any** state (no live-state needed). The only blocker is
the calling convention:
- `int_outbuf4` calls `Orig(idx, buf)` as a **cdecl** NativeFunction (stack
  args). The original reads ESI/ECX → would read garbage. ✗
- `fastcall_reg` sets only **ECX/EDX** (`mov ecx,imm; mov edx,imm`) and captures
  **EAX** as the return. The leaf needs **ESI** set and returns via the **ECX
  out-buffer**. ✗
→ Needs a new handler. Spec below.

## Backfills assessed (also blocked)

- **Camera predicates 0x0047c1f0 (+ 0x0047c230/270/2d0 family)** — listing of
  0x0047c1f0: `MOV EBP,[ESP+8]` (one stack arg), `TEST EBX,EBX` (count in
  **EBX**), `MOV EDI,EAX` (base in **EAX**), loop `CALL 0x00478030(EDI,EBP)`
  derefing the base+i*0x10. `fastcall_reg` can't set EAX/EBX, and a seeded base
  AVs in FUN_00478030. → new arg_type (EAX base + EBX count + 1 stack arg) **and**
  a valid race-state array pointer.
- **AI grid 0x004150e0 (+ 0x00443d10)** — decomp: consumes **two x87 float
  args** (`FUN_004a2c48()` ×2), indexes the race-state track grids `DAT_007f1a9c`
  / `DAT_007f9a9c`, returns 0/1. → needs a float-pair (x87/fastcall) arg_type
  and a loaded track. No existing arg_type passes x87 float args.

## New arg_type specs needed (for a harness-ext session — NOT invented here)

1. `esi_idx_ecx_outbuf4` (unblocks #5, high value, deterministic):
   trampoline per side `MOV ESI,imm32(idx); MOV ECX,<scratch16>; JMP target`;
   zero a 16-byte scratch per test; after the call read 4×u32 from scratch as the
   packed fingerprint; tests = scalar indices `[-1,0,1,2,3,4,5,6,…]` (incl. 3 and
   out-of-range). Return is void. **SWEEP-CRITICAL** (diff_template.js is not
   auto-merged by frida-sweep).
2. `eax_base_ebx_count_stack1` (unblocks the camera-predicate family): set
   EAX=base ptr, EBX=count, push 1 stack arg; needs a valid base array — only
   meaningful scenario-attach with the array populated.
3. `x87_float_pair_ret_int` (unblocks the ai-grid pair): pass two floats on the
   x87 stack, capture EAX; scenario-attach with a loaded track.

## Recommendation

Highest-value, lowest-risk path: a small harness-ext session that adds
`esi_idx_ecx_outbuf4`, then promote **0x00413bc0** (clean, deterministic, GREEN
at any state). Defer 0x0041d870 / 0x0041e850 (draw dispatchers — no memory
observable; candidates for a draw-callee-intercept arg_type or DEFERRED) and
0x00413bb0 (destructive — DEFERRED). 0x00413b80 only if a weak constant-field
diff is explicitly accepted. Re-selecting scenario-attach candidates that fit
EXISTING arg_types (scalar/ECX-EDX value-return predicates over the unlocked-66
list) is the alternative that needs no harness change.

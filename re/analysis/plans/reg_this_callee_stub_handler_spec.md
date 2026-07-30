# Handler spec — `reg_this_callee_stub` (render EBX/ESI/EAX ctor+dispatch family)

Authored orch-iter5 (2026-07-30). Decision: user chose the **callee-stub handler**
over capture-replay for the 7 live-RW-callee render rows the read-fleet flagged
NEEDS_NEW_HANDLER (brief `re/orchestrator/read_fleet/runs/20260730_103452/argtype_render_b2.md`).

**This is SWEEP-CRITICAL** — it is the FIRST handler that intercepts *callees* rather
than seeding inputs. `frida-sweep` does not auto-merge `diff_template.js`; the handler +
`run_diff_scenario_batch.py` forwarding + `ARG_TYPES.md` regen must all merge to main
before any promoted row re-verifies at sweep.

## Why a new handler (evidence)

The family takes its `this` in a register (EBX for the 3 ctors, ESI/EAX for the
dispatch/destructor variants) and calls into the **live RenderWare object graph**, so no
existing seed-only handler makes them deterministic:
- `FUN_004b3fc0` builds a clump by dereferencing it and enumerating atomics.
- `FUN_004b5190` derefs `[handle+0x18]` and calls `0x00543d40`.
- `FUN_004b5260` applies colour to a live atomic handle.

Fix: seed the register-`this` + a scratch clump, and **Interceptor.replace** the live-RW
callees with deterministic stubs, so the ctor runs cold and produces a fixed `this`-struct
that a correct port reproduces bit-for-bit.

## Proving case — `0x0041ad60` (17-atomic Class A ctor) ONLY for iter6

Facts from `re/analysis/callers_c2_unblock/portcap_0x0041ad60.md`:
- `this` in **EBX** (caller-set); entry-EAX preserved into ESI = the "clump" arg.
- `CALL FUN_004b3fc0` @ 0x0041ad6d — stack at callee entry: `[ESP+4]=clump (ESI)`,
  `[ESP+8]=buffer (LEA [ESP+8], the 17-entry stack buffer)`. Fills the buffer with 17
  atomic handles. **Its EAX return is discarded.**
- `MOV [EBX+0x5c], ESI` (clump) ; `MOV [EBX+0x60], *(ESI+4)` (clump frame/root).
- `CALL FUN_004b6520(this=EBX, len=0x50)` @ 0x0041ad7e — zero-fills a slot region.
- loop 0..16: `handle=buf[i]; idx=FUN_004b5190(handle,0,0); *(this+idx*4)=handle`.
- `this` record is one 0x74-byte slot of DAT_0063c8d0.

## Handler design

### Trampoline (per side; EBX preserved because it is callee-saved under mscdecl)
```
53                push ebx                      ; save harness EBX
BB ?? ?? ?? ??    mov  ebx, imm32(scratchThis)  ; patched once per side (constant addr)
B8 ?? ?? ?? ??    mov  eax, imm32(scratchClump)  ; patched per test (varies for non-degeneracy)
E8 ?? ?? ?? ??    call rel32 -> target           ; rel32 computed at build
5B                pop  ebx
C3                ret
```
`new NativeFunction(tramp, 'void', [], 'mscdecl')`. One trampoline per side
(`TARGET_ADDR` = 0x0041ad60 original; `reimplAddr` = the ported export). Model on the
existing `esi_idx_ecx_outbuf4` block (diff_template.js ~L2010) — same push/seed/call/pop/ret
shape, extended with the EAX seed window.

### Callee stubs (installed ONCE before the test loop, reverted after) — GLOBAL by RVA so
both the orig and reimpl trampolines hit the identical stubs.
- `Interceptor.replace(FUN_004b3fc0, new NativeCallback((clump, buf) => { for i in 0..N-1: buf.add(i*4).writeU32(HANDLE_BASE + i); return 0; }, 'int', ['pointer','pointer']))`
  — args per the stack layout above: arg0=clump, arg1=buffer.
- `Interceptor.replace(FUN_004b5190, new NativeCallback((handle, a, b) => (handle - HANDLE_BASE) & 0xffffffff, 'int', ['int','int','int']))`
  — deterministic index = handle − base, so the 17 handles scatter to slots 0..16.
- `Interceptor.replace(FUN_004b6520, new NativeCallback((thisPtr, len) => 0, 'int', ['pointer','int']))`
  — noop (the ctor’s own writes dominate; both sides identical anyway).
- (Class C `0x0041cd20` adds `FUN_004b5260(atomic, &colorBuf)` → noop stub, sig `['int','pointer']`.)

### Scratch + fingerprint
- `scratchThisO/R = Memory.alloc(structSize)` (structSize ≥ 0x74; use 0x80). Zero both before
  each test. Retain in a `_keep[]` array (Frida-GC hazard, same as struct_call_observe).
- `scratchClump = Memory.alloc(16)`; set `*(clump+4)` = a per-test frame sentinel so
  `this+0x60` is deterministic and varies across tests.
- After `FnO()` / `FnR()`, pack `scratchThis[0..structSize)` as a hex fingerprint (mirror
  `packU32x4`, generalised to structSize/4 dwords); compare O vs R.

### Non-degeneracy (multiple vectors without a scalar input)
Vary per test: `scratchClump` value, `*(clump+4)` frame sentinel, and `HANDLE_BASE`
(e.g. test k: clumpFrame=0x3000+k, HANDLE_BASE=0x1000 + k*0x100). Each yields a different
`this`-struct (17 distinct scattered handles + clump@0x5c + frame@0x60); a port with a wrong
offset, wrong loop count, or wrong stride diverges. Require ≥3 vectors, all non-degenerate.

## The port (author verbatim, EBX-this)

New TU `mashedmod/src/mashed_re/Render/ParticleEmitterCtors.cpp`. FUN_0041ad60 reads `this`
from EBX and the clump from EAX — a non-standard register convention, so write it
`__declspec(naked)` mirroring the original's prologue exactly (see the ABI-mismatch memory
[[feedback_installed_hook_abi_mismatch]] and [[feedback_diff_reimpl_asm_vs_original]] — do NOT
model contiguous stack slots as separate C++ locals). RH_ScopedInstall(…, 0x0041ad60);
export name e.g. `ParticleEmitterCtorA`. Add the TU to BOTH `build.bat` AND
`mashedmod/asi_sources.rsp` (else the .asi silently drops the hook —
[[project_asi_builds_from_rsp_not_buildbat]]).

## Registry entry (hooks_registry.py)
```
'particle_emitter_ctor_a': {
    'rva': 0x0041ad60, 'export': 'ParticleEmitterCtorA',
    'signature': {'ret': 'void', 'args': []},           # registers ARE the args
    'arg_type': 'reg_this_callee_stub', 'scenario': 'race',   # or menu — stubs remove the live-RW need; try menu first
    'this_reg': 'ebx', 'struct_size': 0x80, 'atom_count': 17,
    'callee_fill':  0x004b3fc0, 'callee_index': 0x004b5190, 'callee_zero': 0x004b6520,
    'tests': [ {'clump_frame': 0x3000, 'handle_base': 0x1000},
               {'clump_frame': 0x3001, 'handle_base': 0x1100},
               {'clump_frame': 0x3002, 'handle_base': 0x1200} ],
}
```
(Field names illustrative — settle them when writing the handler; keep the callee RVAs in the
registry, not hard-coded, so the ESI/EAX variants reuse the handler with different RVAs.)

## Verify + gate
- Because the stubs remove the live-RW dependency, try a **menu**-scenario one-boot first
  (cheaper); fall back to race if the ctor needs any other live global.
- exec-pipeline: build + one-boot state_batch on `particle_emitter_ctor_a`.
- GREEN ≥3/3 non-degenerate → re-classify C2→C3. Caller-gate is **already satisfied**
  (all 8 family fns have C2 callers, E8-scan verified iter5: ad60←FUN_0041b450 C2).
- This is a hook-bypassed synthetic A/B → **C3, never C4** ([[feedback_no_overclaiming_c_levels]]).

## After the proving case
Generalise the same handler (RVA-parameterised `this_reg` + callee set) to the rest of the
family: EBX-ctors 0x0041c320 (24-atomic) / 0x0041cd20 (34-atomic, +colour stub); then the
EAX-RpClumpDestroy destructors 0x0041b440/beb0/cb00 (single tail-call — a simpler `call_observe`
variant: seed EAX=this with `+offset` pointing at a scratch clump, stub RpClumpDestroy to record
its arg, assert both sides pass the same pointer); then the ESI-bit-gated dispatchers
0x0041ae60/af00. Regenerate `ARG_TYPES.md` after the handler lands.

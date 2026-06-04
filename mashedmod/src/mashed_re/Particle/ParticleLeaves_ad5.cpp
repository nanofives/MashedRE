// Mashed RE — Particle-pool leaves (c3-batch-ad session 5).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// HARVEST result: 1/12 candidates promotable. The other 11 in this pool are
// DirectInput/DirectSound COM vtable-dispatch functions (E_FAIL/E_INVALIDARG
// HRESULT skip-lists, indirect vtable[N] calls on live COM interfaces),
// Win32 critical-section wrappers (EnterCriticalSection on *(p+0x14)), or void
// wrappers into uncharacterized callees — none are force-callable with
// synthetic inputs without a live COM object / initialized CRITICAL_SECTION.
// See the c3-batch-ad-s5 row in re/PROMOTION_QUEUE.md for the per-RVA skip
// rationale. This matches the explicit "DirectSound/threading SKIP" caveat in
// the batch plan.
//
// Functions in this file:
//   0x0049fa40  ConstZeroRet_0049fa40 — __stdcall u32(u32 ignored); XOR EAX,EAX; RET 4
//
// Analysis notes:
//   re/analysis/particle_promote_ad6/0x0049fa40.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// ConstZeroRet_0049fa40  --  0x0049fa40
//
// Original: FUN_0049fa40 (5 bytes, 0x0049fa40..0x0049fa44)
//
// Raw bytes (read from original\MASHED.exe.unpatched at 0x0049fa40):
//   33 C0        XOR EAX, EAX     ; return value = 0
//   C2 04 00     RET 4            ; return, pop one 4-byte stack arg (callee clean)
//
// The `RET 4` proves the function takes exactly one 4-byte stack parameter and
// uses callee stack-cleanup — i.e. it is __stdcall (or __thiscall/other
// callee-clean) taking one argument, which it ignores, and unconditionally
// returns 0. The C2 transcript (0x0049fa40.md) recorded the signature as
// `FUN_0049fa40(void)` returning 0; the byte-level `RET 4` corrects that to a
// single (ignored) 4-byte argument. The observable behavior — return 0 — is
// unchanged.
//
// No branches, no memory accesses, no side effects. Pure constant-return leaf.
//
// Calling convention: __stdcall (callee cleans 4 bytes) — matches `RET 4` so the
// inline-JMP install is ABI-correct (the reimpl emits `ret 4`, identical stack
// discipline to the original).
//
// Constants:
//   0x00000000 — return value (XOR EAX,EAX) [0x0049fa40]
//
// Uncertainties (non-blocking):
//   - The single 4-byte argument is read by neither the original nor the reimpl;
//     its semantic role at the (currently un-enumerated) call sites is unknown.
//     Does not block C3 — the function is total (returns 0 for every input), so
//     bit-identity holds for all argument values.
//
// ref: re/analysis/particle_promote_ad6/0x0049fa40.md
// ---------------------------------------------------------------------------

// 0x0049fa40
extern "C" __declspec(dllexport)
std::uint32_t __stdcall ConstZeroRet_0049fa40(std::uint32_t /*arg (ignored)*/)
{
    return 0u;  // XOR EAX,EAX ; RET 4
}

RH_ScopedInstall(ConstZeroRet_0049fa40, 0x0049fa40);

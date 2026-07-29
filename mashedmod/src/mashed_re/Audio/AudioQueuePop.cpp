// Mashed RE — audio queue pop thunk (STATE lane, race-gated, 2026-07-29).
//
// Selected by measurement, not by size: the exercise pre-screen
// (re/analysis/plans/prescreen_recipe.md) showed 0x005b0f40 is called during a
// Quick Battle race and not before it, and the shape screen
// (scripts/shape_screen.py) showed it is the ONLY one of the eight race-gated
// audio candidates that a synthetic A/B can safely call. Of the other seven:
// 0x005b8080 is CloseHandle(*(p+0xc)) and 0x005aeed0 is
// WaitForSingleObject(h, 0) — force-calling either corrupts the running game —
// and the rest dispatch through function pointers that only exist in live
// objects.
//
// Transcribed from capstone disasm of original\MASHED.exe.unpatched.
// Binary anchor: MASHED.exe size=2,846,720
//   sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// 0x005ad2e0 (C2) — consumes one entry from the queue object passed in.
// Head of it, for the record:
//   005ad2e0 mov eax,[esp+4] / mov ecx,[eax+0xc] / test ecx,ecx / jbe 0x5ad2f2
//   005ad2eb dec ecx / mov [eax+0xc],ecx / xor eax,eax / ret
// i.e. it DECREMENTS the count at +0xc when non-zero and returns 0, otherwise
// takes the 0x5ad2f2 path. That makes it state-MUTATING, so this hook is
// registered with captured live arguments and must not be replayed more times
// than the evidence needs.
using QueuePopFn = std::uint32_t(__cdecl*)(std::uint32_t);
static const QueuePopFn s_FUN_005ad2e0 =
    reinterpret_cast<QueuePopFn>(0x005ad2e0);

constexpr std::uint32_t kQueueOff = 0x28u;

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x005b0f40  AudioQueuePop(void* obj) -> forwarded result of 0x005ad2e0
//
// Verbatim 0x005b0f40..0x005b0f4e (15 bytes; a single nop at 0x005b0f4f then a
// DIFFERENT function at 0x005b0f50 — an earlier version of the shape screen
// merged the two and mis-attributed a call to 0x005b0f70):
//   005b0f40 mov eax,[esp+4]
//   005b0f44 mov ecx,[eax+0x28]
//   005b0f47 push ecx
//   005b0f48 call 0x5ad2e0
//   005b0f4d pop ecx                 ; discards the argument (not add esp,4)
//   005b0f4e ret
//
// TWO details that a decompiler would hide, both of which have burned this
// project before:
//
// 1. IMPLICIT EAX RETURN. Ghidra types this void — nothing writes EAX after the
//    call, so the callee's EAX IS this function's return value. Declaring it
//    void is exactly the U-9025 defect (memory project_u9025_nondeterministic),
//    where a dropped implicit return meant a worker thread was never spawned.
//    Declared uint32 and the callee's value returned.
//
// 2. `pop ecx` rather than `add esp,4`. Same stack effect, but it CLOBBERS ECX.
//    ECX is caller-saved under cdecl so no caller may rely on it, and MSVC will
//    likewise treat ECX as scratch — no naked shim needed. Recorded because the
//    two encodings are not interchangeable if a caller is ever found that does
//    depend on it.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioQueuePop(void* obj) {
    const std::uint32_t q = *reinterpret_cast<const std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(obj) + kQueueOff);
    return s_FUN_005ad2e0(q);
}
RH_ScopedInstall(AudioQueuePop, 0x005b0f40);

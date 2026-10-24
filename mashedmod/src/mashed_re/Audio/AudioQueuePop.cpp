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
#include "../Core/ShadowAB.h"
#include <cstdint>
#include <cstdlib>

namespace {

// Raw free = RwGlobals slot +0x10c through the trampoline 0x004522d0 (mov ecx,[0x007d3ff8] / jmp [ecx+0x10c]).
// Exe arm: the CRT free, the declared deviation of Audio/AudioMemory.cpp's AudioRawFree. (audiolink 2026-10-23)
#ifdef MASHED_STANDALONE
void QueueRawFree(void* p) { std::free(p); }
#else
void QueueRawFree(void* p) { reinterpret_cast<void (__cdecl*)(void*)>(0x004522d0u)(p); }
#endif

constexpr std::uint32_t kQueueOff = 0x28u;

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ad2e0  AudioQueueConsume(void* q) -> 0 / 1   (C2; ported audiolink 2026-10-23 so AudioQueuePop has no
// address in the exe. Exported for path1, NOT installed. Bytes: verify/audiolink_20261023/disasm_slice7.txt)
//   005ad2e0 eax = q ; ecx = [q+0xc] ; test / jbe 0x5ad2f2      ; count == 0 -> unlink path
//   005ad2eb dec ecx ; [q+0xc] = ecx ; return 0
//   005ad2f2 ecx = [q+0x18] ; edx = [q+0x14] ; [ecx] = edx      ; prev->next = next
//   005ad2fa ecx = [q+0x14] ; edx = [q+0x18] ; [ecx+4] = edx    ; next->prev = prev (both RE-READ after the store)
//   005ad303 test byte [q+0x10],1 ; jne 0x5ad314                 ; bit 0 set -> keep the node
//   005ad30b RwFree(q) through 0x004522d0
//   005ad314 return 1
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioQueueConsume(void* q) {
    volatile std::uint32_t* const w = static_cast<volatile std::uint32_t*>(q);
    const std::uint32_t count = w[0xc / 4];
    if (count != 0u) {                                                     // 0x005ad2e7 / 0x005ad2e9
        w[0xc / 4] = count - 1u;                                           // 0x005ad2ec
        return 0u;
    }
    *reinterpret_cast<volatile std::uint32_t*>(w[0x18 / 4]) = w[0x14 / 4];        // 0x005ad2f2..0x005ad2f8
    reinterpret_cast<volatile std::uint32_t*>(w[0x14 / 4])[1] = w[0x18 / 4];      // 0x005ad2fa..0x005ad300
    if ((static_cast<volatile std::uint8_t*>(q)[0x10] & 1u) == 0u) {       // 0x005ad303 / 0x005ad306
        QueueRawFree(q);                                                   // 0x005ad30c
    }
    return 1u;                                                             // 0x005ad314
}

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
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioQueuePop(void* obj);
static std::uint32_t __cdecl AudioQueuePop_impl(void* obj) {
    const std::uint32_t q = *reinterpret_cast<const std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(obj) + kQueueOff);
    return AudioQueueConsume(reinterpret_cast<void*>(q));                 // 0x005b0f48
}
// --- 0x005b0f40 shadow A/B (TT-11) -- GENERATED by re/tools/shadow_gen.py -----------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioQueuePop(void* obj) {
    SHADOW_AB_COUNTER(ab, "AudioQueuePop", 0x005b0f40u, ShadowAB::kPhaseRace);
    return ShadowAB::Run(ab, AudioQueuePop_impl, obj);
}
RH_ScopedInstall(AudioQueuePop, 0x005b0f40);

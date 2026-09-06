// Mashed RE — promote-round round 72 (audio-state switch predicate + gated message-post).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_004627b0 (0x004627b0): jump-table switch over the audio-device enum at 0x00773920.
//   MOV EAX,[0x00773920] ; DEC ; CMP 6 / JA default(0) ; MOVZX/JMP jumptable ; cases 2,3,4,7 -> 1
//   -> return (DAT_00773920 in {2,3,4,7}) ? 1 : 0
//
// FUN_0042bf30 (0x0042bf30): gated message-post (cdecl 6 args).
//   if (*(int*)0x0067eab0 == 0) {
//     [0x67eac0]=p3 ; [0x67eabc]=p2 ; [0x67eac8]=p4 ; [0x67eacc]=p5 ; [0x67ead0]=p6 ; [0x67eab4]=p1 ;
//     [0x67eab0]=1 ; [0x67eab8]=0 ; [0x67ead4]=0 ;
//     if (*(int*)0x0067ea5c != 0) { [0x67ead4]=1 ; [0x67ea5c]=0 ; }
//   }

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004627b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioStateActive(void) {
    switch (*reinterpret_cast<std::int32_t*>(0x00773920u)) {
    case 2: case 3: case 4: case 7:
        return 1;
    }
    return 0;
}
RH_ScopedInstall(AudioStateActive, 0x004627b0);

// 0x0042bf30
// TRACKER NAME: MenuModalPost (hooks.csv, re-filed frontend 2026-09-05).
// This is the modal POSTER: it fills the request block at 0x0067eab4..0x0067ead0
// and raises DAT_0067eab0. It never draws. The renderer is 0x00433f40,
// MenuModalRender, in Frontend/MenuModal.cpp, which paints whatever is posted
// here once the ramp DAT_0067eab8 reaches 0x28.
//
// The exported symbol below KEEPS the old Post0042bf30 name on purpose, as does
// the registry key 'post_0042bf30', so the 2026-06-13 promotion evidence
// (log/diff_post_0042bf30.csv) and hooks_registry.py keep resolving. Do not
// rename it to match the tracker without re-running that diff.
//
// Its gate DAT_0067eab0 doubles as the setup-screen input freeze that
// 0x0042f7b0 (FrontendCursorUpdate) early-outs on -- one flag, two jobs.
// See re/analysis/race_hud_capture_20260902.md Findings 29, 32 and 37.
extern "C" __declspec(dllexport) void __cdecl Post0042bf30(std::uint32_t p1, std::uint32_t p2, std::uint32_t p3,
                                                           std::uint32_t p4, std::uint32_t p5, std::uint32_t p6) {
    if (*reinterpret_cast<std::int32_t*>(0x0067eab0u) == 0) {
        *reinterpret_cast<std::uint32_t*>(0x0067eac0u) = p3;
        *reinterpret_cast<std::uint32_t*>(0x0067eabcu) = p2;
        *reinterpret_cast<std::uint32_t*>(0x0067eac8u) = p4;
        *reinterpret_cast<std::uint32_t*>(0x0067eaccu) = p5;
        *reinterpret_cast<std::uint32_t*>(0x0067ead0u) = p6;
        *reinterpret_cast<std::uint32_t*>(0x0067eab4u) = p1;
        *reinterpret_cast<std::uint32_t*>(0x0067eab0u) = 1;
        *reinterpret_cast<std::uint32_t*>(0x0067eab8u) = 0;
        *reinterpret_cast<std::uint32_t*>(0x0067ead4u) = 0;
        if (*reinterpret_cast<std::int32_t*>(0x0067ea5cu) != 0) {
            *reinterpret_cast<std::uint32_t*>(0x0067ead4u) = 1;
            *reinterpret_cast<std::uint32_t*>(0x0067ea5cu) = 0;
        }
    }
}
RH_ScopedInstall(Post0042bf30, 0x0042bf30);

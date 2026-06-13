// Mashed RE — promote-round round 53 (one scattered-global clearer + four strided table clearers).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All five are __cdecl void(void), self-contained (zero callees), byte-verified
// from the listing (first byte not 0xE9):
//
// FUN_00405400 (0x00405400): zero three adjacent globals.
//   33c0                  XOR EAX,EAX
//   c705 749d6300 0       MOV [0x00639d74],0
//   a3 789d6300           MOV [0x00639d78],EAX  (=0)
//   a3 709d6300           MOV [0x00639d70],EAX  (=0)
//   c3                    RET
//
// FUN_0048f680/6b0/6e0/710: strided "zero the first dword of each record" loops:
//   b8 <base>             MOV EAX,base
//   c700 0                MOV [EAX],0           ; loop body
//   83c0 <step>           ADD EAX,step          ; record stride (bytes)
//   3d <end>              CMP EAX,end
//   7c f0                 JL loop
// Bases/steps/ends (byte-verified):
//   0048f680: base 0x0076a100 step 0x38 end 0x0076d900  (len 0x3800)
//   0048f6b0: base 0x00766a00 step 0x20 end 0x00766f00  (len 0x0500)
//   0048f6e0: base 0x00770718 step 0x24 end 0x00771528  (len 0x0e10)
//   0048f710: base 0x00769f50 step 0x24 end 0x0076a0b8  (len 0x0168)
// The range_init diff fills the whole [base,end) span with a sentinel, calls,
// and snapshots it — so it also proves the reimpl touches ONLY the strided slots.

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x00405400
extern "C" __declspec(dllexport) void __cdecl Clear639d70x3(void) {
    *reinterpret_cast<std::uint32_t*>(0x00639d74u) = 0;   // MOV [0x00639d74],0
    *reinterpret_cast<std::uint32_t*>(0x00639d78u) = 0;   // MOV [0x00639d78],EAX
    *reinterpret_cast<std::uint32_t*>(0x00639d70u) = 0;   // MOV [0x00639d70],EAX
}
RH_ScopedInstall(Clear639d70x3, 0x00405400);

// Shared strided "*p = 0; p += step" clear over [base, end).
static inline void StridedClear(std::uint32_t base, std::uint32_t step, std::uint32_t end) {
    for (std::uint32_t p = base; static_cast<std::int32_t>(p) < static_cast<std::int32_t>(end); p += step)
        *reinterpret_cast<std::uint32_t*>(p) = 0;
}

// 0x0048f680
extern "C" __declspec(dllexport) void __cdecl StridedClear76a100(void) { StridedClear(0x0076a100u, 0x38u, 0x0076d900u); }
RH_ScopedInstall(StridedClear76a100, 0x0048f680);

// 0x0048f6b0
extern "C" __declspec(dllexport) void __cdecl StridedClear766a00(void) { StridedClear(0x00766a00u, 0x20u, 0x00766f00u); }
RH_ScopedInstall(StridedClear766a00, 0x0048f6b0);

// 0x0048f6e0
extern "C" __declspec(dllexport) void __cdecl StridedClear770718(void) { StridedClear(0x00770718u, 0x24u, 0x00771528u); }
RH_ScopedInstall(StridedClear770718, 0x0048f6e0);

// 0x0048f710
extern "C" __declspec(dllexport) void __cdecl StridedClear769f50(void) { StridedClear(0x00769f50u, 0x24u, 0x0076a0b8u); }
RH_ScopedInstall(StridedClear769f50, 0x0048f710);

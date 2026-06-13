// Mashed RE — promote-round round 75 (global-field getter + float-threshold predicate).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Both are __cdecl, self-contained (zero callees), byte-verified from the listing.
//
// FUN_004495d0 (0x004495d0): global-field getter + constant.
//   MOV EAX,[0x00896278] ; MOV EAX,[EAX+0x4] ; ADD EAX,0x40
//   -> return *(int*)(*(int*)0x00896278 + 4) + 0x40
//
// FUN_0044e020 (0x0044e020): per-record float threshold predicate.
//   MOV EAX,[ESP+4] ; IMUL EAX,0xf8 ; FLD float [EAX+0x8900b0] ; FCOMP float [0x5ce2d8] ;
//   FNSTSW AX ; TEST AH,5 ; JP ret0 ; MOV EAX,1   (x87 "below" test -> matches C++ < for finite floats)
//   -> return (*(float*)(0x008900b0 + idx*0xf8) < *(float*)0x005ce2d8) ? 1 : 0

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x004495d0
extern "C" __declspec(dllexport) int __cdecl GlobalFieldGet896278(void) {
    return *reinterpret_cast<int*>(*reinterpret_cast<int*>(0x00896278u) + 4) + 0x40;  // [[0x896278]+4] + 0x40
}
RH_ScopedInstall(GlobalFieldGet896278, 0x004495d0);

// 0x0044e020
extern "C" __declspec(dllexport) std::uint32_t __cdecl FloatLt44e020(std::int32_t idx) {
    return (*reinterpret_cast<float*>(0x008900b0u + static_cast<std::uint32_t>(idx) * 0xf8u)
            < *reinterpret_cast<float*>(0x005ce2d8u)) ? 1u : 0u;                       // FLD/FCOMP < threshold
}
RH_ScopedInstall(FloatLt44e020, 0x0044e020);

// Mashed RE — promote-round round 76 (global-field getter; loop opportunistic, past the 200 goal).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_0044c370 (0x0044c370): __cdecl, self-contained (zero callees), byte-verified.
//   MOV EAX,[0x00896000] ; MOV EAX,[EAX+0x4] ; ADD EAX,0x40
//   -> return *(int*)(*(int*)0x00896000 + 4) + 0x40

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0044c370
extern "C" __declspec(dllexport) int __cdecl GlobalFieldGet896000(void) {
    return *reinterpret_cast<int*>(*reinterpret_cast<int*>(0x00896000u) + 4) + 0x40;  // [[0x896000]+4] + 0x40
}
RH_ScopedInstall(GlobalFieldGet896000, 0x0044c370);

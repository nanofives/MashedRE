// Mashed RE — promote-round round 57 (Pool-J per-slot aim-vector pointer accessor).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_0045a110 (0x0045a110): pointer-returning getter (computes an absolute image
// VA from the index; no memory read). gta-reversed convention: a pointer getter
// returns the ORIGINAL-image VA as a constant.
//   8b442404      MOV EAX,[ESP+4]          ; i
//   85c0 7c0e     TEST EAX,EAX ; JL  oob    ; i < 0   -> 0
//   83f804 7d09   CMP EAX,4 ; JGE oob        ; i >= 4  -> 0
//   6bc058        IMUL EAX,EAX,0x58
//   051cba6800    ADD EAX,0x68ba1c
//   c3 / 33c0 c3  RET  /  XOR EAX,EAX ; RET

#include "../Core/HookSystem.h"

#include <cstdint>

// 0x0045a110
extern "C" __declspec(dllexport) std::uint32_t __cdecl PtrGet68ba1c(std::int32_t i) {
    if (i >= 0 && i < 4) return 0x0068ba1cu + i * 0x58u;   // IMUL 0x58 ; ADD 0x68ba1c
    return 0;                                              // XOR EAX,EAX
}
RH_ScopedInstall(PtrGet68ba1c, 0x0045a110);

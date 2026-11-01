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

// MATCH-S07: the body of 0x0042bf30 (Post0042bf30) moved to Match/MatchS07_0042bf30.cpp,
// which compiles the byte-matched source re/match/s07/0042bf30.c.

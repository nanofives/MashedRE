// TOOLCHAIN vc19
// LANG c++
// FLAGS: /O2 /EHa /DMASHED_STANDALONE
// MATCH: 0x00458920 _FUN_00458920
#define MATCH_PORT 1
#define FUN_00458570 Port_00458570
namespace U {
extern "C" {
#include "C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/L156-REGCONV-EVIDENCE/re/match/s09/00458920.c"
}
}
using namespace U;

// The exe's adapter (mashedmod/src/mashed_re/Match/MatchS09_00458920.cpp:40), with `call PK_458570_w` replaced by a
// call to the original 0x00458570 (EDX holds the target; the original helper's live-in registers are EAX and ESI,
// verify/ddkregconv7_20261113/RESULT_DDKOBJ-REGCONV-FALLBACK-7.md section 1).
extern "C" __declspec(naked) float Port_00458570(V3_00458920* /*out*/, int /*idx*/, int* /*rec*/) {
    __asm {
        push ebp
        push ebx
        push edi
        push esi
        mov  esi, dword ptr [esp + 20]
        mov  eax, dword ptr [esp + 24]
        push dword ptr [esp + 28]
        mov  edx, 0x00458570
        call edx
        add  esp, 4
        pop  esi
        pop  edi
        pop  ebx
        pop  ebp
        ret
    }
}


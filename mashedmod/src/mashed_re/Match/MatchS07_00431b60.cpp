// MATCH-S07: port body of 0x00431b60 = the byte-matched source re/match/s07/00431b60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/SineOscillators.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00431b60 SinGlobalProduct431b60
#define sin MatchS07_00431b60_Fsin
// The matched source calls sin() as an intrinsic (#pragma intrinsic(sin)); XP DDK cl emits that as the bare FSIN the
// original runs (FLD / FMUL / FSIN / RET). VS2022 emits `jmp __CIsin` instead (the CRT routine, not FSIN), so sin binds
// to this helper, which runs FSIN on the product the way the old Util/SineOscillators.cpp body did (inline __asm, FSTP
// qword). The product reaches it as a double: the x87 FMUL result is already rounded to the precision control's
// 24 or 53 bits, so the store is exact. Needs the x87 build (/arch:IA32) so the FMUL runs on the x87 as before.
extern "C" double MatchS07_00431b60_Fsin(double x) {
    double r;
    __asm {
        fld   qword ptr [x]
        fsin
        fstp  qword ptr [r]
    }
    return r;
}
#pragma warning(push)
#pragma warning(disable : 4163)  // the source's #pragma intrinsic(sin) now names the helper
#pragma comment(linker, "/EXPORT:SinGlobalProduct431b60=_SinGlobalProduct431b60")
extern "C" {
#include "../../../../re/match/s07/00431b60.c"
}
#pragma warning(pop)
RH_ScopedInstall(SinGlobalProduct431b60, 0x00431b60);

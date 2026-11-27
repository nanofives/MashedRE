// RACE-LOAD-004E99B0 (run 57, L-184): CRT math entry points of the race load on the MASHED_ORIG_TOP path, bound by name
// (re/tools/bind/porttop_bind.py crt route: MashedCrt_<name>_<rva> in Match/PortTop_Crt*.cpp).
//   _acos  rva 004a3384  hooks.csv C2 "CRT::acos". Called by 0x004783f0 (re/match/s12/004783f0.c:88). The original, for
//          |x| < 1 (high dword of |x| below 0x3ff00000, 0x004a33a1), sets the x87 control word to 0x027f (53-bit, 0x004a3394 /
//          0x004a70a5) and computes fpatan(fsqrt((1 + x) * (1 - x)), x) (0x004a33a8..0x004a33b6). This body runs that sequence
//          under the same control word. |x| >= 1 and NaN go to the exe CRT's acos: [UNCERTAIN] that its results there equal
//          the original's 0x004a33d5.. path (test: re/orchestrator/UNCERTAIN_REGISTER.md, RACE-LOAD-004E99B0 acos line).
// EXE-ONLY: the .asi runs inside MASHED.exe and calls the originals directly.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cmath>

extern "C" {
// 0x004a3384
double __cdecl MashedCrt_acos_004a3384(double x)
{
    if (!(std::fabs(x) < 1.0)) {
        return std::acos(x);
    }
    double r;
    unsigned short cw;
    unsigned short cw53 = 0x027f;
    __asm {
        fnstcw cw
        fldcw cw53
        fld x
        fld1
        fadd st(0), st(1)
        fld1
        fsub st(0), st(2)
        fmulp st(1), st(0)
        fsqrt
        fxch st(1)
        fpatan
        fstp r
        fldcw cw
    }
    return r;
}
}
#endif // MASHED_STANDALONE

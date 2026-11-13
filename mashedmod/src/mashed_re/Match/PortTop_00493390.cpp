// RO-BIND-TIMER (replay oracle 2, L-125 (a)): exe-only port body of rva 0x00493390 = the byte-matched source
// re/match/s15/00493390.c (match.py BYTE-IDENTICAL 239 B, relocheck OK checked=22; hooks.csv C4 c4=match).
// It is the tick-boundary clock: QPC/3 MHz via 0x004950b0, writes DAT_007f1000 = 50 (0x004933d5), DAT_007f1004,
// DAT_007f1010, DAT_007f0ffc, DAT_00771984, DAT_007f0ff0 += 50, DAT_007f1030 = now, and the 3 s window at
// DAT_007f102c / DAT_007f1024 / DAT_007f1028. Its only caller is the matched 0x00493480 (Match/MatchS15_00493480.cpp),
// which binds FUN_00493390 here in the exe. The exe reaches it only on the MASHED_ORIG_TOP path with the sub-knob
// MASHED_OT_TIMER=1 (Match/PortTop_00492370.cpp). The .asi keeps the raw-address call (no .asi body for this RVA).
// Callee 0x004950b0 binds by name to its matched exe body QpcTimeScaledTo3Mhz (Match/MatchS16_004950b0.cpp).
//
// OT-TIMER-VS2022-FORM: the matched text is compiled as PortTop_Body_00493390, and PortTop_Bound_00493390 (the name
// the caller binds) runs it and then re-stores DAT_007f1010 in the original's x87 form. VS2022 compiles the source line
// `DAT_007f1010 = (float)now * 3.3333333e-07f` as cvtsi2ss + mulss (now rounded to 24 bits, then the product rounded
// again). The original loads now exact and rounds the product once:
//   0x00493414 test edx,edx / 0x00493416 mov [esp+4],edx / 0x0049341a fild dword [esp+4]
//   0x00493428 jge 0x00493430 / 0x0049342a fadd dword [0x005cc94c] (float 2^32, the unsigned fix-up)
//   0x00493436 fmul dword [0x005ccd68] (3.3333333e-07f) / 0x0049344b fstp dword [0x007f1010]
// The re-store repeats that sequence on now = DAT_007f1030 (the body stores now there at 0x00493459; nothing reads
// DAT_007f1010 between the two stores). On the object build (MASHED_DDKOBJ=1) ddkobj.py substitutes the DDK object
// for PortTop_Body_00493390, which already stores the x87 form; the re-store then writes the same bits again.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00493390 PortTop_Body_00493390
#define FUN_004950b0 QpcTimeScaledTo3Mhz   // matched, Match/MatchS16_004950b0.cpp
// noinline: the wrapper below must keep a real call to the body, so ddkobj.py can repoint it to the DDK object.
extern "C" __declspec(noinline) void PortTop_Body_00493390(void);
extern "C" {
#include "../../../../re/match/s15/00493390.c"
}
#undef FUN_00493390

extern "C" {
// 0x00493390 (exe binding: the matched body, then DAT_007f1010 in the x87 form of 0x0049341a..0x0049344b)
__declspec(noinline) void PortTop_Bound_00493390(void) {
    PortTop_Body_00493390();
    static const float k2p32 = 4294967296.0f;     // 0x005cc94c
    static const float kScale = 3.3333333e-07f;   // 0x005ccd68
    unsigned now = *reinterpret_cast<volatile unsigned*>(0x007f1030u);
    float out;
    __asm {
        mov   edx, now
        test  edx, edx
        fild  dword ptr [now]
        jge   pos
        fadd  dword ptr [k2p32]
    pos:
        fmul  dword ptr [kScale]
        fstp  dword ptr [out]
    }
    *reinterpret_cast<volatile float*>(0x007f1010u) = out;
}
}
#endif // MASHED_STANDALONE

// PORT-TOP-4669B0 (RO-AUDIOTICK-BODIES follow-up): exe-only port body of rva 0x004669b0 = the byte-matched source
// re/match/topm01/004669b0.c (match.py --bytes BYTE-IDENTICAL 160 B, relocheck OK checked=24; hooks.csv C4 c4=match,
// src sha256 cf57913d1c6c156d, verify/porttopm01_20261108/RESULT_PORT-TOP-MATCH-01.md).
// Its exe caller is the matched 0x00402750 AppInitialiseOnBootup (Match/PortTop_00402750.cpp, call at 0x004028db),
// reached only under MASHED_ORIG_TOP. Sub-knob MASHED_OT_AUDIOINIT=1 (default OFF) runs this body. Off, the call keeps
// running the hand stand-in piece 3 (PortTop_Piece_004669b0 -> HandTop_InitAudio), as before this row.
//
// The body is the only writer of DAT_006904e8 in re/match (= 1, re/match/topm01/004669b0.c). That word gates the
// matched audio-tick bodies 0x0045d3f0 / 0x00466b50 / 0x0045d430 (RO-AUDIOTICK-BODIES), so with this knob and
// MASHED_OT_AUDIOTICK=1 those bodies pass their gate and reach their callees (named stops, Match/PortTop_00492370.cpp).
//
// Callees of 0x004669b0 (re/match/topm01/004669b0.c):
//  - 0x004625b0 -> AudioCharacterBankPathsBuild (Audio/AudioCharacterBankPaths.cpp, exe body, writes 0x00690158..).
//  - 0x004627f0 (sound engine HW init) -> its exe body PortTop_Bound_004627f0 (L-126 provisional, re/match/s10/004627f0.c;
//    PORT-TOP-AUDIO-HWINIT-4627F0). Before that row: hand piece 3 (HandTop_InitAudio: exe audio, permdict.rws, menu music).
//  - 0x0045d460 (PERMDICT.RWS loader) -> its exe body PortTop_Bound_0045d460 (L-126 provisional, re/match/s10/0045d460.c;
//    PORT-TOP-AUDIO-BODY-45D460). Before that row: a stand-in that logged its first call and did not run.
//  - 0x00429290 (called 3 times with the 0x006036e8 / 0x00603768 / 0x006037e8 strings) -> stand-in, logs its first
//    call. Same choice as 0x00402750's binding of 0x00429290 (PortTop_StandIn04E_00429290).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#define MATCH_PORT 1
#define FUN_004669b0 PortTop_Bound_004669b0
#define FUN_004625b0 AudioCharacterBankPathsBuild
#define FUN_004627f0 PortTop_AInit_004627f0
#define FUN_0045d460 PortTop_AInit_0045d460
#define FUN_00429290 PortTop_AInit_00429290
extern "C" {
#include "../../../../re/match/topm01/004669b0.c"
}

namespace {
void LogAI(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[ORIG_TOP] [OT_AUDIOINIT] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}
int s_knob = -1;
int s_ran = 0;
int Knob() {
    if (s_knob < 0) {
        const char* const v = std::getenv("MASHED_OT_AUDIOINIT");
        s_knob = (v && v[0] && v[0] != '0') ? 1 : 0;
    }
    return s_knob;
}
int MemI(unsigned a) { return *reinterpret_cast<volatile int*>(a); }
unsigned MemU(unsigned a) { return *reinterpret_cast<volatile unsigned*>(a); }
}  // namespace

extern "C" {
extern int g_PortTop04E_InitFailed;          // Match/PortTop_00402750_Pieces.cpp
void PortTop_Piece_004669b0(void);           // Match/PortTop_00402750_Pieces.cpp (piece 3)

// rva 0x004627f0 sound engine HW init -> its exe body PortTop_Bound_004627f0 (Match/PortTop_004627f0.cpp, L-126
// provisional body re/match/s10/004627f0.c, PORT-TOP-AUDIO-HWINIT-4627F0). Before that row it ran hand piece 3.
void PortTop_Bound_004627f0(void);
void PortTop_AInit_004627f0(void) {
    LogAI("0x004627f0 sound engine HW init: provisional exe body (re/match/s10/004627f0.c)");
    PortTop_Bound_004627f0();
    LogAI("0x004627f0 returned: DAT_00632ba0=0x%08X DAT_0069049c=0x%08X DAT_0068f61c=0x%08X DAT_006904a0=%d DAT_0068f4c8=%d "
          "DAT_007dcd20=0x%08X", MemU(0x00632ba0u), MemU(0x0069049cu), MemU(0x0068f61cu), MemI(0x006904a0u), MemI(0x0068f4c8u),
          MemU(0x007dcd20u));
}
// rva 0x0045d460 PERMDICT.RWS loader -> its exe body PortTop_Bound_0045d460 (Match/PortTop_0045d460.cpp, L-126
// provisional body re/match/s10/0045d460.c, PORT-TOP-AUDIO-BODY-45D460). Before that row: a stand-in that did not run.
void PortTop_Bound_0045d460(void);
void PortTop_AInit_0045d460(void) {
    LogAI("0x0045d460 PERMDICT.RWS loader: provisional exe body (re/match/s10/0045d460.c)");
    PortTop_Bound_0045d460();
    LogAI("0x0045d460 returned: DAT_0068fc88=%d DAT_0068fc84=%d DAT_0088e674=%d DAT_0069047c=0x%08X DAT_00604604=0x%08X",
          MemI(0x0068fc88u), MemI(0x0068fc84u), MemI(0x0088e674u), MemU(0x0069047cu), MemU(0x00604604u));
}
// rva 0x00429290 (string arg calls inside 0x004669b0) -> stand-in
void PortTop_AInit_00429290(char* a, char* b, int c) {
    static int once;
    if (!once) { once = 1; LogAI("rva 0x00429290: stand-in, not run (first call %p %p %d)", a, b, c); }
}

// 1 once the matched 0x004669b0 ran (the self-test in Match/PortTop_00492370.cpp then expects DAT_006904e8 == 1).
int PortTop_AudioInit_Ran(void) { return s_ran; }

// Dispatcher (not a body) at 0x00402750's call to 0x004669b0.
void PortTop_AudioInit_004669b0(void) {
    if (!Knob()) { PortTop_Piece_004669b0(); return; }
    if (g_PortTop04E_InitFailed) return;
    LogAI("MASHED_OT_AUDIOINIT on: 0x004669b0 runs its matched exe body (re/match/topm01/004669b0.c)");
    const int r = PortTop_Bound_004669b0();
    s_ran = 1;
    // Self-test: the eleven globals the body writes (re/match/topm01/004669b0.c), and its return value 1.
    unsigned fails = 0, checks = 0;
    auto chk = [&](bool ok, const char* what, unsigned got, unsigned want) {
        ++checks;
        if (!ok) { ++fails; LogAI("self-test FAIL %s: got 0x%08X expected 0x%08X", what, got, want); }
    };
    chk(r == 1, "return", static_cast<unsigned>(r), 1u);
    chk(MemI(0x006904e8u) == 1, "DAT_006904e8", MemU(0x006904e8u), 1u);
    chk(MemI(0x0068fcccu) == 1000, "DAT_0068fccc", MemU(0x0068fcccu), 1000u);
    chk(MemU(0x0068fcbcu) == 0x3f800000u, "DAT_0068fcbc (1.0f)", MemU(0x0068fcbcu), 0x3f800000u);
    chk(MemI(0x006900d0u) == 0, "DAT_006900d0", MemU(0x006900d0u), 0u);
    for (unsigned i = 0; i < 4; ++i) chk(MemU(0x008aa300u + 4 * i) == 0, "DAT_008aa300[i]", MemU(0x008aa300u + 4 * i), 0u);
    for (unsigned i = 0; i < 3; ++i) chk(MemU(0x00690490u + 4 * i) == 0, "DAT_00690490[i] (0.0f)", MemU(0x00690490u + 4 * i), 0u);
    LogAI("self-test %s (%u of %u checks failed)", fails ? "FAIL" : "PASS", fails, checks);
}
}  // extern "C"
#endif  // MASHED_STANDALONE

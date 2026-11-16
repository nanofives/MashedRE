// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05: named stand-ins for the frame subtree of the run function (rva 00492290) on the MASHED_ORIG_TOP path (exe
// only). Each logs its first call (and a call count where it can repeat); the reason is in the comment and in
// verify/porttop05_20261112/RESULT_PORT-TOP-05.md. Not bodies of these RVAs.
#ifdef MASHED_STANDALONE
#include <cstdio>

namespace {
void StandInLog(const char* rva, const char* what, int* once) {
    if (*once) return;
    *once = 1;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva %s: stand-in (PORT-TOP-05) %s\n", rva, what);
        std::fclose(f);
    }
}
} // namespace

extern "C" {
void PortTop_HandFrameMenu(void);
void PortTop_HandFrameInputGlobals(void);

// [RWC-FRAME-CLUMP] run 46: the DAT_0067eca4 1 -> 2 -> 3 ladder of 0x0043dfd0, on the RW core frame route only
// (MASHED_RWC_FRAME, RwCore_FrameRoute). The hand menu keeps its phase in its own model (Frontend/MenuNavSM.cpp
// g_anim_phase) and writes 0x0067eca4 only for the length of one 0x004325c0 call (Anim_Tick restores it), so on the
// core frame the word stays 1 and 0x0043c5b0 returns before any draw (RESULT_RWC-FRAME-CLUMP.md 11.2).
// Transcribed from re/match/s08/0043dfd0.c lines 310..338 (17.7%, PROVISIONAL.tsv, no exe body):
//   0x0067f19c == 2: the source sets it to 1 and returns before the ladder. Here the ladder is skipped and 0x0067f19c
//                    is not written (the hand code owns it).
//   phase 1: DAT_0067eca8 += (int)(DAT_007f1004 * _DAT_005cc9fc) (0x0043e0a7; 0x005cc9fc = 0x447a0000 = 1000.0); at
//            >= 0xff: DAT_0067eca8 = 0xff, DAT_0067eca4 = 2, then FUN_0043d2a0(DAT_0067ecac, 0).
//   phase 2: FUN_004325c0(), DAT_0067eca8 += (int)(DAT_007f1004 * _DAT_005cda88) (0x0043e101; 0x005cda88 = 0xc3fa0000
//            = -500.0); below 0: DAT_0067eca8 = 0, DAT_0067eca4 = 3.
// Two calls are not made here, both declared deviations of this stand-in: FUN_0043d2a0 (54.3%, no exe body; the hand
// menu builds its screen stack itself) and FUN_004325c0 (the hand menu's Anim_Tick already calls the matched body once
// per frame). Phases 3 and up (the 3 -> 4 write at 0x0067e914 != 0, the menu rows) are not ported here: that is the
// frontend machine itself (PORT-TOP-08). The float -> int conversion is (int) truncation, as __ftol at 0x0043e0a7.
bool RwCore_FrameRoute(void);
long MashedD3D9Wrap_PresentCount(void);  // Compat/D3d9Wrap.cpp:489 (LONG)
static void PortTop_RwcPhaseLadder_0043dfd0(void) {
    volatile int& phase = *reinterpret_cast<volatile int*>(static_cast<unsigned long>(0x0067eca4u));
    volatile int& fade = *reinterpret_cast<volatile int*>(static_cast<unsigned long>(0x0067eca8u));
    const float dt = *reinterpret_cast<const volatile float*>(static_cast<unsigned long>(0x007f1004u));
    const float k1 = 1000.0f;   // _DAT_005cc9fc = 0x447a0000 (read from MASHED.exe.unpatched; the exe image may not hold .rdata)
    const float k2 = -500.0f;   // _DAT_005cda88 = 0xc3fa0000 (same)
    static unsigned s_logged;
    const int p0 = phase;
    static unsigned s_calls;
    if (++s_calls == 1 || s_calls == 60) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x0043dfd0: RW core frame route phase ladder call %u: DAT_0067eca4 = %d, DAT_0067eca8 = %d, "
                            "DAT_007f1004 = %.6f, DAT_0067f19c = %d, DAT_00771964 = %08X (0x00403050 gate), "
                            "DAT_00771a04 = %08X (0x00493f70) (RWC-FRAME-CLUMP)\n", s_calls, p0, static_cast<int>(fade), dt,
                         *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x0067f19cu)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00771964u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00771a04u)));
            std::fclose(f);
        }
    }
    if (*reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x0067f19cu)) == 2) return;
    if (p0 == 1) {
        const int step = static_cast<int>(dt * k1);
        if (fade + step < 0xff) fade = fade + step;
        else { fade = 0xff; phase = 2; }
    } else if (p0 == 2) {
        fade = fade + static_cast<int>(dt * k2);
        if (fade < 0) { fade = 0; phase = 3; }
    }
    // RWC-FRAME-LADDER-CHECK (run 51): one line per ladder call while the phase is 1 or 2 (and the call that leaves 2),
    // with the call index and the D3D9 present count, so the transition frames can be set against the original's write
    // watch of 0x0067eca4 / 0x0067eca8. At most 96 lines. Log only; nothing written.
    static unsigned s_steps;
    if ((p0 == 1 || p0 == 2) && s_steps < 96) {
        ++s_steps;
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x0043dfd0: ladder step call %u presents %ld: DAT_0067eca4 %d -> %d, DAT_0067eca8 = %d, "
                            "DAT_007f1004 = %.6f (%08X), DAT_0067ecac = %d, DAT_0067e9f8 = %d, DAT_0067ed78 =%08X %08X %08X %08X, "
                            "DAT_00898ac0 = %08X %08X %08X %08X (RWC-FRAME-LADDER-CHECK)\n",
                         s_calls, MashedD3D9Wrap_PresentCount(), p0, static_cast<int>(phase), static_cast<int>(fade), dt,
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x007f1004u)),
                         *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x0067ecacu)),
                         *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x0067e9f8u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x0067ed78u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x0067ed7cu)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x0067ed80u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x0067ed84u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00898ac0u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00898ac4u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00898ac8u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00898accu)));
            std::fclose(f);
        }
    }
    if (phase != p0 && s_logged < 8) {
        ++s_logged;
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x0043dfd0: RW core frame route phase ladder: DAT_0067eca4 %d -> %d, DAT_0067eca8 = %d, "
                            "DAT_007f1004 = %.6f, DAT_0067e914 = %d, DAT_0067ecac = %d, k1 = %.1f, k2 = %.1f (RWC-FRAME-CLUMP)\n",
                         p0, phase, fade, dt, *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x0067e914u)),
                         *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x0067ecacu)), k1, k2);
            std::fclose(f);
        }
    }
}

// [RWC-FRAME-MOVIE-BACKDROP] run 50: the movie block of 0x0043dfd0, on the RW core frame route only (RwCore_FrameRoute).
// Disassembly of MASHED.exe.unpatched (re/match/s08/0043dfd0.c lines 296..302):
//   0x0043e030 push 0; 0x0043e031 call 0x494ef0 (thunk of 0x00493f70 = return DAT_00771a04)
//   == 0: 0x0043e042..0x0043e045 push 1, push 0, push 0, call 0x494f00 (jmp 0x494a80): start movie 0 (frontend.mpg) with
//         DAT_00771a08 = 1, DAT_00771a04 = 1, DAT_00771a18 = RwRasterCreate(0, 0, 0, 0x84)
//   != 0: 0x0043e04f..0x0043e050 push 0, call 0x494f10 (jmp 0x494480): event poll 0x004942b0 and 0x004938e0 (texture into the
//         raster), then 0x0043e058 call 0x494f30: DAT_00771a50 = DAT_00771a54 = DAT_00771a18
// then 0x0043e05d 0x0043c000, 0x0043e069 0x0040acd0, and the 0x0067f19c test before the ladder. This block runs first, as
// in the original. Bodies: RwCore_00494ef0 (re/match/s16/00494ef0.c, 100%), FUN_00494a80 (Match/MatchS16_00494a80.cpp,
// 100%; its RwRasterCreate binding routes to RwCore_004c77c0 on this route, its 0x004944c0 binding takes the core's
// device [0x007d4110] on this route, Match/PortTop_MovieBind.cpp), PortTop_Movie_00494480 / _00494f30
// (Match/PortTop_MovieCore.cpp). Not made here: 0x0043c000 and 0x0040acd0 (not ported by this stand-in, as before).
// Declared deviation: PortTop_MovieBind_Pump() runs before 0x00494480. The original's texture renderer copies frames on
// the DirectShow streaming thread; the binding's graph has no renderer of its own and needs a caller on this thread.
unsigned int RwCore_00494ef0(void);
void FUN_00494a80(unsigned int param_1, unsigned int param_2, unsigned int param_3);
unsigned int PortTop_Movie_00494480(void);
void PortTop_Movie_00494f30(void);
void PortTop_MovieBind_Pump(void);
static void PortTop_RwcMovieBlock_0043dfd0(void) {
    static unsigned s_calls;
    const unsigned flag = RwCore_00494ef0();
    if (flag == 0) {
        FUN_00494a80(0, 0, 1);
    } else {
        PortTop_MovieBind_Pump();
        PortTop_Movie_00494480();
        PortTop_Movie_00494f30();
    }
    if (++s_calls <= 2 || s_calls == 60) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x0043dfd0: RW core frame route movie block call %u: 0x00493f70 read %u -> %s; "
                            "DAT_00771a04 = %08X, DAT_00771a08 = %08X, DAT_00771a18 = %08X, DAT_00771a50 = %08X, DAT_00771a10 = %08X, "
                            "DAT_006147d8 = %d (RWC-FRAME-MOVIE-BACKDROP)\n", s_calls, flag,
                         flag == 0 ? "0x00494a80(0, 0, 1)" : "0x00494480 + 0x00494f30",
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00771a04u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00771a08u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00771a18u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00771a50u)),
                         *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00771a10u)),
                         *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x006147d8u)));
            std::fclose(f);
        }
    }
}

// rva 0043dfd0 (frontend machine): the hand menu navigation and the input-globals write, in the hand order.
unsigned g_PortTop05_FrontendCalls = 0;
void PortTop_StandIn05_0043dfd0(void) {
    static int once;
    StandInLog("0x0043dfd0", "hand menu navigation + input globals (HandTop_FrameMenu, HandTop_FrameInputGlobals)", &once);
    ++g_PortTop05_FrontendCalls;
    if (RwCore_FrameRoute()) { PortTop_RwcMovieBlock_0043dfd0(); PortTop_RwcPhaseLadder_0043dfd0(); }
    PortTop_HandFrameMenu();
    PortTop_HandFrameInputGlobals();
}
// rva 004111c0 (race tick; rva 00492d30 cases 3 / 6, once per 50 of DAT_007f1000): the knob path's race is the hand
// session (PortTop_HandFrameGameFlow after the frame machine). Counted, nothing run.
unsigned g_PortTop05_RaceTickCalls = 0;
void PortTop_StandIn05_004111c0(unsigned int) {
    static int once;
    StandInLog("0x004111c0", "race tick: the hand session runs the race (counted, not run)", &once);
    ++g_PortTop05_RaceTickCalls;
}
// rva 0040d270 (course load; rva 0040d440 = case 2 of the game-mode machine): the hand path loaded the course at the menu launch.
void PortTop_StandIn05_0040d270(int table, int row) {
    // logged on every call: case 2 runs once per launch
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x0040d270: stand-in (PORT-TOP-05) course load, args table=%08X row=%d; the hand "
                        "path loaded the course at the menu launch\n", static_cast<unsigned>(table), row);
        std::fclose(f);
    }
}
// rva 0043df00 (tail of rva 004030d0): matched, not compiled (level 2).
void PortTop_StandIn05_0043df00(void) { static int once; StandInLog("0x0043df00", "matched, not compiled (level 2)", &once); }
// rva 0040ab40 / 0040ac80 / 0040de10 / 0040e360 / 0043d2a0: callees of the modal step, reached only after a modal box
// reached state 3.
void PortTop_StandIn05_0040ab40(void) { static int once; StandInLog("0x0040ab40", "matched, not compiled (level 2)", &once); }
void PortTop_StandIn05_0040ac80(void) { static int once; StandInLog("0x0040ac80", "matched, not compiled (level 2)", &once); }
void PortTop_StandIn05_0040de10(void) { static int once; StandInLog("0x0040de10", "matched, not compiled (level 2)", &once); }
void PortTop_StandIn05_0040e360(int) { static int once; StandInLog("0x0040e360", "port body in the .asi-only Util/PromoLoop_round10.cpp", &once); }
void PortTop_StandIn05_0043d2a0(int, int) { static int once; StandInLog("0x0043d2a0", "no matched source, no exe body", &once); }
// rva 004ce8e0 (image write under rva 00498810): matched, not compiled (level 2).
void PortTop_StandIn05_004ce8e0(int, char*) { static int once; StandInLog("0x004ce8e0", "matched, not compiled (level 2)", &once); }
}
#endif // MASHED_STANDALONE

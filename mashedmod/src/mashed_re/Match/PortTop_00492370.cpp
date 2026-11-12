// PORT-TOP-02 (L-96, top-down lane): the MASHED_ORIG_TOP knob path. EXE ONLY (exe_sources.rsp, not asi_sources.rsp).
//
// With MASHED_ORIG_TOP=1 (default OFF, read once at WinMain entry in exe_main.cpp) the exe's WinMain hands control to
// the byte-matched original WinMain 0x00492370 (re/match/s15/00492370.c, match.py 100%) and its run function
// rva 0x00492290 (re/match/s15/00492290.c, 100%), compiled verbatim here as C++ with MATCH_PORT. Three more matched
// sources the two bodies call: 0x00499cc0 (s16) compiles in PortTop_Leaves.cpp, 0x00492d20 and 0x00499730 in their existing wrappers.
// Window create 0x00499ba0, its window procedure 0x00499820, the OS-version logger 0x00499890 and the command-line
// parser 0x00493900 have their own TUs (their Win32 typedefs cannot share one TU).
//
// Every callee binds by name, in this order (verify/porttop_20261108/PLAN 7): a matched exe body, else an existing
// port body, else a stand-in = the hand top level's piece doing that job (exe_main.cpp HandTop_*), else a logged
// "missing" stub that returns what the reference returns. No absolute-address call anywhere (6 RVA thunks only).
// The full table, with the RenderWare entry points for PORT-TOP-03, is verify/porttop_20261108/BINDINGS.tsv.
//
// The exe maps the original data range zero-filled, so the original strings these bodies read are zero there. Under
// the knob only, PortTop_SeedRdata writes their original bytes back (PortTop_RdataSeed.inc, generated from the
// anchored binary by verify/porttop_20261108/gen_rdata_seed.py).

#ifdef MASHED_STANDALONE

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MATCH_PORT 1

extern "C" {

// ---- exe-side targets (exe_main.cpp PORT-TOP-02 export block and existing exe bodies) ----
void PortTop_HandPrelude(void);
void PortTop_HandPlaceWindow(void);
int  PortTop_HandInitRest(void* hInstance);
void PortTop_HandPollKeyboard(void);
void PortTop_HandFrameUpdate(void);
int  PortTop_HandRenderFrame(void);
void PortTop_HandAppDestroy(void);
void AppDestroy(void);
int  DataZeroFill(void);                       // Boot/SubsystemInit.cpp, matched 0x004924f0 (PORT-TOP-04D)
int  MashedRw_InstallRwGlobals(void);          // LibRw/RwApiBind.cpp (PORT-TOP-04D)                        // Match/PortTop_00402a40.cpp (PORT-TOP-04C)
int  SubsystemInit(void);                      // Match/MatchS15_00492270.cpp
void thunk_EngineStopDispatch(void);           // Match/MatchS15_00493550.cpp
bool thunk_LaunchLangGate(void);               // Match/MatchS15_00493540.cpp
unsigned int thunk_HwExitDispatch(void);       // Match/MatchS15_00493560.cpp
void PortTop_SeedDInput(void);                 // Match/PortTop_InputPort.cpp
void PortTop_HandGameModeMachine(void);
void PortTop_SetHwnd(void* hwnd);
int  MainLoopInit(void);                       // Match/MatchS15_00492770.cpp
__declspec(dllimport) void __stdcall PostQuitMessage(int nExitCode);

// ---- logging: one line per knob-path step into mashed_re.log (the hand prelude truncated it) ----
static void PortTop_Log(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[ORIG_TOP] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

// ---- .rdata seed (knob only) ----
struct PortTopSeed { unsigned va; unsigned n; const char* bytes; };
static const PortTopSeed kPortTopSeed[] = {
#include "PortTop_RdataSeed.inc"
};
static void PortTop_SeedRdata(void) {
    unsigned done = 0;
    for (const PortTopSeed& s : kPortTopSeed) {
        __try {
            std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(s.va)), s.bytes, s.n);
            ++done;
        } __except (1) {
            PortTop_Log("seed 0x%08X (%u bytes) faulted", s.va, s.n);
        }
    }
    PortTop_Log("rdata seed: %u/%u strings written", done, static_cast<unsigned>(sizeof(kPortTopSeed) / sizeof(kPortTopSeed[0])));
}

// The reference MASHED.exe runs with the no_focus_pause patch (0x004996d3 JNZ->JMP: the pump never calls WaitMessage),
// and the hand path bypasses the same gate (PumpOnce). Equivalent here: keep the pump's focus flag DAT_0077391c set.
// MASHED_ORIG_TOP_FOCUSPAUSE=1 keeps the unpatched behaviour (WaitMessage while unfocused).
static int s_focusPause = 0;
static void PortTop_NoFocusPause(void) {
    if (!s_focusPause) *reinterpret_cast<volatile int*>(0x0077391cu) = 1;
}

// Config log 0x004963e0 / 0x00496400: the exe's bodies (ConfigLogErrorCore / ConfigLogDebugCore) sit in an anonymous
// namespace in Save/SettingsConfig.cpp (a PORT-SAVE-01 file), so they cannot be bound by name yet. In the reference both
// are no-ops (the disable_log patch: 0x004963e7 JMP, 0x00496400 RET). The stand-ins write nothing the game reads; they
// mirror the text into mashed_re.log so the boot steps the matched WinMain logs are visible.
void PortTop_CfgLogError(const char* msg) {
    if (msg) PortTop_Log("cfglog: %.*s", static_cast<int>(std::strcspn(msg, "\n")), msg);
}
void PortTop_CfgLogDebug(const char* fmt, ...) {
    char buf[0x200];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt ? fmt : "", ap);
    va_end(ap);
    PortTop_Log("cfglog: %.*s", static_cast<int>(std::strcspn(buf, "\n")), buf);
}

// ---- WinMain 0x00492370 callees ----
// rva 0x00493540 -> 0x00495150: the matched exe bodies since PORT-TOP-04A (Match/MatchS15_00493540.cpp ->
// Match/PortTop_00495150.cpp: QPF store 0x00495120, DirectInput object 0x004955b0, joypads 0x004960a0, keyboard
// 0x004963b0, controller defaults 0x00498510 (missing)). The thunk is compiled as bool (AL only), so the value is
// widened here before the matched WinMain reads it as int. WinMain passes two arguments the thunk does not read.
int PortTop_Bound_00493540(int, int) {
    PortTop_SeedDInput();
    const int r = thunk_LaunchLangGate() ? 1 : 0;
    const unsigned qlo = *reinterpret_cast<volatile unsigned*>(0x00771e70u);
    PortTop_Log("0x00493540 -> 0x00495150 boot gate (matched) returned %d; QPF at 0x00771e70 = %u; DirectInput "
                "0x00771e78 = %s, keyboard 0x00772fb8 = %s, joypads 0x00772fac = %d", r, qlo,
                *reinterpret_cast<void* volatile*>(0x00771e78u) ? "created" : "NULL",
                *reinterpret_cast<void* volatile*>(0x00772fb8u) ? "created" : "NULL",
                *reinterpret_cast<volatile int*>(0x00772facu));
    return r;
}
// stand-in for 0x004924f0 (zero-fill 0x007f0f60 + boot params): the hand boot chain (exe_main ExecuteFrontendBootChain
// steps 1-3) does this job inside the 0x00402750 stand-in. [PORT-TOP-04D] The matched body is compiled into the exe
// (Boot/SubsystemInit.cpp, re/match/topm01/004924f0.c) but not run by default: on the knob path it changes the colour
// select screen (walk_04: player 1's keyboard icon moves right), because its 0x007f0a40 table defaults survive the hand
// init, which zeroes only 0x007f0f60.. and does not restore 0x007f0a40 from the save (exe_main.cpp:189). Diagnostic
// switch MASHED_TOP04D_DATAZERO=1 runs it (verify/porttop_20261108/RESULT_PORT-TOP-04D.md).
int PortTop_StandIn_004924f0(void) {
    const char* const v = std::getenv("MASHED_TOP04D_DATAZERO");
    if (v && v[0] == '1') {
        const int r = DataZeroFill();
        PortTop_Log("0x004924f0 DataZeroFill (matched, MASHED_TOP04D_DATAZERO=1) returned %d", r);
        return r;
    }
    PortTop_Log("0x004924f0 data init: stand-in (hand boot chain steps 1-3, run inside the 0x00402750 stand-in)");
    return 1;
}
// rva 0x00492270 SubsystemInit: the matched exe body (Match/MatchS15_00492270.cpp) since PORT-TOP-03. Under
// MASHED_STANDALONE it calls the matched RW engine bring-up 0x00493710 (Match/PortTop_00493710.cpp) with its RW core
// calls on librw (LibRw/RwApiBind.cpp: RwEngineInit/Open/Start; RwEngineStart creates the exe device and librw adopts
// it), and the missing stubs for DisplayInit 0x004921d0 / ViewportInit 0x00428590. Before the call: the exe's window
// global is 0x00499ba0's window (DAT_007e9584) and the dev-harness window placement the hand path does after
// CreateWindow.
int PortTop_Bound_00492270(void) {
    PortTop_SetHwnd(*reinterpret_cast<void* volatile*>(0x007e9584u));
    PortTop_HandPlaceWindow();
    const int r = SubsystemInit();
    PortTop_Log("0x00492270 SubsystemInit (matched) returned %d", r);
    return r;
}
// rva 0x00402750 AppInitialiseOnBootup: the matched exe body (Match/PortTop_00402750.cpp) since PORT-TOP-04E. Its callees
// bind to matched exe bodies, to the five pieces of the hand init (HandTop_InitRest split at its call points), to the
// piz file system on the exe's piz reader, or to stand-ins (Match/PortTop_00402750_Pieces.cpp). Diagnostic switch
// MASHED_TOP04E_HANDINIT=1 runs the old stand-in (the whole hand init in one call) instead.
int Orig_00402750(void);
extern int g_PortTop04E_InitFailed;
int PortTop_Bound_00402750(void) {
    int r;
    const char* const v = std::getenv("MASHED_TOP04E_HANDINIT");
    if (v && v[0] == '1') {
        void* hinst = *reinterpret_cast<void* volatile*>(0x007e9580u);
        r = PortTop_HandInitRest(hinst);
        PortTop_Log("0x00402750 AppInitialiseOnBootup: stand-in HandTop_InitRest (MASHED_TOP04E_HANDINIT=1) returned %d", r);
    } else {
        // [PORT-TOP-04G] the RwGlobals record before the matched loaders: 0x0040bbb0 runs before piece 4 and its
        // 0x0042a6b0 calls the string functions at +0xcc / +0xd4 (LibRw/RwApiBind.cpp).
        MashedRw_InstallRwGlobals();
        r = Orig_00402750();
        if (g_PortTop04E_InitFailed) r = 0;
        PortTop_Log("0x00402750 AppInitialiseOnBootup (matched) returned %d", r);
    }
    // [PORT-TOP-04D] the RW memory functions behind 0x007d3ff8 (LibRw/RwApiBind.cpp): the B15 bridge the hand init just
    // installed keeps its Im2D slots, the matched teardown's +0x10c free goes to librw.
    MashedRw_InstallRwGlobals();
    PortTop_NoFocusPause();
    return r;
}
// [PORT-TOP-04C] 0x00402a40 AppDestroy: the matched exe body (Match/PortTop_00402a40.cpp = re/match/s01/raw/00402a40.c),
// every callee bound by name; the callees that do not run are the named stand-ins of Match/PortTop_StandIns04C.cpp. The
// three cameras DisplayInit 0x004921d0 made are destroyed by it (0x00467020 / 0x00467070 / 0x004b4880 on the RW camera
// records of LibRw/RwApiBind.cpp). After it, the exe subsystems whose state is not in the original globals (exe audio,
// DirectInput, the small.mpg graph) are shut down by the hand path (HandTop_AppDestroy): the matched body cannot see them.
static unsigned PortTop_Dw(unsigned a) { return *reinterpret_cast<const unsigned volatile*>(a); }
void PortTop_Bound_00402a40(void) {
    PortTop_Log("0x00402a40 AppDestroy (matched) enter: cameras 0x006905b0=%08X 0x006905b4=%08X 0x0067f18c=%08X "
                "0x0067f190=%08X, DAT_005f9bd8=%u, DAT_00771a04=%u, DAT_007f0a30=%08X, DAT_008a94a8=%08X",
                PortTop_Dw(0x006905b0u), PortTop_Dw(0x006905b4u), PortTop_Dw(0x0067f18cu), PortTop_Dw(0x0067f190u),
                PortTop_Dw(0x005f9bd8u), PortTop_Dw(0x00771a04u), PortTop_Dw(0x007f0a30u), PortTop_Dw(0x008a94a8u));
    AppDestroy();
    PortTop_Log("0x00402a40 AppDestroy (matched) returned; then HandTop_AppDestroy for the exe audio / DirectInput / small.mpg");
    PortTop_HandAppDestroy();
}
// rva 0x004c5930 RwTexDictionaryDestroy: bound to librw (LibRw/RwApiBind.cpp, rw::TexDictionary::destroy) since
// PORT-TOP-03. WinMain calls it only when DAT_0077195c != 0, which 0x004921d0 sets; 0x004921d0 is not run in the exe
// yet, so it stays unreached.
// rva 0x00493550 -> 0x004938c0: the matched exe bodies (Match/MatchS15_00493550.cpp -> MatchS15_004938c0.cpp) since
// PORT-TOP-03, RW calls on librw (RwEngineStop also releases the exe device, the counterpart of RwEngineStart).
void PortTop_Bound_00493550(void) {
    PortTop_Log("0x00493550 -> 0x004938c0 SoftwareTidyUpBeforeExiting (matched, RW calls on librw)");
    thunk_EngineStopDispatch();
}
// rva 0x00493560 -> 0x004954f0 HardwareExitApplication: the matched exe bodies since PORT-TOP-04A
// (Match/MatchS15_00493560.cpp -> Match/PortTop_004954f0.cpp: keyboard, joypad and DirectInput release).
int PortTop_Bound_00493560(void) {
    const unsigned r = thunk_HwExitDispatch();
    PortTop_Log("0x00493560 -> 0x004954f0 HardwareExitApplication (matched) returned %u; DirectInput 0x00771e78 = %s, "
                "keyboard 0x00772fb8 = %s", r, *reinterpret_cast<void* volatile*>(0x00771e78u) ? "LEFT" : "released",
                *reinterpret_cast<void* volatile*>(0x00772fb8u) ? "LEFT" : "released");
    return static_cast<int>(r);
}

// ---- run function 0x00492290 callees ----
// rva 0x00492770 MainLoopInit: the matched exe body (Match/MatchS15_00492770.cpp), its 5 callees bound by name since
// PORT-TOP-01. Called directly since PORT-TOP-04A (the SEH guard is gone). 0x00495110's QPF divisor is the one the
// boot gate 0x00495120 stored, as in the original order.
int PortTop_MainLoopInit(void) {
    const int r = MainLoopInit();
    PortTop_Log("0x00492770 MainLoopInit returned %d", r);
    return r;
}
// rva 0x004929d0 game-mode machine: the matched exe body Orig_004929d0 (Match/PortTop_004929d0.cpp, re/match/s15/004929d0.c
// 100%) since PORT-TOP-05; case 2 calls the matched 0x0040d440 (Match/PortTop_0040d440.cpp). Each change of the phase word
// DAT_00771968 is logged. Diagnostic switch MASHED_TOP05_HANDFRAME=1 runs the old stand-in (the hand loop's
// GameModeMachine_Tick, a no-op unless MASHED_PORTSTATE_GAMEFLOW_FU=1) and the old 0x00492d30 stand-in.
int Orig_004929d0(void);
int Orig_00492d30(void);
int  PortTop_HandFrameBegin(void);
void PortTop_HandFrameInputGlobals(void);
void PortTop_HandFrameGameFlow(void);
extern unsigned g_PortTop05_FrontendCalls;
extern unsigned g_PortTop05_RaceTickCalls;
static unsigned s_frame = 0;
static int s_handFrame = -1;
static int PortTop_HandFrameSwitch(void) {
    if (s_handFrame < 0) {
        const char* const v = std::getenv("MASHED_TOP05_HANDFRAME");
        s_handFrame = (v && v[0] == '1') ? 1 : 0;
        PortTop_Log("frame subtree: %s", s_handFrame ? "MASHED_TOP05_HANDFRAME=1, hand stand-ins for 0x004929d0 / 0x00492d30"
                                                      : "matched 0x004929d0 and 0x00492d30 (PORT-TOP-05)");
    }
    return s_handFrame;
}
static int s_phaseSeen = -1;
static void PortTop_LogPhase(const char* where) {
    const int ph = *reinterpret_cast<volatile int*>(0x00771968u);
    if (ph != s_phaseSeen) {
        PortTop_Log("frame %u %s: DAT_00771968 %d -> %d (DAT_0067ecb0=%d DAT_0067eca4=%d DAT_0063ba7c=%d DAT_0063ba78=%d)",
                    s_frame, where, s_phaseSeen, ph, *reinterpret_cast<volatile int*>(0x0067ecb0u),
                    *reinterpret_cast<volatile int*>(0x0067eca4u), *reinterpret_cast<volatile int*>(0x0063ba7cu),
                    *reinterpret_cast<volatile int*>(0x0063ba78u));
        s_phaseSeen = ph;
    }
}
int PortTop_Bound_004929d0(void) {
    if (s_frame == 0) PortTop_Log("first frame: run function 0x00492290 loop entered");
    if (PortTop_HandFrameSwitch()) {
        PortTop_HandGameModeMachine();
        return 1;
    }
    PortTop_LogPhase("before 0x004929d0");
    const int r = Orig_004929d0();
    PortTop_LogPhase("after 0x004929d0");
    return r;
}
// rva 0x004967e0 (input poll, C2, no exe body) under the matched 0x00492d20: the hand keyboard poll.
void PortTop_StandIn_004967e0(void) {
    PortTop_HandPollKeyboard();
}
// rva 0x00492d30 frame machine: the matched exe body Orig_00492d30 (Match/PortTop_00492d30.cpp, re/match/s15/00492d30.c
// 100%) since PORT-TOP-05. The hand frame update is split around it (exe_main.cpp HandTop_Frame*):
//   before  the deterministic frame cap and the dev harness steps (no original counterpart)
//   inside  the frontend machine 0x0043dfd0 = the hand menu navigation + input globals (stand-in, cases 1/4/5/6)
//   after   the input globals when 0x0043dfd0 did not run this frame (cases 2/3/7), then the hand session (GameFlow_Update,
//           the race of the knob path; 0x004111c0 is a counting stand-in)
int PortTop_Bound_00492d30(void) {
    if (PortTop_HandFrameSwitch()) {
        PortTop_HandFrameUpdate();
        return 1;
    }
    if (!PortTop_HandFrameBegin()) return 1;   // frame cap reached: WM_QUIT is posted, the pump ends the loop
    const unsigned fe = g_PortTop05_FrontendCalls;
    const int r = Orig_00492d30();
    if (g_PortTop05_FrontendCalls == fe) PortTop_HandFrameInputGlobals();
    PortTop_HandFrameGameFlow();
    if (s_frame < 600 ? (s_frame % 120) == 0 : (s_frame % 1800) == 0)
        PortTop_Log("frame %u: 0x00492d30 (matched) returned %d, phase %d, 0x0043dfd0 stand-in calls %u, 0x004111c0 calls %u",
                    s_frame, r, *reinterpret_cast<volatile int*>(0x00771968u), g_PortTop05_FrontendCalls,
                    g_PortTop05_RaceTickCalls);
    return r;
}
// stand-in for 0x00492e90 (frame render, 26 callees): the hand RenderFrame. A device loss posts WM_QUIT, which the
// matched pump 0x00499690 turns into the loop exit (the hand loop breaks on it).
int PortTop_StandIn_00492e90(void) {
    const int r = PortTop_HandRenderFrame();
    if (s_frame == 0 || (s_frame < 600 && (s_frame % 120) == 0))
        PortTop_Log("frame %u rendered (%d)", s_frame, r);
    ++s_frame;
    return r;
}
// rva 0x004926c0 AudioTickAndAvg: the exe body exists (Match/MatchS15_004926c0.cpp) but its 5 callees have none and its
// forwarders call raw original addresses, which fault in the exe. Not called; the hand audio runs on its own thread.
void PortTop_Missing_004926c0(void) {}
// rva 0x00493480 FpsDiscretise: the exe body exists but forwards to raw 0x00493390 (no exe body). Not called; the hand
// frame pacing is the device present + limiter inside RenderFrame.
int PortTop_Missing_00493480(void) { return 0; }

} // extern "C"

// ---- the matched bodies ----
#define FUN_00492370 Orig_00492370
#define FUN_00492290 Orig_00492290
#define FUN_00492d20 IntroSplashFrameTickShim      // matched, Match/MatchS15_00492d20.cpp (shared with the asi)
#define FUN_00499cc0 Orig_00499cc0                  // matched, PortTop_Leaves.cpp
// WinMain 0x00492370
#define FUN_00499ba0 Orig_00499ba0                  // matched, PortTop_00499ba0.cpp
#define FUN_00493900 Orig_00493900                  // matched (66.9% source), PortTop_00493900.cpp
#define FUN_00493540 PortTop_Bound_00493540       // matched, Match/MatchS15_00493540.cpp (PORT-TOP-04A)
#define FUN_004924f0 PortTop_StandIn_004924f0  // stand-in; MASHED_TOP04D_DATAZERO=1 runs the matched DataZeroFill (PORT-TOP-04D)
#define FUN_00492270 PortTop_Bound_00492270       // matched, Match/MatchS15_00492270.cpp (PORT-TOP-03)
#define FUN_005c9d00 GetRaceEndTrigger              // port, Util/GameStateGetters.cpp
#define FUN_004996f0 WindowShow                     // matched, Match/MatchS16_004996f0.cpp
#define FUN_004963e0 PortTop_CfgLogError           // stand-in (exe body not exe-visible, see above)
#define FUN_00402750 PortTop_Bound_00402750       // matched, Match/PortTop_00402750.cpp (PORT-TOP-04E)
#define FUN_00402a40 PortTop_Bound_00402a40       // matched, Match/PortTop_00402a40.cpp (PORT-TOP-04C)
#define FUN_004c5930 MashedRw_TexDictionaryDestroy_004c5930  // RW: RwTexDictionaryDestroy -> librw (PORT-TOP-03)
#define FUN_00493550 PortTop_Bound_00493550       // matched, Match/MatchS15_00493550.cpp (PORT-TOP-03)
#define FUN_00493560 PortTop_Bound_00493560       // matched, Match/MatchS15_00493560.cpp (PORT-TOP-04A)
// run function 0x00492290
#define FUN_00492770 PortTop_MainLoopInit
#define FUN_00499690 WindowMsgPump                  // matched, Match/MatchS16_00499690.cpp
#define FUN_004929d0 PortTop_Bound_004929d0     // matched, Match/PortTop_004929d0.cpp (PORT-TOP-05)
#define FUN_00492d30 PortTop_Bound_00492d30     // matched, Match/PortTop_00492d30.cpp (PORT-TOP-05)
#define FUN_00492e90 PortTop_StandIn_00492e90
#define FUN_004926c0 PortTop_Missing_004926c0
#define FUN_00493480 PortTop_Missing_00493480

extern "C" {
#include "../../../../re/match/s15/00492370.c"
#include "../../../../re/match/s15/00492290.c"

int ProvBody_0043dfd0_SelfTest(void);
int PortTop_WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow) {
    PortTop_HandPrelude();
    s_focusPause = (std::getenv("MASHED_ORIG_TOP_FOCUSPAUSE") != nullptr);
    PortTop_Log("MASHED_ORIG_TOP=1: WinMain 0x00492370 (matched) takes over");
    PortTop_SeedRdata();
    PortTop_NoFocusPause();
    ProvBody_0043dfd0_SelfTest();   // PROVISIONAL-BODIES: MASHED_PROV_43DFD0_SELFTEST=1 only (default off)
    const int r = Orig_00492370(static_cast<int>(reinterpret_cast<std::uintptr_t>(hInstance)),
                                static_cast<int>(reinterpret_cast<std::uintptr_t>(hPrevInstance)),
                                static_cast<int>(reinterpret_cast<std::uintptr_t>(lpCmdLine)), nCmdShow);
    PortTop_Log("WinMain 0x00492370 returned %d after %u frames", r, s_frame);
    return r;
}
} // extern "C"

#endif // MASHED_STANDALONE

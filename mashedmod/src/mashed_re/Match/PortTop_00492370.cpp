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
#include <intrin.h>

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
void PortTop_Bound_00493390(void);             // Match/PortTop_00493390.cpp (RO-BIND-TIMER)
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

// Config log 0x004963e0 / 0x00496400 (and log open / close 0x00496490 / 0x00496470): the matched bodies since
// BIND-CRT-STDIO-CALLERS (Match/PortTop_004963e0.cpp, PortTop_00496400.cpp, PortTop_00496490.cpp, PortTop_00496470.cpp),
// writing mashed.log in the origtop_rw overlay. Their fputs adapter mirrors each line into mashed_re.log ("cfglog:").

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
// ---- RO-BIND-TIMER (replay oracle 2): the tick boundary and the clock globals, sub-knob MASHED_OT_TIMER ----
// With MASHED_OT_TIMER=1 (default OFF) the matched run loop's last call runs its matched exe body FpsDiscretise
// (Match/MatchS15_00493480.cpp), whose callee 0x00493390 binds to Match/PortTop_00493390.cpp. With
// MASHED_OT_AUDIOTICK=1 (default OFF, own sub-knob) the call before it runs the matched AudioTickAndAvg
// (Match/MatchS15_004926c0.cpp). Its audio branch runs whenever DAT_006147b8 != 0, and that word is file-initialised
// to 1 (.data, seeded by Compat/PortStateSeed.inc), so it calls 0x0045d3f0, 0x00466b50 and 0x0045d430 every frame. Those
// run their matched exe bodies (RO-AUDIOTICK-BODIES, Match/PortTop_0045d3f0.cpp / PortTop_00466b50.cpp /
// PortTop_0045d430.cpp). Each returns at once while DAT_006904e8 == 0, and each of their callees binds to a named stop
// (PortTop_AudioCalleeStop below) that logs and ends the process. Off, each stays the no-op it was (the
// hand frame pacing is the device present + limiter inside RenderFrame). Every call runs a self-test against an
// independent model of the matched sources (re/match/s15/00493390.c, 00493480.c, 004926c0.c), from the globals read
// before it.
int  FpsDiscretise(void);                      // Match/MatchS15_00493480.cpp
void AudioTickAndAvg(void);                    // Match/MatchS15_004926c0.cpp
static int s_otTimer = -1, s_otAudio = -1;
static unsigned s_tmCalls480 = 0, s_tmCalls6c0 = 0, s_tmAudioBranch = 0, s_tmX87Form = 0, s_tmChecks = 0, s_tmFails = 0;
static int OtTimerOn(void) {
    if (s_otTimer < 0) {
        const char* const v = std::getenv("MASHED_OT_TIMER");
        s_otTimer = (v && v[0] && v[0] != '0') ? 1 : 0;
        PortTop_Log("[OT_TIMER] MASHED_OT_TIMER %s: %s", s_otTimer ? "on" : "off",
                    s_otTimer ? "0x00493480 (-> 0x00493390) runs its matched exe body" : "0x00493480 stays a no-op");
    }
    return s_otTimer;
}
static int OtAudioOn(void) {
    if (s_otAudio < 0) {
        const char* const v = std::getenv("MASHED_OT_AUDIOTICK");
        s_otAudio = (v && v[0] && v[0] != '0') ? 1 : 0;
        PortTop_Log("[OT_TIMER] MASHED_OT_AUDIOTICK %s: 0x004926c0 %s", s_otAudio ? "on" : "off",
                    s_otAudio ? "runs its matched exe body" : "stays a no-op");
    }
    return s_otAudio;
}
static int&      MemI(unsigned a) { return *reinterpret_cast<int*>(static_cast<std::uintptr_t>(a)); }
static unsigned& MemU(unsigned a) { return *reinterpret_cast<unsigned*>(static_cast<std::uintptr_t>(a)); }
static unsigned  FBits(float f) { unsigned u; std::memcpy(&u, &f, 4); return u; }
static unsigned  MemFBits(unsigned a) { return MemU(a); }
// (float)(u * c) with the product exact and one round-to-nearest-even to 24 bits: what x87 fild + fmul + fstp gives
// under PC_24, and under PC_53 / PC_64 while the exact product fits (u < 2^32, c a float: 56 bits at most, one rounding).
static unsigned  X87MulExact(unsigned u, float c) {
    if (u == 0) return FBits(0.0f * c);
    unsigned cb = FBits(c);
    const int ce = static_cast<int>((cb >> 23) & 0xFF) - 127 - 23;
    const unsigned long long cm = (cb & 0x7FFFFFu) | 0x800000u;
    unsigned long long p = static_cast<unsigned long long>(u) * cm;   // value = p * 2^ce
    int top = 63; while (!((p >> top) & 1ull)) --top;
    int e = ce;
    if (top > 23) {
        const int sh = top - 23;
        const unsigned long long rem = p & ((1ull << sh) - 1), half = 1ull << (sh - 1);
        p >>= sh; e += sh;
        if (rem > half || (rem == half && (p & 1ull))) { ++p; if (p >> 24) { p >>= 1; ++e; } }
    } else { p <<= (23 - top); e -= (23 - top); }
    const int be = e + 23 + 127;                                      // normal range only (the timer values are)
    return (cb & 0x80000000u) | (static_cast<unsigned>(be) << 23) | (static_cast<unsigned>(p) & 0x7FFFFFu);
}
static void TmCheck(bool ok, const char* what, unsigned got, unsigned want) {
    ++s_tmChecks;
    if (!ok && ++s_tmFails <= 8)
        PortTop_Log("[OT_TIMER] self-test FAIL %s: got 0x%08X expected 0x%08X (call %u)", what, got, want, s_tmCalls480);
}
static void PortTop_TimerSummary(void);
// RO-AUDIOTICK-BODIES: named stop for a callee of the matched 0x0045d3f0 / 0x00466b50 / 0x0045d430 exe bodies. Those
// bodies reach a callee only while DAT_006904e8 != 0, and the exe never sets that word (its only writer is 0x004669b0,
// run in the exe as the hand stand-in piece 3). A stop here means that gate changed.
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret) {
    PortTop_Log("[OT_TIMER] STOP: the matched 0x%08X called original 0x%08X (DAT_006904e8 = %d, DAT_006904ec = %d); the "
                "exe has no body for it. Return address %p", caller, rva, MemI(0x006904e8u), MemI(0x006904ecu), ret);
    PortTop_TimerSummary();
    std::exit(static_cast<int>(rva));
}
unsigned PortTop_Bound_0045d3f0(int);    // Match/PortTop_0045d3f0.cpp
void     PortTop_Bound_00466b50(void);   // Match/PortTop_00466b50.cpp
unsigned PortTop_Bound_0045d430(void);   // Match/PortTop_0045d430.cpp
int      PortTop_AudioInit_Ran(void);   // Match/PortTop_004669b0.cpp (PORT-TOP-4669B0): 1 once the matched 0x004669b0 ran

// Dispatcher (not a body) for the matched AudioTickAndAvg. Model (re/match/s15/004926c0.c): DAT_00771978 counts up and wraps to 0 at 60, where
// DAT_0077197c = DAT_00771994 / 60 and DAT_00771994 = 0. Otherwise DAT_00771994 grows by a QPC delta (>= 0).
void PortTop_Timer_004926c0(void) {
    if (!OtAudioOn()) return;
    const int cnt0 = MemI(0x00771978u), acc0 = MemI(0x00771994u);
    const int e8 = MemI(0x006904e8u), ec0 = MemI(0x006904ecu);
    AudioTickAndAvg();
    ++s_tmCalls6c0;
    const int cnt1 = MemI(0x00771978u), acc1 = MemI(0x00771994u);
    // RO-AUDIOTICK-BODIES: DAT_006147b8 != 0 after the call <=> the audio branch ran (key 4 zeroes it and skips the
    // branch). With DAT_006904e8 == 0, 0x0045d3f0 / 0x0045d430 return 1 and 0x00466b50 returns, writing nothing:
    // DAT_006904ec must be unchanged (re/match/s10/0045d3f0.c, 0045d430.c, 00466b50.c).
    if (MemI(0x006147b8u) != 0) {
        ++s_tmAudioBranch;
        // PORT-TOP-4669B0: under MASHED_OT_AUDIOINIT=1 the matched 0x004669b0 set the gate to 1; the bodies then reach
        // their callees (named stops) inside AudioTickAndAvg, before this check.
        TmCheck(e8 == PortTop_AudioInit_Ran(), "6c0 DAT_006904e8 gate on the exe path", static_cast<unsigned>(e8),
                static_cast<unsigned>(PortTop_AudioInit_Ran()));
        TmCheck(MemI(0x006904ecu) == ec0, "6c0 DAT_006904ec unchanged", MemU(0x006904ecu), static_cast<unsigned>(ec0));
        if (s_tmAudioBranch == 1) {
            const unsigned r3f0 = PortTop_Bound_0045d3f0(0), r430 = PortTop_Bound_0045d430();
            TmCheck(r3f0 == 1, "0045d3f0 return", r3f0, 1);
            TmCheck(r430 == 1, "0045d430 return", r430, 1);
            TmCheck(MemI(0x006904ecu) == ec0, "006904ec unchanged after direct calls", MemU(0x006904ecu), static_cast<unsigned>(ec0));
        }
    }
    if (cnt0 + 1 == 60) {
        TmCheck(cnt1 == 0, "6c0 counter wrap", cnt1, 0);
        TmCheck(acc1 == 0, "6c0 accumulator reset", acc1, 0);
        TmCheck(MemI(0x0077197cu) >= acc0 / 60, "6c0 average >= old/60", MemI(0x0077197cu), acc0 / 60);
    } else {
        TmCheck(cnt1 == cnt0 + 1, "6c0 counter", cnt1, cnt0 + 1);
        TmCheck(acc1 >= acc0, "6c0 accumulator grows", acc1, acc0);
    }
}
// Dispatcher (not a body) for the matched FpsDiscretise and its callee. Model of both from the globals before the call. The only free input
// is now = the QPC/3 MHz value 0x00493390 stores into DAT_007f1030.
int PortTop_Timer_00493480(void) {
    if (!OtTimerOn()) return 0;
    const int r84 = MemI(0x00771984u), r1030 = MemI(0x007f1030u), rff0 = MemI(0x007f0ff0u);
    const unsigned r102c = MemU(0x007f102cu);
    const int r1024 = MemI(0x007f1024u), r1028 = MemI(0x007f1028u), r9d4 = MemI(0x007719d4u);
    const int ret = FpsDiscretise();
    ++s_tmCalls480;
    const unsigned now = MemU(0x007f1030u);
    // 0x00493390
    const unsigned t = now + static_cast<unsigned>(r84);
    int ffc = static_cast<int>(t - static_cast<unsigned>(r1030));
    if (ffc > 200000) ffc = 200000; else if (ffc < 0) ffc = -ffc;
    int d = ffc - 50000;
    if (d > -64 && d < 64) d = 0; else { if (d < -2) d += 2; if (d > 2) d -= 2; }
    TmCheck(MemI(0x007f0ffcu) == ffc, "390 DAT_007f0ffc", MemI(0x007f0ffcu), ffc);
    TmCheck(MemI(0x00771984u) == d, "390 DAT_00771984", MemI(0x00771984u), d);
    // DAT_007f1010 = (float)now * 3.3333333e-07f. Two forms (RO-AUDIOTICK-BODIES): the VS2022 body converts now to float
    // first (cvtsi2ss, then mulss); the object build's DDK body loads now exact (fild) and rounds the product once. They
    // differ by 1 ulp once now >= 2^24. Either form passes; the summary counts the calls where only the x87 form held.
    const unsigned f1010 = FBits(static_cast<float>(now) * 3.3333333e-07f);
    const unsigned x1010 = X87MulExact(now, 3.3333333e-07f);
    const unsigned g1010 = MemFBits(0x007f1010u);
    if (g1010 != f1010 && g1010 == x1010) ++s_tmX87Form;
    TmCheck(g1010 == f1010 || g1010 == x1010, "390 DAT_007f1010", g1010, f1010);
    TmCheck(MemI(0x007f0ff0u) == rff0 + 50, "390 DAT_007f0ff0 += 50", MemI(0x007f0ff0u), rff0 + 50);
    const bool win = t > r102c + 3000000u;
    TmCheck(MemI(0x007f1028u) == (win ? r1024 : r1028), "390 DAT_007f1028", MemI(0x007f1028u), win ? r1024 : r1028);
    TmCheck(MemU(0x007f102cu) == (win ? t : r102c), "390 DAT_007f102c", MemU(0x007f102cu), win ? t : r102c);
    // 0x00493480 on DAT_007f1000 = 50 (0x00493390 stores it at 0x004933d5): t snaps to 50, a = acc + 50
    int a = r9d4 + 50, n = 0;
    if (a >= 50) { n = static_cast<int>(static_cast<unsigned>(a) / 50u); a -= n * 50; }
    if (a > 47) { ++n; a = 0; } else if (a < 3) a = 0;
    TmCheck(MemI(0x007719d4u) == a, "480 DAT_007719d4", MemI(0x007719d4u), a);
    TmCheck(MemI(0x007f1000u) == n * 50, "480 DAT_007f1000", MemI(0x007f1000u), n * 50);
    const unsigned dt = FBits(static_cast<float>(n * 50) * 0.00033333333f);
    TmCheck(MemFBits(0x007f1004u) == dt, "480 DAT_007f1004", MemFBits(0x007f1004u), dt);
    TmCheck(ret == 1, "480 return", ret, 1);
    if (s_tmCalls480 == 1 || (s_tmCalls480 % 120) == 0)
        PortTop_Log("[OT_TIMER] call %u: now %u, DAT_007f1000 %d, DAT_007f0ffc %d, DAT_00771984 %d, DAT_007f0ff0 %d, "
                    "DAT_00771978 %d; checks %u failed %u", s_tmCalls480, now, MemI(0x007f1000u), MemI(0x007f0ffcu),
                    MemI(0x00771984u), MemI(0x007f0ff0u), MemI(0x00771978u), s_tmChecks, s_tmFails);
    return ret;
}
static void PortTop_TimerSummary(void) {
    if (s_otTimer != 1 && s_otAudio != 1) return;
    PortTop_Log("[OT_TIMER] self-test %s (%u of %u checks failed), 0x00493480 calls %u, 0x004926c0 calls %u, audio branch "
                "%u, DAT_007f1010 x87-only form %u; bodies at %p / %p / %p; 0x0045d3f0 / 0x00466b50 / 0x0045d430 at %p / %p / %p",
                s_tmFails ? "FAIL" : "PASS", s_tmFails, s_tmChecks, s_tmCalls480, s_tmCalls6c0, s_tmAudioBranch, s_tmX87Form,
                reinterpret_cast<void*>(&FpsDiscretise), reinterpret_cast<void*>(&AudioTickAndAvg),
                reinterpret_cast<void*>(&PortTop_Bound_00493390), reinterpret_cast<void*>(&PortTop_Bound_0045d3f0),
                reinterpret_cast<void*>(&PortTop_Bound_00466b50), reinterpret_cast<void*>(&PortTop_Bound_0045d430));
}

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
#define FUN_004963e0 Orig_004963e0                // matched, Match/PortTop_004963e0.cpp (BIND-CRT-STDIO-CALLERS)
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
#define FUN_004926c0 PortTop_Timer_004926c0     // RO-BIND-TIMER: matched body under MASHED_OT_AUDIOTICK=1, else no-op
#define FUN_00493480 PortTop_Timer_00493480     // RO-BIND-TIMER: matched body under MASHED_OT_TIMER=1, else returns 0

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
    PortTop_TimerSummary();
    return r;
}
} // extern "C"

#endif // MASHED_STANDALONE

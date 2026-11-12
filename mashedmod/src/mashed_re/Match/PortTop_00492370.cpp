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
// rva 0x004929d0 game-mode machine: the port wrapper GameModeMachine_Tick (Race/GameModeMachine.cpp), the hand loop's
// call for this job (default it is a no-op; MASHED_PORTSTATE_GAMEFLOW_FU=1 runs the matched 0x004929d0).
static unsigned s_frame = 0;
int PortTop_Port_004929d0(void) {
    if (s_frame == 0) PortTop_Log("first frame: run function 0x00492290 loop entered");
    PortTop_HandGameModeMachine();
    return 1;
}
// rva 0x004967e0 (input poll, C2, no exe body) under the matched 0x00492d20: the hand keyboard poll.
void PortTop_StandIn_004967e0(void) {
    PortTop_HandPollKeyboard();
}
// stand-in for 0x00492d30 GameTickStateMachine7 (7-case machine on DAT_00771968, unported tree): the hand frame update
// (deterministic frame cap, harness steps, menu navigation, input globals, GameFlow_Update).
int PortTop_StandIn_00492d30(void) {
    PortTop_HandFrameUpdate();
    return 1;
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
// With MASHED_OT_TIMER=1 (default OFF) the matched run loop's last two calls run their matched exe bodies:
// AudioTickAndAvg (Match/MatchS15_004926c0.cpp) and FpsDiscretise (Match/MatchS15_00493480.cpp),
// whose callee 0x00493390 binds to Match/PortTop_00493390.cpp. Off, both stay the no-ops they were (the hand frame
// pacing is the device present + limiter inside RenderFrame). Every call runs a self-test against an independent
// model of the matched sources (re/match/s15/00493390.c, 00493480.c, 004926c0.c), from the globals read before it.
int  FpsDiscretise(void);                      // Match/MatchS15_00493480.cpp
void AudioTickAndAvg(void);                    // Match/MatchS15_004926c0.cpp
static int s_otTimer = -1;
static unsigned s_tmCalls480 = 0, s_tmCalls6c0 = 0, s_tmChecks = 0, s_tmFails = 0;
static int OtTimerOn(void) {
    if (s_otTimer < 0) {
        const char* const v = std::getenv("MASHED_OT_TIMER");
        s_otTimer = (v && v[0] && v[0] != '0') ? 1 : 0;
        PortTop_Log("[OT_TIMER] %s: 0x004926c0 / 0x00493480 (-> 0x00493390) %s", s_otTimer ? "on" : "off",
                    s_otTimer ? "run their matched exe bodies" : "stay no-ops");
    }
    return s_otTimer;
}
static int&      MemI(unsigned a) { return *reinterpret_cast<int*>(static_cast<std::uintptr_t>(a)); }
static unsigned& MemU(unsigned a) { return *reinterpret_cast<unsigned*>(static_cast<std::uintptr_t>(a)); }
static unsigned  FBits(float f) { unsigned u; std::memcpy(&u, &f, 4); return u; }
static unsigned  MemFBits(unsigned a) { return MemU(a); }
static void TmCheck(bool ok, const char* what, unsigned got, unsigned want) {
    ++s_tmChecks;
    if (!ok && ++s_tmFails <= 8)
        PortTop_Log("[OT_TIMER] self-test FAIL %s: got 0x%08X expected 0x%08X (call %u)", what, got, want, s_tmCalls480);
}
static void PortTop_TimerSummary(void);
static void PortTop_Stop(unsigned rva, void* ret) {
    PortTop_Log("[OT_TIMER] STOP: the matched 0x004926c0 called original 0x%08X (DAT_006147b8 = %d); the exe has no "
                "body for it. Return address %p", rva, MemI(0x006147b8u), ret);
    PortTop_TimerSummary();
    std::exit(static_cast<int>(rva));
}
unsigned PortTop_Stop_0045d3f0(int) { PortTop_Stop(0x0045d3f0u, _ReturnAddress()); return 0; }
void     PortTop_Stop_00466b50(int) { PortTop_Stop(0x00466b50u, _ReturnAddress()); }
unsigned PortTop_Stop_0045d430(void) { PortTop_Stop(0x0045d430u, _ReturnAddress()); return 0; }

// Dispatcher (not a body) for the matched AudioTickAndAvg. Model (re/match/s15/004926c0.c): DAT_00771978 counts up and wraps to 0 at 60, where
// DAT_0077197c = DAT_00771994 / 60 and DAT_00771994 = 0. Otherwise DAT_00771994 grows by a QPC delta (>= 0).
void PortTop_Timer_004926c0(void) {
    if (!OtTimerOn()) return;
    const int cnt0 = MemI(0x00771978u), acc0 = MemI(0x00771994u);
    AudioTickAndAvg();
    ++s_tmCalls6c0;
    const int cnt1 = MemI(0x00771978u), acc1 = MemI(0x00771994u);
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
    const unsigned f1010 = FBits(static_cast<float>(now) * 3.3333333e-07f);
    TmCheck(MemFBits(0x007f1010u) == f1010, "390 DAT_007f1010", MemFBits(0x007f1010u), f1010);
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
    if (s_otTimer != 1) return;
    PortTop_Log("[OT_TIMER] self-test %s (%u of %u checks failed), 0x00493480 calls %u, 0x004926c0 calls %u; bodies at "
                "%p / %p / %p", s_tmFails ? "FAIL" : "PASS", s_tmFails, s_tmChecks, s_tmCalls480, s_tmCalls6c0,
                reinterpret_cast<void*>(&FpsDiscretise), reinterpret_cast<void*>(&AudioTickAndAvg),
                reinterpret_cast<void*>(&PortTop_Bound_00493390));
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
#define FUN_004963e0 PortTop_CfgLogError           // stand-in (exe body not exe-visible, see above)
#define FUN_00402750 PortTop_Bound_00402750       // matched, Match/PortTop_00402750.cpp (PORT-TOP-04E)
#define FUN_00402a40 PortTop_Bound_00402a40       // matched, Match/PortTop_00402a40.cpp (PORT-TOP-04C)
#define FUN_004c5930 MashedRw_TexDictionaryDestroy_004c5930  // RW: RwTexDictionaryDestroy -> librw (PORT-TOP-03)
#define FUN_00493550 PortTop_Bound_00493550       // matched, Match/MatchS15_00493550.cpp (PORT-TOP-03)
#define FUN_00493560 PortTop_Bound_00493560       // matched, Match/MatchS15_00493560.cpp (PORT-TOP-04A)
// run function 0x00492290
#define FUN_00492770 PortTop_MainLoopInit
#define FUN_00499690 WindowMsgPump                  // matched, Match/MatchS16_00499690.cpp
#define FUN_004929d0 PortTop_Port_004929d0
#define FUN_00492d30 PortTop_StandIn_00492d30
#define FUN_00492e90 PortTop_StandIn_00492e90
#define FUN_004926c0 PortTop_Timer_004926c0     // RO-BIND-TIMER: matched body under MASHED_OT_TIMER=1, else no-op
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

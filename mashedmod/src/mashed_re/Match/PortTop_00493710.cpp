// PORT-TOP-03 (L-96 top-down lane, L-98): the RW engine bring-up 0x00493710 for the MASHED_ORIG_TOP path. EXE ONLY
// (exe_sources.rsp, not asi_sources.rsp).
//
// Compiles re/match/s15/00493710.c (MATCH-S15 h8, best 94.2%, the only source of this RVA; hooks.csv has no exe body
// for it) as Orig_00493710. SubsystemInit 0x00492270 (Match/MatchS15_00492270.cpp) calls it through
// PortTop_Bound_00493710 under MASHED_STANDALONE. Its RenderWare core calls bind to the RW API binding layer
// (LibRw/RwApiBind.cpp, librw); its game callees bind by name in the PLAN 7 order (matched exe body, port body,
// stand-in, missing stub). The table is verify/porttop_20261108/BINDINGS.tsv.
//
// Also here: the two other SubsystemInit callees, DisplayInit 0x004921d0 and ViewportInit 0x00428590. Both have exe
// bodies (Match/MatchS15_004921d0.cpp, Match/MatchS06_00428590.cpp) whose callees still forward to raw original
// addresses, which fault in the exe. They are not run under the knob (missing stubs below), as before PORT-TOP-03.

#ifdef MASHED_STANDALONE

#include <cstdarg>
#include <cstdio>
#include <cstring>

#define MATCH_PORT 1

extern "C" {

static void PortTop93_Log(const char* fmt, ...) {
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

// ---- game callees of 0x00493710 with no exe body ----
// rva 0x00495780 HardwareInstallFileSystem (calls 0x004955d0, no exe body; installs RenderWare file systems for
// RtFSManager, which librw does not have: see MashedRw_RtFSManagerOpen_00550350). Missing: no-op.
void PortTop_Missing_00495780(void) {
    PortTop93_Log("0x00495780 HardwareInstallFileSystem: missing (0x004955d0 unported, no RtFSManager in librw)");
}
// rva 0x004951f0 HardwareSetVideoMode (re/match/s16/004951f0.c): mode selection through 0x00499400 on DAT_006147bc,
// then caps 0x004cbb60 into DAT_00771e58 and two shader-version log lines. The reference runs with the skip_selector
// patch at 0x004951f0 (MOV EAX,[0x006147bc] -> XOR EAX,EAX). Stand-in: the exe's windowed 640x480 device is chosen by
// InitD3D9 inside the RwEngineStart binding (the hand path's choice). Returns 1 (the original's success value).
int PortTop_StandIn_004951f0(void) {
    PortTop93_Log("0x004951f0 HardwareSetVideoMode: stand-in (exe InitD3D9 picks the windowed mode at RwEngineStart)");
    return 1;
}
// rva 0x00493600: two 0x004ce790 calls with ("bmp", 0x004935a0, 0x0052d530) and ("png", 0x004935d0, 0x00513f90)
// (re/match/s15/00493600.c). [PORT-OT-IMAGE-FMT] bound to PortTop_Bound_00493600 (Match/PortTop_00493600.cpp): with
// MASHED_ORIG_TOP_RWMOD=1 the matched body runs from RegisterRwModules once the RwGlobals image module exists; with the
// knob OFF it returns 1 without registering, as the missing stub did.
// rva 0x004936f0 / 0x00493700: function pointers 0x00493710 hands to 0x004cbc80 / 0x004cbc60. Not in hooks.csv, no exe
// body. The RW API shims keep the pointers and never call them; these exist so the matched source links.
void PortTop_Missing_004936f0(void) { PortTop93_Log("0x004936f0 called: missing body"); }
void PortTop_Missing_00493700(void) { PortTop93_Log("0x00493700 called: missing body"); }
// rva 0x004a2cbd = printf (third-party-library[msvc-crt]). The original prints to the stdout of a GUI process (no
// console). Exe: the formatted line goes to mashed_re.log.
int PortTop_Crt_004a2cbd(char* fmt, ...) {
    char buf[0x200];
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsnprintf(buf, sizeof(buf), fmt ? fmt : "", ap);
    va_end(ap);
    PortTop93_Log("printf: %.*s", static_cast<int>(std::strcspn(buf, "\n")), buf);
    return n;
}

// ---- ViewportInit 0x00428590's loading-disc draw (bound from Match/MatchS06_00428590.cpp, PORT-TOP-04B) ----
// rva 0x00428450: sin of DAT_0067d974, a white quad of the LoadIcon texture (*DAT_00771960) through 0x00450b10, then
// DAT_0067d974 += 0.1 (re/match/s06/00428450.c, best 84.6%). Its only port body is the .asi-only naked-asm HudSpinCoinAnim
// (HUD/TextCluster.cpp, raw-address calls), and an exe copy of the 84.6% source would be a second body of this RVA (rva-lint
// CROSS-TARGET). Missing: nothing is drawn, DAT_0067d974 keeps the 1.5707964 ViewportInit stored.
void PortTop_Missing_00428450(int x, int y) {
    PortTop93_Log("0x00428450 loading-disc draw (%d, %d): missing body, not drawn", x, y);
}

// The RW API binding layer (LibRw/RwApiBind.cpp) symbols are declared by the matched source itself, through the
// #defines below (C linkage, the source's own prototypes).

} // extern "C"

// ---- bindings of rva 0x00493710 ----
#define FUN_00493710 Orig_00493710
#define FUN_005c9d00 GetRaceEndTrigger                       // port, Util/GameStateGetters.cpp (xor eax,eax; ret)
#define FUN_004963e0 Orig_004963e0                           // matched, Match/PortTop_004963e0.cpp (BIND-CRT-STDIO-CALLERS)
#define FUN_004c32b0 MashedRw_EngineInit_004c32b0            // RW: RwEngineInit -> rw::Engine::init
#define FUN_00550350 MashedRw_RtFSManagerOpen_00550350       // RW: RtFSManagerOpen -> shim
#define thunk_FUN_00495780 PortTop_Missing_00495780          // missing
#define FUN_00495270 HWNDGet                                 // matched, Match/MatchS16_00495270.cpp (exe since PORT-TOP-03)
#define FUN_00493640 MashedRw_AttachPlugins_00493640         // stand-in: librw plugin registration
#define FUN_004c30b0 MashedRw_EngineOpen_004c30b0            // RW: RwEngineOpen -> rw::Engine::open
#define FUN_00550390 MashedRw_RtFSManagerClose_00550390      // RW: RtFSManagerClose -> shim
#define FUN_004c3270 MashedRw_EngineTerm_004c3270            // RW: RwEngineTerm -> rw::Engine::term
#define FUN_004c3040 MashedRw_EngineClose_004c3040           // RW: RwEngineClose -> rw::Engine::close
#define FUN_004951f0 PortTop_StandIn_004951f0                // stand-in
#define FUN_004c9f50 MashedRw_D3D9Set7d4134_004c9f50         // RW D3D9 driver setting -> shim
#define FUN_004c9eb0 MashedRw_D3D9SetRefreshRate_004c9eb0    // RW D3D9 driver setting -> shim
#define FUN_004c9f60 MashedRw_D3D9Set7d413c_004c9f60         // RW D3D9 driver setting -> shim
#define FUN_004c2fb0 MashedRw_EngineStart_004c2fb0           // RW: RwEngineStart -> exe device + rw::Engine::start
#define FUN_0045b350 RwInitNullStub                          // matched, Match/MatchS09_0045b350.cpp (exe since PORT-TOP-03)
#define FUN_004c2f00 MashedRw_EngineGetCurrentMode_004c2f00  // RW: RwEngineGetCurrentMode -> rw::Engine::getCurrentVideoMode
#define FUN_004c2ed0 MashedRw_EngineGetModeInfo_004c2ed0     // RW: RwEngineGetModeInfo -> rw::Engine::getVideoModeInfo
#define FUN_004a2cbd PortTop_Crt_004a2cbd                    // CRT printf -> mashed_re.log
#define FUN_00493600 PortTop_Bound_00493600                  // matched, Match/PortTop_00493600.cpp (deferred under RWMOD)
#define FUN_005584c0 MashedRw_Alloc7dc8b8_005584c0           // RW core -> shim
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80  // RW: RwTexDictionarySetCurrent -> rw::TexDictionary::setCurrent
#define FUN_004cbc90 MashedRw_Get7d459c_004cbc90             // RW core global get -> shim
#define FUN_004cbc80 MashedRw_Set7d459c_004cbc80             // RW core global set -> shim
#define FUN_004cbc70 MashedRw_Get7d4598_004cbc70             // RW core global get -> shim
#define FUN_004cbc60 MashedRw_Set7d4598_004cbc60             // RW core global set -> shim
#define FUN_004936f0 PortTop_Missing_004936f0
#define FUN_00493700 PortTop_Missing_00493700

extern "C" {
#include "../../../../re/match/s15/00493710.c"

// SubsystemInit 0x00492270's call (it passes one argument, 0; 0x00493710 reads none).
unsigned int PortTop_Bound_00493710(int p0) {
    (void)p0;
    const unsigned int r = Orig_00493710();
    PortTop93_Log("0x00493710 RW engine bring-up (matched, RW calls on librw) returned %u", r);
    return r;
}
} // extern "C"

#endif // MASHED_STANDALONE

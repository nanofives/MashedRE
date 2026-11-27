// PORT-TOP-03 (L-96 top-down lane, L-98): the RW engine bring-up 0x00493710 for the MASHED_ORIG_TOP path. EXE ONLY
// (exe_sources.rsp, not asi_sources.rsp).
//
// Compiles re/match/pilot/a2_00493710.c (100%, --bytes BYTE-IDENTICAL, relocheck OK with StubA/StubB/StubC/printf
// unchecked) as Orig_00493710. DDKOBJ-RETARGET-493710 (2026-11-15) moved it off the s15 draft re/match/s15/00493710.c
// (94.2%, relocheck BAD), so the object build links the certified bytes
// (verify/ddkobj_20261114/RESULT_DDKOBJ-RETARGET-493710.md). The pilot source has no port preamble: its two globals and
// its StubA/StubB/StubC/printf/FUN_004951e0 names are bound below. SubsystemInit 0x00492270 (Match/MatchS15_00492270.cpp) calls it through
// PortTop_Bound_00493710 under MASHED_STANDALONE. Its RenderWare core calls bind to the RW API binding layer
// (LibRw/RwApiBind.cpp, librw); its game callees bind by name in the PLAN 7 order (matched exe body, port body,
// stand-in, missing stub). The table is verify/porttop_20261108/BINDINGS.tsv.
//
// Also here: the two other SubsystemInit callees, DisplayInit 0x004921d0 and ViewportInit 0x00428590. Both have exe
// bodies (Match/MatchS15_004921d0.cpp, Match/MatchS06_00428590.cpp) whose callees still forward to raw original
// addresses, which fault in the exe. They are not run under the knob (missing stubs below), as before PORT-TOP-03.

#ifdef MASHED_STANDALONE

#include <cstdarg>
#include <cstdint>
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
// rva 0x004951e0: `jmp 0x00495780` (the pilot source's comment), the call 0x00493710 makes for HardwareInstallFileSystem.
// The pilot source discards its return value. Forwards to the missing stub above.
// RACE-VFS-COURSELUA (run 56): on the RW core frame route (MASHED_RWC_CORE + MASHED_RWC_FRAME, both default OFF), where
// RtFSManagerOpen already runs its core body RwCore_00550350 (LibRw/RwApiBind.cpp, [RWC-FRAME]), the thunk runs the
// matched HardwareInstallFileSystem PortTop_Bound_00495780 (re/match/s16/00495780.c) -> PortTop_Bound_004955d0
// (re/match/s16/004955d0.c): the disk file systems through 0x00551190 + 0x00550430 on the core's FS globals
// (DAT_007dc754 list, DAT_007dc75c count) and the default device through 0x00550790 (DAT_007dc76c), as the original
// does (verify/racelua2_20261122/RESULT_RACE-LOAD-LUA-2.md section 4, vfs_boot2.jsonl). Off that route: missing, as before.
bool RwCore_FrameRoute(void);
bool PortTop_Bound_00495780(void);
int PortTop_Thunk_004951e0(void) {
    if (RwCore_FrameRoute()) {
        const bool r = PortTop_Bound_00495780();
        PortTop93_Log("0x00495780 HardwareInstallFileSystem (matched, RW core FS) returned %d: [0x007dc75c] %d file "
                      "systems, default [0x007dc76c] %08X", r ? 1 : 0, *reinterpret_cast<volatile int*>(0x007dc75cu),
                      *reinterpret_cast<volatile unsigned*>(0x007dc76cu));
        return r ? 1 : 0;
    }
    PortTop_Missing_00495780();
    return 0;
}
// rva 0x005c9d00 (`xor eax,eax; ret`, identical-code folded): the pilot source calls it under three names, StubA(&buf)
// and StubB(&buf) with one pointer argument and StubC() with none. Exe body GetRaceEndTrigger
// (Match/PortBatch_GAMEPLAY_005c9d00.cpp, C linkage). StubA / StubB bind to it by name below; StubC has another
// prototype in the source, so it goes through this forwarder (one C-linkage name cannot carry two prototypes in C++).
int GetRaceEndTrigger(void* p);
int PortTop93_StubC_005c9d00(void) { return GetRaceEndTrigger(0); }
// rva 0x007719d8 / 0x007719dc: the two globals 0x00493710 stores (pilot source `extern int`, hidden from the port by
// its #ifndef MATCH_PORT). Bound to their original addresses below.
static const std::uintptr_t k_007719d8 = 0x007719d8u;
static const std::uintptr_t k_007719dc = 0x007719dcu;
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
int PortTop_Crt_004a2cbd(const char* fmt, ...) {
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
// RWC-FIRST-FRAME-IM2D: under MASHED_RWC_FRAME (RwCore_FrameRoute, RwCore/RwCore_Knob.cpp) the call goes to the RW core
// closure's body RwCore_00428450 (the 84.6% source, provisional, PROVISIONAL.tsv), which draws through the matched
// Im2D quad RwCore_00450b10 (86.6%, provisional) and the core's device block. The comparison against the original
// (verify/rwcim2d_20261113) needs the call's inputs and the Im2D traffic, so for this one call the RwGlobals entries
// +0x20 (fpRenderStateSet) and +0x30 (fpIm2DRenderPrimitive) are wrapped by logging thunks and restored after.
bool RwCore_FrameRoute(void);
void RwCore_00428450(int x, int y);
typedef int(__cdecl* PortTop93_RsSet)(int state, unsigned value);
typedef int(__cdecl* PortTop93_Im2DPrim)(int type, const unsigned* verts, int n);
static PortTop93_RsSet s_im2dRs;
static PortTop93_Im2DPrim s_im2dPrim;
static int __cdecl PortTop93_LogRsSet(int state, unsigned value) {
    const int r = s_im2dRs(state, value);
    PortTop93_Log("[IM2D] RenderStateSet(%d, 0x%08X) = %d", state, value, r);
    return r;
}
static int __cdecl PortTop93_LogIm2DPrim(int type, const unsigned* verts, int n) {
    PortTop93_Log("[IM2D] Im2DRenderPrimitive(type %d, verts %08X, n %d)", type, static_cast<unsigned>(
                  reinterpret_cast<unsigned long>(verts)), n);
    for (int i = 0; i < n && i < 8; ++i) {
        const unsigned* v = verts + i * 7;   // RwIm2DVertex, 0x1c bytes: x y z rhw color u v
        float f[7];
        std::memcpy(f, v, sizeof(f));
        PortTop93_Log("[IM2D] v%d hex %08X %08X %08X %08X %08X %08X %08X = x %.9g y %.9g z %.9g rhw %.9g col %08X u %.9g "
                      "v %.9g", i, v[0], v[1], v[2], v[3], v[4], v[5], v[6], f[0], f[1], f[2], f[3], v[4], f[5], f[6]);
    }
    const int r = s_im2dPrim(type, verts, n);
    PortTop93_Log("[IM2D] Im2DRenderPrimitive = %d", r);
    return r;
}
void PortTop_Missing_00428450(int x, int y) {
    if (RwCore_FrameRoute()) {
        unsigned char* const g = *reinterpret_cast<unsigned char**>(0x007d3ff8);   // RwGlobals
        const unsigned tex = *reinterpret_cast<unsigned*>(0x00771960)
                                 ? **reinterpret_cast<unsigned**>(0x00771960) : 0u;
        float ang;
        std::memcpy(&ang, reinterpret_cast<void*>(0x0067d974), 4);
        PortTop93_Log("[IM2D] 0x00428450(%d, %d) on the RW core: [0x0067d974] %08X (%.9g), [0x00771960] %08X -> tex %08X, "
                      "screen [0x0067ea54] %u x [0x0067ea56] %u, RwGlobals %08X", x, y,
                      *reinterpret_cast<unsigned*>(0x0067d974), ang, *reinterpret_cast<unsigned*>(0x00771960), tex,
                      *reinterpret_cast<unsigned short*>(0x0067ea54), *reinterpret_cast<unsigned short*>(0x0067ea56),
                      static_cast<unsigned>(reinterpret_cast<unsigned long>(g)));
        s_im2dRs = *reinterpret_cast<PortTop93_RsSet*>(g + 0x20);
        s_im2dPrim = *reinterpret_cast<PortTop93_Im2DPrim*>(g + 0x30);
        *reinterpret_cast<PortTop93_RsSet*>(g + 0x20) = PortTop93_LogRsSet;
        *reinterpret_cast<PortTop93_Im2DPrim*>(g + 0x30) = PortTop93_LogIm2DPrim;
        RwCore_00428450(x, y);
        *reinterpret_cast<PortTop93_RsSet*>(g + 0x20) = s_im2dRs;
        *reinterpret_cast<PortTop93_Im2DPrim*>(g + 0x30) = s_im2dPrim;
        std::memcpy(&ang, reinterpret_cast<void*>(0x0067d974), 4);
        PortTop93_Log("[IM2D] 0x00428450 returned; [0x0067d974] %08X (%.9g)", *reinterpret_cast<unsigned*>(0x0067d974), ang);
        return;
    }
    PortTop93_Log("0x00428450 loading-disc draw (%d, %d): missing body, not drawn", x, y);
}

// The RW API binding layer (LibRw/RwApiBind.cpp) symbols are declared by the matched source itself, through the
// #defines below (C linkage, the source's own prototypes).

} // extern "C"

// ---- bindings of rva 0x00493710 ----
#define FUN_00493710 Orig_00493710
#define StubA GetRaceEndTrigger                              // rva 0x005c9d00, Match/PortBatch_GAMEPLAY_005c9d00.cpp
#define StubB GetRaceEndTrigger                              // rva 0x005c9d00
#define StubC PortTop93_StubC_005c9d00                       // rva 0x005c9d00 through the forwarder above
#define DAT_007719d8 (*reinterpret_cast<int*>(k_007719d8))   // original address
#define DAT_007719dc (*reinterpret_cast<int*>(k_007719dc))   // original address
#define printf PortTop_Crt_004a2cbd                          // rva 0x004a2cbd CRT printf -> mashed_re.log
#define FUN_004963e0 Orig_004963e0                           // matched, Match/PortTop_004963e0.cpp (BIND-CRT-STDIO-CALLERS)
#define FUN_004c32b0 MashedRw_EngineInit_004c32b0            // RW: RwEngineInit -> rw::Engine::init (source passes 3 args, cdecl)
#define FUN_00550350 MashedRw_RtFSManagerOpen_00550350       // RW: RtFSManagerOpen -> shim
#define FUN_004951e0 PortTop_Thunk_004951e0                  // jmp 0x00495780 -> missing stub
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
#include "../../../../re/match/pilot/a2_00493710.c"

// SubsystemInit 0x00492270's call (it passes one argument, 0; 0x00493710 reads none).
unsigned int PortTop_Bound_00493710(int p0) {
    (void)p0;
    const unsigned int r = Orig_00493710();
    PortTop93_Log("0x00493710 RW engine bring-up (matched, RW calls on librw) returned %u", r);
    return r;
}
} // extern "C"

#endif // MASHED_STANDALONE

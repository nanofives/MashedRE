// LibRw/RwApiBind.cpp -- PORT-TOP-03 (L-96 top-down lane, L-98): the RW API binding layer. EXE ONLY.
//
// Under L-98 the RenderWare core stays librw: the matched game bodies the MASHED_ORIG_TOP path runs call RW core
// entry points by their original names (FUN_<rva> in the re/match sources), and the exe binds each one here to a
// librw-backed function of the same C signature (cdecl, int-sized arguments as the matched source declares them).
// The original's own RW core bodies (0x004c32b0 ...) read the RwGlobals pointer at 0x007d3ff8, which the exe maps
// zero-filled, so they are never the exe's binding.
//
// One block per entry point: the RW 3.x API it implements, the call site(s) in the matched sources, and the librw
// call. Entries with no librw equivalent are explicit shims, each saying what it does instead. The table with every
// RW row of the knob path is verify/porttop_20261108/BINDINGS.tsv (kind bound / shim).
//
// The device: in the original, RwEngineStart creates the D3D9 device (RW's D3D9 driver) and RwEngineStop releases
// it. In the exe the device is the exe's own (InitD3D9, exe_main.cpp) and librw adopts it (deps/librw/MASHED_PATCHES.md
// P1). So the RwEngineStart binding creates the exe device and starts librw on it, and the RwEngineStop binding stops
// librw and releases the exe device. Between them the state is the one the hand path's HandTop_InitDevice leaves.
//
// Isolated TU like RwBridge.cpp (compiled with the librw include path, mashedmod/build.bat). Nothing in
// asi_sources.rsp may reference this file.

#include "RwBridge.h"
#include "RwRaceSubmit.h"

#define WITH_D3D
#include <rw.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

void BindLog(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[RW_BIND] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

HWND s_rwWindow = nullptr;          // RwEngineOpenParams.displayID of the last RwEngineOpen
bool s_rwOpen = false;              // Engine::open succeeded and close has not run
bool s_rwStarted = false;           // Engine::start succeeded and stop has not run

// Values the shims below keep (the original stores them in RW core globals; nothing in the exe reads them).
unsigned s_shim7d4134 = 0, s_shim6181c4 = 0, s_shim7d413c = 0;
unsigned s_shim7d4598 = 0, s_shim7d459c = 0;

}  // namespace

extern "C" {

// exe side (exe_main.cpp PORT-TOP export block)
void* PortTop_HandCreateDevice(void);
void  PortTop_HandDeviceShutdown(void);
int   PortTop_Width(void);
int   PortTop_Height(void);

// ---------------------------------------------------------------------------------------------------------------
// Engine lifecycle (RW 3.x rwengine: RwEngineInit -> plugin attach -> RwEngineOpen -> RwEngineStart, and back down
// with RwEngineStop -> RwEngineClose -> RwEngineTerm). Call sites: 0x00493710 (bring-up, re/match/s15/00493710.c)
// and 0x004938c0 (tear-down, re/match/s15/004938c0.c).

// RwBool RwEngineInit(const RwMemoryFunctions* memFuncs, RwUInt32 initFlags/resArenaSize). The matched 0x00493710
// passes two arguments, both results of the xor-eax stub 0x005c9d00 (= 0). librw: rw::Engine::init() with its default
// memory functions (the call accepts a MemoryFunctions*; NULL selects the default, as RW 3.x does for NULL memFuncs).
int MashedRw_EngineInit_004c32b0(int memFuncs, int arenaSize) {
    (void)memFuncs; (void)arenaSize;
    const bool ok = mashed_re::LibRw::EnginePhaseInit();
    BindLog("RwEngineInit -> rw::Engine::init %s", ok ? "ok" : "FAILED");
    return ok ? 1 : 0;
}

// RenderwareAttachPlugins 0x00493640 (game function, 14 RpXxxPluginAttach-shaped calls + 3 more, re/match/s15/00493640.c):
// in RW 3.x plugins attach between RwEngineInit and RwEngineOpen, which is where librw requires its plugin registration
// too. Stand-in: librw's plugin set (RwBridge.cpp RegisterPlugins, the list the hand path registers). Returns 1 like the
// original on success.
int MashedRw_AttachPlugins_00493640(void) {
    mashed_re::LibRw::EnginePhasePlugins();
    BindLog("0x00493640 RenderwareAttachPlugins -> librw plugin registration (stand-in)");
    return 1;
}

// RwBool RwEngineOpen(RwEngineOpenParams* params), params->displayID = the window (0x00495270 HWNDGet writes
// DAT_007e9584 into the first dword at 0x00493710). librw: rw::Engine::open({window}) + select the last subsystem.
int MashedRw_EngineOpen_004c30b0(int* params) {
    s_rwWindow = params ? reinterpret_cast<HWND>(static_cast<std::uintptr_t>(static_cast<unsigned>(params[0]))) : nullptr;
    const bool ok = mashed_re::LibRw::EnginePhaseOpen(s_rwWindow);
    s_rwOpen = ok;
    BindLog("RwEngineOpen(hwnd=%p) -> rw::Engine::open %s", static_cast<void*>(s_rwWindow), ok ? "ok" : "FAILED");
    return ok ? 1 : 0;
}

// RwBool RwEngineStart(void). The original's D3D9 driver creates the device here. Exe: InitD3D9 makes the exe device,
// librw adopts it and starts (rw::Engine::start), then the librw race renderer takes the camera / state block step it
// takes after the hand bring-up (RaceSubmit_InitOnStartedEngine).
int MashedRw_EngineStart_004c2fb0(void) {
    void* const dev = PortTop_HandCreateDevice();
    if (!dev) { BindLog("RwEngineStart: exe InitD3D9 FAILED"); return 0; }
    if (!mashed_re::LibRw::EnginePhaseStartAdopted(dev)) {
        BindLog("RwEngineStart -> rw::Engine::start on the adopted device FAILED");
        return 0;
    }
    s_rwStarted = true;
    if (!mashed_re::LibRw::RaceSubmit_InitOnStartedEngine(s_rwWindow, static_cast<IDirect3DDevice9*>(dev),
                                                         PortTop_Width(), PortTop_Height())) {
        BindLog("RwEngineStart: librw race renderer init FAILED (see log/librw_race.txt)");
        return 0;
    }
    BindLog("RwEngineStart -> exe device %p adopted, rw::Engine::start ok", dev);
    return 1;
}

// RwBool RwEngineStop(void). librw: scene objects of the race renderer, rw::Engine::stop (frees librw's device-side
// buffers while the device still exists), then the exe releases its device (ShutdownD3D9) -- the counterpart of the
// device creation in the RwEngineStart binding.
int MashedRw_EngineStop_004c2f60(void) {
    if (!s_rwStarted) { BindLog("RwEngineStop: engine not started"); return 0; }
    mashed_re::LibRw::RaceSubmit_ShutdownSceneOnly();
    rw::Engine::stop();
    s_rwStarted = false;
    PortTop_HandDeviceShutdown();
    BindLog("RwEngineStop -> rw::Engine::stop, exe device released");
    return 1;
}

// RwBool RwEngineClose(void). librw: rw::Engine::close.
int MashedRw_EngineClose_004c3040(void) {
    if (!s_rwOpen) { BindLog("RwEngineClose: engine not open"); return 0; }
    rw::Engine::close();
    s_rwOpen = false;
    BindLog("RwEngineClose -> rw::Engine::close");
    return 1;
}

// RwBool RwEngineTerm(void). librw: rw::Engine::term. The matched 0x004938c0 / 0x00493710 declare it bool / void.
int MashedRw_EngineTerm_004c3270(void) {
    rw::Engine::term();
    BindLog("RwEngineTerm -> rw::Engine::term");
    return 1;
}

// RwInt32 RwEngineGetCurrentMode(void). librw: rw::Engine::getCurrentVideoMode. The matched 0x00493710 / 0x004921d0
// declare it short.
int MashedRw_EngineGetCurrentMode_004c2f00(void) {
    return rw::Engine::getCurrentVideoMode();
}

// RwVideoMode* RwEngineGetModeInfo(RwVideoMode* modeinfo, RwInt32 modeIndex). RwVideoMode is 6 dwords here (width,
// height, depth, flags, refRate, format: the 0x2c frame of 0x00493710). librw: rw::Engine::getVideoModeInfo fills the
// first four; librw's VideoMode has no refRate / format, so those two are written 0. NULL when librw has no such mode.
void* MashedRw_EngineGetModeInfo_004c2ed0(void* modeinfo, int modeIndex) {
    int* const out = static_cast<int*>(modeinfo);
    if (!out) return nullptr;
    rw::VideoMode vm;
    std::memset(&vm, 0, sizeof(vm));
    if (rw::Engine::getVideoModeInfo(&vm, modeIndex) == nullptr) return nullptr;
    out[0] = vm.width;
    out[1] = vm.height;
    out[2] = vm.depth;
    out[3] = static_cast<int>(vm.flags);
    out[4] = 0;
    out[5] = 0;
    return modeinfo;
}

// ---------------------------------------------------------------------------------------------------------------
// Texture dictionaries (RW 3.x rwtexdict).

// RwTexDictionary* RwTexDictionarySetCurrent(RwTexDictionary* dict): the matched body (re/match/s19/004c5c80.c) writes
// the argument into the texture module's current-dictionary slot (RwGlobals + texture offset + 0x10) and returns it.
// rva 0x00493710 calls it with 0. librw: rw::TexDictionary::setCurrent (same slot in librw's texture module globals).
int MashedRw_TexDictionarySetCurrent_004c5c80(int dict) {
    rw::TexDictionary::setCurrent(reinterpret_cast<rw::TexDictionary*>(static_cast<std::uintptr_t>(static_cast<unsigned>(dict))));
    return dict;
}

// RwBool RwTexDictionaryDestroy(RwTexDictionary* dict): the matched body (re/match/s19/004c5930.c) clears the current
// dictionary if it is this one, destroys every texture in it (0x004c5a60, through the plugin destructors 0x004d8060),
// unlinks it and frees it. librw: rw::TexDictionary::destroy does the same (texture.cpp: current reset, textures
// destroyed, plugin destructors, unlink, free). WinMain 0x00492370 calls it on DAT_0077195c (set by 0x004921d0).
int MashedRw_TexDictionaryDestroy_004c5930(int dict) {
    rw::TexDictionary* const d = reinterpret_cast<rw::TexDictionary*>(static_cast<std::uintptr_t>(static_cast<unsigned>(dict)));
    if (d) d->destroy();
    BindLog("RwTexDictionaryDestroy(0x%08X) -> rw::TexDictionary::destroy", static_cast<unsigned>(dict));
    return 1;
}

// RwTexture* RwTexDictionaryFindNamedTexture(RwTexDictionary* dict, const RwChar* name): the matched body
// (re/match/s19/004c5c00.c) walks the dictionary's texture list comparing names. librw: rw::TexDictionary::find.
int MashedRw_TexDictionaryFindNamedTexture_004c5c00(int dict, const char* name) {
    rw::TexDictionary* const d = reinterpret_cast<rw::TexDictionary*>(static_cast<std::uintptr_t>(static_cast<unsigned>(dict)));
    if (!d || !name) return 0;
    return static_cast<int>(reinterpret_cast<std::uintptr_t>(d->find(name)));
}

// ---------------------------------------------------------------------------------------------------------------
// Shims: RW core calls of 0x00493710 / 0x004938c0 with no librw equivalent. Each keeps the original's return value.

// RtFSManagerOpen(RwUInt32 maxNbFS) 0x00550350, called with -1: the RenderWare file-system toolkit (RtFSManager).
// librw has no file-system manager; the exe opens files through the CRT and its own piz reader. Shim: returns TRUE.
int MashedRw_RtFSManagerOpen_00550350(int maxNbFS) {
    BindLog("RtFSManagerOpen(%d): shim, librw has no RtFSManager", maxNbFS);
    return 1;
}
// RtFSManagerClose(void) 0x00550390 (closes every installed file system, re/match/s24/00550390.c). Shim: no-op.
void MashedRw_RtFSManagerClose_00550390(void) {
    BindLog("RtFSManagerClose: shim, librw has no RtFSManager");
}

// rva 0x004c9f50 stores its argument at 0x007d4134 (re/match/s20/004c9f50.c); 0x00493710 passes 1. 0x004c9eb0 stores its
// argument at 0x006181c4 and lowers it to the best refresh rate the adapter enumerates through the IDirect3D9 at
// 0x007d4108 (re/match/s20/004c9eb0.c); 0x00493710 passes 0x3c (60). 0x004c9f60 stores its argument at 0x007d413c and
// pushes it to the device through vtable slot 0x50 when 0x007d4120 is nonzero, else sets four flags 0x006181b4..c0
// (re/match/s20/004c9f60.c); 0x00493710 passes 1. All three are RW D3D9-driver settings taken before RwEngineStart;
// librw has none of them, and the exe's device is created windowed by InitD3D9 (refresh rate not applicable).
// Shims: keep the value, change nothing in librw.
void MashedRw_D3D9Set7d4134_004c9f50(int v) { s_shim7d4134 = static_cast<unsigned>(v); }
void MashedRw_D3D9SetRefreshRate_004c9eb0(unsigned v) { s_shim6181c4 = v; }
void MashedRw_D3D9Set7d413c_004c9f60(int v) { s_shim7d413c = static_cast<unsigned>(v); }

// rva 0x004cbc60 / 0x004cbc70 set / get the dword at 0x007d4598; 0x004cbc80 / 0x004cbc90 set / get the dword at 0x007d459c
// (re/match/s20/004cbc60.c .. 004cbc90.c). 0x00493710 saves the old values to 0x007719dc / 0x007719d8 and installs the
// function pointers 0x00493700 / 0x004936f0. No exe code reads either global, and the exe has no body for the two
// installed functions, so nothing must call them: the shims keep the values in this file, librw is not told.
void     MashedRw_Set7d4598_004cbc60(void* cb) { s_shim7d4598 = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(cb)); }
unsigned MashedRw_Get7d4598_004cbc70(void) { return s_shim7d4598; }
void     MashedRw_Set7d459c_004cbc80(void* cb) { s_shim7d459c = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(cb)); }
unsigned MashedRw_Get7d459c_004cbc90(void) { return s_shim7d459c; }

// rva 0x005584c0 allocates 0x1c000 and 0x3000 bytes through the RwGlobals allocator (RwGlobals+0x108, hint 0x40180) into
// 0x007dc8b8 / 0x007dc8bc and sets 0x007dc8ac = 1 (re/match/rwl05/005584c0.c); 0x00558470 frees both
// (re/match/s24/00558470.c). No exe code reads the two buffers and librw has no counterpart. Shims: return 1 / no-op.
int MashedRw_Alloc7dc8b8_005584c0(void) {
    BindLog("0x005584c0: shim, no librw counterpart (two RW-heap buffers, no exe reader)");
    return 1;
}
void MashedRw_Free7dc8b8_00558470(void) {}

}  // extern "C"

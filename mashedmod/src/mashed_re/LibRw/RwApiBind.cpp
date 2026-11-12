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
#include "RwRasterBridge.h"
#include "../Txd/TxdDecoder.h"
#include "../D3d9Render/PngLoader.h"

#define WITH_D3D
#include <rw.h>

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "../Math/RwLutTables.h"

// [PORT-TOP-04F] whole-file read from the piz mount stack, else the disk (Match/PortTop_RwStream04F.cpp).
extern "C" void* PortTop_Fs_ReadAll(const char* name, unsigned* len, const char** from);

namespace {
// [CLUMP-TEXREAD] RW 3.x's texture find callback 0x004c7490 (RwTextureRead 0x004c5cb0 calls it through the texture
// module's +0x18 slot; nothing installs another): with a current dictionary (module +0x10, set by 0x004c5c80) it
// searches that one only (0x004c74af..); with none it walks the module's dictionary list from its head (0x004c7529..),
// every dictionary in turn, comparing names case-insensitively, first hit wins. RwTexDictionaryCreate 0x004c5890 links a
// new dictionary at the list head (0x004c58d4..), so the walk is newest first. librw's defaultFindCB searches the
// current dictionary only and returns nil without one ("TODO: RW searches *all* TXDs otherwise",
// deps/librw/src/texture.cpp:307), which left e.g. oilslicker.dff's `PowerUpModelsFinal` null with the current
// dictionary 0 (original-side read: every RwTextureRead of the powerup inits runs with the current dictionary 0 and
// resolves from PowerUpModels.txd; verify/portorigtop_20261109/RESULT_CLUMP-TEXREAD.md).
// librw keeps its dictionary list private, so the bindings keep the RW list of the dictionaries they create (every
// dictionary of the MASHED_ORIG_TOP path: DictFromTxdBytes below), newest last, and RwTexDictionaryDestroy removes one.
std::vector<rw::TexDictionary*> s_rwTexDicts;

rw::Texture* RwRuleFindTexture(const char* name) {
    if (!name) return nullptr;
    if (rw::TexDictionary* const cur = rw::TexDictionary::getCurrent()) return cur->find(name);
    for (auto it = s_rwTexDicts.rbegin(); it != s_rwTexDicts.rend(); ++it)
        if (rw::Texture* const t = (*it)->find(name)) return t;
    return nullptr;
}


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

// [RWC-CORE-RUN] RwCore/RwCore_Knob.cpp: MASHED_RWC_CORE (default OFF, MASHED_ORIG_TOP path only) runs the engine
// bring-up on the matched RW core bodies instead of librw. These bindings stay as the fallback (L-124).
bool RwCore_Enabled(void);
int  RwCore_EngineInit(int memFuncs, int initFlags);
int  RwCore_EngineOpen(int* params);
int  RwCore_EngineStart(void);
int  RwCore_AttachPlugins(void);
int  RwCore_EngineStop(void);
int  RwCore_EngineClose(void);
int  RwCore_EngineTerm(void);
int  RwCore_Set7d4134(int v);
int  RwCore_SetRefresh(int v);
int  RwCore_Set7d413c(int v);
void RwCore_Log(const char* fmt, ...);

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
    if (RwCore_Enabled()) return RwCore_EngineInit(memFuncs, arenaSize);   // [RWC-CORE-RUN]
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
    if (RwCore_Enabled()) {   // [RWC-CORE-RUN run 38] the matched 0x00493640 (re/match/s15/00493640.c) on the RW core
        return RwCore_AttachPlugins();
    }
    mashed_re::LibRw::EnginePhasePlugins();
    // [PORT-TOP-04I] RpPTankPluginAttach is one of 0x00493640's calls. Its registration FUN_00534a80 stores the PTank
    // plugin offset at 0x007dc57c (0x00534ac4), which the matched 0x004881d0 / 0x004880a0 read (OBJ(h) =
    // *(atomic + [0x007dc57c])). The PTank atomics on this path are the exe's RW-layout records (MashedRwPTankAtomic
    // below), so the shim stores that record's plugin slot offset, 0x70. The exe's RfxEmitter stand-in atomic accepts any
    // aligned offset below 0x7c other than 0x18 (Render/RfxEmitter.cpp Create), and 0x70 is one.
    *reinterpret_cast<volatile std::uint32_t*>(static_cast<std::uintptr_t>(0x007dc57cu)) = 0x70u;
    BindLog("0x00493640 RenderwareAttachPlugins -> librw plugin registration (stand-in); [0x007dc57c] = 0x70 (PTank record)");
    return 1;
}

// RwBool RwEngineOpen(RwEngineOpenParams* params), params->displayID = the window (0x00495270 HWNDGet writes
// DAT_007e9584 into the first dword at 0x00493710). librw: rw::Engine::open({window}) + select the last subsystem.
int MashedRw_EngineOpen_004c30b0(int* params) {
    if (RwCore_Enabled()) return RwCore_EngineOpen(params);   // [RWC-CORE-RUN]
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
    if (RwCore_Enabled()) return RwCore_EngineStart();   // [RWC-CORE-RUN]
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
    if (RwCore_Enabled()) return RwCore_EngineStop();   // [RWC-CORE-RUN]
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
    if (RwCore_Enabled()) return RwCore_EngineClose();   // [RWC-CORE-RUN]
    if (!s_rwOpen) { BindLog("RwEngineClose: engine not open"); return 0; }
    rw::Engine::close();
    s_rwOpen = false;
    BindLog("RwEngineClose -> rw::Engine::close");
    return 1;
}

// RwBool RwEngineTerm(void). librw: rw::Engine::term. The matched 0x004938c0 / 0x00493710 declare it bool / void.
int MashedRw_EngineTerm_004c3270(void) {
    if (RwCore_Enabled()) return RwCore_EngineTerm();   // [RWC-CORE-RUN]
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
    if (d) {
        for (auto it = s_rwTexDicts.begin(); it != s_rwTexDicts.end(); ++it)
            if (*it == d) { s_rwTexDicts.erase(it); break; }   // [CLUMP-TEXREAD]
        d->destroy();
    }
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

// [PORT-TOP-04E] RwTextureDestroy 0x004c5a60 (re/match/rwl01/004c5a60.c): decrements the count at +0x54; at <= 0 runs
// the plugin destructors of registry 0x00618138 (0x004d8060), unlinks +8 / +0xc from the dictionary list when +4 is
// set, destroys the raster at +0 (RwRasterDestroy) and returns the record to the RwGlobals +0x11c free list. librw:
// rw::Texture::destroy (texture.cpp: refCount--, at <= 0 plugin destruct, inDict.remove, raster->destroy, free).
// Its caller in the AppDestroy tree, rva 0x00428400, passes DAT_0067d964 / 70 / 68 / 6c; the matched body has no null check, the
// binding skips a null pointer (the knob path value while the fonts family is a stand-in).
int MashedRw_TextureDestroy_004c5a60(int tex) {
    rw::Texture* const t = reinterpret_cast<rw::Texture*>(static_cast<std::uintptr_t>(static_cast<unsigned>(tex)));
    if (t) t->destroy();
    BindLog("RwTextureDestroy(0x%08X) -> %s", static_cast<unsigned>(tex), t ? "rw::Texture::destroy" : "null, nothing to destroy");
    return 1;
}
// RwImageSetPath 0x004ce2d0 (re/match/s20/004ce2d0.c): copies the string (length through RwGlobals +0xf4) into the
// image module's path buffer, growing it through RwGlobals +0x110; returns the argument, 0 when the realloc fails.
// librw: rw::Image::setSearchPath (image.cpp: frees the old paths, strdups the new one, splits at ';'). 0x00428400
// passes 0x005cc4de, which is "" in the anchored binary.
int MashedRw_ImageSetPath_004ce2d0(const char* path) {
    rw::Image::setSearchPath(path);
    BindLog("RwImageSetPath(\"%s\") -> rw::Image::setSearchPath", path ? path : "(null)");
    return static_cast<int>(reinterpret_cast<std::uintptr_t>(path));
}
// RpWorldDestroy 0x004e5700. librw: rw::World::destroy. 0x0042c2a0 calls it only when DAT_0067ea00 != 0.
int MashedRw_WorldDestroy_004e5700(int world) {
    rw::World* const w = reinterpret_cast<rw::World*>(static_cast<std::uintptr_t>(static_cast<unsigned>(world)));
    if (w) w->destroy();
    BindLog("RpWorldDestroy(0x%08X) -> %s", static_cast<unsigned>(world), w ? "rw::World::destroy" : "null, nothing to destroy");
    return 1;
}
// Shim for rva 0x004d41e0 (re/match/s20/004d41e0.c), which walks the 0x28-byte entries at *(p+4) calling 0x004d3e90, frees the buffer at
// +0x20 and returns the record through RwGlobals +0x11c. 0x00496ce0 (AppDestroy tree) calls it on DAT_00773094 /
// DAT_00773088, the two handles PCVS_Init 0x00496e40 creates. [UNCERTAIN] its RW catalog name: no identity is
// recorded, and librw has no counterpart for the record. On the knob path PCVS_Init is the hand piece 1, which
// creates neither handle, so both are 0. Shim: a null record is skipped; a non-null one is logged and left alone.
void MashedRw_Destroy4d41e0_004d41e0(int rec) {
    BindLog("0x004d41e0(0x%08X): shim, %s", static_cast<unsigned>(rec),
            rec ? "no librw counterpart, record left alone" : "null, nothing to destroy");
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
// [RWC-CORE-RUN] on the RW core path the three run their matched / provisional bodies (the driver reads them).
void MashedRw_D3D9Set7d4134_004c9f50(int v) { if (RwCore_Enabled()) { RwCore_Set7d4134(v); return; } s_shim7d4134 = static_cast<unsigned>(v); }
void MashedRw_D3D9SetRefreshRate_004c9eb0(unsigned v) { if (RwCore_Enabled()) { RwCore_SetRefresh(static_cast<int>(v)); return; } s_shim6181c4 = v; }
void MashedRw_D3D9Set7d413c_004c9f60(int v) { if (RwCore_Enabled()) { RwCore_Set7d413c(v); return; } s_shim7d413c = static_cast<unsigned>(v); }

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

// ===============================================================================================================
// PORT-TOP-04B: cameras, frames and rasters for DisplayInit 0x004921d0 / ViewportInit 0x00428590.
//
// The matched game bodies read and write RwCamera fields at fixed offsets themselves (re/match/s07/0042d420.c and
// re/match/s11/00467110.c store the rasters at +0x60 / +0x64 and test the frame at +0x04; 0x0042d4a0 / 0x0042f530 read
// +0x60 / +0x64; 0x004c1be0 reads +0x60; 0x004c1a00 / 0x004c19f0 call through +0x18 / +0x1c). librw's rw::Camera has
// another layout (beginUpdateCB at +0x14, frameBuffer far past +0x60), so a librw Camera* cannot be handed to them.
// The RwCameraCreate binding therefore returns an RW 3.x-layout camera record (the fields 0x004c1d30 writes, up to
// +0x94) with the librw camera that backs it in an exe-only tail. Every binding below that takes a camera reads the
// record, copies the fields the game code may have written (rasters, projection) into the librw camera, and acts on
// librw. Frames and rasters are librw objects as they are: the game code only stores and passes them.
// ===============================================================================================================
namespace {
bool PTankRecordSetFrame(int object, int frame);   // [PORT-TOP-04I] defined with MashedRwPTankAtomic below
bool LightRecordSetFrame(int object, int frame);   // [PORT-TOP-04J] defined with MashedRwLight below

struct MashedRwCamera {                 // RW 3.x RwCamera, offsets as written by 0x004c1d30 (re/match/rwl01/004c1d30.c)
    std::uint8_t type;                  // +0x00  4 (rwCAMERA)
    std::uint8_t subType, flags, privateFlags;
    void*        parent;                // +0x04  frame (rw::Frame*)
    void*        lFrame[2];             // +0x08  frame link: kept by librw in the backing camera, not here
    void*        sync;                  // +0x10  0x004c1de0 in the original; no exe body, left 0
    std::int32_t projection;            // +0x14  1 = perspective
    void*        beginUpdate;           // +0x18  0x004c2bb0 in the original -> MashedRw_CameraBeginUpdateCb_004c2bb0
    void*        endUpdate;             // +0x1c  0x004c2b80 in the original -> MashedRw_CameraEndUpdateCb_004c2b80
    float        viewMatrix[16];        // +0x20  not maintained (librw computes its own at beginUpdate)
    void*        frameBuffer;           // +0x60  rw::Raster*
    void*        zBuffer;               // +0x64  rw::Raster*
    float        viewWindow[2];         // +0x68
    float        recipViewWindow[2];    // +0x70
    float        viewOffset[2];         // +0x78
    float        nearPlane;             // +0x80
    float        farPlane;              // +0x84
    float        fogPlane;              // +0x88
    float        zScale, zShift;        // +0x8c / +0x90  0x004c1a70 output; not maintained (no reader)
    rw::Camera*  lib;                   // +0x94  exe only: the librw camera behind this record
};
static_assert(offsetof(MashedRwCamera, parent) == 0x04, "RwCamera +0x04");
static_assert(offsetof(MashedRwCamera, projection) == 0x14, "RwCamera +0x14");
static_assert(offsetof(MashedRwCamera, beginUpdate) == 0x18, "RwCamera +0x18");
static_assert(offsetof(MashedRwCamera, endUpdate) == 0x1c, "RwCamera +0x1c");
static_assert(offsetof(MashedRwCamera, frameBuffer) == 0x60, "RwCamera +0x60");
static_assert(offsetof(MashedRwCamera, zBuffer) == 0x64, "RwCamera +0x64");
static_assert(offsetof(MashedRwCamera, viewWindow) == 0x68, "RwCamera +0x68");
static_assert(offsetof(MashedRwCamera, recipViewWindow) == 0x70, "RwCamera +0x70");
static_assert(offsetof(MashedRwCamera, nearPlane) == 0x80, "RwCamera +0x80");
static_assert(offsetof(MashedRwCamera, farPlane) == 0x84, "RwCamera +0x84");
static_assert(offsetof(MashedRwCamera, fogPlane) == 0x88, "RwCamera +0x88");
static_assert(offsetof(MashedRwCamera, lib) == 0x94, "exe tail");

constexpr std::uint8_t kRwCameraType = 4;   // 0x004c1d30 writes 4 at +0x00

template <class T> T* FromInt(int v) { return reinterpret_cast<T*>(static_cast<std::uintptr_t>(static_cast<unsigned>(v))); }
template <class T> int ToInt(T* p) { return static_cast<int>(reinterpret_cast<std::uintptr_t>(p)); }

MashedRwCamera* AsCamera(void* obj) {
    MashedRwCamera* c = static_cast<MashedRwCamera*>(obj);
    return (c && c->type == kRwCameraType && c->lib) ? c : nullptr;
}

// Copy what the game code can write directly into the record over to the librw camera.
void SyncToLib(MashedRwCamera* c) {
    c->lib->frameBuffer = static_cast<rw::Raster*>(c->frameBuffer);
    c->lib->zBuffer = static_cast<rw::Raster*>(c->zBuffer);
    c->lib->projection = c->projection;
}

bool s_sceneOpenedByBind = false;   // BeginScene issued by the begin-update binding (see there)

}  // namespace

extern "C" {

void* __cdecl MashedRw_CameraBeginUpdateCb_004c2bb0(void* cam);
void* __cdecl MashedRw_CameraEndUpdateCb_004c2b80(void* cam);

// RwCamera* RwCameraCreate(void): re/match/rwl01/004c1d30.c allocates the camera (type hint 0x30005), type 4, frame 0,
// sync 0x004c1de0, beginUpdate 0x004c2bb0 (+0x18), endUpdate 0x004c2b80 (+0x1c), view window 1 x 1 (+0x68) with its
// reciprocals (+0x70), view offset 0 (+0x78), near 0.05 (0x3d4ccccd, +0x80), far 10 (0x41200000, +0x84), fog 5
// (0x40a00000, +0x88), rasters 0, projection 1 (+0x14), then 0x004c1a70 and the plugin constructors (0x004d8000).
// librw: rw::Camera::create (same defaults: camera.cpp Camera::create) behind an RW-layout record (see above).
int MashedRw_CameraCreate_004c1d30(void) {
    MashedRwCamera* c = static_cast<MashedRwCamera*>(std::calloc(1, sizeof(MashedRwCamera)));
    if (!c) return 0;
    c->lib = rw::Camera::create();
    if (!c->lib) { std::free(c); return 0; }
    c->type = kRwCameraType;
    c->beginUpdate = reinterpret_cast<void*>(&MashedRw_CameraBeginUpdateCb_004c2bb0);
    c->endUpdate = reinterpret_cast<void*>(&MashedRw_CameraEndUpdateCb_004c2b80);
    c->viewWindow[0] = c->viewWindow[1] = 1.0f;
    c->recipViewWindow[0] = c->recipViewWindow[1] = 1.0f;
    c->nearPlane = 0.05f;
    c->farPlane = 10.0f;
    c->fogPlane = 5.0f;
    c->projection = 1;
    BindLog("RwCameraCreate -> rw::Camera::create: record %p, librw camera %p", static_cast<void*>(c), static_cast<void*>(c->lib));
    return ToInt(c);
}

// RwCamera* RwCameraClone(RwCamera* camera): re/match/rwl01/004c2bf0.c creates a camera (0x004c1d30), gives it the
// source's frame (0x004c0740 with +0x04), copies +0x18 +0x1c +0x68..+0x88 +0x60 +0x64 +0x14, runs 0x004c1a70 and the
// plugin copy constructors (0x004d8090). librw: rw::Camera::clone (same frame, window, offset, planes, projection,
// rasters, plugin copy).
int MashedRw_CameraClone_004c2bf0(int camera) {
    MashedRwCamera* const s = AsCamera(FromInt<void>(camera));
    if (!s) return 0;
    MashedRwCamera* c = static_cast<MashedRwCamera*>(std::calloc(1, sizeof(MashedRwCamera)));
    if (!c) return 0;
    SyncToLib(s);
    c->lib = s->lib->clone();
    if (!c->lib) { std::free(c); return 0; }
    c->type = kRwCameraType;
    c->parent = s->parent;
    c->beginUpdate = s->beginUpdate;
    c->endUpdate = s->endUpdate;
    std::memcpy(c->viewWindow, s->viewWindow, sizeof(float) * 2);
    std::memcpy(c->recipViewWindow, s->recipViewWindow, sizeof(float) * 2);
    std::memcpy(c->viewOffset, s->viewOffset, sizeof(float) * 2);
    c->nearPlane = s->nearPlane;
    c->farPlane = s->farPlane;
    c->fogPlane = s->fogPlane;
    c->frameBuffer = s->frameBuffer;
    c->zBuffer = s->zBuffer;
    c->projection = s->projection;
    BindLog("RwCameraClone(%p) -> rw::Camera::clone: record %p", static_cast<void*>(s), static_cast<void*>(c));
    return ToInt(c);
}

// RwBool RwCameraDestroy(RwCamera* camera): re/match/s19/004c1cf0.c runs the plugin destructors (0x004d8060), detaches
// the frame (0x004c0790) and frees the camera. librw: rw::Camera::destroy (plugin destruct, setFrame(nil), free).
int MashedRw_CameraDestroy_004c1cf0(int camera) {
    MashedRwCamera* const c = AsCamera(FromInt<void>(camera));
    if (!c) return 0;
    // [PORT-TOP-04D] one line per destroyed camera (closes the PORT-TOP-04C trace gap).
    BindLog("RwCameraDestroy(%08X) -> rw::Camera::destroy: librw camera %p, rasters %p / %p",
            static_cast<unsigned>(camera), static_cast<void*>(c->lib), static_cast<void*>(c->frameBuffer),
            static_cast<void*>(c->zBuffer));
    c->lib->destroy();
    std::free(c);
    return 1;
}

// RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* viewWindow): re/match/s19/004c1c80.c stores the
// window at +0x68, 1/x and 1/y at +0x70 / +0x74 (1.0f from 0x005cc320) and marks the frame dirty (0x004c0e50).
// librw: rw::Camera::setViewWindow.
int MashedRw_CameraSetViewWindow_004c1c80(int camera, const float* window) {
    MashedRwCamera* const c = AsCamera(FromInt<void>(camera));
    if (!c || !window) return 0;
    c->viewWindow[0] = window[0];
    c->viewWindow[1] = window[1];
    c->recipViewWindow[0] = 1.0f / window[0];
    c->recipViewWindow[1] = 1.0f / window[1];
    const rw::V2d w = { window[0], window[1] };
    c->lib->setViewWindow(&w);
    return camera;
}

// RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal nearClip): re/match/s19/004c1a40.c stores +0x80, runs
// rva 0x004c1a70, marks the frame dirty. librw: rw::Camera::setNearPlane.
int MashedRw_CameraSetNearClipPlane_004c1a40(int camera, float nearClip) {
    MashedRwCamera* const c = AsCamera(FromInt<void>(camera));
    if (!c) return 0;
    c->nearPlane = nearClip;
    c->lib->setNearPlane(nearClip);
    return camera;
}

// RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal farClip): re/match/s19/004c1b10.c stores +0x84, runs
// rva 0x004c1a70, marks the frame dirty. librw: rw::Camera::setFarPlane.
int MashedRw_CameraSetFarClipPlane_004c1b10(int camera, float farClip) {
    MashedRwCamera* const c = AsCamera(FromInt<void>(camera));
    if (!c) return 0;
    c->farPlane = farClip;
    c->lib->setFarPlane(farClip);
    return camera;
}

// RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* colour, RwInt32 clearMode): re/match/s19/004c1bb0.c calls the
// device standard function at RwGlobals +0x9c (camera, colour, mode) and returns the camera when it succeeds. The
// matched body reads RwGlobals, which the exe does not have, so this is the binding. librw: rw::Camera::clear (RwRGBA
// and rw::RGBA are both r, g, b, a bytes; clear bits 1 / 2 / 4 = image / z / stencil in both).
int MashedRw_CameraClear_004c1bb0(int camera, const unsigned char* colour, int mode) {
    MashedRwCamera* const c = AsCamera(FromInt<void>(camera));
    if (!c || !colour) return 0;
    SyncToLib(c);
    rw::RGBA col = { colour[0], colour[1], colour[2], colour[3] };
    c->lib->clear(&col, static_cast<rw::uint32>(mode));
    BindLog("RwCameraClear(%p, %02x%02x%02x%02x, %d) -> rw::Camera::clear", static_cast<void*>(c),
            colour[0], colour[1], colour[2], colour[3], mode);
    return camera;
}

// The camera's begin / end update functions, installed at +0x18 / +0x1c by 0x004c1d30 (0x004c2bb0 / 0x004c2b80 in the
// original; no matched source and no hooks.csv row for either). The matched RwCameraBeginUpdate 0x004c1a00 and
// RwCameraEndUpdate 0x004c19f0 call them through the record. librw: rw::Camera::beginUpdate / endUpdate.
// Under device adoption librw issues neither BeginScene nor EndScene (deps/librw MASHED_PATCHES.md P3: the exe owns the
// scene bracket of its frames). ViewportInit's update runs at boot, outside every exe frame, so the binding brackets it
// itself, as RW's D3D9 driver does in its own camera update.
void* __cdecl MashedRw_CameraBeginUpdateCb_004c2bb0(void* cam) {
    MashedRwCamera* const c = AsCamera(cam);
    if (!c || !c->lib->getFrame()) return nullptr;
    SyncToLib(c);
    c->lib->beginUpdate();
    s_sceneOpenedByBind = false;
    if (rw::d3d::d3ddevice) s_sceneOpenedByBind = SUCCEEDED(rw::d3d::d3ddevice->BeginScene());
    BindLog("camera begin update %p -> rw::Camera::beginUpdate, BeginScene %s", cam, s_sceneOpenedByBind ? "ok" : "not opened");
    return cam;
}
void* __cdecl MashedRw_CameraEndUpdateCb_004c2b80(void* cam) {
    MashedRwCamera* const c = AsCamera(cam);
    if (!c) return nullptr;
    if (s_sceneOpenedByBind && rw::d3d::d3ddevice) rw::d3d::d3ddevice->EndScene();
    s_sceneOpenedByBind = false;
    c->lib->endUpdate();
    BindLog("camera end update %p -> EndScene, rw::Camera::endUpdate", cam);
    return cam;
}

// RwFrame* RwFrameCreate(void): re/match/rwl01/004c0b30.c allocates a frame (type hint 0x3000e) and initialises it
// (rva 0x004c0b70). librw: rw::Frame::create.
int MashedRw_FrameCreate_004c0b30(void) {
    return ToInt(rw::Frame::create());
}

// RwBool RwFrameDestroy(RwFrame* frame): re/match/s19/004c0c20.c runs the plugin destructors, unlinks the frame from
// its parent and re-roots its children. librw: rw::Frame::destroy.
int MashedRw_FrameDestroy_004c0c20(int frame) {
    rw::Frame* const f = FromInt<rw::Frame>(frame);
    if (f) f->destroy();
    return 1;
}

// [PORT-TOP-04C] RpClumpDestroy (hooks.csv name of rva 0x004e6e00, no matched source): rw::Clump::destroy. Reached from
// the matched callees of AppDestroy 0x00402a40 (0x0041c0e0 / 0x0041c2c0 / 0x0041da80 ... each guards on a nonzero global).
int MashedRw_ClumpDestroy_004e6e00(int clump) {
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    BindLog("RpClumpDestroy(0x%08X) -> rw::Clump::destroy", static_cast<unsigned>(clump));
    if (c) c->destroy();
    return 1;
}

// void* _rwObjectHasFrameSetFrame(void* object, RwFrame* frame): re/match/s19/004c0740.c unlinks the object from its
// old frame, stores the new frame at +0x04, links the object into the frame's object list and marks the frame dirty
// (rva 0x004c0e50). Knob-path callers pass a camera (0x00467110, 0x004670a0, 0x0042d420, 0x004c2bf0), a PTank record
// (PORT-TOP-04I) or a light record (0x00401000 / 0x004014b0, PORT-TOP-04J). For a camera this binding takes the camera record: librw rw::Camera::setFrame (librw keeps the link in its own camera).
void MashedRw_ObjectHasFrameSetFrame_004c0740(int object, int frame) {
    if (PTankRecordSetFrame(object, frame)) return;   // [PORT-TOP-04I] a PTank atomic record (0x004e7e30's callers)
    if (LightRecordSetFrame(object, frame)) return;   // [PORT-TOP-04J] a light record (0x00401000, 0x004014b0)
    MashedRwCamera* const c = AsCamera(FromInt<void>(object));
    if (!c) { BindLog("0x004c0740: object %08X is neither a camera, a PTank record nor a light record, ignored", static_cast<unsigned>(object)); return; }
    c->parent = FromInt<void>(frame);
    c->lib->setFrame(FromInt<rw::Frame>(frame));
}

// RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags): re/match/rwl01/004c77c0.c
// allocates the raster (type hint 0x30407), stores width / height / depth at +0x0c / +0x10 / +0x14 and calls the
// driver's raster create (RwGlobals +0x58) with flags. librw: rw::Raster::create(width, height, depth, flags); librw
// splits flags the same way (type = low 3 bits, 1 = z buffer, 2 = camera; 0x80 = do not allocate) and, like RW's D3D9
// driver, marks a 0 x 0 raster as not allocated (d3d.cpp rasterCreate), which RwRasterSubRaster requires.
int MashedRw_RasterCreate_004c77c0(int width, int height, int depth, int flags) {
    rw::Raster* const r = rw::Raster::create(width, height, depth, flags);
    BindLog("RwRasterCreate(%d, %d, %d, 0x%x) -> rw::Raster::create %p", width, height, depth, flags, static_cast<void*>(r));
    return ToInt(r);
}

// RwBool RwRasterDestroy(RwRaster* raster): re/match/rwl01/004c7650.c plugin destructors, driver destroy (+0x5c), free.
// librw: rw::Raster::destroy.
int MashedRw_RasterDestroy_004c7650(int raster) {
    rw::Raster* const r = FromInt<rw::Raster>(raster);
    if (r) r->destroy();
    return 1;
}

// RwRaster* RwRasterSubRaster(RwRaster* subRaster, RwRaster* raster, RwRect* rect): re/match/s19/004c7760.c returns 0
// unless the sub raster has the do-not-allocate flag (cFlags 0x80); then width / height from the rect, offset = parent
// offset + rect x / y, driver call (+0x78), parent = the parent's parent. librw: rw::Raster::subRaster (same test and
// fields; rw::Rect and RwRect are both x, y, w, h).
int MashedRw_RasterSubRaster_004c7760(int subRaster, int raster, int* rect) {
    rw::Raster* const s = FromInt<rw::Raster>(subRaster);
    rw::Raster* const p = FromInt<rw::Raster>(raster);
    if (!s || !p || !rect || !(s->flags & rw::Raster::DONTALLOCATE)) return 0;
    rw::Rect r = { rect[0], rect[1], rect[2], rect[3] };
    s->subRaster(p, &r);
    return subRaster;
}

// RwRaster* RwRasterShowRaster(RwRaster* raster, void* dev, RwUInt32 flags): re/match/s19/004c7730.c flushes a list
// (rva 0x004d8c40) and calls the driver's show-raster (RwGlobals +0x98); 0x004c1be0 passes (camera raster, 0, 1). The
// driver presents the frame. NOT librw's showRaster: on a flags / PresentationInterval mismatch it Resets the device
// (d3ddevice.cpp showRaster), and under adoption that device is the exe's, whose default-pool resources a Reset would
// drop. The binding presents on the adopted device directly; the vsync flag is not applied (the interval is the exe
// device's). Returns the raster when Present succeeds.
int MashedRw_RasterShowRaster_004c7730(int raster, int dev, int flags) {
    (void)dev;
    if (!raster || !rw::d3d::d3ddevice) return 0;
    const HRESULT hr = rw::d3d::d3ddevice->Present(nullptr, nullptr, nullptr, nullptr);
    BindLog("RwRasterShowRaster(%08X, flags %d) -> Present on the adopted device: 0x%08lX", static_cast<unsigned>(raster),
            flags, static_cast<unsigned long>(hr));
    return SUCCEEDED(hr) ? raster : 0;
}

// rva 0x004b3eb0 (no matched source; decomp via re/tools/decomp_pc.py 2026-11-08): opens an RW memory stream on
// (param_1 = data, param_2 = size) with 0x004cc230(3 = memory, 1 = read, &{data, size}), finds chunk 0x23 with
// rva 0x004cc5e0(stream, 0x23, param_3, param_4), reads the texture dictionary with 0x0054f8d0(stream) when found, closes
// the stream with 0x004cc160 and returns the dictionary (0 on failure). DisplayInit passes the locked RWTEXDICTIONARY
// 404 resource and (0, 0). Those four RW calls read RwGlobals, and librw has no reader for chunk 0x23, so this port
// body reads the chunk with the exe's decoder of that format (Txd/TxdDecoder.h, written from 0x0054f8d0) and builds
// the dictionary on librw: rw::TexDictionary::create, one rw::Texture per entry (RwRasterBridge TextureFromTxdTexture).
// [CLUMP-TEXREAD] RwRuleFindTexture / s_rwTexDicts (RW's find rule 0x004c7490): see the top of this file.

// A chunk-0x23 texture dictionary (header included) decoded by Txd/TxdDecoder.h and built on librw, one rw::Texture per
// entry. Shared by 0x004b3eb0 (memory) and the 0x0054f8d0 binding (stream).
static rw::TexDictionary* DictFromTxdBytes(const std::uint8_t* bytes, std::size_t size, const char* tag) {
    std::unique_ptr<mashed_re::Txd::Dictionary> txd(new mashed_re::Txd::Dictionary());
    if (!txd->Decode(bytes, size)) {
        BindLog("%s: chunk 0x23 decode failed (%s)", tag, txd->last_error());
        return nullptr;
    }
    rw::TexDictionary* const dict = rw::TexDictionary::create();
    if (!dict) return nullptr;
    int added = 0;
    for (std::uint32_t i = 0; i < txd->count(); ++i) {
        rw::Texture* const t = static_cast<rw::Texture*>(mashed_re::LibRw::TextureFromTxdTexture(txd->texture(i)));
        if (t) { dict->add(t); ++added; }
    }
    BindLog("%s: TXD (%u bytes) -> rw::TexDictionary %p with %d of %u textures", tag, static_cast<unsigned>(size),
            static_cast<void*>(dict), added, txd->count());
    s_rwTexDicts.push_back(dict);   // [CLUMP-TEXREAD] the RW dictionary list (RwRuleFindTexture)
    return dict;
}

int PortTop_Port_004b3eb0(int data, int size, int lengthOut, int versionOut) {
    (void)lengthOut; (void)versionOut;
    const std::uint8_t* const bytes = FromInt<const std::uint8_t>(data);
    if (!bytes || size <= 0) return 0;
    return ToInt(DictFromTxdBytes(bytes, static_cast<std::size_t>(size), "0x004b3eb0"));
}

// [PORT-TOP-04F] rva 0x0054f8d0 (re/match/rwl04/0054f8d0.c, 54.9%, not a matched body): reads the chunk-0x23 texture
// dictionary whose header RwStreamFindChunk has just consumed. Its callers 0x004b3d80 / 0x004b3eb0 call it right after
// RwStreamFindChunk(stream, 0x23). Binding: the streams of the knob path are memory streams (type 3: [3] position,
// [4] size, [5] base; Match/PortTop_RwStream04F.cpp), so it decodes from the header 12 bytes back with the exe's
// decoder of the format (written from 0x0054f8d0, as 0x004b3eb0 above), then moves the position past the chunk.
int MashedRw_TexDictStreamRead_0054f8d0(int stream) {
    std::uint32_t* const s = FromInt<std::uint32_t>(stream);
    if (!s || s[0] != 3 || s[3] < 12 || s[3] > s[4]) {
        BindLog("0x0054f8d0(0x%08X): binding reads memory streams only, returns 0", static_cast<unsigned>(stream));
        return 0;
    }
    const std::uint8_t* const base = FromInt<const std::uint8_t>(static_cast<int>(s[5]));
    const std::uint32_t hdr = s[3] - 12;
    std::uint32_t chunk = 0;
    std::memcpy(&chunk, base + hdr + 4, 4);
    rw::TexDictionary* const dict = DictFromTxdBytes(base + hdr, s[4] - hdr, "0x0054f8d0");
    if (dict) s[3] = (s[3] + chunk <= s[4]) ? s[3] + chunk : s[4];
    return ToInt(dict);
}

// [PORT-TOP-04G] RwTexDictionaryAddTexture rva 0x004c5bc0 (re/match/s19/004c5bc0.c): when the texture is in a
// dictionary it is unlinked (rwLinkListRemoveLLLink on +lInDictionary), texture->dict = dict, and it is linked at the head
// of dict->texturesInDict (rwLinkListAddLLLink inserts after the list head); returns the texture. librw:
// rw::TexDictionary::addFront (inDict.remove when t->dict, t->dict = this, textures.add = head insert). 0x0040bbb0
// calls it only when RwTextureRead("test") returned a texture.
int MashedRw_TexDictionaryAddTexture_004c5bc0(int dict, int tex) {
    rw::TexDictionary* const d = reinterpret_cast<rw::TexDictionary*>(static_cast<std::uintptr_t>(static_cast<unsigned>(dict)));
    rw::Texture* const t = reinterpret_cast<rw::Texture*>(static_cast<std::uintptr_t>(static_cast<unsigned>(tex)));
    if (d && t) d->addFront(t);
    BindLog("RwTexDictionaryAddTexture(0x%08X, 0x%08X) -> rw::TexDictionary::addFront", static_cast<unsigned>(dict),
            static_cast<unsigned>(tex));
    return tex;
}

// [PORT-TOP-04F] RwTextureRead rva 0x004c5cb0 (re/match/s19/004c5cb0.c): the find callback (default 0x004c7490, the
// current dictionary only, case-insensitive), a hit gets its count raised and is returned; else the read callback
// (default 0x004c61b0, image files), and a read texture is linked into the current dictionary when there is one; a
// miss returns 0. librw keeps the same current dictionary (rw::TexDictionary::setCurrent, 0x004c5c80 binding above)
// and the same find callback (rw::Texture::findCB). The read side: librw's default reader opens image files on the
// disk only, the original's goes through the RtFS piz file system, so the binding reads "<name>.png" then
// "<name>.bmp" (PortTop_Fs_ReadAll: piz mount stack, then the disk under "original/"), decodes it with the exe's WIC
// decoder (D3d9Render/PngLoader.h) and builds a C8888 texture named <name>. [UNCERTAIN] the original's extension
// order: the image formats 0x004c61b0 tries are registered at run time and no registration list is recorded; the
// font36.piz logos are one format each (MASHEDNEWLOGO.PNG, BIGE.BMP, PROLOGICII.BMP, DOLBYDIG.BMP), so the order picks
// no different file there. The mask name is not applied (0x004283a0 passes 0).
int MashedRw_TextureRead_004c5cb0(const char* name, const char* mask) {
    if (!name) return 0;
    if (rw::Texture* const hit = RwRuleFindTexture(name)) {   // [CLUMP-TEXREAD] 0x004c7490's rule
        hit->refCount++;
        return ToInt(hit);
    }
    static const char* const kExt[] = {".png", ".bmp"};
    for (const char* ext : kExt) {
        char file[128];
        std::snprintf(file, sizeof(file), "%s%s", name, ext);
        unsigned len = 0;
        const char* from = "";
        void* const blob = PortTop_Fs_ReadAll(file, &len, &from);
        if (!blob) continue;
        std::uint32_t w = 0, h = 0;
        std::uint8_t* const bgra = mashed_re::D3d9Render::DecodeImageToBGRA(static_cast<const std::uint8_t*>(blob), len, &w, &h);
        std::free(blob);
        if (!bgra) {
            BindLog("RwTextureRead(\"%s\"): %s from %s did not decode", name, file, from);
            return 0;
        }
        rw::Raster* const ras = rw::Raster::create(static_cast<int>(w), static_cast<int>(h), 32,
                                                   rw::Raster::C8888 | rw::Raster::TEXTURE);
        std::uint8_t* const dst = ras ? ras->lock(0, rw::Raster::LOCKWRITE) : nullptr;
        if (!dst) {
            if (ras) ras->destroy();
            std::free(bgra);
            return 0;
        }
        for (std::uint32_t y = 0; y < h; ++y)
            std::memcpy(dst + static_cast<std::size_t>(y) * ras->stride, bgra + static_cast<std::size_t>(y) * w * 4, w * 4);
        ras->unlock(0);
        std::free(bgra);
        rw::Texture* const t = rw::Texture::create(ras);
        if (!t) { ras->destroy(); return 0; }
        std::strncpy(t->name, name, sizeof(t->name) - 1);
        t->name[sizeof(t->name) - 1] = '\0';
        if (mask) {
            std::strncpy(t->mask, mask, sizeof(t->mask) - 1);
            t->mask[sizeof(t->mask) - 1] = '\0';
        }
        if (rw::TexDictionary* const cur = rw::TexDictionary::getCurrent()) cur->add(t);
        BindLog("RwTextureRead(\"%s\"): %s from %s, %ux%u -> rw::Texture %p", name, file, from, w, h, static_cast<void*>(t));
        return ToInt(t);
    }
    BindLog("RwTextureRead(\"%s\"): not in the current dictionary, no .png / .bmp found, returns 0", name);
    return 0;
}

// [PORT-OT-IMAGE-FMT] the four RW toolkit image functions 0x00493600 hands to 0x004ce790 (directly or through the read
// wrappers 0x004935a0 / 0x004935d0, Match/PortTop_004ce790.cpp). librw provides all four (deps/librw/src/bmp.cpp,
// png.cpp), so they bind rather than being ported. Disassembly of original/MASHED.exe.unpatched:
//  - rva 0x0052cb00 BMP reader (re/match/s23/0052cb00.c header: RwStream open / read, 'BM' magic, RwImageCreate).
//  - rva 0x0052d530 BMP writer (image [esp+4], name [esp+8]): 0x4cc230(2, 2, name) at 0x0052d53e, bytes 0x42 / 0x4d
//    through 0x4cbe80 (at 0x0052d56d / 0x0052d57f). Returns the image (at 0x0052d7c5 mov eax, ebp), 0 when the open
//    fails (at 0x0052d7d0).
//  - rva 0x00514330 PNG reader (name [ebp+8]): 0x4cc230(2, 1, name) at 0x0051434b, then 0x5173f0 with "1.0.12"
//    (string at 0x00620e1c).
//  - rva 0x00513f90 PNG writer (image [ebp+8], name [ebp+0xc]): 0x4cc230(2, 2, name) at 0x00513fb6, then 0x514d00 with
//    "1.0.12". A null name or image skips the write (at 0x00513fa1 / 0x00513fa9). The image is returned (at 0x005142bd).
// Images on the knob path are librw objects (the RW API binding layer), so the image argument and result are rw::Image*.
// librw opens the file through rw::engine->filefuncs (fopen on the disk), not through the RtFS piz file system the
// original's RwStreamOpen uses. No ORIG_TOP row calls them yet: 0x004ce8e0's caller 0x00498810 sits under a stand-in.
// The readers check the file first: rw::readPNG asserts that the file exists (png.cpp: assert(data != nil)).
static bool ImageFileExists(const char* name) {
    if (!name) return false;
    void* const f = rw::engine->filefuncs.rwfopen(name, "rb");
    if (!f) return false;
    rw::engine->filefuncs.rwfclose(f);
    return true;
}
void* MashedRw_RtBMPImageRead_0052cb00(const char* name) {
    rw::Image* const img = ImageFileExists(name) ? rw::readBMP(name) : nullptr;
    BindLog("RtBMPImageRead(\"%s\") -> rw::readBMP %p", name ? name : "(null)", static_cast<void*>(img));
    return img;
}
void* MashedRw_RtPNGImageRead_00514330(const char* name) {
    rw::Image* const img = ImageFileExists(name) ? rw::readPNG(name) : nullptr;
    BindLog("RtPNGImageRead(\"%s\") -> rw::readPNG %p", name ? name : "(null)", static_cast<void*>(img));
    return img;
}
void* MashedRw_RtBMPImageWrite_0052d530(void* image, const char* name) {
    if (!image || !name) return nullptr;
    rw::writeBMP(static_cast<rw::Image*>(image), name);
    BindLog("RtBMPImageWrite(%p, \"%s\") -> rw::writeBMP", image, name);
    return image;
}
void* MashedRw_RtPNGImageWrite_00513f90(void* image, const char* name) {
    if (image && name) {
        rw::writePNG(static_cast<rw::Image*>(image), name);
        BindLog("RtPNGImageWrite(%p, \"%s\") -> rw::writePNG", image, name);
    }
    return image;
}

}  // extern "C"

// ===============================================================================================================
// [PORT-TOP-04H] clumps for the perm-clump loader 0x004025f0 (family 3): RpClumpStreamRead, RpClumpForAllAtomics,
// RwFrameUpdateObjects and the first-atomic lookup 0x004b3f90.
//
// Layout facts these bindings rest on (RW 3.x offsets from the matched / decompiled callers, librw from rwobjects.h):
//  - clump +0x04 = its frame: 0x004b3bf0 passes *(clump + 4) to 0x004c0e50. librw rw::Clump starts with an
//    rw::Object (type, subType, flags, privateFlags, parent), so a librw Clump* has its frame at +0x04 too.
//  - frame +0xa0 = root, +0x03 = privateFlags: 0x004c0e50 reads them. librw rw::Frame has the same two (asserted below),
//    so 0x004c0e50's job is librw's rw::Frame::updateObjects.
//  - atomic +0x18 = geometry, geometry +0x14 = numVertices, +0x5c = morph target, morph +0x14 = vertices: read by
//    0x00401690 (re/match/s01/00401690.c). librw has numVertices at +0x14 and MorphTarget::vertices at +0x14 (asserted),
//    but Atomic::geometry at +0x14 and Geometry::morphTargets at +0x48. So 0x004b3f90's binding hands back an RW-layout
//    view (MashedRwAtomicView) of the librw atomic: the four fields 0x00401690 reads, nothing else.
// ===============================================================================================================
namespace {
struct MashedRwGeometryView {           // the RW 3.x RpGeometry fields 0x00401690 / 0x0048fef0 read
    std::uint8_t pad00[0x08];
    std::uint32_t flags;                // +0x08 (RpGeometry.flags; PORT-OT-TRIPOOL: 0x0048fef0 ORs 0x40 at 0x0048fef8)
    std::uint8_t pad0c[0x14 - 0x0c];
    std::int32_t numVertices;           // +0x14
    std::uint8_t pad18[0x5c - 0x18];
    rw::MorphTarget* morphTarget;       // +0x5c (librw MorphTarget: vertices at +0x14, as RW's RpMorphTarget)
    rw::Geometry* lib;                  // exe tail: the librw geometry this view copies (owner of flags)
};
struct MashedRwAtomicView {             // the RW 3.x RpAtomic fields read through 0x004b3f90's result
    std::uint8_t pad00[0x04];
    rw::Frame* frame;                   // +0x04 (RwObject.parent)
    std::uint8_t pad08[0x18 - 0x08];
    MashedRwGeometryView* geometry;     // +0x18
    std::uint8_t pad1c[0x3c - 0x1c];
    rw::Clump* clump;                   // +0x3c (RpAtomic.clump; POWERUPINIT-A: 0x00412010 reads it)
    rw::Atomic* lib;                    // exe tail
    MashedRwGeometryView geomView;      // exe tail
};
static_assert(offsetof(MashedRwGeometryView, flags) == 0x08, "RpGeometry +0x08");
static_assert(offsetof(rw::Geometry, flags) == 0x08, "librw Geometry flags @ +0x08 (RW's offset too)");
static_assert(rw::Geometry::MODULATE == 0x40, "rpGEOMETRYMODULATEMATERIALCOLOR = 0x40 in both");
static_assert(offsetof(rw::Material, texture) == 0x00, "RpMaterial texture @ +0x00 (0x004e8090 reads it)");
static_assert(offsetof(MashedRwGeometryView, numVertices) == 0x14, "RpGeometry +0x14");
static_assert(offsetof(MashedRwGeometryView, morphTarget) == 0x5c, "RpGeometry +0x5c");
static_assert(offsetof(MashedRwAtomicView, frame) == 0x04, "RpAtomic +0x04");
static_assert(offsetof(MashedRwAtomicView, geometry) == 0x18, "RpAtomic +0x18");
static_assert(offsetof(MashedRwAtomicView, clump) == 0x3c, "RpAtomic +0x3c");
static_assert(offsetof(rw::Clump, object) + offsetof(rw::Object, parent) == 0x04, "clump frame @ +0x04");
static_assert(offsetof(rw::Frame, root) == 0xa0, "RwFrame root @ +0xa0");
static_assert(offsetof(rw::MorphTarget, vertices) == 0x14, "RpMorphTarget verts @ +0x14");
static_assert(offsetof(rw::Geometry, numVertices) == 0x14, "RpGeometry numVertices @ +0x14");
// One view per call, kept to process exit (13 calls on the knob path: one per perm slot).
std::vector<std::unique_ptr<MashedRwAtomicView>> s_atomicViews;

// RW order of a clump's atomics. RW's RpClumpStreamRead 0x004e7420 links each atomic it reads at the head of the
// clump's list (the decomp's `*(atomic + 0x44) = list; *(atomic + 0x40) = list->next; list->next->prev = ...;
// list->next = atomic + 0x40`), and RpClumpForAllAtomics 0x004e66d0 (re/match/s21/004e66d0.c) walks from the head, so
// the last atomic in file order comes first. librw's Clump::streamRead appends (Clump::addAtomic), so the RW order is
// librw's list walked backwards.
rw::Atomic* RwOrderAtomic(rw::Clump* c, rw::LLLink* l) {
    return (l == c->atomics.end()) ? nullptr : LLLinkGetData(l, rw::Atomic, inClump);
}

// One RW-layout view of librw atomic `a` owned by clump `c`, for the binding of 0x004b3f90 and for the call site of
// PORT-OT-TRIPOOL's 0x0048fef0. The view owns nothing: frame, clump and geometry stay the librw objects' own, and the geometry
// view's flags are a copy that MashedRw_GeometryForAllMaterials_004e89a0 writes back to `lib` (see there).
MashedRwAtomicView* NewAtomicView(rw::Atomic* a, rw::Clump* c) {
    std::unique_ptr<MashedRwAtomicView> v(new MashedRwAtomicView());
    std::memset(v.get(), 0, sizeof(*v));
    v->lib = a;
    v->frame = a->getFrame();
    v->clump = c;
    if (rw::Geometry* const g = a->geometry) {
        v->geomView.flags = g->flags;
        v->geomView.numVertices = g->numVertices;
        v->geomView.morphTarget = g->morphTargets;
        v->geomView.lib = g;
        v->geometry = &v->geomView;
    }
    s_atomicViews.push_back(std::move(v));
    return s_atomicViews.back().get();
}

// The view whose geometry view is `p`, or null (p is then taken as a librw geometry).
MashedRwGeometryView* AsGeometryView(void* p) {
    for (const auto& v : s_atomicViews)
        if (p == &v->geomView) return &v->geomView;
    return nullptr;
}
}  // namespace

extern "C" {

// RpClump* RpClumpStreamRead(RwStream* stream) rva 0x004e7420 (no matched source; decomp_pc.py 2026-11-09): reads the
// clump struct chunk (atomic count), the frame list (0x0e), the geometry list (0x1a), the atomics (0x14) and the
// extensions, with the clump chunk header already consumed by RwStreamFindChunk(stream, 0x10) (0x004b3bf0). librw:
// rw::Clump::streamRead, which takes the stream at the same point (it reads the struct chunk first). The streams of the
// knob path are memory streams (Match/PortTop_RwStream04F.cpp: [0] type 3, [3] position, [4] size, [5] base), so the
// binding reads from base + position and moves the position by what librw consumed.
int MashedRw_ClumpStreamRead_004e7420(int stream) {
    std::uint32_t* const s = FromInt<std::uint32_t>(stream);
    if (!s || s[0] != 3 || s[3] > s[4]) {
        BindLog("RpClumpStreamRead(0x%08X): binding reads memory streams only, returns 0", static_cast<unsigned>(stream));
        return 0;
    }
    rw::uint8* const base = FromInt<rw::uint8>(static_cast<int>(s[5]));
    rw::StreamMemory mem;
    mem.open(base + s[3], s[4] - s[3]);
    // [CLUMP-TEXREAD] the material reader resolves texture names through rw::Texture::findCB: RW's rule for this read.
    rw::Texture* (*const saveFind)(const char*) = rw::Texture::findCB;
    rw::Texture::findCB = RwRuleFindTexture;
    rw::Clump* const c = rw::Clump::streamRead(&mem);
    rw::Texture::findCB = saveFind;
    const std::uint32_t used = mem.tell();
    mem.close();
    s[3] = (s[3] + used <= s[4]) ? s[3] + used : s[4];
    int atomics = 0;
    if (c) for (rw::LLLink* l = c->atomics.link.next; l != c->atomics.end(); l = l->next) ++atomics;
    BindLog("RpClumpStreamRead(0x%08X) -> rw::Clump::streamRead %p (%u bytes, %d atomics)", static_cast<unsigned>(stream),
            static_cast<void*>(c), used, atomics);
    return ToInt(c);
}

// RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack cb, void* data) rva 0x004e66d0 (re/match/s21/004e66d0.c):
// calls cb(atomic, data) for each atomic from the list head, stops at the first cb that returns 0, returns the clump.
// librw: the clump's atomic list in RW order (RwOrderAtomic above). The atomic handed to cb is the librw rw::Atomic, so
// a callback may store or pass it but not read RW atomic fields from it. No knob-path caller reads fields: 0x004b3f90
// binds to the view below.
int MashedRw_ClumpForAllAtomics_004e66d0(int clump, int cb, int data) {
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    using Cb = int(__cdecl*)(int, int);
    Cb const fn = reinterpret_cast<Cb>(static_cast<std::uintptr_t>(static_cast<unsigned>(cb)));
    if (!c || !fn) return clump;
    for (rw::LLLink* l = c->atomics.link.prev; l != c->atomics.end();) {
        rw::LLLink* const prev = l->prev;
        if (!fn(ToInt(RwOrderAtomic(c, l)), data)) break;
        l = prev;
    }
    return clump;
}

// RwFrame* RwFrameUpdateObjects(RwFrame* frame) rva 0x004c0e50 (decomp_pc.py 2026-11-09; matched body only in the .asi's
// Match/MatchS19_004c0e50.cpp): when the root's privateFlags (+0xa0 -> +0x03) have neither bit 0 nor bit 1 set, links
// the root into the dirty list at RwGlobals +0xbc; then root privateFlags |= 3, frame privateFlags |= 0x0c. librw:
// rw::Frame::updateObjects (frame.cpp: the same root test, dirty-list add and flag bits). The matched body reads
// RwGlobals, which the knob path's record does not carry (+0xbc), so it is not the exe's binding.
void MashedRw_FrameUpdateObjects_004c0e50(int frame) {
    if (rw::Frame* const f = FromInt<rw::Frame>(frame)) f->updateObjects();
}

// rva 0x004b3f90 (no matched source; decomp_pc.py 2026-11-09): returns 0 for a null clump, else
// RpClumpForAllAtomics(clump, 0x004b4070, &out) with out = 0 first, where 0x004b4070 stores its atomic argument into
// *data and returns 0 (stop): the first atomic in RW order. Binding: that atomic (RwOrderAtomic on the list's RW head)
// as an RW-layout view (MashedRwAtomicView above). The only knob-path caller, 0x00401690, reads atomic +0x18 ->
// geometry +0x14 / +0x5c -> morph +0x14 and nothing else. A clump with no atomic returns 0, as 0x004b3f90 does.
// [POWERUPINIT-A] the second knob-path caller, 0x00411f30, stores the view in DAT_0063bb38[i], and the matched
// teardown 0x00412010 reads atomic +0x3c (RpAtomic.clump, the clump the atomic belongs to) and destroys it. The view
// carries that field: the clump argument, which owns the atomic.
int MashedRw_ClumpFirstAtomic_004b3f90(int clump) {
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    if (!c) return 0;
    rw::Atomic* const a = RwOrderAtomic(c, c->atomics.link.prev);
    if (!a) return 0;
    MashedRwAtomicView* const v = NewAtomicView(a, c);
    BindLog("0x004b3f90(clump %p) -> atomic %p view %p (geometry %p, %d vertices)", static_cast<void*>(c),
            static_cast<void*>(a), static_cast<void*>(v), static_cast<void*>(a->geometry),
            a->geometry ? a->geometry->numVertices : 0);
    return ToInt(v);
}

// [PORT-OT-TRIPOOL] the RW-layout view of a librw atomic that RpClumpForAllAtomics (binding above) handed to a
// callback, for the matched 0x0048fef0, which reads atomic +0x18 (0x0048fef5) as the geometry. The 0x0048ff20 call
// site passes the slot's atomic (+0x08, stored by the matched callback 0x0048fed0) and the slot's clump (+0x04).
// Returns 0 for a null atomic.
int MashedRw_AtomicView_0048fef0(int atomic, int clump) {
    rw::Atomic* const a = FromInt<rw::Atomic>(atomic);
    if (!a) return 0;
    return ToInt(NewAtomicView(a, FromInt<rw::Clump>(clump)));
}

// [ATOMIC-VIEW] RpClumpForAllAtomics rva 0x004e66d0 for a call site whose callback is a matched body that reads the
// RW 3.x RpAtomic layout (0x0040cda0 -> 0x0040bb10 -> 0x0040bae0 reads the geometry at atomic +0x18, 0x0040bae0
// first instruction after the 0x0053fb70 call). The same walk as MashedRw_ClumpForAllAtomics_004e66d0 above (librw
// atomics in RW order, stop when the callback returns 0), but each atomic is handed over as its RW-layout view
// (NewAtomicView, the 0x004b3f90 / 0x0048fef0 view). Returns the clump.
int MashedRw_ClumpForAllAtomicsAsViews_004e66d0(int clump, int cb, int data) {
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    using Cb = int(__cdecl*)(int, int);
    Cb const fn = reinterpret_cast<Cb>(static_cast<std::uintptr_t>(static_cast<unsigned>(cb)));
    if (!c || !fn) return clump;
    int n = 0;
    for (rw::LLLink* l = c->atomics.link.prev; l != c->atomics.end();) {
        rw::LLLink* const prev = l->prev;
        ++n;
        if (!fn(ToInt(NewAtomicView(RwOrderAtomic(c, l), c)), data)) break;
        l = prev;
    }
    BindLog("0x004e66d0(clump %p) as views: %d atomics", static_cast<void*>(c), n);
    return clump;
}

// rva 0x0053fb70 RpMatFXAtomicEnableEffects (third-party-library[renderware]): flags the atomic for the MatFX
// pipeline. librw: rw::MatFX::enableEffects. Takes an RW-layout view (resolved to its librw atomic) or a librw atomic.
// Returns the pointer it was given.
int MashedRw_MatFXAtomicEnableEffects_0053fb70(int atomic) {
    rw::Atomic* a = FromInt<rw::Atomic>(atomic);
    for (const auto& v : s_atomicViews)
        if (static_cast<void*>(v.get()) == static_cast<void*>(a)) { a = v->lib; break; }
    if (a) rw::MatFX::enableEffects(a);
    BindLog("0x0053fb70(%08X) -> rw::MatFX::enableEffects(atomic %p)", static_cast<unsigned>(atomic), static_cast<void*>(a));
    return atomic;
}

// rva 0x0053fbb0 RpMatFXMaterialSetEffects (third-party-library[renderware]): (material, effect). RW's rpMATFXEFFECT
// numbering (0 none, 1 bump, 2 env, 3 bump+env, 4 dual, 5 uv, 6 dual+uv) is librw's MatFX enum order
// (deps/librw/src/rwplugins.h). librw rw::Material keeps the texture at +0x00 as RpMaterial does (static_assert above),
// which is the one field the matched caller 0x0040baa0 reads. Returns the material.
int MashedRw_MatFXMaterialSetEffects_0053fbb0(int material, int effect) {
    static_assert(rw::MatFX::ENVMAP == 2 && rw::MatFX::DUAL == 4, "rpMATFXEFFECT numbering");
    rw::Material* const m = FromInt<rw::Material>(material);
    if (m) rw::MatFX::setEffects(m, static_cast<rw::uint32>(effect));
    BindLog("0x0053fbb0(material %p, %d) -> rw::MatFX::setEffects", static_cast<void*>(m), effect);
    return material;
}

// rva 0x0053fd30 RpMatFXMaterialSetupEnvMap (third-party-library[renderware]): (material, texture, frame,
// useFrameBufferAlpha, coefficient). librw: the material's MatFX env effect gets the texture (addRef, as RW refcounts
// it), the frame, the FB-alpha flag and the coefficient. Returns the material, 0 when the material has no MatFX data.
int MashedRw_MatFXMaterialSetupEnvMap_0053fd30(int material, int texture, int frame, int fbAlpha, float coef) {
    rw::Material* const m = FromInt<rw::Material>(material);
    rw::MatFX* const fx = m ? rw::MatFX::get(m) : nullptr;
    if (!fx) {
        BindLog("0x0053fd30(material %p): no MatFX data, returns 0", static_cast<void*>(m));
        return 0;
    }
    fx->setEnvTexture(FromInt<rw::Texture>(texture));
    fx->setEnvFrame(FromInt<rw::Frame>(frame));
    fx->setEnvFBAlpha(fbAlpha);
    fx->setEnvCoefficient(coef);
    BindLog("0x0053fd30(material %p, texture %08X, frame %08X, fbAlpha %d, coef %g) -> MatFX env",
            static_cast<void*>(m), static_cast<unsigned>(texture), static_cast<unsigned>(frame), fbAlpha, coef);
    return material;
}

// ---------------------------------------------------------------------------------------------------------------
// [ATOMIC-VIEW-2] RW calls of the DepthCharge init 0x004548e0 (type-table entry 1) and its 0x004548a0 tree
// (Match/PortTop_004548e0.cpp, Match/PortTop_004548a0.cpp). All seven are RW 3.x library functions
// (hooks.csv third-party-library[renderware]); their matched bodies walk RW's own frame / atomic layouts
// (RwFrame child list at +0x98 / +0x9c, root at +0xa0, the world plugin slot), which the exe's librw objects do not
// have, so each is a librw binding.
// ---------------------------------------------------------------------------------------------------------------

// rva 0x004e69a0 RpAtomicClone (re/match/rwl02/004e69a0.c): a new atomic with the source's type / flags, no frame and
// the same geometry, in no world. librw: rw::Atomic::clone (Object::copy leaves the parent nil). The 0x004548e0 caller
// passes the RW-layout view from 0x004b3f90, which resolves to its librw atomic. Returns the librw clone.
int MashedRw_AtomicClone_004e69a0(int atomic) {
    rw::Atomic* a = FromInt<rw::Atomic>(atomic);
    for (const auto& v : s_atomicViews)
        if (static_cast<void*>(v.get()) == static_cast<void*>(a)) { a = v->lib; break; }
    rw::Atomic* const n = a ? a->clone() : nullptr;
    BindLog("0x004e69a0(%08X) -> rw::Atomic::clone(atomic %p) %p", static_cast<unsigned>(atomic), static_cast<void*>(a),
            static_cast<void*>(n));
    return ToInt(n);
}

// rva 0x004e7e30 RpAtomicSetFrame (re/match/rwl02/004e7e30.c) for a librw atomic: links the atomic to the frame and
// marks it dirty (privateFlags |= 1 in RW, WORLDBOUNDDIRTY in librw). The exe's Orig_004e7e30 stays the body for the
// RW-layout PTank records (Match/PortTop_004e7e30.cpp). librw: rw::Atomic::setFrame. Returns the atomic.
int MashedRw_AtomicSetFrame_004e7e30(int atomic, int frame) {
    rw::Atomic* const a = FromInt<rw::Atomic>(atomic);
    if (a) {
        a->setFrame(FromInt<rw::Frame>(frame));
        a->object.object.privateFlags |= rw::Atomic::WORLDBOUNDDIRTY;
    }
    return atomic;
}

// rva 0x004c1040 RwFrameAddChild (re/match/rwl01/004c1040.c, C2): (parent, child); unlinks the child from its old
// parent, links it at the head of the parent's child list, re-roots it and marks the hierarchy dirty. librw:
// rw::Frame::addChild(child, append = 0), the same head insertion. Returns the parent.
int MashedRw_FrameAddChild_004c1040(int parent, int child) {
    rw::Frame* const p = FromInt<rw::Frame>(parent);
    rw::Frame* const c = FromInt<rw::Frame>(child);
    if (p && c) p->addChild(c, 0);
    return parent;
}

// rva 0x004c15c0 RwFrameSetIdentity (re/match/rwl01/004c15c0.c): the frame matrix (+0x10) to identity with type
// flags |= 0x20003, then the root on the dirty list. librw: matrix.setIdentity() and updateObjects(). Returns nothing.
void MashedRw_FrameSetIdentity_004c15c0(int frame) {
    rw::Frame* const f = FromInt<rw::Frame>(frame);
    if (!f) return;
    f->matrix.setIdentity();
    f->updateObjects();
}

// rva 0x004c1210 RwFrameRemoveChild (re/match/rwl01/004c1210.c, C4 by match, a RenderWare library function): unlinks
// the frame from its parent, re-roots it and its children, marks it dirty. librw: rw::Frame::removeChild. Returns the
// frame. The matched body walks the RW child list (+0x98 / +0x9c), so it cannot run on a librw frame.
int MashedRw_FrameRemoveChild_004c1210(int frame) {
    rw::Frame* const f = FromInt<rw::Frame>(frame);
    if (f) f->removeChild();
    return frame;
}

// rva 0x004e4440 RpAtomicGetWorld (re/match/rwl02/004e4440.c): the dword at atomic + [0x007d7170], the atomic's world
// plugin slot. librw: rw::Atomic::world.
int MashedRw_AtomicGetWorld_004e4440(int atomic) {
    rw::Atomic* const a = FromInt<rw::Atomic>(atomic);
    return a ? ToInt(a->world) : 0;
}

// rva 0x004e43b0 RpWorldRemoveAtomic (re/match/rwl02/004e43b0.c): (world, atomic); unlinks the atomic from the world's
// sectors and clears its world slot. librw: rw::World::removeAtomic. Returns the world.
int MashedRw_WorldRemoveAtomic_004e43b0(int world, int atomic) {
    rw::World* const w = FromInt<rw::World>(world);
    rw::Atomic* const a = FromInt<rw::Atomic>(atomic);
    if (w && a && a->world == w) w->removeAtomic(a);
    return world;
}

// rva 0x004c39b0 RwV3dNormalize (re/match/s19/004c39b0.c, hooks.csv RwV3dNormalize C2): (out, in) with RW's sqrt /
// inverse-sqrt lookup tables. The exe holds the original's two tables (Math/RwLutTables.cpp), and LutNormalize is this
// function's arithmetic on them. The original's zero-length error report (0x004d7ff0 / 0x004d8480) is not raised.
// Returns the length.
float MashedRw_V3dNormalize_004c39b0(float* out, const float* in) {
    return mashed_re::Math::LutNormalize(out, in);
}

// RpGeometry* RpGeometryForAllMaterials(RpGeometry* geometry, RpMaterialCallBack cb, void* data) rva 0x004e89a0
// (re/match/rwl02/004e89a0.c, 76.7%, not the L-115 class, so this is a binding, not that body). Disassembly of
// MASHED.exe.unpatched: count = geometry +0x24 (0x004e89aa), cached at [esp+0x14] (0x004e89af); for i < count,
// cb(materials[i], data) with materials = geometry +0x20 (0x004e89bd / 0x004e89c1), stop when cb returns 0
// (0x004e89cc); returns geometry (0x004e89d7). librw: the same walk over Geometry::matList in list order.
// An RW-layout geometry view (MashedRwAtomicView::geomView) is resolved to its librw geometry first, and the view's
// +0x08 flags are written to it: the matched 0x0048fef0 ORs 0x40 into the view at 0x0048fef8 and calls this binding
// right after (0x0048ff07), so this is the first point the librw geometry, the flags' one owner, can take the store.
// Any other pointer is taken as a librw geometry. Returns the pointer it was given.
int MashedRw_GeometryForAllMaterials_004e89a0(int geometry, int cb, int data) {
    rw::Geometry* g = FromInt<rw::Geometry>(geometry);
    static int s_logged = 0;
    if (MashedRwGeometryView* const gv = AsGeometryView(g)) {
        g = gv->lib;
        if (g) {
            if (s_logged < 2)
                BindLog("RpGeometryForAllMaterials(view %p -> geometry %p): flags %08X -> %08X (view +0x08), %d materials",
                        static_cast<void*>(gv), static_cast<void*>(g), g->flags, gv->flags, g->matList.numMaterials);
            g->flags = gv->flags;
        }
    } else if (s_logged < 2) {
        BindLog("RpGeometryForAllMaterials(geometry %p): librw geometry", static_cast<void*>(g));
    }
    ++s_logged;
    using Cb = int(__cdecl*)(int, int);
    Cb const fn = reinterpret_cast<Cb>(static_cast<std::uintptr_t>(static_cast<unsigned>(cb)));
    if (!g || !fn) return geometry;
    const rw::int32 n = g->matList.numMaterials;
    for (rw::int32 i = 0; i < n; ++i)
        if (!fn(ToInt(g->matList.materials[i]), data)) break;
    return geometry;
}

}  // extern "C"

// ===============================================================================================================
// [PORT-TOP-04I] PTank atomics for 0x004881d0 / 0x004880a0 (lead decision 2026-11-09: an RW-layout record, the 04B
// camera-record pattern). librw has no PTank, so RpPTankAtomicCreate 0x00534b60 returns this record. It carries every
// field the matched callers touch, at the RW offsets they use (re/match/s14/004881d0.c, 004880a0.c, re/match/rwl02/
// 004e7e30.c, 004e8090.c, re/match/s14/00489250.c):
//   atomic +0x03 privateFlags (0x004e7e30 |= 1), +0x04 frame (0x004c0740 binding), +0x18 geometry,
//   atomic + [0x007dc57c] -> the PTank block (OBJ(h): +0x04, +0x40, +0xa4, +0xa8, +0xac),
//   geometry +0x20 -> material list -> material[0] +0x00 texture (0x004e8090), geometry +0x5c -> morph target +0x04
//   (16-byte bounding sphere, written whole).
// The plugin slot sits at +0x70, the value the 0x00493640 shim stores at 0x007dc57c. Nothing in the exe draws these
// records: they are created at boot and destroyed in AppDestroy (0x00489250 -> RpAtomicDestroy 0x004e6920 below).
// ===============================================================================================================
namespace {
constexpr std::uint32_t kPTankRecordMagic = 0x4b4e5450u;   // exe-only tag
struct MashedRwMaterialView {           // RW 3.x RpMaterial: texture at +0x00 (0x004e8090)
    rw::Texture* texture;
    std::uint8_t pad04[0x1c];
};
struct MashedRwMorphView {              // RW 3.x RpMorphTarget: parent +0x00, bounding sphere +0x04
    void* parent;
    float sphere[4];
    void* verts;
    void* normals;
};
struct MashedRwPTankGeometryView {      // RW 3.x RpGeometry: material list at +0x20, morph target at +0x5c
    std::uint8_t pad00[0x20];
    MashedRwMaterialView** materials;   // +0x20 (RpMaterialList.materials)
    std::int32_t numMaterials;          // +0x24
    std::int32_t space;                 // +0x28
    std::uint8_t pad2c[0x5c - 0x2c];
    MashedRwMorphView* morphTarget;     // +0x5c
};
struct MashedRwPTankAtomic {
    std::uint8_t type, subType, flags, privateFlags;   // +0x00 RwObject
    rw::Frame* frame;                                  // +0x04 RwObject.parent
    std::uint8_t pad08[0x18 - 0x08];
    MashedRwPTankGeometryView* geometry;               // +0x18
    std::uint8_t pad1c[0x70 - 0x1c];
    std::uint8_t* ptank;                               // +0x70 = atomic + [0x007dc57c]
    std::uint32_t magic;                               // exe tail
    MashedRwPTankGeometryView geom;
    MashedRwMaterialView* matList[1];
    MashedRwMaterialView mat;
    MashedRwMorphView morph;
    alignas(4) std::uint8_t ptankData[0x100];          // the PTank block (callers write up to +0xac)
};
static_assert(offsetof(MashedRwPTankAtomic, frame) == 0x04, "RpAtomic +0x04");
static_assert(offsetof(MashedRwPTankAtomic, geometry) == 0x18, "RpAtomic +0x18");
static_assert(offsetof(MashedRwPTankAtomic, ptank) == 0x70, "PTank slot = the 0x00493640 shim's [0x007dc57c]");
static_assert(offsetof(MashedRwPTankGeometryView, materials) == 0x20, "RpGeometry +0x20");
static_assert(offsetof(MashedRwPTankGeometryView, morphTarget) == 0x5c, "RpGeometry +0x5c");
static_assert(offsetof(MashedRwMorphView, sphere) == 0x04, "RpMorphTarget +0x04");
static_assert(offsetof(rw::Texture, filterAddressing) == 0x50, "RwTexture +0x50 (0x004881d0 writes 2)");
static_assert(offsetof(rw::Texture, refCount) == 0x54, "RwTexture +0x54 (0x004e8090 raises it)");

MashedRwPTankAtomic* AsPTank(int obj) {
    MashedRwPTankAtomic* const a = FromInt<MashedRwPTankAtomic>(obj);
    return (a && a->magic == kPTankRecordMagic) ? a : nullptr;
}

// The _rwObjectHasFrameSetFrame binding's PTank case: store the frame at +0x04 and mark it dirty (the matched body's
// RwFrameUpdateObjects tail; librw rw::Frame::updateObjects). The record is not a librw object, so no librw link is made.
bool PTankRecordSetFrame(int object, int frame) {
    MashedRwPTankAtomic* const a = AsPTank(object);
    if (!a) return false;
    a->frame = FromInt<rw::Frame>(frame);
    if (a->frame) a->frame->updateObjects();
    return true;
}
}  // namespace

// The exe's copy of 0x00534b60's flag normalisation (Render/RfxEmitter.cpp, "0x00534b60").
namespace mashed_re { namespace Rfx { std::uint32_t PTankNormaliseFlags(std::uint32_t f); } }

extern "C" {

// RpAtomic* RpPTankAtomicCreate(RwInt32 maxPCount, RwUInt32 dataFlags, RwUInt32 platFlags) rva 0x00534b60 (decomp
// 2026-11-09): normalises dataFlags (the part Render/RfxEmitter.cpp PTankNormaliseFlags ports), then FUN_00534d00(maxPCount,
// flags, platFlags, &DAT_00623c78) builds the atomic. Binding: a MashedRwPTankAtomic record. The PTank block gets the
// three stores Render/RfxEmitter.h cites for FUN_00534d00: +0x00 = maxPCount (0x00534d67), +0x04 = 0 (0x00534d69),
// +0x40 = 0xffffffff (0x00534d79). [UNCERTAIN] the other FUN_00534d00 stores (+0xa0 / +0x3c, where the normalised flags go,
// the ARRAY-layout allocation at 0x0053510c..): not decoded, left 0. No knob-path code reads them: the two callers
// (given the "lensflare-fs8" texture) write +0x04 / +0xa4 / +0xa8 / +0xac and OR into +0x40.
int MashedRw_PTankAtomicCreate_00534b60(int maxPCount, unsigned dataFlags, int platFlags) {
    MashedRwPTankAtomic* const a = static_cast<MashedRwPTankAtomic*>(std::calloc(1, sizeof(MashedRwPTankAtomic)));
    if (!a) return 0;
    a->type = 1;                                   // rpATOMIC
    a->magic = kPTankRecordMagic;
    a->ptank = a->ptankData;
    a->geometry = &a->geom;
    a->geom.materials = a->matList;
    a->geom.numMaterials = 1;
    a->geom.space = 1;
    a->matList[0] = &a->mat;
    a->geom.morphTarget = &a->morph;
    a->morph.parent = &a->geom;
    std::uint32_t* const t = reinterpret_cast<std::uint32_t*>(a->ptankData);
    t[0x00 / 4] = static_cast<std::uint32_t>(maxPCount);
    t[0x04 / 4] = 0;
    t[0x40 / 4] = 0xffffffffu;
    BindLog("RpPTankAtomicCreate(%d, 0x%08X -> 0x%08X, %d) -> PTank record %p", maxPCount, dataFlags,
            mashed_re::Rfx::PTankNormaliseFlags(dataFlags), platFlags, static_cast<void*>(a));
    return ToInt(a);
}

// RwBool RpAtomicDestroy(RpAtomic* atomic) rva 0x004e6920 (no matched source; decomp 2026-11-09): plugin destructors
// (0x004d8060), the instance data at +0x14 (0x004d8bd0), the geometry at +0x18 (RpGeometryDestroy 0x004e8ea0, which
// destroys its materials and so their textures), the frame unlink (0x004c0790), then the free list at RwGlobals +0x11c;
// returns 1. Binding: a PTank record drops its material's texture (RwTextureDestroy, the count 0x004e8090 raised) and is
// freed; a librw atomic goes to rw::Atomic::destroy. 0x00489250 calls RwFrameDestroy on +0x04 first.
int MashedRw_AtomicDestroy_004e6920(int atomic) {
    if (MashedRwPTankAtomic* const a = AsPTank(atomic)) {
        rw::Texture* const tex = a->mat.texture;
        BindLog("RpAtomicDestroy(PTank record %p): material texture %p released, record freed", static_cast<void*>(a),
                static_cast<void*>(tex));
        if (tex) MashedRw_TextureDestroy_004c5a60(ToInt(tex));
        a->magic = 0;
        std::free(a);
        return 1;
    }
    if (rw::Atomic* const la = FromInt<rw::Atomic>(atomic)) la->destroy();
    return 1;
}

}  // extern "C"

// ===============================================================================================================
// [PORT-TOP-04J] the cup world of 0x00401000 / 0x004014b0 (family 1): RpWorldCreate, the lights, the world's clump.
//
// The matched 0x00401000 writes one RW-layout light field itself (DAT_0063656c->flags = 1, +0x02) and calls the matched
// light leaves RpLightSetColor 0x004e4900 (+0x18..+0x24, +0x03), RpLightSetConeAngle 0x004e4b90 (+0x28) and
// RpLightSetRadius 0x004e48e0 (+0x14, then the frame at +0x04). librw rw::Light keeps radius / colour / cone at other
// offsets (an ObjectWithFrame of 0x14 bytes plus the plugin base), so RpLightCreate 0x004e4dd0 returns an RW-layout record
// (the 04B camera / 04I PTank pattern) that owns a librw rw::Light. The record's fields go to the librw light in
// RpWorldAddLight 0x004e4810, the first binding that hands the light to librw (0x00401000 calls every setter before it).
// The world is a librw rw::World (RpWorldCreate 0x004e5850). No matched body on the knob path reads world fields: the
// world calls (0x004e4810 / 0x004e4860 / 0x004e4450 / 0x004e45b0) are bindings. Nothing in the exe renders this world.
// ===============================================================================================================
namespace {
constexpr std::uint32_t kLightRecordMagic = 0x5448474cu;   // exe-only tag
struct MashedRwLight {                  // RW 3.x RpLight, offsets as written by 0x004e4dd0 (re/match/rwl02/004e4dd0.c)
    std::uint8_t type, subType, flags, privateFlags;   // +0x00 RwObject
    void* parent;                                      // +0x04 frame
    void* lFrameNext;                                  // +0x08 RwObjectHasFrame.lFrame
    void* lFramePrev;                                  // +0x0c
    void* sync;                                        // +0x10
    float radius;                                      // +0x14
    float color[4];                                    // +0x18 RwRGBAReal
    float minusCosAngle;                               // +0x28
    void* sectorsNext;                                 // +0x2c WorldSectorsInLight
    void* sectorsPrev;                                 // +0x30
    void* inWorldNext;                                 // +0x34
    void* inWorldPrev;                                 // +0x38
    std::uint16_t lightFrame;                          // +0x3c
    std::uint16_t pad3e;
    std::uint32_t magic;                               // exe tail
    rw::Light* lib;
};
static_assert(offsetof(MashedRwLight, parent) == 0x04, "RpLight +0x04");
static_assert(offsetof(MashedRwLight, radius) == 0x14, "RpLight radius +0x14 (0x004e48e0)");
static_assert(offsetof(MashedRwLight, color) == 0x18, "RpLight colour +0x18 (0x004e4900)");
static_assert(offsetof(MashedRwLight, minusCosAngle) == 0x28, "RpLight minusCosAngle +0x28 (0x004e4b90)");
static_assert(offsetof(MashedRwLight, sectorsNext) == 0x2c, "RpLight +0x2c (0x004e4dd0, 0x004e4860)");
static_assert(offsetof(MashedRwLight, inWorldNext) == 0x34, "RpLight +0x34 (0x004e4810)");
static_assert(offsetof(MashedRwLight, lightFrame) == 0x3c, "RpLight +0x3c (0x004e4dd0)");
static_assert(offsetof(rw::Geometry, flags) == 0x08, "RpGeometry flags @ +0x08 (0x004b52c0)");

MashedRwLight* AsLight(int obj) {
    MashedRwLight* const l = FromInt<MashedRwLight>(obj);
    return (l && l->magic == kLightRecordMagic) ? l : nullptr;
}

// The record's RW fields into the librw light: radius +0x14, colour +0x18, minusCosAngle +0x28, flags +0x02 and
// privateFlags +0x03 (0x004e4900's grey-colour bit, the bit librw Light::setColor keeps in privateFlags).
void LightRecordSync(MashedRwLight* l) {
    rw::Light* const ll = l->lib;
    if (!ll) return;
    ll->radius = l->radius;
    ll->color.red = l->color[0];
    ll->color.green = l->color[1];
    ll->color.blue = l->color[2];
    ll->color.alpha = l->color[3];
    ll->minusCosAngle = l->minusCosAngle;
    ll->setFlags(l->flags);
    ll->object.object.privateFlags = l->privateFlags;
}

// The _rwObjectHasFrameSetFrame binding's light case: the frame at +0x04 and librw's ObjectWithFrame::setFrame (which
// links the light into the frame and marks it dirty, the matched body's 0x004c0e50 tail).
bool LightRecordSetFrame(int object, int frame) {
    MashedRwLight* const l = AsLight(object);
    if (!l) return false;
    l->parent = FromInt<void>(frame);
    if (l->lib) l->lib->setFrame(FromInt<rw::Frame>(frame));
    return true;
}
}  // namespace

extern "C" {

// RpWorld* RpWorldCreate(RwBBox* bbox) rva 0x004e5850 (re/match/rwl02/004e5850.c, C2): allocates the world (type hint
// 0x3000b), its sector tree and plugin data. librw: rw::World::create (empty light / clump lists, librw keeps no bbox).
// Call site: 0x00401000 with the (-100..100)^3 box.
int MashedRw_WorldCreate_004e5850(int bbox) {
    rw::World* const w = rw::World::create(FromInt<rw::BBox>(bbox));
    BindLog("RpWorldCreate(bbox %08X) -> rw::World::create %p", static_cast<unsigned>(bbox), static_cast<void*>(w));
    return ToInt(w);
}

// RpLight* RpLightCreate(RwInt32 type) rva 0x004e4dd0 (re/match/rwl02/004e4dd0.c BYTE-IDENTICAL): allocates through
// RwGlobals +0x118 and stores type 3 (+0x00), subType = type (+0x01), colour 1,1,1,1 (+0x18..+0x24), frame 0 (+0x04),
// sync 0x004d7ff0 (+0x10), radius 0 (+0x14), minusCosAngle 0 (+0x28), privateFlags 1 (+0x03), the self-linked list at
// +0x2c, +0x34 / +0x38 = 0, +0x3c = RwGlobals +0x0a (int16) - 1, flags 3 (+0x02), then the plugin constructors
// (0x004d8000). Binding: a MashedRwLight with those stores (sync 0: nothing calls it through the record) and a librw
// rw::Light::create(type) (librw uses the same type numbers: 2 = AMBIENT, 0x82 = SOFTSPOT).
int MashedRw_LightCreate_004e4dd0(int type) {
    MashedRwLight* const l = static_cast<MashedRwLight*>(std::calloc(1, sizeof(MashedRwLight)));
    if (!l) return 0;
    l->type = 3;
    l->subType = static_cast<std::uint8_t>(type);
    l->color[0] = l->color[1] = l->color[2] = l->color[3] = 1.0f;
    l->privateFlags = 1;
    l->sectorsNext = l->sectorsPrev = &l->sectorsNext;
    const std::uintptr_t g = *reinterpret_cast<const std::uintptr_t*>(static_cast<std::uintptr_t>(0x007d3ff8u));
    const std::int16_t k = g ? *reinterpret_cast<const std::int16_t*>(g + 0x0a) : 0;
    l->lightFrame = static_cast<std::uint16_t>(k - 1);
    l->flags = 3;
    l->magic = kLightRecordMagic;
    l->lib = rw::Light::create(type);
    BindLog("RpLightCreate(0x%X) -> light record %p (rw::Light %p)", static_cast<unsigned>(type), static_cast<void*>(l),
            static_cast<void*>(l->lib));
    return ToInt(l);
}

// RwBool RpLightDestroy(RpLight* light) rva 0x004e4d90 (re/match/rwl02/004e4d90.c BYTE-IDENTICAL): plugin destructors
// (0x004d8060), frame unlink (0x004c0790), free through RwGlobals +0x11c, returns 1. Binding: rw::Light::destroy (which
// unsets the frame) and the record is freed. Call site: 0x004014b0, after RpWorldRemoveLight.
int MashedRw_LightDestroy_004e4d90(int light) {
    MashedRwLight* const l = AsLight(light);
    if (!l) {
        BindLog("RpLightDestroy(0x%08X): not a light record, ignored", static_cast<unsigned>(light));
        return 1;
    }
    BindLog("RpLightDestroy(light record %p) -> rw::Light::destroy %p, record freed", static_cast<void*>(l),
            static_cast<void*>(l->lib));
    if (l->lib) l->lib->destroy();
    l->magic = 0;
    std::free(l);
    return 1;
}

// RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light) rva 0x004e4810 (re/match/rwl02/004e4810.c BYTE-IDENTICAL):
// stores the world in the light's plugin slot ([0x007d7178]), links the light at world +0x3c (subType < 0x80) or, after
// RwFrameUpdateObjects on its frame, at world +0x34, returns the world. Binding: the record's fields into the librw
// light (LightRecordSync), then rw::World::addLight (the same < POINT split, the same frame update).
int MashedRw_WorldAddLight_004e4810(int world, int light) {
    rw::World* const w = FromInt<rw::World>(world);
    MashedRwLight* const l = AsLight(light);
    if (!w || !l || !l->lib) {
        BindLog("RpWorldAddLight(0x%08X, 0x%08X): world or light record missing, ignored", static_cast<unsigned>(world),
                static_cast<unsigned>(light));
        return world;
    }
    LightRecordSync(l);
    w->addLight(l->lib);
    BindLog("RpWorldAddLight(world %p, light record %p type 0x%02X flags %u radius %g colour %g/%g/%g/%g minusCos %g) -> "
            "rw::World::addLight", static_cast<void*>(w), static_cast<void*>(l), l->subType, l->flags,
            static_cast<double>(l->radius), static_cast<double>(l->color[0]), static_cast<double>(l->color[1]),
            static_cast<double>(l->color[2]), static_cast<double>(l->color[3]), static_cast<double>(l->minusCosAngle));
    return world;
}

// RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light) rva 0x004e4860 (re/match/rwl02/004e4860.c BYTE-IDENTICAL):
// clears the plugin slot, frees the light's sector links (+0x2c list), unlinks +0x34, returns the world. Binding:
// rw::World::removeLight when the librw light is in a world.
int MashedRw_WorldRemoveLight_004e4860(int world, int light) {
    MashedRwLight* const l = AsLight(light);
    rw::World* const w = FromInt<rw::World>(world);
    if (w && l && l->lib && l->lib->world) w->removeLight(l->lib);
    BindLog("RpWorldRemoveLight(%p, light record %p) -> rw::World::removeLight", static_cast<void*>(w), static_cast<void*>(l));
    return world;
}

// rva 0x004e4450 (re/match/rwl02/004e4450.c, C2; hooks.csv name RwFrameAddChild, naming doubt aq_s4: RpWorldAddClump):
// (world, clump): links the clump at world +0x2c, counts it (+0x24), stores the world in the clump's plugin slot
// ([0x007d7174]), adds the clump's atomics / lights / cameras (0x004e66d0 / 0x004e6760 / 0x004e6710 callbacks), then
// updates the clump frame, returns the world. librw: rw::World::addClump (the same three lists, the same frame update).
int MashedRw_WorldAddClump_004e4450(int world, int clump) {
    rw::World* const w = FromInt<rw::World>(world);
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    if (w && c && !c->world) w->addClump(c);
    BindLog("0x004e4450(world %p, clump %p) -> rw::World::addClump", static_cast<void*>(w), static_cast<void*>(c));
    return world;
}

// rva 0x004e45b0 (re/match/rwl02/004e45b0.c BYTE-IDENTICAL; hooks.csv name RwFrameRemoveChild, naming doubt aq_s4:
// RpWorldRemoveClump): (world, clump): when the clump's world slot is set, uncounts and unlinks it, removes its atomics,
// lights and cameras and clears the slot, returns the world. Else the error path, returns 0. librw: rw::World::removeClump.
int MashedRw_WorldRemoveClump_004e45b0(int world, int clump) {
    rw::World* const w = FromInt<rw::World>(world);
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    if (!c || !c->world) {
        BindLog("0x004e45b0(world %p, clump %p): clump in no world, returns 0", static_cast<void*>(w), static_cast<void*>(c));
        return 0;
    }
    c->world->removeClump(c);
    BindLog("0x004e45b0(world %p, clump %p) -> rw::World::removeClump", static_cast<void*>(w), static_cast<void*>(c));
    return world;
}

// rva 0x004b5300 (no matched source; disassembly 0x004b5300..0x004b531c): the RpClumpForAllAtomics callback of 0x004b5320.
// Calls 0x004b52f0(atomic, data[0], data[1]) and returns the atomic. 0x004b52f0 (re/match/s18/004b52f0.c, BYTE-IDENTICAL)
// reads the geometry at RW atomic +0x18 and calls 0x004b52c0 (re/match/s18/004b52c0.c, BYTE-IDENTICAL): geometry +0x08
// |= mask when set != 0, else &= ~mask. The ForAllAtomics binding hands this callback a librw rw::Atomic (geometry at
// +0x14), so the binding does that work on librw: a->geometry->flags, which sits at +0x08 as in RW (asserted above).
// The two matched leaves stay out of the exe: the .asi has hand bodies of both (Util/PromoLoop_sessionB.cpp Or4b52c0 /
// Thunk4b52f0), and one RVA has one body per target (rva-lint CROSS-TARGET). Call site: 0x00401000 -> 0x004b5320(cup
// clump, 0x40, 1).
void* MashedRw_AtomicGeometryFlags_004b5300(void* atomic, void* data) {
    rw::Atomic* const a = static_cast<rw::Atomic*>(atomic);
    const unsigned* const d = static_cast<const unsigned*>(data);
    if (!a || !a->geometry) return atomic;
    const std::uint32_t before = a->geometry->flags;
    if (d[1]) a->geometry->flags = before | d[0];      // 0x004b52c0 set arm
    else      a->geometry->flags = before & ~d[0];     // 0x004b52c0 clear arm
    BindLog("0x004b5300(atomic %p): mask 0x%X, set %u: geometry %p flags 0x%08X -> 0x%08X", atomic, d[0], d[1],
            static_cast<void*>(a->geometry), before, a->geometry->flags);
    return atomic;
}

// ---------------------------------------------------------------------------------------------------------------
// [POWERUPINIT-C] RW calls of the powerup type-table inits (Match/PortTop_00452f70.cpp .. PortTop_0045b830.cpp). Each
// init loads a clump through the matched 0x0042a5d0 (a librw rw::Clump), clones it, and collects the clone's atomics
// through 0x004b3fc0 -> RpClumpForAllAtomics (the binding above hands out librw rw::Atomic pointers). So every atomic,
// frame and clump these bindings receive is a librw object.
// ---------------------------------------------------------------------------------------------------------------

// rva 0x004e6ab0 RpClumpClone (re/match/rwl02/004e6ab0.c, C2): a new clump with copies of the frame hierarchy and the
// atomics, out of any world. librw: rw::Clump::clone.
int MashedRw_ClumpClone_004e6ab0(int clump) {
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    rw::Clump* const n = c ? c->clone() : nullptr;
    BindLog("0x004e6ab0(clump %p) -> rw::Clump::clone %p", static_cast<void*>(c), static_cast<void*>(n));
    return ToInt(n);
}

// rva 0x004e4800 (re/match/rwl02/004e4800.c, BYTE-IDENTICAL): returns the dword at clump + [0x007d7174], the clump's
// world plugin slot. Every caller on this path passes a clump: 0x00454bd0 (entry +0x10), 0x0045b360 (slot
// +0x1c), 0x00457ba0 (mine +0x58) and 0x00458020 (dropper +0x14). librw: rw::Clump::world.
int MashedRw_ClumpGetWorld_004e4800(int clump) {
    rw::Clump* const c = FromInt<rw::Clump>(clump);
    return c ? ToInt(c->world) : 0;
}

// rva 0x004b5190 (re/match/s18/004b5190.c, BYTE-IDENTICAL): atomic +0x18 (the geometry) -> user-data array p1, element
// p2 as an int (0x00543d70 / 0x00543df0). librw: rw::UserDataArray on a->geometry. The original has no guard for a
// geometry without user data. The binding returns -1 there, which no init's switch takes (cases 0..6), and logs it.
int MashedRw_AtomicPartCode_004b5190(int atomic, int array, int element) {
    rw::Atomic* const a = FromInt<rw::Atomic>(atomic);
    rw::Geometry* const g = a ? a->geometry : nullptr;
    if (!g || rw::UserDataArray::geometryGetCount(g) <= array) {
        BindLog("0x004b5190(atomic %p, %d, %d): no user-data array, returns -1", static_cast<void*>(a), array, element);
        return -1;
    }
    rw::UserDataArray* const u = rw::UserDataArray::geometryGet(g, array);
    if (!u || u->numElements <= element) {
        BindLog("0x004b5190(atomic %p, %d, %d): no element, returns -1", static_cast<void*>(a), array, element);
        return -1;
    }
    return u->getInt(element);
}

// rva 0x004b52f0 (re/match/s18/004b52f0.c, BYTE-IDENTICAL): geometry (atomic +0x18) -> 0x004b52c0: geometry +0x08 |=
// mask when set != 0, else &= ~mask. The same work as the 0x004b5300 binding above, on a->geometry->flags.
void MashedRw_AtomicGeometryFlags_004b52f0(int atomic, int mask, int set) {
    unsigned d[2] = {static_cast<unsigned>(mask), static_cast<unsigned>(set)};
    MashedRw_AtomicGeometryFlags_004b5300(FromInt<void>(atomic), d);
}

// rva 0x004c13e0 RwFrameScale (re/match/s19/004c13e0.c, BYTE-IDENTICAL): 0x004c5010 on the frame matrix (+0x10) with
// (v, combine), then the dirty-list link of the root, returns the frame. librw: rw::Frame::scale. RW and librw number
// the combine ops the same way (0 replace, 1 preconcat, 2 postconcat).
int MashedRw_FrameScale_004c13e0(int frame, const void* v, int combine) {
    rw::Frame* const f = FromInt<rw::Frame>(frame);
    if (f && v) f->scale(static_cast<const rw::V3d*>(v), static_cast<rw::CombineOp>(combine));
    BindLog("0x004c13e0(frame %p, combine %d) -> rw::Frame::scale", static_cast<void*>(f), combine);
    return frame;
}

// rva 0x004c0ed0 RwFrameGetLTM (re/match/s19/004c0ed0.c, BYTE-IDENTICAL): syncs the hierarchy when the root is dirty
// (0x004d8350), returns frame +0x50 (the LTM). librw: rw::Frame::getLTM. rw::Matrix has RW's 0x40-byte layout.
void* MashedRw_FrameGetLTM_004c0ed0(int frame) {
    rw::Frame* const f = FromInt<rw::Frame>(frame);
    static_assert(sizeof(rw::Matrix) == 0x40, "RwMatrix is 0x40 bytes");
    return f ? static_cast<void*>(f->getLTM()) : nullptr;
}

}  // extern "C"

// ---------------------------------------------------------------------------------------------------------------
// [RWG-RASTER-BIND] RwImageSetFromRaster (rva 0x004d52d0) and RwGlobals +0x60 = stdFunc[rwSTANDARDIMAGEGETRASTER]
// (original 0x004de1b0) on rw::Raster. Identity of 0x004d52d0 (verify/portorigtop_20261109/RESULT_RWG-RASTER-BIND.md):
// its 51 bytes (re/match/s21/004d52d0.c, BYTE-IDENTICAL) call RwGlobals +0x60 with (image, raster, 0), which is
// stdFunc[6] = rwSTANDARDIMAGEGETRASTER "Get image from a raster" (rwplcore.h:5693, stdFunc at +0x48 after the 0x38-byte
// RwDevice at +0x10, rwplcore.h:6289-6301 / 5834-5855; RwStandardFunc(pOut, pInOut, nI) rwplcore.h:5743), then on success
// tests raster +0x22 bit 0 = RwRaster.privateFlags rwRASTERGAMMACORRECTED (rwcore.h:1622 / 1588) and ORs image +0x0
// with 2 = RwImage.flags rwIMAGEGAMMACORRECTED (rwcore.h:2263 / 2244), returning the image or 0: the contract of
// RwImage *RwImageSetFromRaster(RwImage *, RwRaster *) (rwcore.h:4141). Its caller 0x00498810 is RW's WriteRaster
// sequence (gta-reversed common.cpp:107-115): create(raster w +0xc, h +0x10, 32), allocate pixels, this, write, destroy.
// The image is the RW 3.x RwImage (the game-code caller 0x00555ff0 reads depth +0xc, stride +0x10, cpPixels +0x14);
// the raster is the knob path's rw::Raster (texture->raster, MashedRw_TextureRead_004c5cb0).
struct MashedRwImageRW {          // RW 3.x RwImage, rwcore.h:2263
    std::int32_t flags, width, height, depth, stride;
    std::uint8_t* cpPixels;
    std::uint8_t* palette;
};
static_assert(offsetof(MashedRwImageRW, depth) == 0x0c && offsetof(MashedRwImageRW, stride) == 0x10 &&
              offsetof(MashedRwImageRW, cpPixels) == 0x14, "RwImage layout");

extern "C" {

// RwGlobals +0x60 (original 0x004de1b0, the D3D9 driver's image-from-raster). pOut = RW-layout image with pixels,
// pInOut = rw::Raster. librw: rw::Raster::toImage (driver rasterToImage) then convertTo32, copied row by row into the
// image's pixels. Only 32-bit images are filled: both callers (0x00498810, 0x00555ff0) create the image at depth 0x20.
// [UNCERTAIN] (U-9803) the 4 / 8-bit image branches of 0x004de1b0 (re/match/rwl01/004de1b0.c, 56.8%) are not reproduced; a
// non-32-bit image returns 0 here. Evidence missing: a knob-path caller with another depth (none exists).
int __cdecl MashedRw_ImageGetRaster_004de1b0(void* out, void* inOut, int nI) {
    MashedRwImageRW* const img = static_cast<MashedRwImageRW*>(out);
    rw::Raster* const ras = static_cast<rw::Raster*>(inOut);
    (void)nI;
    if (!img || !ras || !img->cpPixels || img->depth != 32) return 0;
    rw::Image* const tmp = ras->toImage();
    if (!tmp) return 0;
    if (tmp->depth != 32) tmp->convertTo32();
    const int w = tmp->width < img->width ? tmp->width : img->width;
    const int h = tmp->height < img->height ? tmp->height : img->height;
    for (int y = 0; y < h; ++y)
        std::memcpy(img->cpPixels + y * img->stride, tmp->pixels + y * tmp->stride, static_cast<std::size_t>(w) * 4u);
    tmp->destroy();
    return 1;
}

// rva 0x004d52d0 RwImageSetFromRaster. Same steps as the matched body, with the raster read as rw::Raster: the
// RwGlobals +0x60 call through the live record at 0x007d3ff8, then rw::Raster::privateFlags bit 0 (librw sets none of
// its raster privateFlags bits, so a knob-path raster never carries rwRASTERGAMMACORRECTED).
void* __cdecl MashedRw_ImageSetFromRaster_004d52d0(void* image, void* raster) {
    using StdFn = int(__cdecl*)(void*, void*, int);
    const std::uint8_t* const g = *reinterpret_cast<std::uint8_t* const*>(static_cast<std::uintptr_t>(0x007d3ff8u));
    StdFn fn = nullptr;
    if (g) std::memcpy(&fn, g + 0x60, sizeof fn);
    if (!fn || !fn(image, raster, 0)) return nullptr;
    if (static_cast<rw::Raster*>(raster)->privateFlags & 1) static_cast<MashedRwImageRW*>(image)->flags |= 2;
    return image;
}

}  // extern "C"

namespace {
// MASHED_DBG_RASTERBIND_SELFTEST=1 (default off, evidence only): a 4 x 3 C8888 rw::Raster set from a known RGBA image,
// read back through MashedRw_ImageSetFromRaster_004d52d0 into an RW-layout 32-bit image; then the same with
// privateFlags bit 0 set. Logs the byte compare and the image flags. No live caller of 0x004d52d0 exists yet.
void RasterBindSelfTest() {
    if (!std::getenv("MASHED_DBG_RASTERBIND_SELFTEST") || !rw::engine) return;
    const int W = 4, H = 3;
    rw::Image* const src = rw::Image::create(W, H, 32);
    src->allocate();
    for (int i = 0; i < W * H * 4; ++i) src->pixels[i] = static_cast<std::uint8_t>(0x11 + i * 7);
    for (int i = 3; i < W * H * 4; i += 4) src->pixels[i] = 0xff;   // opaque, so no alpha path differs
    rw::Raster* const ras = rw::Raster::create(W, H, 32, rw::Raster::C8888 | rw::Raster::TEXTURE);
    if (!ras) { src->destroy(); BindLog("[RWG-RASTER-BIND] self-test: raster create failed"); return; }
    ras->setFromImage(src);
    std::uint8_t px[W * H * 4] = {};
    MashedRwImageRW img = {0, W, H, 32, W * 4, px, nullptr};
    void* const r1 = MashedRw_ImageSetFromRaster_004d52d0(&img, ras);
    int diff = 0;
    for (int i = 0; i < W * H * 4; ++i) diff += px[i] != src->pixels[i];
    const std::int32_t f1 = img.flags;
    ras->privateFlags |= 1;
    img.flags = 0;
    void* const r2 = MashedRw_ImageSetFromRaster_004d52d0(&img, ras);
    BindLog("[RWG-RASTER-BIND] self-test 0x004d52d0 on rw::Raster %dx%d C8888: ret %s, %d of %d bytes differ "
            "(first px %02x%02x%02x%02x), flags %d; privateFlags|=1: ret %s, flags %d",
            W, H, r1 == &img ? "image" : "NULL", diff, W * H * 4, px[0], px[1], px[2], px[3], f1,
            r2 == &img ? "image" : "NULL", img.flags);
    ras->destroy();
    src->destroy();
}
}  // namespace

// ---------------------------------------------------------------------------------------------------------------
// [PORT-TOP-04D] RwGlobals (RwEngineInstance) for the MASHED_ORIG_TOP path. Matched game bodies call the RW memory
// functions through the pointer at 0x007d3ff8: free at +0x10c (0x004114c0, 0x00484130; re/analysis/boot_app_init_d3/
// 0x004114c0.md "free at DAT_007d3ff8+0x10c"), alloc at +0x108 (re/analysis/bucket_004ddfb0/0x004e8750.md), realloc
// at +0x110 (re/analysis/bucket_004c4270/0x004ce2d0.md "+0x110 (RwRealloc)"). Without this, 0x007d3ff8 holds the B15 Im2D
// fake device (D3d9Render/RwIm2DBridge.cpp, 0x34 bytes) and +0x10c lies past it (PORT-TOP-04C knob_on_1: 0xC0000005).
//
// The record keeps the RW 3.x layout the frontend's Im2D draws read (+0x18 near-screen Z, +0x20 render-state set,
// +0x30 Im2D primitive: the same three fields the B15 device fills, copied from it) and puts librw's memory functions
// (rw::Engine::memfuncs) at +0x108..+0x114. The free call is not routed through the B15 device.
namespace {
struct MashedRwGlobals {
    std::uint8_t dev[0x34];          // +0x00..+0x33: copied from the B15 device (+0x18 / +0x20 / +0x30 used)
    std::uint8_t pad34[0xcc - 0x34];
    void* rwstrcpy;                  // +0xcc  [PORT-TOP-04G] strcpy(dst, src): re/match/s06/0042a470.c, 0042a6b0.c
    void* padd0;                     // +0xd0
    void* rwstrcat;                  // +0xd4  [PORT-TOP-04G] strcat(dst, src): re/match/s06/0042a470.c, 0042a6b0.c
    std::uint8_t padd8[0x108 - 0xd8];
    void* rwmalloc;                  // +0x108
    void* rwfree;                    // +0x10c
    void* rwrealloc;                 // +0x110
    void* rwcalloc;                  // +0x114
    // [PORT-OT-RWG-MODULES] below: zero unless MASHED_ORIG_TOP_RWMOD=1 (RegisterRwModules). MASHED.exe values from the
    // original-side probe (verify/portorigtop_20261109/ORIG_RWG.json, RESULT_PORT-OT-RWG-MODULES.md).
    void* flalloc;                   // +0x118 freelist alloc, original 0x004cca80
    void* flfree;                    // +0x11c freelist free, original 0x004ccba0
    std::uint8_t mod[0x440 - 0x120]; // +0x120..+0x43f: the module data at +0x134 / +0x14c / +0x170 / +0x3f4
};
static_assert(offsetof(MashedRwGlobals, rwmalloc) == 0x108, "RwGlobals malloc @ +0x108");
static_assert(offsetof(MashedRwGlobals, rwfree) == 0x10c, "RwGlobals free @ +0x10c");
static_assert(offsetof(MashedRwGlobals, rwrealloc) == 0x110, "RwGlobals realloc @ +0x110");
static_assert(offsetof(MashedRwGlobals, rwstrcpy) == 0xcc, "RwGlobals strcpy @ +0xcc");
static_assert(offsetof(MashedRwGlobals, rwstrcat) == 0xd4, "RwGlobals strcat @ +0xd4");
static_assert(offsetof(MashedRwGlobals, flalloc) == 0x118, "RwGlobals freelist alloc @ +0x118");
static_assert(offsetof(MashedRwGlobals, flfree) == 0x11c, "RwGlobals freelist free @ +0x11c");
static_assert(sizeof(MashedRwGlobals) == 0x440, "RwGlobals record covers +0x3f4 + 0x40 and +0x170 + 0x220");
MashedRwGlobals s_rwGlobals;

char* __cdecl RwGlobalsStrcpy(char* d, const char* s) { return std::strcpy(d, s); }
char* __cdecl RwGlobalsStrcat(char* d, const char* s) { return std::strcat(d, s); }

void* __cdecl RwGlobalsMalloc(std::size_t n, unsigned hint) { return rw::Engine::memfuncs.rwmalloc(n, hint); }
void __cdecl RwGlobalsFree(void* p) {
    BindLog("RwGlobals +0x10c free(%p) -> rw::Engine::memfuncs.rwfree", p);
    if (p) rw::Engine::memfuncs.rwfree(p);
}
void* __cdecl RwGlobalsRealloc(void* p, std::size_t n, unsigned hint) { return rw::Engine::memfuncs.rwrealloc(p, n, hint); }
void* __cdecl RwGlobalsCalloc(std::size_t n, std::size_t sz, unsigned hint) {
    void* const p = rw::Engine::memfuncs.rwmalloc(n * sz, hint);
    if (p) std::memset(p, 0, n * sz);
    return p;
}

// ---------------------------------------------------------------------------------------------------------------
// [PORT-OT-RWG-MODULES] MASHED_ORIG_TOP_RWMOD=1 (default OFF; verify/portorigtop_20261109/PREREG_PORT-OT-RWG-MODULES.md).
// Read only here, and this file's installer runs only on the MASHED_ORIG_TOP path, so the default path never sees it.
// ON: the four RW module-offset globals hold MASHED.exe's values and the record carries each module's data, so the
// matched module readers (0x004c3b30 ... 0x004ce8e0) and the exe's own readers of those globals (Math/RwLutGuard.h,
// Collision/RwpSolverMath2.cpp, Render/TextureLoaderCluster.cpp, ...) find bound slots instead of offset 0.
bool RwModKnob() {
    static const bool s = [] {
        const char* const e = std::getenv("MASHED_ORIG_TOP_RWMOD");
        return e && e[0] == '1';
    }();
    return s;
}

// Freelist header, as 0x004cca80 / 0x004ccba0 read it: +0x0 entry size (imul at 0x004ccaf1, div at 0x004ccbde),
// +0x4 entries per block (0x004ccaa8, 0x004ccbc1), +0x8 bitmap bytes (0x004cca8e, 0x004ccbab), +0xc alignment
// (0x004ccaf5..0x004ccb06: entries are rounded up to it), +0x10 the block list head (0x004cca8b, 0x004ccba8), +0x18
// flags (0x004ccc03: bit 2 releases an empty block). The four headers the modules point at live at their original
// addresses (the exe maps the original data range).
struct FlEntry { void* raw; void* fl; };
std::mutex s_flMu;
std::unordered_map<void*, FlEntry> s_flLive;

// RwGlobals +0x118 = 0x004cca80 (freelist alloc, cdecl (freeList, hint) -> entry or 0). Binding: one allocation per
// entry through the RwGlobals +0x108 memory function (the one 0x004cca80 allocates its blocks with, 0x004ccb48), entry
// size and alignment from the header, no block pool. The entry is remembered with its freelist for +0x11c.
void* __cdecl RwGlobalsFreeListAlloc(void* fl, unsigned hint) {
    if (!fl) return nullptr;
    const std::uint32_t* const h = static_cast<const std::uint32_t*>(fl);
    const std::uint32_t size = h[0];
    const std::uint32_t align = h[3] ? h[3] : 1u;
    std::uint8_t* const raw = static_cast<std::uint8_t*>(rw::Engine::memfuncs.rwmalloc(size + align - 1u, hint));
    if (!raw) return nullptr;
    void* const p = reinterpret_cast<void*>((reinterpret_cast<std::uintptr_t>(raw) + align - 1u) &
                                            ~static_cast<std::uintptr_t>(align - 1u));
    std::lock_guard<std::mutex> lk(s_flMu);
    s_flLive[p] = FlEntry{raw, fl};
    return p;
}
// RwGlobals +0x11c = 0x004ccba0 (freelist free, cdecl (freeList, entry); re/match/rwl01/004ccba0.c): an entry outside
// every block of that freelist returns 0 and frees nothing (0x004ccbd3..0x004ccbd9), else the entry is released and
// the freelist is returned (0x004ccc42 mov eax, edi). Binding: the same two results over the entries +0x118 handed out.
void* __cdecl RwGlobalsFreeListFree(void* fl, void* p) {
    void* raw = nullptr;
    {
        std::lock_guard<std::mutex> lk(s_flMu);
        const auto it = s_flLive.find(p);
        if (it == s_flLive.end() || it->second.fl != fl) return nullptr;
        raw = it->second.raw;
        s_flLive.erase(it);
    }
    rw::Engine::memfuncs.rwfree(raw);
    return fl;
}
}  // namespace

// Exe bodies the vector and matrix modules bind (Math/RwV3dTransformPointsCPU.cpp, Math/RwMatrixMultiplyCpu.cpp).
extern "C" void* __cdecl RwV3dTransformPointsCPU_C(void* dst, const void* src, int count, const void* m);
extern "C" void* __cdecl RwV3dTransformVectorsCPU_C(void* dst, const void* src, int count, const void* m);
extern "C" void __cdecl RwMatrixMultiplyCPU_C(void* out, const void* A, const void* B);

namespace {
// Vector module +0x8 = 0x004dc4a0: the body of 0x004dc370 (+0xc) for one point, same SSE order (0x004dc4fa..0x004dc50c
// against 0x004dc3d2..0x004dc3e7), cdecl (out, in, matrix), returns out (0x004dc524).
void* __cdecl RwModPointSingle(void* out, const void* in, const void* m) {
    RwV3dTransformPointsCPU_C(out, in, 1, m);
    return out;
}
// Vector module +0x10 = 0x004dc530: the body of 0x004dc410 (+0x14) for one vector, same order (0x004dc57e..0x004dc58d
// against 0x004dc466..0x004dc478), cdecl (out, in, matrix), returns out (0x004dc5a5).
void* __cdecl RwModVectorSingle(void* out, const void* in, const void* m) {
    RwV3dTransformVectorsCPU_C(out, in, 1, m);
    return out;
}

// [PORT-OT-RWG-STRINGS] the RwGlobals string slots the ORIG_TOP rows call (same knob). MASHED.exe values from the
// original-side probe (RwGlobals block, verify/portorigtop_20261109/RESULT_PORT-OT-RWG-STRINGS.md); each binding keeps the
// original's arguments and result.
// +0xc4 = 0x004a2b60 (hooks.csv: libid identical to libcmt _sprintf). Call: re/match/s02/004113b0.c "%sAll%d.rep".
int __cdecl RwGlobalsSprintf(char* d, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsprintf(d, fmt, ap);
    va_end(ap);
    return n;
}
// +0xd0 = 0x004a34b0 (hooks.csv _strncpy, CRT). Call: re/match/s20/004ce790.c (dst, src, 0x12).
char* __cdecl RwGlobalsStrncpy(char* d, const char* s, std::size_t n) { return std::strncpy(d, s, n); }
// +0xe8 = 0x004af340 (msvc-crt): bytes compared unsigned until the first difference or a common NUL; 0 when equal
// (0x004af380), else sbb / shl 1 / add 1 = -1 when the first string's byte is below the second's, +1 above
// (0x004af384..0x004af38b).
int __cdecl RwGlobalsStrcmp(const char* a, const char* b) {
    const unsigned char* x = reinterpret_cast<const unsigned char*>(a);
    const unsigned char* y = reinterpret_cast<const unsigned char*>(b);
    for (;; ++x, ++y) {
        if (*x != *y) return *x < *y ? -1 : 1;
        if (*x == 0) return 0;
    }
}
// +0xf4 = 0x004a9410 (hooks.csv _strlen, CRT).
std::size_t __cdecl RwGlobalsStrlen(const char* s) { return std::strlen(s); }
}  // namespace

// Render/RwStrCase.cpp, Render/RwStrSearch.cpp: the exe bodies of 0x004d86d0 / 0x004d8700 / 0x004d8750 (hooks.csv C4).
extern "C" void __cdecl RwStrupr(char* s);
extern "C" void __cdecl RwStrlwr(char* s);
extern "C" char* __cdecl RwStrrchr(char* s, char c);
// [PORT-OT-IMAGE-FMT] Match/PortTop_00493600.cpp: runs the matched 0x00493600 once if 0x00493710 reached it (-1 if not).
extern "C" int PortTop_RunPending_00493600(void);
// [PORT-OT-RWG-DEVSLOTS] Render/D3d9StateCache.cpp: the exe body of 0x004d6910 (hooks.csv D3d9State_Get, C3).
// D3d9Render/RwIm2DBridge.cpp: the B15 bridge's indexed Im2D draw (from the disassembly of 0x004dbc90).
extern "C" std::uint32_t __cdecl D3d9State_Get(std::uint32_t state_enum, std::uint32_t* out);
extern "C" int __cdecl RwIm2DBridge_RenderIndexedPrimitive(int primType, void* verts, int numVerts, const void* indices,
                                                           int numIndices);

namespace {
// +0xf8 = 0x004d86d0 / +0xfc = 0x004d8700: eax is loaded with s at entry (0x004d86d0 `mov eax,[esp+4]`) and kept, so the
// original returns s; the exe bodies return void, so these wrappers return s.
char* __cdecl RwGlobalsStrupr(char* s) { RwStrupr(s); return s; }
char* __cdecl RwGlobalsStrlwr(char* s) { RwStrlwr(s); return s; }
// +0xdc = 0x004d8750 (RwStrrchr). Call: re/match/s20/004ce8e0.c (name, 0x2e).
char* __cdecl RwGlobalsStrrchr(char* s, int c) { return RwStrrchr(s, static_cast<char>(c)); }

// MASHED.exe values (original-side probe, ORIG_RWG.json "mods"): *(0x007d3ffc) = 0x134, *(0x007d4028) = 0x14c,
// *(0x007d4054) = 0x3f4, *(0x007d4628) = 0x170.
constexpr std::uint32_t kModVector = 0x134, kModMatrix = 0x14c, kModTexture = 0x3f4, kModImage = 0x170;

struct FlHeaderInit { std::uint32_t addr, entrySize, perBlock, bitmapBytes, align, flags; };
// The four freelist headers, words +0x0 / +0x4 / +0x8 / +0xc / +0x18 as MASHED.exe holds them (original-side probe
// "dumps", RESULT_PORT-OT-RWG-MODULES.md). The block list (+0x10 / +0x14) starts empty (self-linked).
const FlHeaderInit kFlHeaders[] = {
    // addr, +0x0 entry size, +0x4 entries per block, +0x8 bitmap bytes, +0xc alignment, +0x18 flags; read at 3.02 .. 33.02 s.
    {0x007d4004u, 0x40u, 0x100u, 0x20u, 4u, 3u},   // matrix module +0x0 (RwMatrix is 0x40 bytes, re/match/s19/004c4600.c)
    {0x007d4030u, 0x18u, 0x5u, 0x1u, 4u, 3u},      // texture module +0xc (RwTexDictionary 0x18, re/match/s19/004c5890.c)
    {0x007d405cu, 0x60u, 0x80u, 0x10u, 4u, 3u},    // texture module +0x8
    {0x007d4604u, 0x34u, 0x80u, 0x10u, 4u, 3u},    // image module +0x218 (ImgFmt 0x34, re/match/s20/004ce790.c)
};

void Put(std::uint32_t off, std::uint32_t v) {
    std::memcpy(reinterpret_cast<std::uint8_t*>(&s_rwGlobals) + off, &v, sizeof v);
}
std::uint32_t U(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }

// [HUDINIT-A] the record's own slots under MASHED_ORIG_TOP_RWMOD=1: the string functions (+0xc4..+0xfc) and the
// freelist alloc / free (+0x118 / +0x11c). They are fields of this record, not module offsets, so they are set at every
// install, the first one included (before the matched 0x00402750). 0x00402750 calls 0x00427ca0 at 0x00402750.c:137,
// before piece 4 (:159), and 0x00427ca0's first call, the Rt2d open 0x00552b60, allocates through +0x118 and +0xf4.
// The module offsets and headers (RegisterRwModules) still wait for the B15 device.
void RegisterRwEarlySlots() {
    // [PORT-OT-RWG-STRINGS] string slots: original +0xc4 0x004a2b60, +0xd0 0x004a34b0, +0xdc 0x004d8750, +0xe8 0x004af340,
    // +0xf4 0x004a9410, +0xf8 0x004d86d0, +0xfc 0x004d8700. +0xc8 / +0xd8 / +0xe0 / +0xe4 / +0xec / +0xf0 / +0x100 / +0x104
    // (original 0x004a42c5 / 0x005c2300 / 0x004d8730 / 0x004a3860 / 0x004a3a90 / 0x004d8680 / 0x004a3ac9 / 0x005c22bd)
    // stay 0: no ORIG_TOP row calls them (ot_measure.py lists).
    Put(0xc4, U(reinterpret_cast<void*>(&RwGlobalsSprintf)));
    Put(0xd0, U(reinterpret_cast<void*>(&RwGlobalsStrncpy)));
    Put(0xdc, U(reinterpret_cast<void*>(&RwGlobalsStrrchr)));
    Put(0xe8, U(reinterpret_cast<void*>(&RwGlobalsStrcmp)));
    Put(0xf4, U(reinterpret_cast<void*>(&RwGlobalsStrlen)));
    Put(0xf8, U(reinterpret_cast<void*>(&RwGlobalsStrupr)));
    Put(0xfc, U(reinterpret_cast<void*>(&RwGlobalsStrlwr)));
    // RwGlobals +0x118 / +0x11c: original 0x004cca80 / 0x004ccba0.
    s_rwGlobals.flalloc = reinterpret_cast<void*>(&RwGlobalsFreeListAlloc);
    s_rwGlobals.flfree = reinterpret_cast<void*>(&RwGlobalsFreeListFree);
}

void RegisterRwModules() {
    RegisterRwEarlySlots();
    // [PORT-OT-RWG-DEVSLOTS] device slots. Original (ORIG_RWG.json): +0x24 0x004d6910, +0x34 0x004dbc90. +0x20 / +0x30
    // (0x004d7480 / 0x004dba60 in the original) already hold the B15 bridge's set-state / draw, copied from the device.
    // +0x24 is the exe body of 0x004d6910: it reads the exe's bytes at the input caches 0x007d6ae8..0x007d6bf8 /
    // 0x007d58b8, which the bridge's set-state (+0x20) does not write. The two ORIG_TOP readers (0x00413a40: states 0x14
    // and 0xe) only hand the value back to +0x20 afterwards, and the bridge ignores both states.
    // +0x34 is the bridge's indexed draw (0x00413a40, 0x004430e0: primType 3).
    // [RWG-RASTER-BIND] +0x60 = stdFunc[rwSTANDARDIMAGEGETRASTER] (original 0x004de1b0) on rw::Raster. Its one row,
    // 0x004d52d0 (RwImageSetFromRaster), is bound too (MashedRw_ImageSetFromRaster_004d52d0): the matched body would read
    // the rw::Raster at RW 3.x +0x22, inside librw's `privateFlags` int (+0xc..+0x22 differ).
    Put(0x24, U(reinterpret_cast<void*>(&D3d9State_Get)));
    Put(0x34, U(reinterpret_cast<void*>(&RwIm2DBridge_RenderIndexedPrimitive)));
    Put(0x60, U(reinterpret_cast<void*>(&MashedRw_ImageGetRaster_004de1b0)));
    RasterBindSelfTest();
    // Vector module (+0x134). Original: +0x0 0x0cc00048 / +0x4 0x0cc04050 (heap: the sqrt / inv-sqrt LUT roots,
    // Math/RwLutGuard.h), +0x8 0x004dc4a0, +0xc 0x004dc370, +0x10 0x004dc530, +0x14 0x004dc410.
    Put(kModVector + 0x0, U(mashed_re::Math::RwLutSqrtTable()));     // == the original's table (Math/RwLutTables.cpp)
    Put(kModVector + 0x4, U(mashed_re::Math::RwLutInvSqrtTable()));
    Put(kModVector + 0x8, U(reinterpret_cast<void*>(&RwModPointSingle)));
    Put(kModVector + 0xc, U(reinterpret_cast<void*>(&RwV3dTransformPointsCPU_C)));
    Put(kModVector + 0x10, U(reinterpret_cast<void*>(&RwModVectorSingle)));
    Put(kModVector + 0x14, U(reinterpret_cast<void*>(&RwV3dTransformVectorsCPU_C)));
    // Matrix module (+0x14c = vector module +0x18). Original: +0x0 0x007d4004 (freelist header), +0x4 0x00020000,
    // +0x8 0x005cb2a0 (RwDeviceMatrixMultiply, exe body RwMatrixMultiplyCPU_C), +0xc / +0x10 / +0x14 0x3c23d70a.
    // +0x18 0x007d3e78 / +0x1c 0x007d45a0 / +0x20 0x007d3e9c stay 0: no reader of them on the knob path found.
    Put(kModMatrix + 0x0, 0x007d4004u);
    Put(kModMatrix + 0x4, 0x00020000u);
    Put(kModMatrix + 0x8, U(reinterpret_cast<void*>(&RwMatrixMultiplyCPU_C)));
    Put(kModMatrix + 0xc, 0x3c23d70au);
    Put(kModMatrix + 0x10, 0x3c23d70au);
    Put(kModMatrix + 0x14, 0x3c23d70au);
    // Image module (+0x170). Original: +0x218 0x007d4604 (freelist header, re/match/s20/004ce790.c Mod.freeList),
    // +0x21c 0x0cc0d5ec (heap: the registered image formats, Mod.formats). The list starts empty here and the
    // [PORT-OT-IMAGE-FMT] deferred 0x00493600 fills it at the end of this function (bmp, then png at the head).
    // +0x0..+0x217 (original: +0x0 0x007d4630, +0x4 heap, +0x8 0x100, then bytes 00 01 02 ...) stay 0: no reader found.
    Put(kModImage + 0x218, 0x007d4604u);
    Put(kModImage + 0x21c, 0u);
    // Texture module (+0x3f4). Original: +0x0 / +0x4 heap (the dictionary list, re/match/s19/004c5890.c TG.texDictList,
    // holding the original's dictionaries), +0x8 0x007d405c and +0xc 0x007d4030 (freelist headers), +0x1c 0 (the current
    // dictionary, Render/TextureLoaderCluster.cpp). The exe's dictionaries are librw's, so the list starts empty
    // (self-linked, as rwLinkListInitialize leaves it). +0x14 0x004c61b0 / +0x18 0x004c7490 / +0x2c 0x004c7200 /
    // +0x30 0x004c6140 are original code addresses with no exe body: left 0, no reader on the knob path found.
    const std::uint32_t head = U(reinterpret_cast<std::uint8_t*>(&s_rwGlobals) + kModTexture);
    Put(kModTexture + 0x0, head);
    Put(kModTexture + 0x4, head);
    Put(kModTexture + 0x8, 0x007d405cu);
    Put(kModTexture + 0xc, 0x007d4030u);
    Put(kModTexture + 0x1c, 0u);
    for (const FlHeaderInit& f : kFlHeaders) {
        std::uint32_t* const h = reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(f.addr));
        h[0] = f.entrySize;
        h[1] = f.perBlock;
        h[2] = f.bitmapBytes;
        h[3] = f.align;
        h[4] = h[5] = f.addr + 0x10u;   // empty block list
        h[6] = f.flags;
    }
    *reinterpret_cast<volatile std::uint32_t*>(static_cast<std::uintptr_t>(0x007d3ffcu)) = kModVector;
    *reinterpret_cast<volatile std::uint32_t*>(static_cast<std::uintptr_t>(0x007d4028u)) = kModMatrix;
    *reinterpret_cast<volatile std::uint32_t*>(static_cast<std::uintptr_t>(0x007d4054u)) = kModTexture;
    *reinterpret_cast<volatile std::uint32_t*>(static_cast<std::uintptr_t>(0x007d4628u)) = kModImage;
    // [PORT-OT-IMAGE-FMT] 0x00493710 reached 0x00493600 before this record existed (Match/PortTop_00493600.cpp). It runs
    // now, on the slots, freelist header and module offset set above: 0x004ce790 x2 ("bmp", "png").
    const int fmt = PortTop_RunPending_00493600();
    BindLog("RwGlobals [MASHED_ORIG_TOP_RWMOD]: image formats via 0x00493600 %s (%d), image +0x21c = 0x%08X",
            fmt < 0 ? "not reached" : "ran", fmt,
            *reinterpret_cast<const std::uint32_t*>(reinterpret_cast<const std::uint8_t*>(&s_rwGlobals) + kModImage + 0x21c));
    // The nodes as re/match/s20/004ce790.c lays them out: +0x0 lower-case ".ext", +0x14 upper-case, +0x28 read,
    // +0x2c write, +0x30 next.
    for (const std::uint8_t* n = *reinterpret_cast<std::uint8_t* const*>(reinterpret_cast<const std::uint8_t*>(&s_rwGlobals) +
                                                                       kModImage + 0x21c);
         n; n = *reinterpret_cast<std::uint8_t* const*>(n + 0x30)) {
        BindLog("  image format node %p: \"%.19s\" / \"%.19s\" read %p write %p next %p", static_cast<const void*>(n),
                reinterpret_cast<const char*>(n), reinterpret_cast<const char*>(n + 0x14),
                *reinterpret_cast<void* const*>(n + 0x28), *reinterpret_cast<void* const*>(n + 0x2c),
                *reinterpret_cast<void* const*>(n + 0x30));
    }
    BindLog("RwGlobals [MASHED_ORIG_TOP_RWMOD]: modules registered (0x007d3ffc 0x134, 0x007d4028 0x14c, 0x007d4054 0x3f4, "
            "0x007d4628 0x170), freelist alloc / free at +0x118 / +0x11c, string slots +0xc4 +0xd0 +0xdc +0xe8 +0xf4 +0xf8 "
            "+0xfc, device slots +0x24 +0x34, record %p size 0x%X",
            static_cast<void*>(&s_rwGlobals), static_cast<unsigned>(sizeof(s_rwGlobals)));
}
}  // namespace

extern "C" {
// Called on the knob path (Match/PortTop_00492370.cpp, Match/PortTop_00402750_Pieces.cpp). [PORT-TOP-04G] it runs three
// times: before the matched 0x00402750 (0x007d3ff8 is 0 then: the record goes in with its Im2D fields zero, which no
// body reads before piece 4 draws), after piece 4 (0x00403640, which installs the B15 bridge and so points 0x007d3ff8 at
// the B15 device again: the device's fields are copied and the record goes back), and after 0x00402750 returns (the
// record is already there: nothing changes). The matched loaders between these points call the string functions at
// +0xcc / +0xd4 (0x0042a6b0 inside 0x0040bbb0, which runs before piece 4). Returns 1.
// [PORT-OT-IMAGE-FMT] the knob, for Match/PortTop_00493600.cpp (1 = MASHED_ORIG_TOP_RWMOD=1).
int MashedRw_RwModKnob(void) { return RwModKnob() ? 1 : 0; }

int MashedRw_InstallRwGlobals(void) {
    void** const slot = reinterpret_cast<void**>(static_cast<std::uintptr_t>(0x007d3ff8u));
    void* const dev = *slot;
    if (dev == static_cast<void*>(&s_rwGlobals)) {
        BindLog("RwGlobals: 0x007d3ff8 already points at the record %p", static_cast<void*>(&s_rwGlobals));
        return 1;
    }
    if (dev) std::memcpy(s_rwGlobals.dev, dev, sizeof(s_rwGlobals.dev));
    s_rwGlobals.rwstrcpy = reinterpret_cast<void*>(&RwGlobalsStrcpy);
    s_rwGlobals.rwstrcat = reinterpret_cast<void*>(&RwGlobalsStrcat);
    s_rwGlobals.rwmalloc = reinterpret_cast<void*>(&RwGlobalsMalloc);
    s_rwGlobals.rwfree = reinterpret_cast<void*>(&RwGlobalsFree);
    s_rwGlobals.rwrealloc = reinterpret_cast<void*>(&RwGlobalsRealloc);
    s_rwGlobals.rwcalloc = reinterpret_cast<void*>(&RwGlobalsCalloc);
    *slot = &s_rwGlobals;
    if (RwModKnob()) RegisterRwEarlySlots();   // [HUDINIT-A] record slots from the first install on
    // [PORT-OT-RWG-MODULES] only once the B15 device has been seen (the install after piece 4): before that, piece 4
    // points 0x007d3ff8 at the 0x34-byte B15 device for a while, and registered offsets would index past it.
    if (dev && RwModKnob()) RegisterRwModules();
    if (dev)
        BindLog("RwGlobals: 0x007d3ff8 %p (B15 device) -> record %p; Im2D +0x18/+0x20/+0x30 copied, strcpy / strcat at "
                "+0xcc / +0xd4, memory functions at +0x108..+0x114 on librw", dev, static_cast<void*>(&s_rwGlobals));
    else
        BindLog("RwGlobals: 0x007d3ff8 is 0 (no B15 device yet) -> record %p with Im2D fields zero; strcpy / strcat at "
                "+0xcc / +0xd4, memory functions at +0x108..+0x114 on librw", static_cast<void*>(&s_rwGlobals));
    return 1;
}
}  // extern "C"

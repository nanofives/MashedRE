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

// [PORT-TOP-04F] whole-file read from the piz mount stack, else the disk (Match/PortTop_RwStream04F.cpp).
extern "C" void* PortTop_Fs_ReadAll(const char* name, unsigned* len, const char** from);

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
// (rva 0x004c0e50). In the knob path every caller passes a camera (0x00467110, 0x004670a0, 0x0042d420, 0x004c2bf0), so
// this binding takes the camera record: librw rw::Camera::setFrame (librw keeps the link in its own camera).
void MashedRw_ObjectHasFrameSetFrame_004c0740(int object, int frame) {
    MashedRwCamera* const c = AsCamera(FromInt<void>(object));
    if (!c) { BindLog("0x004c0740: object %08X is not a camera record, ignored", static_cast<unsigned>(object)); return; }
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
    if (rw::Texture* const hit = rw::Texture::findCB(name)) {
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

}  // extern "C"

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
    std::uint8_t pad34[0x108 - 0x34];
    void* rwmalloc;                  // +0x108
    void* rwfree;                    // +0x10c
    void* rwrealloc;                 // +0x110
    void* rwcalloc;                  // +0x114
    std::uint8_t tail[0x100];        // zero: nothing on the knob path reads past +0x114 (no reader found)
};
static_assert(offsetof(MashedRwGlobals, rwmalloc) == 0x108, "RwGlobals malloc @ +0x108");
static_assert(offsetof(MashedRwGlobals, rwfree) == 0x10c, "RwGlobals free @ +0x10c");
static_assert(offsetof(MashedRwGlobals, rwrealloc) == 0x110, "RwGlobals realloc @ +0x110");
MashedRwGlobals s_rwGlobals;

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
}  // namespace

extern "C" {
// Called once on the knob path after the hand init (PortTop_StandIn_00402750) has installed the B15 bridge. Returns 1
// when 0x007d3ff8 now points at the record, 0 when there was no device to copy (bridge not installed).
int MashedRw_InstallRwGlobals(void) {
    void** const slot = reinterpret_cast<void**>(static_cast<std::uintptr_t>(0x007d3ff8u));
    void* const dev = *slot;
    if (!dev) {
        BindLog("RwGlobals: 0x007d3ff8 is 0 (no B15 device), record not installed");
        return 0;
    }
    std::memcpy(s_rwGlobals.dev, dev, sizeof(s_rwGlobals.dev));
    s_rwGlobals.rwmalloc = reinterpret_cast<void*>(&RwGlobalsMalloc);
    s_rwGlobals.rwfree = reinterpret_cast<void*>(&RwGlobalsFree);
    s_rwGlobals.rwrealloc = reinterpret_cast<void*>(&RwGlobalsRealloc);
    s_rwGlobals.rwcalloc = reinterpret_cast<void*>(&RwGlobalsCalloc);
    *slot = &s_rwGlobals;
    BindLog("RwGlobals: 0x007d3ff8 %p (B15 device) -> record %p; Im2D +0x18/+0x20/+0x30 copied, memory functions at "
            "+0x108..+0x114 on librw", dev, static_cast<void*>(&s_rwGlobals));
    return 1;
}
}  // extern "C"

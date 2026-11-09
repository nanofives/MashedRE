// PORT-TOP-04E (L-96 top-down lane): the callees the matched 0x00402750 AppInitialiseOnBootup (Match/PortTop_00402750.cpp)
// binds that are not matched exe bodies. EXE ONLY, reached only on the MASHED_ORIG_TOP path. Not bodies of these RVAs.
//
//  - hand pieces: HandTop_InitRest (exe_main.cpp) split at 0x00402750's call points. Each original callee doing a job
//    the hand init does is bound to the one piece doing it; the five pieces run in the hand init's order.
//  - piz file system: 0x004b6540 / 0x004b6560 (file system setup) and the mount / unmount wrappers 0x00495280 /
//    0x004952f0 on the exe's piz reader (Piz/PizReader.h). The mount opens the archive the original opens (path
//    "d:\toastart\..." -> "original/toastart/...") and keeps it on a mount stack; the unmount closes the newest one.
//  - stand-ins: the loaders not moved yet. Each logs its first call and returns 0, which leaves the original global
//    it would write at 0, the hand path's value (0x007f09e4, 0x007f0a30, 0x008aa318 / 0x008aa314, 0x00636ac8).
//  - 0x004caea0: a stand-in device whose SetRenderState slot (vtable +0xe4) records the two calls and does not reach
//    the exe's D3D9 device (librw keeps its own render-state cache for that device).
// Evidence: verify/porttop_20261108/RESULT_PORT-TOP-04E.md.
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>

#include "../Piz/PizReader.h"

namespace {
void Log04E(const char* fmt, ...) {
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
void StandInLog(const char* rva, const char* job, int* once) {
    if (*once) return;
    *once = 1;
    Log04E("rva %s: stand-in, not run (%s) (PORT-TOP-04E)", rva, job);
}
void* Hinst() { return *reinterpret_cast<void* volatile*>(0x007e9580u); }

// ---- piz file system ----
struct Mount {
    char orig[128];
    char path[300];
    std::unique_ptr<mashed_re::Piz::Archive> arc;
};
constexpr int kMaxMounts = 8;
Mount s_mounts[kMaxMounts];
int s_mountTop = 0;
int s_pizMode = -1;
void* s_pizIdle = nullptr;
int s_pizIdleArg = 0;

// "d:\toastart\common\font36.piz" -> "original/toastart/common/font36.piz" (the hand loaders' root, relative to the
// MASHED_ROOT working directory).
void MapPizPath(const char* in, char* out, std::size_t n) {
    const char* p = in;
    if (p[0] && p[1] == ':') p += 2;
    while (*p == '\\' || *p == '/') ++p;
    std::snprintf(out, n, "original/%s", p);
    for (char* c = out; *c; ++c) if (*c == '\\') *c = '/';
}
}  // namespace

extern "C" {
void PortTop_HandInit_DataInput(void* h);
int  PortTop_HandInit_Movies(void* h);
void PortTop_HandInit_Audio(void);
void PortTop_HandInit_Rasters(void);
void PortTop_HandInit_FrontendAssets(void);

// 1 once a piece failed (only piece 2 can: no System32 dsound.dll). Later pieces then do nothing, and the 0x00402750
// wrapper returns 0, as HandTop_InitRest returns false.
int g_PortTop04E_InitFailed = 0;

// ---- hand pieces ----
// rva 0x00496e40 PCVS_Init -> piece 1 (data wedge, boot chain, DirectInput keyboard)
void PortTop_Piece_00496e40(void) {
    if (g_PortTop04E_InitFailed) return;
    PortTop_HandInit_DataInput(Hinst());
    Log04E("0x00496e40 PCVS_Init: piece 1 HandTop_InitDataInput (data wedge, boot chain, DirectInput)");
}
// rva 0x00495350 HardwareShowIntroVideo -> piece 2 (dsound.dll guard, boot movies)
void PortTop_Piece_00495350(void) {
    if (g_PortTop04E_InitFailed) return;
    const int ok = PortTop_HandInit_Movies(Hinst());
    if (!ok) g_PortTop04E_InitFailed = 1;
    Log04E("0x00495350 HardwareShowIntroVideo: piece 2 HandTop_InitMovies returned %d", ok);
}
// rva 0x004669b0 sound engine init hub -> piece 3 (exe audio, permdict.rws bank, menu music)
void PortTop_Piece_004669b0(void) {
    if (g_PortTop04E_InitFailed) return;
    PortTop_HandInit_Audio();
    Log04E("0x004669b0 sound init: piece 3 HandTop_InitAudio");
}
// rva 0x00403640 BackBuffRasterOpen wrapper -> piece 4 (wedge probe, texture upload, B15 bridge, B16 thunks).
// 0x00403640 returns 1 (re/match/s01/raw/00403640.c).
int PortTop_Piece_00403640(int cam) {
    if (g_PortTop04E_InitFailed) return 1;
    PortTop_HandInit_Rasters();
    Log04E("0x00403640(%08X) BackBuffRasterOpen: piece 4 HandTop_InitRasters", static_cast<unsigned>(cam));
    return 1;
}
// rva 0x00428390 FrontendStateSet(3) -> piece 5 (frontend assets, settings, save restore, nav init, fonts)
void PortTop_Piece_00428390(int state) {
    if (g_PortTop04E_InitFailed) return;
    PortTop_HandInit_FrontendAssets();
    Log04E("0x00428390 FrontendStateSet(%d): piece 5 HandTop_InitFrontendAssets", state);
}

// ---- piz file system ----
// rva 0x004b6540 (thunk to 0x004b6640, file system setup with the mode flag 0x00402750 derives from DAT_00773920)
void PortTop_PizFs_Init_004b6540(int flag) {
    s_pizMode = flag;
    Log04E("[PIZ_FS] 0x004b6540 file system setup, mode flag %d (exe piz reader, Piz/PizReader.h)", flag);
}
// rva 0x004b6560 (thunk to 0x004b6610): stores the callback 0x00402750 passes (0x00429290) and its argument. The exe
// reader loads synchronously and has no point to call it, so it is recorded only.
void PortTop_PizFs_SetIdle_004b6560(void* fn, int arg) {
    s_pizIdle = fn;
    s_pizIdleArg = arg;
    Log04E("[PIZ_FS] 0x004b6560 callback %p arg %d recorded (not called by the exe reader)", fn, arg);
}
// rva 0x00495280 piz mount wrapper
void PortTop_PizFs_Mount_00495280(const char* name) {
    if (!name) return;
    if (s_mountTop >= kMaxMounts) {
        Log04E("[PIZ_FS] 0x00495280 mount %s: mount stack full (%d)", name, kMaxMounts);
        return;
    }
    Mount& m = s_mounts[s_mountTop++];
    std::snprintf(m.orig, sizeof(m.orig), "%s", name);
    MapPizPath(name, m.path, sizeof(m.path));
    m.arc.reset(new mashed_re::Piz::Archive());
    const bool ok = m.arc->Load(m.path);
    Log04E("[PIZ_FS] 0x00495280 mount %s -> %s: %s, %u entries (depth %d)", name, m.path,
           ok ? "OK" : m.arc->last_error(), ok ? m.arc->count() : 0u, s_mountTop);
}
// rva 0x004952f0 piz unmount wrapper: closes the newest mount
void PortTop_PizFs_Unmount_004952f0(void) {
    if (s_mountTop <= 0) {
        Log04E("[PIZ_FS] 0x004952f0 unmount: nothing mounted");
        return;
    }
    Mount& m = s_mounts[--s_mountTop];
    Log04E("[PIZ_FS] 0x004952f0 unmount %s (depth %d)", m.orig, s_mountTop);
    m.arc.reset();
    m.orig[0] = 0;
    m.path[0] = 0;
}
// The mounted archive holding <entry> (newest first), for the matched loaders that move off their stand-ins.
const void* PortTop_PizFs_Find(const char* entry, unsigned* len) {
    for (int i = s_mountTop - 1; i >= 0; --i) {
        const mashed_re::Piz::Archive* a = s_mounts[i].arc.get();
        if (!a || !a->valid()) continue;
        for (std::uint32_t k = 0; k < a->count(); ++k) {
            if (_stricmp(a->entry(k).name, entry) == 0) {
                std::uint32_t n = 0;
                const std::uint8_t* b = a->blob(k, &n);
                if (len) *len = n;
                return b;
            }
        }
    }
    if (len) *len = 0;
    return nullptr;
}

// ---- 0x004caea0: stand-in device ----
namespace {
long __stdcall FakeDevSetRenderState(void* self, int state, int value) {
    (void)self;
    Log04E("0x004caea0 device stand-in: SetRenderState(0x%x, %d) recorded, not sent to the D3D9 device", state, value);
    return 0;
}
struct FakeDev { void** vt; };
void* s_fakeVt[58] = {};
FakeDev s_fakeDev = {s_fakeVt};
}  // namespace
// rva 0x004caea0 (no source): 0x00402750 stores its result in DAT_00771a0c and calls SetRenderState through vtable
// slot 57 (+0xe4) twice (0x19 = 5, 0x18 = 0).
void* PortTop_StandIn04E_004caea0(void) {
    s_fakeVt[57] = reinterpret_cast<void*>(&FakeDevSetRenderState);
    Log04E("0x004caea0: stand-in device %p (SetRenderState slot +0xe4 records only)", static_cast<void*>(&s_fakeDev));
    return &s_fakeDev;
}

// ---- stand-ins (loaders not moved yet) ----
#define SI(rva, job) int PortTop_StandIn04E_##rva() { static int once; StandInLog("0x" #rva, job, &once); return 0; }
SI(00429290, "title tick, passed to 0x004b6560 as a callback only")
SI(00494c80, "small.mpg graph setup; piece 2 plays the boot movies")
SI(004283a0, "fonts family")
SI(00427ca0, "fonts family; exe body FontText_HudInit, not run here yet")
SI(004275d0, "fonts family")
SI(004274d0, "fonts family")
SI(004274e0, "fonts family: english.dat")
SI(004113b0, "0x24a3c block alloc")
SI(00558240, "RW raster create, no source")
SI(004841d0, "0x006ce840 table init; 0 skips Rec18Create, DAT_007f0a30 stays 0")
SI(00484170, "Rec18Create, unreached")
SI(0040bbb0, "sprite dictionaries")
SI(004723d0, "0x00691500 table init")
SI(0040bb50, "sprite lookup SemiC / SemiC2; exe body exists, not run here yet")
SI(0040bb30, "sprite lookup lensflare-fs8")
SI(004881d0, "lens flare object create")
SI(0047ba00, "back buffer raster consumer")
SI(0045bae0, "powerups family")
SI(00418980, "powerups family")
SI(0041b450, "Panel family")
SI(0041bec0, "Panel family")
SI(0041cb10, "Panel family; exe body exists, not run here yet")
SI(0041d6e0, "Panel family; exe body exists, not run here yet")
SI(0041def0, "Panel family")
SI(0041d8b0, "Panel family")
SI(0041c100, "Panel family")
SI(0041db90, "Panel family")
SI(0041a1e0, "Panel family")
SI(0041eaa0, "particle emitters")
SI(00420d00, "PanelSortInit")
SI(00412890, "loader")
SI(00471eb0, "loader")
SI(00404830, "car material tables")
SI(00425bc0, "perm family")
SI(004025f0, "perm family")
SI(004b3d80, "pc.txd; DAT_00636ac8 stays 0")
SI(00431b40, "Set67eaa8(0.3f)")
SI(00402f50, "BootDefaultParamsInit; exe body exists, not run here yet")
SI(004026d0, "BootQueueFlush(4); exe body exists, not run here yet")
#undef SI
}  // extern "C"
#endif  // MASHED_STANDALONE

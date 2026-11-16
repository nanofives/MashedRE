// RWC-CORE-RUN (L-124 (C) step 2): the MASHED_RWC_CORE knob. EXE ONLY, read only on the MASHED_ORIG_TOP path.
//
// Default OFF. With MASHED_ORIG_TOP=1 and MASHED_RWC_CORE=1 the RW engine entry points that the matched bring-up
// 0x00493710 calls (LibRw/RwApiBind.cpp bindings) run the RW core bodies of RwCore/RwCore_<rva>.cpp instead of librw:
// RwEngineInit 0x004c32b0, RwEngineOpen 0x004c30b0, RwEngineStart 0x004c2fb0, RwEngineStop 0x004c2f60, RwEngineClose
// 0x004c3040, RwEngineTerm 0x004c3270 and the driver setters 0x004c9f50 / 0x004c9eb0 / 0x004c9f60. The D3D9 device comes from the RW driver 0x004c7a70 through the RWC-DEVICE wrapper
// (RwCore_Hand.cpp RwCoreHand_Direct3DCreate9). The librw bindings stay in the tree as the fallback (L-124).
//
// Scope of this row (verify/rwccorerun_20261111/RESULT_RWC-CORE-RUN.md): the engine bring-up only. Everything after
// RwEngineStart in the top-down path still calls librw bindings and the hand frame (PortTop_HandRenderFrame), which
// cannot run on an RW core engine. So after a successful RwEngineStart the knob logs the engine state (the probe),
// stops the engine on the core (RwEngineStop) and returns failure to 0x00493710, which then runs its own failure path
// (RtFSManagerClose, RwEngineClose, RwEngineTerm, on the core) and the boot ends.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <windows.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../Compat/D3d9Wrap.h"

namespace {
inline std::uint32_t& U32(std::uint32_t va) { return *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(va)); }
}

extern "C" {

#include "RwCore_DataSeedDecl.inc"

// RW core bodies (generated TUs). C linkage, cdecl, the sources' own prototypes differ only in types.
int RwCore_004c32b0(unsigned memFuncs, unsigned initFlags, unsigned resArenaSize);
int RwCore_004c30b0(void* params);
int RwCore_004c2fb0(void);
int RwCore_004c2f60(void);
int RwCore_004c3040(void);
int RwCore_004c3270(void);
int RwCore_004c9f50(int v);
int RwCore_004c9eb0(int v);
int RwCore_004c9f60(int v);

void RwCore_Log(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[RWC_CORE] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

bool RwCore_Enabled(void) {
    static int s = -1;
    if (s < 0) {
        char v[8] = {};
        s = (GetEnvironmentVariableA("MASHED_RWC_CORE", v, sizeof(v)) > 0 && v[0] && v[0] != '0') ? 1 : 0;
    }
    return s == 1;
}

// RWC-PLUGIN-ATTACH run 40: MASHED_RWC_FRAME=1 (only with MASHED_RWC_CORE=1, default OFF). RwEngineStart returns its
// result instead of the probe's 0, the librw bindings with a core body call it (LibRw/RwApiBind.cpp lines tagged
// [RWC-FRAME], verify/rwccorerun_20261111/route_bindings.py), and the boot ends after the first RwRasterShowRaster on
// the core (RwCore_FrameShowRaster). Without it the knob behaves as in RWC-CORE-RUN.
bool RwCore_FrameRoute(void) {
    static int s = -1;
    if (s < 0) {
        char v[8] = {};
        s = (RwCore_Enabled() && GetEnvironmentVariableA("MASHED_RWC_FRAME", v, sizeof(v)) > 0 && v[0] && v[0] != '0')
                ? 1 : 0;
    }
    return s == 1;
}

// A missing body was called (RwCore_Missing.cpp): log it and end the process with its code.
void RwCore_Stop(unsigned rva, unsigned code) {
    RwCore_Log("STOP: rva 0x%08X called, no body on the RW core path (kind missing or stop in "
               "verify/rwccorerun_20261111/RwCore_Bodies.tsv); exit 0x%08X", rva, code);
    ExitProcess(code);
}

// The original's file-backed bytes of the RW core data ranges, then the code-pointer fix-ups inside them.
static void RwCore_SeedData(void) {
    static bool done = false;
    if (done) return;
    done = true;
    unsigned seeds = 0, fixes = 0, faults = 0;
#define RWC_SEED(va, n, bytes)                                                                         \
    __try { std::memcpy(reinterpret_cast<void*>(static_cast<std::uintptr_t>(va)), bytes, n); ++seeds; } \
    __except (1) { ++faults; }
#define RWC_FIX(va, sym, orig)                                                                         \
    __try { U32(va) = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&sym)); ++fixes; }    \
    __except (1) { ++faults; }
#include "RwCore_DataSeed.inc"
#undef RWC_SEED
#undef RWC_FIX
    RwCore_Log("data seed: %u ranges, %u code-pointer fix-ups, %u faults (device block 0x00618224 -> %08X)", seeds, fixes,
               faults, U32(0x00618224u));
}

static void RwCore_Probe(void);

// First access violation on the knob path: log the faulting address and the stack words that point into the exe image
// (as exe-relative offsets, to read against mashed_re.map), then let the process die as before.
static LONG CALLBACK RwCore_Veh(EXCEPTION_POINTERS* ep) {
    static LONG once = 0;
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || InterlockedExchange(&once, 1)) return EXCEPTION_CONTINUE_SEARCH;
    const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    const std::uintptr_t ip = reinterpret_cast<std::uintptr_t>(ep->ExceptionRecord->ExceptionAddress);
    RwCore_Log("AV at %08X (exe+0x%X) %s %08X; eax %08X ecx %08X edx %08X ebx %08X esi %08X edi %08X esp %08X", static_cast<unsigned>(ip),
               static_cast<unsigned>(ip - base), ep->ExceptionRecord->ExceptionInformation[0] ? "write" : "read",
               static_cast<unsigned>(ep->ExceptionRecord->ExceptionInformation[1]), ep->ContextRecord->Eax, ep->ContextRecord->Ecx,
               ep->ContextRecord->Edx, ep->ContextRecord->Ebx, ep->ContextRecord->Esi, ep->ContextRecord->Edi, ep->ContextRecord->Esp);
    const std::uint32_t* sp = reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(ep->ContextRecord->Esp));
    char buf[1024];
    int n = 0;
    for (int i = 0; i < 256 && n < 900; ++i) {
        const std::uintptr_t v = sp[i];
        if (v > base && v < base + 0x01000000u) n += std::snprintf(buf + n, sizeof(buf) - n, " +0x%X", static_cast<unsigned>(v - base));
    }
    RwCore_Log("AV stack exe offsets:%s", buf);
    // run 38: the raw top of stack (an EIP-0 fault's caller is [esp]) with the module each word falls in.
    n = 0;
    for (int i = 0; i < 8 && n < 900; ++i) {
        HMODULE hm = nullptr;
        char mod[64] = "-";
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               reinterpret_cast<LPCSTR>(static_cast<std::uintptr_t>(sp[i])), &hm) && hm) {
            char path[MAX_PATH];
            if (GetModuleFileNameA(hm, path, MAX_PATH)) {
                const char* s = std::strrchr(path, '\\');
                std::snprintf(mod, sizeof(mod), "%s+0x%X", s ? s + 1 : path,
                              static_cast<unsigned>(sp[i] - reinterpret_cast<std::uintptr_t>(hm)));
            }
        }
        n += std::snprintf(buf + n, sizeof(buf) - n, " [esp+%X]=%08X(%s)", i * 4, sp[i], mod);
    }
    RwCore_Log("AV raw stack:%s", buf);
    // run 38: the module registry RwEngineInit built. 0x00617fe0 is the registry itself (`push 0x617fe0` at
    // 0x004c2f7e): +0x10 head, +0x14 tail; node +0x08 id, +0x20 ctor, +0x24 dtor, +0x30 next, +0x34 prev
    // (0x004d7de0 / 0x004d8000 / 0x004d8060).
    const std::uint32_t reg = 0x00617fe0u;
    int k = 0;
    for (std::uint32_t nd = U32(reg + 0x10); nd && k < 40; nd = U32(nd + 0x30), ++k)
        RwCore_Log("registry node %d at %08X: id 0x%X ctor %08X dtor %08X next %08X prev %08X", k, nd, U32(nd + 8),
                   U32(nd + 0x20), U32(nd + 0x24), U32(nd + 0x30), U32(nd + 0x34));
    RwCore_Probe();   // engine state at the fault (module constructors that ran before it)
    return EXCEPTION_CONTINUE_SEARCH;
}

// RWC-FRAME-492E90: [0x007d3ff8] after the core's RwEngineStart under MASHED_RWC_FRAME: the engine instance RwEngineOpen
// allocated (0x007d3ec8 is only the static template RwEngineInit fills). Hand RwGlobals installs must not replace it.
static void* s_coreGlobals = nullptr;
void* RwCore_GlobalsAtInit(void) { return s_coreGlobals; }
bool PortTop_RwgSwapLive(void);   // Match/PortTop_RwgSwap.cpp (PORT-UNBLOCK-RWGLOBALS, MASHED_RWC_RWGSWAP)
int RwCore_EngineInit(int memFuncs, int initFlags) {
    AddVectoredExceptionHandler(1, RwCore_Veh);
    RwCore_SeedData();
    // 0x00493737 pushes 0x200000 before the two 0x005c9d00 results; it stays on the stack as RwEngineInit's third
    // argument (resArenaSize) at the call 0x00493743, cleaned by `add esp, 0xc` at 0x00493748.
    const int r = RwCore_004c32b0(static_cast<unsigned>(memFuncs), static_cast<unsigned>(initFlags), 0x200000u);
    RwCore_Log("RwEngineInit 0x004c32b0(%d, %d, 0x200000) = %d; RwGlobals [0x007d3ff8] = %08X, engine status +0x124 = %d", memFuncs,
               initFlags, r, U32(0x007d3ff8u), U32(0x007d3ff8u) ? static_cast<int>(U32(U32(0x007d3ff8u) + 0x124)) : -1);
    return r;
}

int RwCore_EngineOpen(int* params) {
    const int r = RwCore_004c30b0(params);
    RwCore_Log("RwEngineOpen 0x004c30b0(hwnd=%08X) = %d; IDirect3D9 [0x007d4108] = %08X", params ? params[0] : 0, r,
               U32(0x007d4108u));
    return r;
}

static void RwCore_Probe(void) {
    const std::uint32_t g = U32(0x007d3ff8u);
    const std::uint32_t off = U32(0x007d4054u);
    RwCore_Log("probe: RwGlobals %08X (expect 007D3EC8), IDirect3D9 [0x007d4108] %08X, device [0x007d4110] %08X, "
               "presents %ld", g, U32(0x007d4108u), U32(0x007d4110u), MashedD3D9Wrap_PresentCount());
    if (g) {
        RwCore_Log("probe: texture module offset [0x007d4054] = 0x%X, instances [0x007d4058] = %u; texture freelist "
                   "[RwGlobals+off+8] = %08X, texdict freelist [RwGlobals+off+0xc] = %08X, current texdict [+0x10] = "
                   "%08X, [0x007d4080] = %08X", off, U32(0x007d4058u), U32(g + off + 8), U32(g + off + 0xc),
                   U32(g + off + 0x10), U32(0x007d4080u));
        RwCore_Log("probe: device block in RwGlobals: +0x10 fpSystem %08X, +0x20 RenderStateSet %08X, +0x30 Im2D "
                   "prim %08X, +0x34 Im2D indexed %08X; curCamera +0x0 %08X", U32(g + 0x10), U32(g + 0x20),
                   U32(g + 0x30), U32(g + 0x34), U32(g + 0));
    }
}

int RwCore_00493640(void);
// RWC-PLUGIN-ATTACH: RenderwareAttachPlugins 0x00493640 on the RW core, between RwEngineInit and RwEngineOpen as in
// the original (0x00493710). Its attaches register their plugins in the registries of the seeded data range; the
// constructors run at RwEngineStart.
int RwCore_AttachPlugins(void) {
    const int r = RwCore_00493640();
    RwCore_Log("RenderwareAttachPlugins 0x00493640() = %d on the RW core; [0x007dc57c] (PTank plugin offset, 0x00534ac4) = 0x%X",
               r, U32(0x007dc57cu));
    return r;
}

// ---- RWC-FRAME-CLUMP run 45: core-side Im2D draw dump (MASHED_RWC_DRAWDUMP=<json path>, frame route only, default OFF).
// Same channel and schema as the original-side capture re/frida/menu_draw_burst.py: the Im2D RenderPrimitive slot of the
// engine instance, [[0x007d3ff8] + 0x30] (RwIm2DRenderPrimitive(prim, verts, num), cdecl), one record per call with the
// raw vertex blob (num * 0x1c bytes, num outside 1..64 read as 4, as the burst tool does) and up to 5 return addresses
// (exe-relative offsets, resolve with --map mashedmod/build/mashed_re.map). The frame delimiter is MASHED_RWC_DRAWDUMP_DELIM
// (RwCore_DrawDumpMark below; menu_draw_burst.py --delim takes the same RVA on the original side).
// MASHED_RWC_DRAWDUMP_SKIP = entries to skip before the first captured frame (default 0), MASHED_RWC_DRAWDUMP_FRAMES =
// frames captured (default 4). Labels core_f0..core_f<K-1>. The wrapper calls the saved slot, so the draws are unchanged.
// [RWC-FRAME-CLUMP] Indexed draws (slot +0x34) are counted per frame, not dumped (the burst tool does not hook +0x34).
typedef int(__cdecl* RwCoreIm2DPrimFn)(int prim, void* verts, int num);
typedef int(__cdecl* RwCoreIm2DIdxFn)(int prim, void* verts, int nverts, void* idx, int nidx);
static RwCoreIm2DPrimFn s_ddPrim = nullptr;
static RwCoreIm2DIdxFn s_ddIdx = nullptr;
static std::FILE* s_ddOut = nullptr;
static int s_ddState = -1;            // -1 env not read, 0 off, 1 armed, 2 written
static unsigned s_ddDelim = 0x00492e90u, s_ddTotal = 0;
static unsigned s_ddSkip = 0, s_ddFrames = 4, s_ddEntries = 0, s_ddDraws = 0, s_ddIdxDraws = 0, s_ddClamped = 0;
static int s_ddCur = -1;              // captured frame index, -1 = not collecting
static char s_ddPath[MAX_PATH] = {};

static unsigned RwCore_DdEnvU(const char* name, unsigned dflt) {
    char v[16] = {};
    return GetEnvironmentVariableA(name, v, sizeof(v)) > 0 ? static_cast<unsigned>(std::strtoul(v, nullptr, 10)) : dflt;
}

static void RwCore_DdCloseFrame(void) {
    if (s_ddCur < 0) return;
    std::fputs("\n ]", s_ddOut);
    // 0x0067eca4: 0x0043c5b0 returns before any draw while it is 0 or 1 (re/match/s08/0043c5b0.c); 0x0067ecb0: screen id.
    RwCore_Log("drawdump core_f%d: %u Im2D prim draws, %u indexed (+0x34, not dumped), %u counts read as 4; DAT_0067eca4 %d "
               "DAT_0067ecb0 %d DAT_00771968 %d", s_ddCur, s_ddDraws, s_ddIdxDraws, s_ddClamped,
               static_cast<int>(U32(0x0067eca4u)), static_cast<int>(U32(0x0067ecb0u)), static_cast<int>(U32(0x00771968u)));
    s_ddCur = -1;
}

static void RwCore_DdFinish(const char* why) {
    if (s_ddState != 1) return;
    RwCore_DdCloseFrame();
    std::fputs("\n}\n", s_ddOut);
    std::fclose(s_ddOut);
    s_ddOut = nullptr;
    s_ddState = 2;
    RwCore_Log("drawdump written (%s): %s, delimiter 0x%08X entries seen %u, Im2D prim calls in the whole run %u", why,
               s_ddPath, s_ddDelim, s_ddEntries, s_ddTotal);
}

static int __cdecl RwCore_DdPrim(int prim, void* verts, int num) {
    ++s_ddTotal;
    if (s_ddCur >= 0 && s_ddOut) {
        int n = num;
        if (n < 1 || n > 64) { n = 4; ++s_ddClamped; }
        std::fprintf(s_ddOut, "%s\n  {\"v\": \"", s_ddDraws ? "," : "");
        const unsigned char* p = static_cast<const unsigned char*>(verts);
        __try {
            for (int i = 0; i < n * 0x1c; ++i) std::fprintf(s_ddOut, "%02x", p[i]);
        } __except (1) {}
        std::fputs("\", \"r\": [", s_ddOut);
        void* fr[5] = {};
        const USHORT got = CaptureStackBackTrace(1, 5, fr, nullptr);
        const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        for (USHORT i = 0; i < got; ++i)
            std::fprintf(s_ddOut, "%s\"0x%x\"", i ? ", " : "",
                         static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(fr[i]) - base));
        std::fprintf(s_ddOut, "], \"prim\": %d, \"num\": %d}", prim, num);
        ++s_ddDraws;
    }
    return s_ddPrim(prim, verts, num);
}

static int __cdecl RwCore_DdIdx(int prim, void* verts, int nverts, void* idx, int nidx) {
    if (s_ddCur >= 0) ++s_ddIdxDraws;
    return s_ddIdx(prim, verts, nverts, idx, nidx);
}

// Installs the wrappers in the engine instance's device block (after the core's RwEngineStart, frame route only).
static void RwCore_DdInstall(void) {
    if (s_ddState < 0) {
        s_ddState = (GetEnvironmentVariableA("MASHED_RWC_DRAWDUMP", s_ddPath, sizeof(s_ddPath)) > 0 && s_ddPath[0]) ? 1 : 0;
        if (s_ddState == 1) {
            s_ddSkip = RwCore_DdEnvU("MASHED_RWC_DRAWDUMP_SKIP", 0);
            s_ddFrames = RwCore_DdEnvU("MASHED_RWC_DRAWDUMP_FRAMES", 4);
            char dv[16] = {};
            if (GetEnvironmentVariableA("MASHED_RWC_DRAWDUMP_DELIM", dv, sizeof(dv)) > 0)
                s_ddDelim = static_cast<unsigned>(std::strtoul(dv, nullptr, 16));
            s_ddOut = std::fopen(s_ddPath, "w");
            if (!s_ddOut) { RwCore_Log("drawdump: cannot open %s", s_ddPath); s_ddState = 0; return; }
            std::fputs("{", s_ddOut);
        }
    }
    if (s_ddState != 1) return;
    const std::uint32_t g = U32(0x007d3ff8u);
    if (!g) return;
    const std::uint32_t pf = U32(g + 0x30), xf = U32(g + 0x34);
    const std::uint32_t mine = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&RwCore_DdPrim));
    const std::uint32_t minex = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&RwCore_DdIdx));
    if (pf != mine) {
        s_ddPrim = reinterpret_cast<RwCoreIm2DPrimFn>(static_cast<std::uintptr_t>(pf));
        U32(g + 0x30) = mine;
        RwCore_Log("drawdump: Im2D prim slot [RwGlobals %08X + 0x30] %08X wrapped (delimiter 0x%08X, skip %u, frames %u) -> %s",
                   g, pf, s_ddDelim, s_ddSkip, s_ddFrames, s_ddPath);
    }
    if (xf != minex && xf) {
        s_ddIdx = reinterpret_cast<RwCoreIm2DIdxFn>(static_cast<std::uintptr_t>(xf));
        U32(g + 0x34) = minex;
    }
}

// Frame delimiter. MASHED_RWC_DRAWDUMP_DELIM (hex RVA, default 0x00492e90): one frame = the span between two entries to
// that RVA. 0x00492e90 = the frame render, marked by its caller PortTop_StandIn_00492e90 (Match/PortTop_00492370.cpp) on
// the frame route; 0x0042e3a0 = the burst tool's default delimiter, marked by the CALL_VIA entry below (run 45: the core
// frame at the title, DAT_0067ecb0 = 33, never entered it, 0 entries in 120 frames).
// Under the dump knob the begin-update of the frame camera DAT_006905b0 (0x004671a0 returns it for -1; RwCameraBeginUpdate
// 0x004c1a00 calls [camera + 0x18]) is wrapped too, and its result logged for the first 3 calls and every 60th: 0x00492e90
// draws the screen (0x0043c5b0) only when it returns non-zero.
typedef int(__cdecl* RwCoreCamFn)(int cam);
static RwCoreCamFn s_ddBegin = nullptr;
static std::uint32_t s_ddBeginCam = 0;
static int __cdecl RwCore_DdBegin(int cam) {
    static unsigned n = 0;
    const int r = s_ddBegin(cam);
    if (++n <= 3 || n % 60 == 0)
        RwCore_Log("drawdump: camera %08X begin-update [+0x18] %08X call %u = %08X; camera +0x60 %08X +0x64 %08X", cam,
                   static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(s_ddBegin)), n, r, U32(cam + 0x60), U32(cam + 0x64));
    return r;
}
static void RwCore_DdWrapCamera(void) {
    const std::uint32_t cam = U32(0x006905b0u);
    const std::uint32_t mine = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&RwCore_DdBegin));
    if (!cam || U32(cam + 0x18) == mine) return;
    if (cam != s_ddBeginCam || !s_ddBegin) s_ddBegin = reinterpret_cast<RwCoreCamFn>(static_cast<std::uintptr_t>(U32(cam + 0x18)));
    s_ddBeginCam = cam;
    RwCore_Log("drawdump: frame camera DAT_006905b0 %08X, begin-update [+0x18] %08X end-update [+0x1c] %08X wrapped", cam,
               U32(cam + 0x18), U32(cam + 0x1c));
    U32(cam + 0x18) = mine;
}

// RWC-FRAME-JITTER-STRICT (default OFF, dump knob only): MASHED_RWC_DRAWDUMP_TPIN = comma list of hex dwords, one per
// dumped frame. At the delimiter that opens dumped frame k (< list length) the clock DAT_007f1010 is overwritten with
// word k before the frame renders. DAT_007f1010 is written by 0x00493390 (re/match/s15/00493390.c, `(float)now *
// 3.3333333e-07f`), run from 0x00493480 outside the frame render, and read by the checker jitter of 0x00473ee0
// (original: fld [0x007f1010] at 0x00474167, fadd st0,st0 at 0x0047416d, w2 stored at 0x0047416f) and by the crossfade
// phase of 0x0042e5b0. The words come from the original-side capture (menu_draw_burst.py --peek-draw 0x007f1010), so a
// dumped frame renders at the clock value of the original frame it is compared with. Each frame logs the clock before the
// pin and at the frame's close.
static unsigned s_ddTpin[16] = {}, s_ddTpinN = 0;
static int s_ddTpinRead = 0;
static void RwCore_DdTpinRead(void) {
    if (s_ddTpinRead) return;
    s_ddTpinRead = 1;
    char v[256] = {};
    if (GetEnvironmentVariableA("MASHED_RWC_DRAWDUMP_TPIN", v, sizeof(v)) <= 0) return;
    char* p = v;
    while (*p && s_ddTpinN < 16) {
        char* q = nullptr;
        s_ddTpin[s_ddTpinN++] = static_cast<unsigned>(std::strtoul(p, &q, 16));
        if (!q || q == p) break;
        p = (*q == ',') ? q + 1 : q;
    }
    RwCore_Log("drawdump: MASHED_RWC_DRAWDUMP_TPIN %u words (DAT_007f1010 pinned at the delimiter of each dumped frame)",
               s_ddTpinN);
}

void RwCore_DrawDumpMark(unsigned rva) {
    if (s_ddState != 1 || rva != s_ddDelim) return;
    RwCore_DdInstall();   // re-wrap if the slot was rewritten since the engine start
    RwCore_DdWrapCamera();
    RwCore_DdTpinRead();
    const unsigned e = s_ddEntries++;
    if (e >= s_ddSkip && e < s_ddSkip + s_ddFrames) {
        if (s_ddCur >= 0 && s_ddTpinN)
            RwCore_Log("drawdump core_f%d close: DAT_007f1010 %08X", s_ddCur, U32(0x007f1010u));
        RwCore_DdCloseFrame();
        s_ddDraws = s_ddIdxDraws = s_ddClamped = 0;
        s_ddCur = static_cast<int>(e - s_ddSkip);
        if (static_cast<unsigned>(s_ddCur) < s_ddTpinN) {
            RwCore_Log("drawdump core_f%d tpin: DAT_007f1010 %08X -> %08X", s_ddCur, U32(0x007f1010u), s_ddTpin[s_ddCur]);
            U32(0x007f1010u) = s_ddTpin[s_ddCur];
        }
        std::fprintf(s_ddOut, "%s\n \"core_f%d\": [", s_ddCur ? "," : "", s_ddCur);
    } else if (e == s_ddSkip + s_ddFrames) {
        if (s_ddCur >= 0 && s_ddTpinN)
            RwCore_Log("drawdump core_f%d close: DAT_007f1010 %08X", s_ddCur, U32(0x007f1010u));
        RwCore_DdFinish("frames done");
    }
}

void RwCore_0042e3a0(void);
// CALL_VIA entry for 0x0042e3a0 (gen_rwcore_tus.py): marks a dump frame, then runs the matched body unchanged.
void RwCore_DrawDumpFrame_0042e3a0(void) {
    RwCore_DrawDumpMark(0x0042e3a0u);
    RwCore_0042e3a0();
}

int RwCore_EngineStart(void) {
    const int r = RwCore_004c2fb0();
    RwCore_Log("RwEngineStart 0x004c2fb0() = %d", r);
    RwCore_Probe();
    if (!r) return 0;
    if (RwCore_FrameRoute()) {
        RwCore_Log("MASHED_RWC_FRAME: RwEngineStart returns %d; the top-down path continues with the bindings on the core", r);
        s_coreGlobals = reinterpret_cast<void*>(static_cast<std::uintptr_t>(U32(0x007d3ff8u)));
        RwCore_DdInstall();
        return r;
    }
    RwCore_Log("calling RwEngineStop 0x004c2f60 (device call 0x12, module destructors 0x004d8060, device call 3)");
    const int s = RwCore_004c2f60();
    RwCore_Log("bring-up probe done: RwEngineStop 0x004c2f60() = %d; returning 0 from RwEngineStart so 0x00493710 runs "
               "its failure path (the rest of the top-down path is not on the RW core yet)", s);
    return 0;
}

// RWC-FRAME-CLUMP run 45: live-clump census before RwEngineStop on the frame route (read only). The clump module close
// 0x004e6250 (lifted) destroys every clump still in the clump freelist ([RwGlobals + [0x007d7200] + 4], 0x004e6262) through
// 0x004e62f0; run 45 vs_frameT AVs there (0x0053ec60 frees into freelist DAT_007dc5e8 = 0). This lists those clumps first:
// pointer, frame (+0x4) and the clump's atomic list head (+0x8), to be read against the boot log's clump pointers.
extern "C" int RwCore_004ccce0(int freelist, void* cb, int data);
static unsigned s_liveClumps = 0;
static void __cdecl RwCore_LiveClumpCb(int clump, int) {
    if (++s_liveClumps <= 64)
        RwCore_Log("live clump %u at RwEngineStop: %08X frame +0x4 %08X atomics +0x8 %08X", s_liveClumps, clump, U32(clump + 4),
                   U32(clump + 8));
}
int RwCore_EngineStop(void) {
    if (RwCore_FrameRoute() && U32(0x007d3ff8u)) {
        const std::uint32_t fl = U32(U32(0x007d3ff8u) + U32(0x007d7200u) + 4);
        s_liveClumps = 0;
        if (fl) RwCore_004ccce0(static_cast<int>(fl), reinterpret_cast<void*>(&RwCore_LiveClumpCb), 0);
        RwCore_Log("live clumps at RwEngineStop: %u (clump freelist %08X)", s_liveClumps, fl);
    }
    const int r = RwCore_004c2f60(); RwCore_Log("RwEngineStop 0x004c2f60() = %d", r); return r;
}
int RwCore_EngineClose(void) { const int r = RwCore_004c3040(); RwCore_Log("RwEngineClose 0x004c3040() = %d", r); return r; }
int RwCore_EngineTerm(void) { const int r = RwCore_004c3270(); RwCore_Log("RwEngineTerm 0x004c3270() = %d", r); return r; }
// RwRasterShowRaster 0x004c7730 on the core under MASHED_RWC_FRAME. After the Nth call (N = MASHED_RWC_FRAME_N, default 1,
// RWC-FRAME-492E90): log the frame, stop, close and terminate the engine on the core, and end the process with
// 0x5257F000 | (result != 0). Calls before the Nth return the result (the first one and every 60th are logged).
extern "C" int RwCore_004c7730(int raster, int dev, int flags);
static unsigned RwCore_FrameLimit(void) {
    static unsigned n = 0;
    if (!n) {
        char v[16] = {};
        if (GetEnvironmentVariableA("MASHED_RWC_FRAME_N", v, sizeof(v)) > 0) n = static_cast<unsigned>(std::strtoul(v, nullptr, 10));
        if (!n) n = 1;
        RwCore_Log("MASHED_RWC_FRAME_N: the boot ends after RwRasterShowRaster call %u on the RW core", n);
    }
    return n;
}
extern "C" int RwCore_FrameShowRaster(int raster, int dev, int flags) {
    static unsigned calls = 0;
    const int r = RwCore_004c7730(raster, dev, flags);
    if (++calls < RwCore_FrameLimit()) {
        if (calls == 1 || calls % 60 == 0)
            RwCore_Log("RwRasterShowRaster 0x004c7730(raster %08X, dev %08X, flags 0x%X) = %08X on the RW core: call %u, "
                       "presents %ld", static_cast<unsigned>(raster), static_cast<unsigned>(dev), flags,
                       static_cast<unsigned>(r), calls, MashedD3D9Wrap_PresentCount());
        return r;
    }
    RwCore_Log("RwRasterShowRaster 0x004c7730(raster %08X, dev %08X, flags 0x%X) = %08X on the RW core: call %u, the "
               "last; presents %ld; device [0x007d4110] %08X", static_cast<unsigned>(raster), static_cast<unsigned>(dev),
               flags, static_cast<unsigned>(r), calls, MashedD3D9Wrap_PresentCount(), U32(0x007d4110));
    RwCore_Probe();
    // PORT-UNBLOCK-RWGLOBALS: the frame route calls none of the exe FastSqrt / Vec2Length / 0x00403160 bodies, so the
    // MASHED_RWC_RWGSWAP gate (and its one-shot self-test) runs once here, with the core RwGlobals live. Inert when off.
    PortTop_RwgSwapLive();
    // [RWC-FRAME-CLUMP] N > 1: the call comes from inside the main loop (RwCameraShowRaster 0x004c1be0 under the frame
    // render 0x00492e90), with the game's clumps, worlds and cameras alive. The original frees them in AppDestroy 0x00402a40
    // before RwEngineStop; stopping the engine here runs plugin closes over live objects (d19: RwFreeListFree 0x004ccba0
    // on a closed freelist from 0x004e4130). So the engine teardown runs only for the boot-time stop (N = 1).
    if (RwCore_FrameLimit() == 1) {
        RwCore_EngineStop();
        RwCore_EngineClose();
        RwCore_EngineTerm();
    } else {
        RwCore_Log("MASHED_RWC_FRAME_N %u: engine teardown not run (game objects alive, AppDestroy not reached)", calls);
    }
    RwCore_DdFinish("boot end");
    RwCore_Log("MASHED_RWC_FRAME: boot ends after present %u on the RW core (exit 0x%08X)", calls, 0x5257F000u | (r ? 1u : 0u));
    ExitProcess(0x5257F000u | (r ? 1u : 0u));
    return r;
}
// RWC-FRAME-CLUMP run 45: the RW Destroy binding entries on the frame route (route_bindings.py SPECIAL). Run 44 d18 and
// run 45 vs_frameT: AppDestroy 0x00402a40 passes objects whose creators are stand-ins on the top-down path, so they are 0:
// 0x0041a3d0 RpClumpDestroy(DAT_0063c620) (AV read 0x00000008 at RwCore_004e6e00+0x15; the only writer of 0x0063c620 in
// re/match is the Panel stand-in 0x0041a1e0, re/match/s03/h1_0041a1e0.c), then 0x0041ccf0 RwTexDictionaryDestroy(DAT_0063cdd0)
// (AV read 0x00000008 at RwCore_004c5930+0x22). In the original the creators always run first, so its Destroy bodies are never
// given 0. Each wrapper skips a 0 (first 8 per RVA logged) and returns what the librw binding returns for 0 (1, the camera
// 0); any other value runs the core body unchanged. The librw bindings skip 0 the same way.
#define RWC_DESTROY0(rva, name, ret0)                                                                                \
    int RwCore_##rva(int obj);                                                                                       \
    int RwCore_FrameDestroy0_##rva(int obj) {                                                                        \
        if (!obj) {                                                                                                  \
            static unsigned n = 0;                                                                                   \
            if (++n <= 8) RwCore_Log("%s 0x%s(0) on the RW core: skipped, its creator did not run (stand-in); call %u", \
                                     name, #rva, n);                                                                 \
            return ret0;                                                                                             \
        }                                                                                                            \
        return RwCore_##rva(obj);                                                                                    \
    }
RWC_DESTROY0(004c5930, "RwTexDictionaryDestroy", 1)
RWC_DESTROY0(004c5a60, "RwTextureDestroy", 1)
RWC_DESTROY0(004e5700, "RpWorldDestroy", 1)
RWC_DESTROY0(004d41e0, "0x004d41e0", 1)
RWC_DESTROY0(004c1cf0, "RwCameraDestroy", 0)
RWC_DESTROY0(004c0c20, "RwFrameDestroy", 1)
RWC_DESTROY0(004e6e00, "RpClumpDestroy", 1)
RWC_DESTROY0(004c7650, "RwRasterDestroy", 1)
RWC_DESTROY0(004e6920, "RpAtomicDestroy", 1)
RWC_DESTROY0(004e4d90, "RpLightDestroy", 1)
#undef RWC_DESTROY0
int RwCore_Set7d4134(int v) { return RwCore_004c9f50(v); }
int RwCore_SetRefresh(int v) { return RwCore_004c9eb0(v); }
int RwCore_Set7d413c(int v) { return RwCore_004c9f60(v); }

} // extern "C"
#endif // MASHED_STANDALONE

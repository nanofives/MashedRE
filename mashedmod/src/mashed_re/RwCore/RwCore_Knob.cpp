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

// A missing body was called (RwCore_Missing.cpp): log it and end the process with its code.
void RwCore_Stop(unsigned rva, unsigned code) {
    RwCore_Log("STOP: rva 0x%08X called, no body on the RW core path (no source, no hand body); exit 0x%08X", rva, code);
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

int RwCore_EngineStart(void) {
    const int r = RwCore_004c2fb0();
    RwCore_Log("RwEngineStart 0x004c2fb0() = %d", r);
    RwCore_Probe();
    if (!r) return 0;
    RwCore_Log("calling RwEngineStop 0x004c2f60 (device call 0x12, module destructors 0x004d8060, device call 3)");
    const int s = RwCore_004c2f60();
    RwCore_Log("bring-up probe done: RwEngineStop 0x004c2f60() = %d; returning 0 from RwEngineStart so 0x00493710 runs "
               "its failure path (the rest of the top-down path is not on the RW core yet)", s);
    return 0;
}

int RwCore_EngineStop(void) { const int r = RwCore_004c2f60(); RwCore_Log("RwEngineStop 0x004c2f60() = %d", r); return r; }
int RwCore_EngineClose(void) { const int r = RwCore_004c3040(); RwCore_Log("RwEngineClose 0x004c3040() = %d", r); return r; }
int RwCore_EngineTerm(void) { const int r = RwCore_004c3270(); RwCore_Log("RwEngineTerm 0x004c3270() = %d", r); return r; }
int RwCore_Set7d4134(int v) { return RwCore_004c9f50(v); }
int RwCore_SetRefresh(int v) { return RwCore_004c9eb0(v); }
int RwCore_Set7d413c(int v) { return RwCore_004c9f60(v); }

} // extern "C"
#endif // MASHED_STANDALONE

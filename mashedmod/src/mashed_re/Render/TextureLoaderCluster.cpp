// Mashed RE — Render/TextureLoaderCluster.cpp
// RenderWare texture and texture-dictionary helpers (wave1-s4, c3-sweep).
//
// Covers:
//   0x004c5800  RwTexDictionarySetCurrent  — single-store setter: writes dict ptr into plugin globals slot
//   0x004c5820  RwTexDictionaryGetCurrent  — single-load getter: reads same slot
//   0x004c5a00  RwTextureCreate            — alloc RwTexture via RW pool, init fields, event notify
//   0x004c5ae0  RwTextureSetName           — strncpy name into tex+0x10, enforce 31-char limit
//   0x004c5b50  RwTextureSetMaskName       — strncpy maskName into tex+0x30, enforce 31-char limit
//
// Deferred (calls live D3D9 vtable at platform-raster-create stage):
//   0x004c77c0  RwRasterCreate             — vtable+0x58 invokes D3D device; deferred per wave1 guardrails.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched; patched copy SHA differs — expected)
//
// Analysis refs:
//   re/analysis/texture_loader_d3_cont1/0x004c5800.md
//   re/analysis/texture_loader_d3_cont1/0x004c5820.md
//   re/analysis/texture_loader_d3_cont1/0x004c5a00.md
//   re/analysis/texture_loader_d3_cont1/0x004c5ae0.md
//   re/analysis/texture_loader_d3_cont1/0x004c5b50.md
//   re/analysis/texture_loader_d3/0x004c77c0.md
//
// Verification strategy:
//   RwTexDictionarySetCurrent / GetCurrent: pure global-slot r/w; quiescent menu
//   call is safe — returns immediately with correct slot value or stores it.
//   RwTextureCreate / SetName / SetMaskName: access live RW engine vtable globals
//   (DAT_007d3ff8). At quiescent main-menu (null/zero texture context) each
//   crashes deterministically the same way as the original —
//   crash_equal_ok=True GREEN per established pattern.

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x004c5800  FUN_004c5800  RwTexDictionarySetCurrent   (25 bytes)
//
// int FUN_004c5800(RwTexDictionary *dict)
//   param_1 = dict pointer (may be NULL to clear)
//   Returns 1 (always).
//
// Body (verbatim decompiler):
//   *(undefined4 *)(DAT_007d4054 + 0x1c + DAT_007d3ff8) = param_1;
//   return 1;
//
// Facts:
//   Write param_1 to DAT_007d4054+0x1c+DAT_007d3ff8:  0x004c5805
//   Return 1:                                           0x004c5815
//
// Plugin globals layout:
//   DAT_007d3ff8 = texture plugin registered slot offset within global RW instance
//   DAT_007d4054 = global RW instance base pointer
//   [DAT_007d4054 + DAT_007d3ff8 + 0x1c] = currentTexDict (RwTexDictionary*)
// ---------------------------------------------------------------------------

static constexpr std::uintptr_t kRwGlobalsPtr  = 0x007d3ff8u;  // texture plugin slot offset
static constexpr std::uintptr_t kRwInstancePtr = 0x007d4054u;  // global RW instance base ptr

// 0x004c5800
extern "C" __declspec(dllexport) std::uint32_t __cdecl
RwTexDictionarySetCurrent(std::uint32_t param_1)
{
    // cited 0x004c5805: write param_1 to slot +0x1c
    std::uint32_t rw_slot  = *reinterpret_cast<std::uint32_t*>(kRwGlobalsPtr);
    std::uint32_t rw_base  = *reinterpret_cast<std::uint32_t*>(kRwInstancePtr);
    *reinterpret_cast<std::uint32_t*>(rw_base + 0x1cu + rw_slot) = param_1;
    return 1u;  // cited 0x004c5815
}

RH_ScopedInstall(RwTexDictionarySetCurrent, 0x004c5800);  // re-enabled 2026-05-24 phase-a2-strict GREEN int_scalar (always returns 1)

// ---------------------------------------------------------------------------
// 0x004c5820  FUN_004c5820  RwTexDictionaryGetCurrent   (15 bytes)
//
// RwTexDictionary *FUN_004c5820(void)
//   No parameters.
//   Returns the currently active RwTexDictionary pointer (may be NULL).
//
// Body (verbatim decompiler):
//   return *(undefined4 *)(DAT_007d4054 + 0x1c + DAT_007d3ff8);
//
// Facts:
//   Read DAT_007d4054+0x1c+DAT_007d3ff8:  0x004c5822
//
// Symmetric getter for the slot written by RwTexDictionarySetCurrent.
// ---------------------------------------------------------------------------

// 0x004c5820
extern "C" __declspec(dllexport) std::uint32_t __cdecl
RwTexDictionaryGetCurrent()
{
    // cited 0x004c5822: read slot +0x1c
    std::uint32_t rw_slot = *reinterpret_cast<std::uint32_t*>(kRwGlobalsPtr);
    std::uint32_t rw_base = *reinterpret_cast<std::uint32_t*>(kRwInstancePtr);
    return *reinterpret_cast<std::uint32_t*>(rw_base + 0x1cu + rw_slot);
}

RH_ScopedInstall(RwTexDictionaryGetCurrent, 0x004c5820);  // re-enabled 2026-05-24 phase-a2-strict GREEN getter

// ---------------------------------------------------------------------------
// 0x004c5a00  FUN_004c5a00  RwTextureCreate   (89 bytes)
//
// RwTexture *FUN_004c5a00(RwRaster *raster)
//   param_1 = raster pointer; stored at result+0x00.
//   Returns allocated and initialised RwTexture ptr, or NULL on alloc failure.
//
// Body (verbatim key sections):
//   puVar1 = (**(code **)(DAT_007d3ff8 + 0x118))
//                (*(undefined4 *)(DAT_007d4054 + 8 + DAT_007d3ff8), 0x30006);
//   // alloc type-hint 0x30006, cited 0x004c5a05
//   // alloc size from DAT_007d4054+8+DAT_007d3ff8, cited 0x004c5a09
//   if (puVar1 != NULL) {
//     puVar1[0x14] = 0;              // dword at +0x50 = 0,   cited 0x004c5a1d
//     *puVar1 = param_1;             // raster ptr at +0x00,  cited 0x004c5a20
//     *(byte)(puVar1+0x51) = 0x11;   // U,V addr=1 at +0x51,  cited 0x004c5a26
//     puVar1[1] = 0;                 // dict=NULL at +0x04,   cited 0x004c5a2c
//     *(byte)(puVar1+4) = 0;         // name[0]=0  at +0x10,  cited 0x004c5a32
//     *(byte)(puVar1+0xc) = 0;       // mask[0]=0  at +0x30,  cited 0x004c5a38
//     puVar1[0x15] = 1;              // refCount=1 at +0x54,  cited 0x004c5a3e
//     *(byte)(puVar1+0x14) = 1;      // filter=1   at +0x50,  cited 0x004c5a44
//     FUN_004d8000(&DAT_00618138, puVar1);  // event notify,  cited 0x004c5a4e
//   }
//   return puVar1;
//
// RwTexture struct (88 bytes):
//   +0x00: RwRaster* raster
//   +0x04: RwTexDictionary* dict (NULL on create)
//   +0x08: RwLLLink inDictionary (not touched here)
//   +0x10: char[32] name (name[0]=0 → empty string)
//   +0x30: char[32] mask (mask[0]=0 → empty string)
//   +0x50: RwUInt32 filterAddressing (byte+0=filter=1=rwFILTER_NEAREST; byte+1=0x11=U,V wrap)
//   +0x54: RwInt32  refCount (=1 on create)
// ---------------------------------------------------------------------------

// External callee: FUN_004d8000 — RW event notification.
using EventNotifyFn_t = void (__cdecl*)(void*, void*);
static EventNotifyFn_t const s_FUN_004d8000_tex =
    reinterpret_cast<EventNotifyFn_t>(0x004d8000u);

// 0x004c5a00
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
RwTextureCreate(std::uint32_t param_1)
{
    // Alloc RwTexture via vtable+0x118, type-hint 0x30006, cited 0x004c5a05/0x004c5a09
    std::uint32_t rw_base  = *reinterpret_cast<std::uint32_t*>(kRwGlobalsPtr);
    std::uint32_t rw_inst  = *reinterpret_cast<std::uint32_t*>(kRwInstancePtr);
    // alloc_size stored at DAT_007d4054+8+DAT_007d3ff8
    std::uint32_t alloc_size = *reinterpret_cast<std::uint32_t*>(rw_inst + 8u + rw_base);
    using AllocFn_t = std::uint32_t* (__cdecl*)(std::uint32_t, std::uint32_t);
    AllocFn_t alloc_fn = *reinterpret_cast<AllocFn_t*>(rw_base + 0x118u);
    std::uint32_t* puVar1 = alloc_fn(alloc_size, 0x30006u);
    if (puVar1 == nullptr) return nullptr;

    // Field initialisation — all addresses cited from analysis note:
    puVar1[0x14] = 0u;                                                   // +0x50 dword clear, cited 0x004c5a1d
    *puVar1 = param_1;                                                   // +0x00 raster ptr,  cited 0x004c5a20
    *reinterpret_cast<std::uint8_t*>(
        reinterpret_cast<std::uintptr_t>(puVar1) + 0x51u) = 0x11u;      // +0x51 UV addr=0x11, cited 0x004c5a26
    puVar1[1] = 0u;                                                      // +0x04 dict=NULL,    cited 0x004c5a2c
    *reinterpret_cast<std::uint8_t*>(puVar1 + 4)  = 0u;                 // +0x10 name[0]=0,   cited 0x004c5a32
    *reinterpret_cast<std::uint8_t*>(puVar1 + 0xcu) = 0u;               // +0x30 mask[0]=0,   cited 0x004c5a38
    puVar1[0x15] = 1u;                                                   // +0x54 refCount=1,  cited 0x004c5a3e
    *reinterpret_cast<std::uint8_t*>(puVar1 + 0x14u) = 1u;              // +0x50 filter=1,    cited 0x004c5a44

    // Plugin event notify, cited 0x004c5a4e
    static constexpr std::uintptr_t kTexCreateEvent = 0x00618138u;
    s_FUN_004d8000_tex(reinterpret_cast<void*>(kTexCreateEvent), puVar1);

    return puVar1;
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(RwTextureCreate, 0x004c5a00);

// ---------------------------------------------------------------------------
// 0x004c5ae0  FUN_004c5ae0  RwTextureSetName   (109 bytes)
//
// RwTexture *FUN_004c5ae0(RwTexture *tex, const char *name)
//   param_1 = texture ptr
//   param_2 = name string (C string, max 31 chars)
//   Returns param_1 (tex).
//
// Body (verbatim decompiler):
//   (**(code **)(DAT_007d3ff8 + 0xd0))(param_1 + 0x10, param_2, 0x20);
//     // strncpy(tex+0x10, name, 32), cited 0x004c5ae8
//   uVar1 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_2);
//     // strlen(name), cited 0x004c5af6
//   if (0x1f < uVar1) {
//     // overflow: error code 0x8000001e, cited 0x004c5aff
//     uStack_8 = 1;
//     uStack_4 = FUN_004d7ff0(0x8000001e, param_2, 0x20, 0x1f, ...);
//     FUN_004d8480(&uStack_8);
//     *(byte)(param_1 + 0x2f) = 0;  // force null at tex+0x2f, cited 0x004c5b3c
//   }
//   return param_1;
//
// tex->name is char[32] at tex+0x10..+0x2f.
// Max allowed: 31 chars (rwTEXTUREBASENAMELENGTH - 1).
// ---------------------------------------------------------------------------

// External callees via RW vtable:
//   DAT_007d3ff8 + 0xd0 = strncpy wrapper
//   DAT_007d3ff8 + 0xf4 = strlen wrapper
//   FUN_004d7ff0 — RW error report
//   FUN_004d8480 — RW error dispatch

using StrncpyFn_t = void (__cdecl*)(void*, const void*, std::uint32_t);
using StrlenFn_t  = std::uint32_t (__cdecl*)(const void*);

using RwErrReportFn_t   = std::uint32_t (__cdecl*)(std::uint32_t, const void*,
                                                    std::uint32_t, std::uint32_t, std::int32_t);
using RwErrDispatchFn_t = void (__cdecl*)(void*);

static RwErrReportFn_t   const s_FUN_004d7ff0 =
    reinterpret_cast<RwErrReportFn_t>(0x004d7ff0u);
static RwErrDispatchFn_t const s_FUN_004d8480 =
    reinterpret_cast<RwErrDispatchFn_t>(0x004d8480u);

// 0x004c5ae0
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
RwTextureSetName(std::uint32_t* param_1, const char* param_2)
{
    std::uint32_t rw_base = *reinterpret_cast<std::uint32_t*>(kRwGlobalsPtr);

    // strncpy(tex+0x10, name, 0x20) via vtable+0xd0, cited 0x004c5ae8
    StrncpyFn_t strncpy_fn = *reinterpret_cast<StrncpyFn_t*>(rw_base + 0xd0u);
    strncpy_fn(reinterpret_cast<std::uint8_t*>(param_1) + 0x10u, param_2, 0x20u);

    // strlen(name) via vtable+0xf4, cited 0x004c5af6
    StrlenFn_t strlen_fn = *reinterpret_cast<StrlenFn_t*>(rw_base + 0xf4u);
    std::uint32_t uVar1 = strlen_fn(param_2);

    if (uVar1 > 0x1fu) {
        // Overflow path: error code 0x8000001e, cited 0x004c5aff
        struct { std::uint32_t a; std::uint32_t b; } stk;
        stk.a = 1u;
        stk.b = s_FUN_004d7ff0(0x8000001eu, param_2, 0x20u, 0x1fu,
                                static_cast<std::int32_t>(
                                    *reinterpret_cast<const std::int8_t*>(param_2 + 0x1fu)));
        s_FUN_004d8480(&stk);
        // Force null-terminator at tex+0x2f, cited 0x004c5b3c
        *reinterpret_cast<std::uint8_t*>(
            reinterpret_cast<std::uintptr_t>(param_1) + 0x2fu) = 0u;
    }

    return param_1;  // cited 0x004c5b40
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(RwTextureSetName, 0x004c5ae0);

// ---------------------------------------------------------------------------
// 0x004c5b50  FUN_004c5b50  RwTextureSetMaskName   (109 bytes)
//
// RwTexture *FUN_004c5b50(RwTexture *tex, const char *maskName)
//   param_1 = texture ptr
//   param_2 = mask name string (C string, max 31 chars)
//   Returns param_1 (tex).
//
// Identical structure to RwTextureSetName; writes to tex+0x30 = mask field.
//
// Body (verbatim decompiler):
//   (**(code **)(DAT_007d3ff8 + 0xd0))(param_1 + 0x30, param_2, 0x20);
//     // strncpy(tex+0x30, maskName, 32), cited 0x004c5b58
//   uVar1 = (**(code **)(DAT_007d3ff8 + 0xf4))(param_2);
//     // strlen(maskName), cited 0x004c5b66
//   if (0x1f < uVar1) {
//     // overflow: error code 0x8000001e, cited 0x004c5b6f
//     uStack_8 = 1;
//     uStack_4 = FUN_004d7ff0(0x8000001e, param_2, 0x20, 0x1f, ...);
//     FUN_004d8480(&uStack_8);
//     *(byte)(param_1 + 0x4f) = 0;  // force null at tex+0x4f, cited 0x004c5bac
//   }
//   return param_1;
//
// tex->mask is char[32] at tex+0x30..+0x4f.
// ---------------------------------------------------------------------------

// 0x004c5b50
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
RwTextureSetMaskName(std::uint32_t* param_1, const char* param_2)
{
    std::uint32_t rw_base = *reinterpret_cast<std::uint32_t*>(kRwGlobalsPtr);

    // strncpy(tex+0x30, maskName, 0x20) via vtable+0xd0, cited 0x004c5b58
    StrncpyFn_t strncpy_fn = *reinterpret_cast<StrncpyFn_t*>(rw_base + 0xd0u);
    strncpy_fn(reinterpret_cast<std::uint8_t*>(param_1) + 0x30u, param_2, 0x20u);

    // strlen(maskName) via vtable+0xf4, cited 0x004c5b66
    StrlenFn_t strlen_fn = *reinterpret_cast<StrlenFn_t*>(rw_base + 0xf4u);
    std::uint32_t uVar1 = strlen_fn(param_2);

    if (uVar1 > 0x1fu) {
        // Overflow path: error code 0x8000001e, cited 0x004c5b6f
        struct { std::uint32_t a; std::uint32_t b; } stk;
        stk.a = 1u;
        stk.b = s_FUN_004d7ff0(0x8000001eu, param_2, 0x20u, 0x1fu,
                                static_cast<std::int32_t>(
                                    *reinterpret_cast<const std::int8_t*>(param_2 + 0x1fu)));
        s_FUN_004d8480(&stk);
        // Force null-terminator at tex+0x4f, cited 0x004c5bac
        *reinterpret_cast<std::uint8_t*>(
            reinterpret_cast<std::uintptr_t>(param_1) + 0x4fu) = 0u;
    }

    return param_1;  // cited 0x004c5bb0
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(RwTextureSetMaskName, 0x004c5b50);

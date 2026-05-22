// Mashed RE — Render/RwPluginHelpers_o3.cpp
// RenderWare plugin-registry + driver-system thin wrappers.
//
// Covers:
//   0x004c2d70  RwPluginRegistryFrozen    — leaf getter: DAT_007d3ff4 (RW registry-frozen gate)
//   0x004c2e70  RwEngineSetSubSystem      — driver-system cmd 0x10 wrapper; bool(int)
//   0x004c2f30  RwEngineSetVideoMode      — driver-system cmd 0x07 wrapper; bool(int)
//   0x004c2ed0  RwEngineGetModeInfo       — driver-system cmd 0x06 wrapper; ptr(ptr,int)
//
// Deferred (anti-island gate fail — callee FUN_004d7de0 is C1):
//   0x004c2d90  FUN_004c2d90              — RwEngineRegisterPlugin shim; promote when FUN_004d7de0 >= C2
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/render_promote_c2_rw_plugin/0x004c2d70.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2e70.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2f30.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2ed0.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2c90.md  (dispatcher callee, C2)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// External callee: FUN_004c2c90 — RW driver-system dispatcher (C2 in hooks.csv).
// Signature: int __cdecl FUN_004c2c90(int ctx_field, int cmd,
//                                      int out_or_in1, int in1, int in2)
// ---------------------------------------------------------------------------
using FUN_004c2c90_t = std::int32_t (__cdecl*)(std::int32_t, std::int32_t,
                                                std::int32_t, std::int32_t,
                                                std::int32_t);
static FUN_004c2c90_t const s_FUN_004c2c90 =
    reinterpret_cast<FUN_004c2c90_t>(0x004c2c90);

// ---------------------------------------------------------------------------
// RwPluginRegistryFrozen  --  0x004c2d70
//
// Original: FUN_004c2d70 (5 bytes, 0x004c2d70..0x004c2d74).
// Decompiled: `return DAT_007d3ff4;`
// Asm: A1 F4 3F 7D 00  MOV EAX,[0x007d3ff4] / C3 RET
//
// Sole caller: FUN_004d7de0 (RwPluginRegistryAddPlugin gate, C1).
// Returns non-zero when the RW plugin registry is frozen (i.e., after
// RwEngineOpen/Start has been called); used as an early-exit guard to
// prevent late registerPlugin calls from corrupting already-assigned offsets.
//
// Leaf-function exemption (CONFIDENCE.md): no callees; Frida A/B bit-identity
// verified against a non-trivial sentinel domain satisfies C2→C3.
// ---------------------------------------------------------------------------

// 0x004c2d70
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwPluginRegistryFrozen() {
    // 0x004c2d70: MOV EAX, [DAT_007d3ff4] — RW plugin-registry frozen gate
    return *reinterpret_cast<const std::uint32_t*>(0x007d3ff4u);
}

RH_ScopedInstall(RwPluginRegistryFrozen, 0x004c2d70);

// ---------------------------------------------------------------------------
// RwEngineSetSubSystem  --  0x004c2e70
//
// Original: FUN_004c2e70 (35 bytes, 0x004c2e70..0x004c2e92).
// Decompiled (verbatim):
//   bool FUN_004c2e70(undefined4 param_1) {
//     int iVar1;
//     iVar1 = FUN_004c2c90(DAT_007d3ff8 + 0x10, 0x10, 0, 0, param_1);
//     return iVar1 != 0;
//   }
//
// Wraps driver-system dispatcher command 0x10 (rwDEVICESYSTEMSETSUBSYSTEM).
// param_1: subsystem index to select (0 = first/only adapter, typical).
// Returns true (success) when dispatcher returns non-zero.
// Dispatcher fallback for cmd 0x10: when param_5==0 → forces success (iVar1!=0).
//
// Callee: FUN_004c2c90 (driver-system dispatcher, C2). Anti-island satisfied.
// Callers: FUN_00498c00 (subsystem enumeration), FUN_00499170, FUN_00499400.
// RW 3.x analog: RwEngineSetSubSystem(RwInt32 idx) → RwBool.
// ---------------------------------------------------------------------------

// 0x004c2e70
extern "C" __declspec(dllexport) std::int32_t __cdecl RwEngineSetSubSystem(std::int32_t param_1) {
    // 0x004c2e70: load DAT_007d3ff8, add 0x10, cmd=0x10, out=0, in1=0, in2=param_1
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    const std::int32_t iVar1 = s_FUN_004c2c90(ctx + 0x10, 0x10, 0, 0, param_1);
    return iVar1 != 0;  // bool cast: non-zero → 1, zero → 0
}

RH_ScopedInstall(RwEngineSetSubSystem, 0x004c2e70);

// ---------------------------------------------------------------------------
// RwEngineSetVideoMode  --  0x004c2f30
//
// Original: FUN_004c2f30 (35 bytes, 0x004c2f30..0x004c2f52).
// Decompiled (verbatim):
//   bool FUN_004c2f30(undefined4 param_1) {
//     int iVar1;
//     iVar1 = FUN_004c2c90(DAT_007d3ff8 + 0x10, 7, 0, 0, param_1);
//     return iVar1 != 0;
//   }
//
// Wraps driver-system dispatcher command 0x7 (rwDEVICESYSTEMUSEMODE).
// param_1: mode index to select.
// Same code shape as RwEngineSetSubSystem (cmd 0x10) but command id differs.
// No fallback for cmd 0x7 in dispatcher: result purely from driver.
//
// Callee: FUN_004c2c90 (driver-system dispatcher, C2). Anti-island satisfied.
// Callers: FUN_00498c00 (enumeration), FUN_00499400 (mode probe).
// RW 3.x analog: RwEngineSetVideoMode(RwInt32 modeIndex) → RwBool.
// ---------------------------------------------------------------------------

// 0x004c2f30
extern "C" __declspec(dllexport) std::int32_t __cdecl RwEngineSetVideoMode(std::int32_t param_1) {
    // 0x004c2f30: load DAT_007d3ff8, add 0x10, cmd=0x07, out=0, in1=0, in2=param_1
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    const std::int32_t iVar1 = s_FUN_004c2c90(ctx + 0x10, 7, 0, 0, param_1);
    return iVar1 != 0;  // bool cast
}

RH_ScopedInstall(RwEngineSetVideoMode, 0x004c2f30);

// ---------------------------------------------------------------------------
// RwEngineGetModeInfo  --  0x004c2ed0
//
// Original: FUN_004c2ed0 (42 bytes, 0x004c2ed0..0x004c2ef9).
// Decompiled (verbatim):
//   undefined4 FUN_004c2ed0(undefined4 param_1, undefined4 param_2) {
//     int iVar1;
//     iVar1 = FUN_004c2c90(DAT_007d3ff8 + 0x10, 6, param_1, 0, param_2);
//     if (iVar1 == 0) { return 0; }
//     return param_1;
//   }
//
// Wraps driver-system dispatcher command 0x6 (rwDEVICESYSTEMGETMODEINFO).
// param_1: out-buffer pointer (receives RwVideoMode struct on success).
// param_2: mode index to query.
// Returns param_1 (the buffer pointer) on success, 0 on failure.
// No dispatcher fallback for cmd 0x6; result purely from driver.
//
// RW 3.x analog: RwEngineGetVideoModeInfo(RwVideoMode *modeInfo, RwInt32 modeIndex).
// Callee: FUN_004c2c90 (driver-system dispatcher, C2). Anti-island satisfied.
// Callers: FUN_00402750 (boot init), FUN_004921d0, FUN_00493710 (RW init),
//          FUN_00498c00 (subsystem enum), FUN_00499400 (mode probe).
// ---------------------------------------------------------------------------

// 0x004c2ed0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwEngineGetModeInfo(
    std::uint32_t param_1, std::int32_t param_2)
{
    // 0x004c2ed0: load DAT_007d3ff8, add 0x10, cmd=0x6, out=param_1, in1=0, in2=param_2
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    const std::int32_t iVar1 = s_FUN_004c2c90(
        ctx + 0x10,
        6,
        static_cast<std::int32_t>(param_1),
        0,
        param_2);
    if (iVar1 == 0) {
        return 0u;
    }
    return param_1;
}

RH_ScopedInstall(RwEngineGetModeInfo, 0x004c2ed0);

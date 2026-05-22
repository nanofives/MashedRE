// Mashed RE — Render/RwPluginHelpers_q1.cpp
// RenderWare driver-system thin wrappers — c3-batch-q-s1 cluster.
//
// Covers (4 promoted; 1 deferred):
//   0x004c2de0  RwEngineGetNumSubSystems   — driver cmd 0x0d; void(void)->uint32
//   0x004c2e10  RwEngineGetSubSystemInfo   — driver cmd 0x0e; uint32(ptr,int)
//   0x004c2e40  RwEngineGetCurrentSubSystem — driver cmd 0x0f; uint32(void)
//   0x004c2ea0  RwEngineGetNumVideoModes   — driver cmd 0x05; uint32(void)
//
// Deferred (anti-island gate fail — callee FUN_004d7de0 is C1):
//   0x004c2d90  FUN_004c2d90              — RwEngineRegisterPlugin shim;
//               promote when FUN_004d7de0 >= C2.
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// All functions share one callee:
//   FUN_004c2c90 (driver-system dispatcher, C2 in hooks.csv)
//   Analysis ref: re/analysis/render_promote_c2_rw_plugin/0x004c2c90.md
//
// Analysis refs:
//   re/analysis/render_promote_c2_rw_plugin/0x004c2de0.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2e10.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2e40.md
//   re/analysis/render_promote_c2_rw_plugin/0x004c2ea0.md

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
// RwEngineGetNumSubSystems  --  0x004c2de0
//
// Original: FUN_004c2de0 (42 bytes, 0x004c2de0..0x004c2e09).
// Decompiled (verbatim):
//   undefined4 FUN_004c2de0(void) {
//     undefined4 local_4;
//     local_4 = 1;
//     FUN_004c2c90(DAT_007d3ff8 + 0x10, 0xd, &local_4, 0, 0);
//     return local_4;
//   }
//
// Wraps driver-system dispatcher command 0x0d (rwDEVICESYSTEMGETNUMSUBSYSTEMS).
// Pre-initializes local_4 = 1 before the call — this is the fallback value:
//   dispatcher case 0xd writes *param_3 = 1 if the indirect callee returns 0.
// The callee's return value is discarded; local_4 holds the answer after the call.
// Returns the count of available display sub-systems (default 1).
//
// Callee: FUN_004c2c90 (driver-system dispatcher, C2). Anti-island satisfied.
// Caller: FUN_00498c00 (VideoModeTableInit, subsystem enumeration entry).
// RW 3.x analog: RwEngineGetNumSubSystems() -> RwInt32.
// ---------------------------------------------------------------------------

// 0x004c2de0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwEngineGetNumSubSystems() {
    // 0x004c2de0: local_4 pre-init = 1 (dispatcher fallback for cmd 0xd)
    std::int32_t local_4 = 1;
    // 0x004c2de0: load DAT_007d3ff8, add 0x10, cmd=0x0d, out=&local_4, in1=0, in2=0
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    s_FUN_004c2c90(ctx + 0x10, 0xd, reinterpret_cast<std::int32_t>(&local_4), 0, 0);
    return static_cast<std::uint32_t>(local_4);
}

RH_ScopedInstall(RwEngineGetNumSubSystems, 0x004c2de0);

// ---------------------------------------------------------------------------
// RwEngineGetSubSystemInfo  --  0x004c2e10
//
// Original: FUN_004c2e10 (42 bytes, 0x004c2e10..0x004c2e39).
// Decompiled (verbatim):
//   undefined4 FUN_004c2e10(undefined4 param_1, undefined4 param_2) {
//     int iVar1;
//     iVar1 = FUN_004c2c90(DAT_007d3ff8 + 0x10, 0xe, param_1, 0, param_2);
//     if (iVar1 == 0) { return 0; }
//     return param_1;
//   }
//
// Wraps driver-system dispatcher command 0x0e (rwDEVICESYSTEMGETSUBSYSTEMINFO).
// param_1: pointer to an RwSubSystemInfo out-buffer (0x50 bytes per analysis note).
// param_2: subsystem index to query.
// Returns param_1 (the buffer pointer) on success, 0 on failure.
// Dispatcher case 0xe special: param_5==0 triggers error callback at
//   *(DAT_007d3ff8+0xCC) with string "Only rendering sub system".
//
// Callee: FUN_004c2c90 (driver-system dispatcher, C2). Anti-island satisfied.
// Caller: FUN_00498c00 (VideoModeTableInit, subsystem enumeration entry).
// RW 3.x analog: RwEngineGetSubSystemInfo(RwSubSystemInfo *info, RwInt32 idx).
// ---------------------------------------------------------------------------

// 0x004c2e10
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwEngineGetSubSystemInfo(
    std::uint32_t param_1, std::int32_t param_2)
{
    // 0x004c2e10: load DAT_007d3ff8, add 0x10, cmd=0x0e, out=param_1, in1=0, in2=param_2
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    const std::int32_t iVar1 = s_FUN_004c2c90(
        ctx + 0x10,
        0xe,
        static_cast<std::int32_t>(param_1),
        0,
        param_2);
    if (iVar1 == 0) {
        return 0u;
    }
    return param_1;
}

RH_ScopedInstall(RwEngineGetSubSystemInfo, 0x004c2e10);

// ---------------------------------------------------------------------------
// RwEngineGetCurrentSubSystem  --  0x004c2e40
//
// Original: FUN_004c2e40 (44 bytes, 0x004c2e40..0x004c2e6b).
// Decompiled (verbatim):
//   undefined4 FUN_004c2e40(void) {
//     int iVar1;
//     undefined4 local_4;
//     iVar1 = FUN_004c2c90(DAT_007d3ff8 + 0x10, 0xf, &local_4, 0, 0);
//     if (iVar1 != 0) { return local_4; }
//     return 0xffffffff;
//   }
//
// Wraps driver-system dispatcher command 0x0f (rwDEVICESYSTEMGETCURRENTSUBSYSTEM).
// local_4 is un-initialized before the call; callee writes it on success path.
// Dispatcher case 0xf fallback: writes *param_3 = 0 if indirect callee returns 0.
// Returns the current sub-system index on success, -1 (0xffffffff) on failure.
//
// Callee: FUN_004c2c90 (driver-system dispatcher, C2). Anti-island satisfied.
// Caller: FUN_00498c00 (VideoModeTableInit, subsystem enumeration entry).
// RW 3.x analog: RwEngineGetCurrentSubSystem() -> RwInt32.
// ---------------------------------------------------------------------------

// 0x004c2e40
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwEngineGetCurrentSubSystem() {
    // 0x004c2e40: load DAT_007d3ff8, add 0x10, cmd=0x0f, out=&local_4, in1=0, in2=0
    std::int32_t local_4;  // no pre-init, matches original
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    const std::int32_t iVar1 = s_FUN_004c2c90(
        ctx + 0x10,
        0xf,
        reinterpret_cast<std::int32_t>(&local_4),
        0,
        0);
    if (iVar1 != 0) {
        return static_cast<std::uint32_t>(local_4);
    }
    return 0xffffffffu;  // -1 sentinel: dispatcher refused / driver not available
}

RH_ScopedInstall(RwEngineGetCurrentSubSystem, 0x004c2e40);

// ---------------------------------------------------------------------------
// RwEngineGetNumVideoModes  --  0x004c2ea0
//
// Original: FUN_004c2ea0 (44 bytes, 0x004c2ea0..0x004c2ecb).
// Decompiled (verbatim):
//   undefined4 FUN_004c2ea0(void) {
//     int iVar1;
//     undefined4 local_4;
//     iVar1 = FUN_004c2c90(DAT_007d3ff8 + 0x10, 5, &local_4, 0, 0);
//     if (iVar1 != 0) { return local_4; }
//     return 0xffffffff;
//   }
//
// Wraps driver-system dispatcher command 0x05 (rwDEVICESYSTEMGETNUMMODES).
// Same shape as RwEngineGetCurrentSubSystem (cmd 0x0f) but cmd id differs.
// No dispatcher fallback for cmd 0x05 (no explicit case label for 0x05 in
//   FUN_004c2c90 switch — falls to default error sink on indirect-callee failure).
// local_4 is meaningful only if the indirect callee returns non-zero.
// Returns mode count on success, -1 (0xffffffff) on failure.
//
// Callee: FUN_004c2c90 (driver-system dispatcher, C2). Anti-island satisfied.
// Caller: FUN_00498c00 (VideoModeTableInit, sizes the video-mode loop).
// RW 3.x analog: RwEngineGetNumVideoModes() -> RwInt32.
// ---------------------------------------------------------------------------

// 0x004c2ea0
extern "C" __declspec(dllexport) std::uint32_t __cdecl RwEngineGetNumVideoModes() {
    // 0x004c2ea0: load DAT_007d3ff8, add 0x10, cmd=0x05, out=&local_4, in1=0, in2=0
    std::int32_t local_4;  // no pre-init, matches original
    const std::int32_t ctx =
        *reinterpret_cast<const std::int32_t*>(0x007d3ff8u);
    const std::int32_t iVar1 = s_FUN_004c2c90(
        ctx + 0x10,
        5,
        reinterpret_cast<std::int32_t>(&local_4),
        0,
        0);
    if (iVar1 != 0) {
        return static_cast<std::uint32_t>(local_4);
    }
    return 0xffffffffu;  // -1 sentinel: driver refused
}

RH_ScopedInstall(RwEngineGetNumVideoModes, 0x004c2ea0);

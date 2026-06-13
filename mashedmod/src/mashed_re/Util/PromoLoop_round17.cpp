// Mashed RE — promote-round round 17 (fresh discovery: single-global leaves).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00496920  TimerTable772ffcGet — util; indexed getter (stride 4)
//   0x00496930  TimerTable773030Get — util; indexed getter (stride 4)
//   0x00485360  DynObjListGetCount  — vehicle; constant getter
//   0x00550790  FsManager7dc76cSet  — util; single-global setter
//
// All bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12.
// Callers confirmed C2 via plates + a round-17 Ghidra reference_to pass
// (Mashed_pool2 read-only): 00496920<-00492d30 TimerTick; 00496930<-004926c0;
// 00485360<-004694e0 VehicleObjectContactSolver; 00550790<-004955d0 (+ 004b67a0).
//
// Analysis:
//   re/analysis/skeleton_prep_boot_winmain_b/00496920.md
//   re/analysis/timer_d2/0x00496930.md
//   re/analysis/bucket_vehicle_004820e0_00485420/00485360.md
//   re/analysis/input_dinput_d3/00550790.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// TimerTable772ffcGet  --  0x00496920   (subsystem: util)
//
// Original: FUN_00496920 (12 bytes, 0x00496920..0x0049692b)
// Bytes: 8B 44 24 04 / 8B 04 85 FC 2F 77 00 / C3
//   (mov eax,[esp+4]; mov eax,[eax*4 + 0x00772ffc]; ret)
//   Scaled-index addressing: 4-byte elements at base + param_1*4.
// Signature: undefined4 FUN_00496920(int param_1)
//
// No bounds check (caller TimerTick uses index 5).
//
// Constants (cited from function body at 0x00496920):
//   0x00772ffc — indexed table base (4-byte elements)
//
// Caller: FUN_00492d30 TimerTick (util, C2).
//
// Uncertainties (non-blocking):
//   U-1143: table element count / type (data-semantic).
// ---------------------------------------------------------------------------

// 0x00496920
extern "C" __declspec(dllexport) std::uint32_t __cdecl TimerTable772ffcGet(int param_1) {
    // 0x00772ffc base + *4 scaled index cited at 0x00496920 body.
    return *reinterpret_cast<const std::uint32_t*>(
        0x00772ffcu + static_cast<std::uint32_t>(param_1) * 4u);
}

RH_ScopedInstall(TimerTable772ffcGet, 0x00496920);

// ---------------------------------------------------------------------------
// TimerTable773030Get  --  0x00496930   (subsystem: util)
//
// Original: FUN_00496930 (12 bytes, 0x00496930..0x0049693b)
// Bytes: 8B 44 24 04 / 8B 04 85 30 30 77 00 / C3
//   (mov eax,[esp+4]; mov eax,[eax*4 + 0x00773030]; ret)
// Signature: undefined4 FUN_00496930(int param_1)
//
// Complement of TimerTable772ffcGet (base 0x34 bytes higher). No bounds check
// (caller uses indices 3/4).
//
// Constants (cited from function body at 0x00496930):
//   0x00773030 — indexed table base (4-byte elements)
//
// Caller: FUN_004926c0 (boot, C2).
//
// Uncertainties (non-blocking):
//   U-1144: table element count / type (data-semantic).
// ---------------------------------------------------------------------------

// 0x00496930
extern "C" __declspec(dllexport) std::uint32_t __cdecl TimerTable773030Get(int param_1) {
    // 0x00773030 base + *4 scaled index cited at 0x00496930 body.
    return *reinterpret_cast<const std::uint32_t*>(
        0x00773030u + static_cast<std::uint32_t>(param_1) * 4u);
}

RH_ScopedInstall(TimerTable773030Get, 0x00496930);

// ---------------------------------------------------------------------------
// DynObjListGetCount  --  0x00485360   (subsystem: vehicle)
//
// Original: FUN_00485360 (6 bytes, 0x00485360..0x00485365)
// Bytes: A1 F8 A0 6F 00 / C3   (mov eax,[0x006fa0f8]; ret)
// Signature: undefined4 FUN_00485360(void)
//
// DynamicObjectList_GetCount — count sibling of DynamicObjectListGetBase
// (0x00485370, promoted round 11).
//
// Constants (cited from function body at 0x00485360):
//   0x006fa0f8 — dynamic-object list count global
//
// Caller: FUN_004694e0 VehicleObjectContactSolver (vehicle, C2).
//
// Uncertainties (non-blocking):
//   U-3575 (prior): list entry struct (data-semantic).
// ---------------------------------------------------------------------------

// 0x00485360
extern "C" __declspec(dllexport) std::uint32_t __cdecl DynObjListGetCount(void) {
    // 0x006fa0f8 cited at 0x00485360 body.
    return *reinterpret_cast<const std::uint32_t*>(0x006fa0f8u);
}

RH_ScopedInstall(DynObjListGetCount, 0x00485360);

// ---------------------------------------------------------------------------
// FsManager7dc76cSet  --  0x00550790   (subsystem: util)
//
// Original: FUN_00550790 (10 bytes, 0x00550790..0x00550799)
// Bytes: 8B 44 24 04 / A3 6C C7 7D 00 / C3
//   (mov eax,[esp+4]; mov [0x007dc76c],eax; ret)
// Signature: void FUN_00550790(undefined4 param_1)
//
// Constants (cited from function body at 0x00550790):
//   0x007dc76c — FS-manager block field (+0x18 from list-head 0x007dc754)
//
// Callers: FUN_004955d0 (boot, C2), FUN_004b67a0 (render, C2).
//
// Uncertainties (non-blocking):
//   U-2593: field role within the 0x007dc7xx block (data-semantic).
// ---------------------------------------------------------------------------

// 0x00550790
extern "C" __declspec(dllexport) void __cdecl FsManager7dc76cSet(std::uint32_t param_1) {
    // 0x007dc76c cited at 0x00550790 body.
    *reinterpret_cast<std::uint32_t*>(0x007dc76cu) = param_1;
}

RH_ScopedInstall(FsManager7dc76cSet, 0x00550790);

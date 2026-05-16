// Mashed RE - Per-frame game-logic dispatch entry + 5 direct callees.
// Session ma2-frida-s5 — C2->C3 promotions for the FrameDispatch fanout.
//
// FrameDispatch (0x00492e90) is the per-frame game-logic tick. The 5 small
// helpers below are direct callees as catalogued in
// re/analysis/physics_collision/00492e90.md (callees_depth1 yaml block).
//
// Naming: PascalCase exports; comment cites the RVA per hook-author convention.

#include "../Core/HookSystem.h"
#include <cstdint>


// ─── 0x0042b8d0  StatePhaseIsIdle ─────────────────────────────────────────────
// 13 bytes (0x0042b8d0..0x0042b8dc).
// Decompilation: `return DAT_0067eca4 == 0;` (Ghidra: bool getter, no callees).
// Used as a state-phase gate in STATE_FN cases 1 and 6.
// ref: re/analysis/game_state/0x0042b8d0.md
static constexpr std::uintptr_t kStatePhase_0067eca4 = 0x0067eca4u;

// 0x0042b8d0
extern "C" __declspec(dllexport) int __cdecl StatePhaseIsIdle() {
    return (*reinterpret_cast<const std::uint32_t*>(kStatePhase_0067eca4) == 0u) ? 1 : 0;
}

RH_ScopedInstall(StatePhaseIsIdle, 0x0042b8d0);


// ─── 0x0042b8f0  StatePhaseIsFinal ────────────────────────────────────────────
// 14 bytes (0x0042b8f0..0x0042b8fd).
// Decompilation: `return DAT_0067eca4 == 5;` (Ghidra: bool getter, no callees).
// Used as a state-phase gate in STATE_FN case 4 (if non-zero, sets
// DAT_00771968 = 6, returns 1).
// ref: re/analysis/game_state/0x0042b8f0.md

// 0x0042b8f0
extern "C" __declspec(dllexport) int __cdecl StatePhaseIsFinal() {
    return (*reinterpret_cast<const std::uint32_t*>(kStatePhase_0067eca4) == 5u) ? 1 : 0;
}

RH_ScopedInstall(StatePhaseIsFinal, 0x0042b8f0);


// ─── 0x0042f510  Vehicle0HandleGet ────────────────────────────────────────────
// 5 bytes (0x0042f510..0x0042f514).
// Decompilation: `return DAT_0067f190;` (Ghidra: single MOV-from-mem + RET).
// Called from FUN_004671a0 vehicle-0 getter path; returns the vehicle-0
// handle when game mode == 3.
// ref: re/analysis/track_loader_d2/0042f510.md
static constexpr std::uintptr_t kVehicle0Handle_0067f190 = 0x0067f190u;

// 0x0042f510
extern "C" __declspec(dllexport) std::uint32_t __cdecl Vehicle0HandleGet() {
    return *reinterpret_cast<const std::uint32_t*>(kVehicle0Handle_0067f190);
}

RH_ScopedInstall(Vehicle0HandleGet, 0x0042f510);


// ─── 0x00498bf0  DisplayActiveFlagGet ─────────────────────────────────────────
// 5 bytes (0x00498bf0..0x00498bf4).
// Decompilation: `return DAT_00773204;` (Ghidra: single MOV-from-mem + RET).
// Caller FUN_004951f0 checks: if non-zero, calls ShowCursor(0).
// Called from FUN_00492e90 per-frame render dispatch.
// ref: re/analysis/promote_c2_video_display/00498bf0.md
static constexpr std::uintptr_t kDisplayActive_00773204 = 0x00773204u;

// 0x00498bf0
extern "C" __declspec(dllexport) std::uint32_t __cdecl DisplayActiveFlagGet() {
    return *reinterpret_cast<const std::uint32_t*>(kDisplayActive_00773204);
}

RH_ScopedInstall(DisplayActiveFlagGet, 0x00498bf0);


// ─── 0x004c19f0  RwVtableSlot07Call ───────────────────────────────────────────
// 10 bytes (0x004c19f0..0x004c19f9).
// Decompilation: `(**(code **)(param_1 + 0x1c))();` (Ghidra: indirect vtable
// dispatch through slot 7 — 0x1c = 7 * 4 bytes). No static callees; Ghidra
// warns "Could not recover jumptable at 0x004c19f8".
// Caller FUN_00495350 passes a render-target handle as param_1.
// Called from FUN_00492e90 per-frame render dispatch.
// ref: re/analysis/intro_splash/0x004c19f0.md
// vtable slot 7 (offset 0x1c from object base).
static constexpr std::uintptr_t kVtableSlot7Offset = 0x1cu;

// 0x004c19f0
extern "C" __declspec(dllexport) void __cdecl RwVtableSlot07Call(int param_1) {
    using VFn = void (*)();
    const VFn fn = *reinterpret_cast<VFn*>(
        static_cast<std::uintptr_t>(static_cast<std::uint32_t>(param_1)) + kVtableSlot7Offset);
    fn();
}

RH_ScopedInstall(RwVtableSlot07Call, 0x004c19f0);

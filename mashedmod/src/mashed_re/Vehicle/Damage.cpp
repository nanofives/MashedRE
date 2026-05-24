// Mashed RE - Vehicle damage cluster (c3_batch_j4): 4 functions.
//
// Promoted candidates from Session 4 of c3_batch_j:
//   0x00419760  VehicleEliminationSlotInit       (vehicle_damage_d4)
//   0x00420de0  VehicleDamageAccumulatorAdd      (vehicle_damage_d4)
//   0x00418de0  VehicleEliminationSlotPostInit   (vehicle_damage_d4, EAX-implicit)
//   0x00467300  VehicleCollisionWinTrigger       (vehicle_promote_c2_b)
//
// Analysis sources:
//   re/analysis/vehicle_damage_d4/0x00419760.md
//   re/analysis/vehicle_damage_d4/0x00420de0.md
//   re/analysis/vehicle_damage_d4/0x00418de0.md
//   re/analysis/vehicle_dynamics/00467300.md
//   re/analysis/vehicle_promote_c2_b/00467300.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ============================================================================
// Cross-callee declarations (resolved at link time via inline-JMP into
// MASHED.exe — both candidates and externals share the same trampolines).
// ============================================================================

// 0x00418a30  FUN_00418a30 — spectator-slot reset (EAX-implicit 'this'; C2)
//   Custom calling convention: EAX is the implicit pointer arg.
//   We invoke via the raw RVA address since calling-conv is non-standard.
//   When the EAX-implicit hooks are installed, the inline-JMP at 0x00418a30
//   takes over; otherwise the original body runs. Either way, EAX must be
//   loaded by the caller.
using EaxImplicitVoid_t = void(__cdecl*)();
static constexpr std::uintptr_t kFUN_00418a30 = 0x00418a30u;

// 0x00418de0 — also implemented in this file (see VehicleEliminationSlotPostInit).
static constexpr std::uintptr_t kFUN_00418de0 = 0x00418de0u;

// 0x00413c70  FUN_00413c70 — race-event/state writer (C2)
//   void(int, int, undefined4)
using FUN_00413c70_t = void(__cdecl*)(int, int, std::uint32_t);
static FUN_00413c70_t const g_FUN_00413c70 =
    reinterpret_cast<FUN_00413c70_t>(0x00413c70u);

// 0x004661a0  FUN_004661a0 — per-entity slot-stop audio (C1)
//   void(int, int)
using FUN_004661a0_t = void(__cdecl*)(int, int);
static FUN_004661a0_t const g_FUN_004661a0 =
    reinterpret_cast<FUN_004661a0_t>(0x004661a0u);


// ============================================================================
// Globals cited by the function bodies (all .bss, no allocation).
// ============================================================================

// 0x00419760 globals:
static constexpr std::uintptr_t kDAT_005f3298  = 0x005f3298u; // skin/colour handle LUT
static constexpr std::uintptr_t kDAT_007f1a1c  = 0x007f1a1cu; // car->skin-index mapping
static constexpr std::uintptr_t kDAT_007f1a14  = 0x007f1a14u; // car->player-slot mapping
static constexpr std::uintptr_t kDAT_0063c018  = 0x0063c018u; // spectator-data region (stride 0x6c)
static constexpr std::uintptr_t kDAT_0063c028  = 0x0063c028u; // spectator slot +0x10 (skin handle field)
static constexpr std::uintptr_t kDAT_0063c04c  = 0x0063c04cu; // per-car status array (stride 0x6c)
static constexpr std::uintptr_t kDAT_0063c06c  = 0x0063c06cu; // spectator slot +0x54 (player-slot field)
static constexpr std::uintptr_t kDAT_0063bf30  = 0x0063bf30u; // linked-list head (stride 0x54)
static constexpr std::uintptr_t kDAT_0063bfd8  = 0x0063bfd8u; // linked-list end bound

// 0x00467300 globals:
static constexpr std::uintptr_t kDAT_00881F90  = 0x00881F90u; // vehicle state stride 0x341
static constexpr std::uintptr_t kDAT_008815A4  = 0x008815A4u; // vehicle contact stride 0x341
static constexpr std::uintptr_t kDAT_007f0fd4  = 0x007f0fd4u; // winning vehicle index
static constexpr std::uintptr_t kDAT_007f0fdc  = 0x007f0fdcu; // collision countdown timer


// ============================================================================
// 0x00420de0  VehicleDamageAccumulatorAdd
//
// Original 30 bytes. __fastcall(ECX, EDX, stack):
//   void __fastcall FUN_00420de0(int param_1, int param_2, float param_3);
//   ECX = param_1 (index), EDX = param_2 (float array base), stack = delta.
//
// Body (Ghidra decomp, cited at 0x00420de0):
//   float v = *(float*)(param_2 + param_1*4) + param_3;
//   *(float*)(param_2 + param_1*4) = v;
//   if (v > _DAT_005cd120) {  // _DAT_005cd120 = 0x42480000 = 50.0f
//       *(float*)(param_2 + param_1*4) = 50.0f;
//   }
//
// MSVC __fastcall passes args 1 and 2 in ECX/EDX. Subsequent args on stack.
// MSVC decorates as @VehicleDamageAccumulatorAdd@N; we /export alias the
// undecorated name for the Frida harness.
// ============================================================================

// _DAT_005cd120: ceiling constant 50.0f (cited at 0x00420df1).
static constexpr std::uintptr_t kCeilingFloatGlobal = 0x005cd120u;

#pragma comment(linker, "/export:VehicleDamageAccumulatorAdd=@VehicleDamageAccumulatorAdd@12")

// 0x00420de0
extern "C" __declspec(dllexport)
void __fastcall VehicleDamageAccumulatorAdd(int param_1, int param_2, float param_3)
{
    // Body cited at 0x00420de0..0x00420dfa.
    float* slot = reinterpret_cast<float*>(param_2 + param_1 * 4);
    float new_val = *slot + param_3;
    *slot = new_val;
    float ceiling = *reinterpret_cast<const float*>(kCeilingFloatGlobal);  // 0x42480000 = 50.0f
    if (new_val > ceiling) {
        // Cited at 0x00420df7: writes 0x42480000 (50.0f) back.
        *slot = ceiling;
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(VehicleDamageAccumulatorAdd, 0x00420de0);


// ============================================================================
// 0x00418de0  VehicleEliminationSlotPostInit
//
// Original 72 bytes. EAX-implicit 'this' (Ghidra in_EAX), no declared params.
//
// Body (Ghidra decomp, cited at 0x00418de0..0x00418e28):
//   *(int*)(EAX+0x44) = 0;
//   *(int*)(EAX+0x48) = 0;
//   *(int*)(EAX+0x58) = 0;
//   *(int*)(EAX+0x5c) = 0;
//   *(int*)(EAX+0x50) = 0;
//   *(int*)(EAX+0x54) = 0;
//   FUN_00413c70(*(undefined4*)(EAX+0x4c), 2, 0);
//   FUN_004661a0(0x14, 0);
//   FUN_004661a0(0x14, 1);
//   FUN_004661a0(0x14, 2);
//   FUN_004661a0(0x14, 3);
//
// Naked entry: capture EAX into a local before any prologue clobber.
// Body is __cdecl helper with explicit EAX-as-arg.
// ============================================================================

static void __cdecl VehicleEliminationSlotPostInit_Body(std::uintptr_t this_ptr)
{
    // Zero the six fields. Cited byte offsets: +0x44, +0x48, +0x58, +0x5c,
    // +0x50, +0x54 (the decomp groups +0x44/+0x48 then +0x58/+0x5c then
    // +0x50/+0x54; we follow the same order for any potential write-watch
    // observability).
    *reinterpret_cast<std::uint32_t*>(this_ptr + 0x44) = 0;
    *reinterpret_cast<std::uint32_t*>(this_ptr + 0x48) = 0;
    *reinterpret_cast<std::uint32_t*>(this_ptr + 0x58) = 0;
    *reinterpret_cast<std::uint32_t*>(this_ptr + 0x5c) = 0;
    *reinterpret_cast<std::uint32_t*>(this_ptr + 0x50) = 0;
    *reinterpret_cast<std::uint32_t*>(this_ptr + 0x54) = 0;

    // Cited at 0x00418e08: FUN_00413c70(*(int*)(EAX+0x4c), 2, 0).
    int p1 = *reinterpret_cast<int*>(this_ptr + 0x4c);
    g_FUN_00413c70(p1, 2, 0);

    // Cited at 0x00418e12..0x00418e24: 4 calls with (0x14, 0..3).
    g_FUN_004661a0(0x14, 0);
    g_FUN_004661a0(0x14, 1);
    g_FUN_004661a0(0x14, 2);
    g_FUN_004661a0(0x14, 3);
}

// 0x00418de0
// EAX carries the implicit pointer arg (in_EAX). Capture it, then call body.
// The harness uses 'eax_implicit_ptr' arg_type to seed EAX via a trampoline.
extern "C" __declspec(dllexport) __declspec(naked)
void __cdecl VehicleEliminationSlotPostInit()
{
    __asm {
        // Save callee-saved registers we might clobber inside Body (none required
        // by the original 0x00418de0, but the C body uses standard cdecl frame).
        push    ebp
        mov     ebp, esp
        push    eax                                       // arg0 = this_ptr (from EAX)
        call    VehicleEliminationSlotPostInit_Body
        add     esp, 4
        mov     esp, ebp
        pop     ebp
        ret
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(VehicleEliminationSlotPostInit, 0x00418de0);


// ============================================================================
// 0x00419760  VehicleEliminationSlotInit
//
// Original 112 bytes. Standard __cdecl(int, int).
//   void FUN_00419760(int param_1, int param_2);
//     param_1 = car index 0..15
//     param_2 = flag (0 = reset only; non-0 = reset + post-init + link)
//
// Body (Ghidra decomp, cited at 0x00419760..0x004197d0):
//   iVar4 = param_1 * 0x6c;
//   if (param_2 == 0) {
//       FUN_00418a30();  // EAX = &DAT_0063c018 + iVar4 (slot base)
//       return;
//   }
//   // param_2 != 0 path
//   FUN_00418a30();  // EAX = &DAT_0063c018 + iVar4
//   FUN_00418de0();  // EAX = &DAT_0063c018 + iVar4
//   uVar1 = *(undefined4*)(&DAT_005f3298 + (&DAT_007f1a1c)[param_1*4] * 4);
//   uVar2 = (&DAT_007f1a14)[param_1*4];
//   (&DAT_0063c04c)[param_1 * 0x1b] = 2;
//   *(undefined4*)(&DAT_0063c028 + iVar4) = uVar1;
//   *(undefined4*)(&DAT_0063c06c + iVar4) = uVar2;
//   // walk linked list at DAT_0063bf30, stride 0x54, find slot+0x50 == 0
//   for (node = DAT_0063bf30; node < 0x0063bfd8; node += 0x54) {
//       if (*(int*)(node + 0x50) == 0) {
//           *(int*)(node + 0x50) = &DAT_0063c018 + iVar4;
//           break;
//       }
//   }
//
// FUN_00418a30 and FUN_00418de0 take EAX as 'this'. We seed EAX via inline
// asm before each call. (FUN_00418de0 is also in this file but lives at a
// fixed RVA; we go through the RVA so the inline-JMP can intercept.)
// ============================================================================

// 0x00419760
extern "C" __declspec(dllexport) void __cdecl VehicleEliminationSlotInit(int param_1, int param_2)
{
    // Cited at 0x00419763.
    std::uintptr_t iVar4 = static_cast<std::uintptr_t>(param_1) * 0x6cu;
    std::uintptr_t slot_base = kDAT_0063c018 + iVar4;

    // param_2 == 0 path: reset-only.
    if (param_2 == 0) {
        // FUN_00418a30() with EAX = slot_base. Inline asm seeds EAX then calls.
        __asm {
            mov     eax, slot_base
            call    kFUN_00418a30
        }
        return;
    }

    // param_2 != 0 path. Cited at 0x00419774..end.
    __asm {
        mov     eax, slot_base
        call    kFUN_00418a30
    }
    __asm {
        mov     eax, slot_base
        call    kFUN_00418de0
    }

    // Cited at 0x00419788: skin/colour handle read.
    std::uint32_t skin_idx = reinterpret_cast<const std::uint32_t*>(kDAT_007f1a1c)[param_1 * 4u];
    std::uint32_t uVar1    = *reinterpret_cast<const std::uint32_t*>(kDAT_005f3298 + skin_idx * 4u);

    // Cited at 0x0041979a.
    std::uint32_t uVar2 = reinterpret_cast<const std::uint32_t*>(kDAT_007f1a14)[param_1 * 4u];

    // Cited at 0x004197a3: status field at stride 0x1b dwords (= 0x6c bytes).
    reinterpret_cast<std::uint32_t*>(kDAT_0063c04c)[param_1 * 0x1bu] = 2u;

    // Cited at 0x004197ad / 0x004197b6: writes to slot.
    *reinterpret_cast<std::uint32_t*>(kDAT_0063c028 + iVar4) = uVar1;
    *reinterpret_cast<std::uint32_t*>(kDAT_0063c06c + iVar4) = uVar2;

    // Cited at 0x004197bf..0x004197ca: linked-list walk; max 4 nodes.
    // Loop while node < 0x0063bfd8 (one-past-end). Stride 0x54.
    for (std::uintptr_t node = kDAT_0063bf30; node < kDAT_0063bfd8; node += 0x54u) {
        // Cited at 0x004197c3: read +0x50.
        if (*reinterpret_cast<std::uint32_t*>(node + 0x50u) == 0u) {
            // Cited at body: writes slot_base into node+0x50.
            *reinterpret_cast<std::uintptr_t*>(node + 0x50u) = slot_base;
            return;
        }
    }
}

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(VehicleEliminationSlotInit, 0x00419760);


// ============================================================================
// 0x00467300  VehicleCollisionWinTrigger
//
// Original 73 bytes. void(int param_1) where param_1 = vehicle index.
//
// Body (Ghidra decomp, cited at 0x00467300..0x00467349):
//   if ((&DAT_00881F90)[param_1 * 0x341] != 0) return;   // CPU vehicle guard
//   if ((&DAT_008815A4)[param_1 * 0x341] == 0) return;   // no active contact
//   for (i = 0; i < 4; i++) FUN_00413c70(i, 3, 0);
//   DAT_007f0fd4 = param_1;
//   DAT_007f0fdc = 6000;
//
// All cited at the canonical_analysis (vehicle_dynamics/00467300.md).
// Callee FUN_00413c70 is C2 mapped.
// U-2632 catalogued (effect-code-3 semantic unknown — non-blocking, raw arg).
// ============================================================================

// Per-vehicle stride for state arrays at DAT_00881F90 / DAT_008815A4 (cited
// across vehicle_dynamics plates).
static constexpr std::uint32_t kVehicleDWordStride = 0x341u;

// 0x00467300
extern "C" __declspec(dllexport) void __cdecl VehicleCollisionWinTrigger(int param_1)
{
    // Cited at 0x00467300 body entry: player guard.
    if (reinterpret_cast<const std::uint32_t*>(kDAT_00881F90)[param_1 * kVehicleDWordStride] != 0u) {
        return;
    }
    // Cited at body: active-contact guard.
    if (reinterpret_cast<const std::uint32_t*>(kDAT_008815A4)[param_1 * kVehicleDWordStride] == 0u) {
        return;
    }

    // Cited at body: 4-channel effect trigger; arg pattern (i, 3, 0).
    for (int i = 0; i < 4; ++i) {
        g_FUN_00413c70(i, 3, 0);
    }

    // Cited at body: write winning vehicle index + countdown timer.
    *reinterpret_cast<std::uint32_t*>(kDAT_007f0fd4) = static_cast<std::uint32_t>(param_1);
    *reinterpret_cast<std::uint32_t*>(kDAT_007f0fdc) = 6000u;
}

RH_ScopedInstall(VehicleCollisionWinTrigger, 0x00467300);  // re-enabled 2026-05-24 c3-vehicle

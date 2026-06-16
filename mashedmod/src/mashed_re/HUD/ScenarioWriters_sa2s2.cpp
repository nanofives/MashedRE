// Mashed RE — scenario-attach writer reimplementations (c3-batch-sa2 session 2).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// These five functions write per-vehicle / race-state globals that are ZERO at
// the quiescent main menu, so a menu-attach A/B gives a false-GREEN
// (INCONCLUSIVE-DEGENERATE). They are verified attached inside a LIVE Quick
// Battle race (scenario:'race' in hooks_registry.py). See
// re/analysis/scenario_attach_lane.md and the probe snapshot
// re/analysis/scenario_attach_probe_2026-06-16.json.
//
// Every reimplementation is a verbatim transcription of the Ghidra
// decompilation (read read-only from Mashed_pool10 on 2026-06-16, cross-checked
// against the listing disassembly) and matches the original __cdecl frames so
// the inline-JMP redirect lines up. RVA + body range cited per function.
//
// INSTALLED + VERIFIED (void_write_observe, scenario:'race'):
//   0x004331a0  RaceFinalizeOnce         — guarded (DAT_0067eca4==0) one-shot
//               race-end init: 4 floats @0x0067ebb0..bc = 210.0f (0x43520000),
//               param->0x0067ecac, zero-clears, calls FUN_0042d3a0/004248b0/
//               00424920. Observe 0x0067ebb0 (=0x43520000). Guard SEEDED to 0.
//   0x00415020  AiLastPlaceFrustration   — int f(int car); last-place catch-up
//               timer. Writes leader-progress float @0x0089a4e8+car*0x74 and the
//               timer @0x0089a4e4+car*0x74. Observe 0x0089a4e8 (car 0).
//   0x004922e0  CarEventTrigger          — void f(int car,u4,u4,u4); guarded
//               per-player pending-event record write into the 0x007f1058 block.
//               Observe 0x007f1058 (slot-0 trigger flag).
//   0x00422b50  VehicleDamageAccum       — void f(int car,int delta); accumulates
//               delta into DAT_008995bc[car*0x4e] (stride 0x138 bytes). Observe
//               0x008995bc (car 0); slot SEEDED to 0 -> readback = sentinel+delta.
//   0x00401340  CupSpinSpeedAndColor     — void f(void); DAT_00636574 =
//               (float)(-(int)DAT_007f0ff0) * DAT_005cc328; then writes a mode-
//               selected ARGB color to *(FUN_004b4080(DAT_00636564)+4). Observe
//               0x00636574.
//
// Analysis notes:
//   re/analysis/game_state_d2/0x004331a0.md
//   re/analysis/ai_update_d2/0x00415020.md
//   re/analysis/bucket_vehicle_004922e0_0057c500/0x004922e0.md
//   re/analysis/ai_update_d6/0x00422b50.md
//   re/analysis/boot_hud_promote_ae1/0x00401340.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Forward declarations for callees (RVAs into MASHED.exe). Each call below
// dispatches to the REAL original address so the diff stays bit-identical.
// ---------------------------------------------------------------------------

// 0x0042d3a0  FUN_0042d3a0 — zeroes 13-entry 64-byte struct array at 0x0067ed78. [C3]
static auto* const s_F0042d3a0 = reinterpret_cast<void(__cdecl*)(void)>(0x0042d3a0);
// 0x004248b0  FUN_004248b0 — per-car snapshot init. [C3]
static auto* const s_F004248b0 = reinterpret_cast<void(__cdecl*)(void)>(0x004248b0);
// 0x00424920  FUN_00424920 — end-of-round accumulator. [C3]
static auto* const s_F00424920 = reinterpret_cast<void(__cdecl*)(void)>(0x00424920);

// 0x00442cc0  FUN_00442cc0 — AI vehicle progress getter (idx -> float, x87 ST0). [C3]
static auto* const s_F00442cc0 = reinterpret_cast<float(__cdecl*)(int)>(0x00442cc0);
// 0x0040e470  FUN_0040e470 — vehicle/car-slot state getter (idx -> int). [C3]
static auto* const s_F0040e470 = reinterpret_cast<int(__cdecl*)(int)>(0x0040e470);
// 0x0040e350  FUN_0040e350 — no-arg game-state getter (int). [C3]
static auto* const s_F0040e350 = reinterpret_cast<int(__cdecl*)(void)>(0x0040e350);

// 0x00472650  FUN_00472650 — random float in [lo,hi) (lo,hi -> float, x87 ST0). [C2]
static auto* const s_F00472650 = reinterpret_cast<float(__cdecl*)(int, int)>(0x00472650);
// 0x004a2c48  FUN_004a2c48 — FPU float(ST0)-to-int with rounding (no stack args). [C2]
static auto* const s_F004a2c48 = reinterpret_cast<int(__cdecl*)(void)>(0x004a2c48);

// 0x004b4080  FUN_004b4080 — resolves a clump/object handle to a pointer (int -> int). [C2]
static auto* const s_F004b4080 = reinterpret_cast<int(__cdecl*)(int)>(0x004b4080);
// 0x0042f6a0  FUN_0042f6a0 — GetRaceSubMode (int). [C3]
static auto* const s_F0042f6a0 = reinterpret_cast<int(__cdecl*)(void)>(0x0042f6a0);

// ---------------------------------------------------------------------------
// Global addresses (cited from the decompilation / listing disassembly).
// ---------------------------------------------------------------------------

// 0x004331a0 group
static const std::uintptr_t k_0067eca4 = 0x0067eca4u; // guard (==0 gate; set to 1)
static const std::uintptr_t k_0067ebb0 = 0x0067ebb0u; // 4 floats -> 210.0f (0x43520000)
static const std::uintptr_t k_0067ebb4 = 0x0067ebb4u;
static const std::uintptr_t k_0067ebb8 = 0x0067ebb8u;
static const std::uintptr_t k_0067ebbc = 0x0067ebbcu;
static const std::uintptr_t k_0067ecac = 0x0067ecacu; // = param_1
static const std::uintptr_t k_0067ebc8 = 0x0067ebc8u;
static const std::uintptr_t k_0067ec20 = 0x0067ec20u;
static const std::uintptr_t k_0067ebcc = 0x0067ebccu;
static const std::uintptr_t k_0067e844 = 0x0067e844u;
static const std::uintptr_t k_0067e9f8 = 0x0067e9f8u;
static const std::uintptr_t k_0067e914 = 0x0067e914u;
static const std::uintptr_t k_0067ec24 = 0x0067ec24u;
static const std::uintptr_t k_0067ea6c = 0x0067ea6cu;
static const std::uintptr_t k_0067ebd0 = 0x0067ebd0u; // 20-dword (80-byte) zero block base
static const std::uintptr_t k_0067ea08 = 0x0067ea08u;

// 0x00415020 group
static const std::uintptr_t k_005d757c = 0x005d757cu; // 0.0 last-place sentinel
static const std::uintptr_t k_005cd088 = 0x005cd088u; // leader-far-ahead threshold
static const std::uintptr_t k_005cc320 = 0x005cc320u; // leader-at-max threshold
static const std::uintptr_t k_007f1008 = 0x007f1008u; // per-frame timer delta
static const std::uintptr_t k_0089a4e4 = 0x0089a4e4u; // timer base (+car*0x74)
static const std::uintptr_t k_0089a4e8 = 0x0089a4e8u; // leader-progress float base (+car*0x74)

// 0x004922e0 group
static const std::uintptr_t k_007f1a14 = 0x007f1a14u; // car->player map (dword stride 4)
static const std::uintptr_t k_007f105c = 0x007f105cu; // per-player guard byte (stride 0x13)
static const std::uintptr_t k_007f1058 = 0x007f1058u; // per-player block base (stride 0x4c)
static const std::uintptr_t k_007f1064 = 0x007f1064u; // block +0x0c
static const std::uintptr_t k_007f1068 = 0x007f1068u; // block +0x10
static const std::uintptr_t k_007f106c = 0x007f106cu; // block +0x14

// 0x00422b50 group
static const std::uintptr_t k_008995bc = 0x008995bcu; // per-vehicle accumulator base (stride 0x138)

// 0x00401340 group
static const std::uintptr_t k_007f0ff0 = 0x007f0ff0u; // source int (read, negated, FILD->float)
static const std::uintptr_t k_005cc328 = 0x005cc328u; // multiplier float
static const std::uintptr_t k_00636574 = 0x00636574u; // destination float (observable)
static const std::uintptr_t k_00636564 = 0x00636564u; // clump handle (arg to FUN_004b4080)

// ===========================================================================
// RaceFinalizeOnce  --  0x004331a0
//
// Original: void FUN_004331a0(undefined4 param_1)  body 0x004331a0..0x00433237
//
// Verbatim decompilation:
//   if (DAT_0067eca4 == 0) {
//     DAT_0067eca4 = 1;
//     FUN_0042d3a0();
//     _DAT_0067ebb0 = _DAT_0067ebb4 = _DAT_0067ebb8 = _DAT_0067ebbc = 0x43520000;
//     DAT_0067ecac = param_1;
//     DAT_0067ebc8 = _DAT_0067ec20 = DAT_0067ebcc = DAT_0067e844 = DAT_0067e9f8
//       = DAT_0067e914 = _DAT_0067ec24 = DAT_0067ea6c = 0;
//     puVar2 = &DAT_0067ebd0; for (i=0x14; i!=0; i--) { *puVar2 = 0; puVar2++; }
//     DAT_0067ea08 = 0;
//     FUN_004248b0(); FUN_00424920();
//   }
//
// 0x43520000 = 210.0f. Guard is one-shot; the harness SEEDS DAT_0067eca4=0
// before each call so the body runs on both sides. Observable: 0x0067ebb0.
// ref: re/analysis/game_state_d2/0x004331a0.md
// ===========================================================================

// 0x004331a0
extern "C" __declspec(dllexport) void __cdecl RaceFinalizeOnce(std::uint32_t param_1)
{
    if (*reinterpret_cast<std::int32_t*>(k_0067eca4) == 0) {
        *reinterpret_cast<std::int32_t*>(k_0067eca4) = 1;
        s_F0042d3a0();
        *reinterpret_cast<std::uint32_t*>(k_0067ebb0) = 0x43520000u;
        *reinterpret_cast<std::uint32_t*>(k_0067ebb4) = 0x43520000u;
        *reinterpret_cast<std::uint32_t*>(k_0067ebb8) = 0x43520000u;
        *reinterpret_cast<std::uint32_t*>(k_0067ebbc) = 0x43520000u;
        *reinterpret_cast<std::uint32_t*>(k_0067ecac) = param_1;
        *reinterpret_cast<std::int32_t*>(k_0067ebc8) = 0;
        *reinterpret_cast<std::int32_t*>(k_0067ec20) = 0;
        *reinterpret_cast<std::int32_t*>(k_0067ebcc) = 0;
        *reinterpret_cast<std::int32_t*>(k_0067e844) = 0;
        *reinterpret_cast<std::int32_t*>(k_0067e9f8) = 0;
        *reinterpret_cast<std::int32_t*>(k_0067e914) = 0;
        *reinterpret_cast<std::int32_t*>(k_0067ec24) = 0;
        *reinterpret_cast<std::int32_t*>(k_0067ea6c) = 0;
        std::uint32_t* p = reinterpret_cast<std::uint32_t*>(k_0067ebd0);
        for (int i = 0x14; i != 0; i--) {
            *p = 0;
            p++;
        }
        *reinterpret_cast<std::int32_t*>(k_0067ea08) = 0;
        s_F004248b0();
        s_F00424920();
    }
}

RH_ScopedInstall(RaceFinalizeOnce, 0x004331a0);

// ===========================================================================
// AiLastPlaceFrustration  --  0x00415020
//
// Original: undefined4 FUN_00415020(int param_1)  body 0x00415020..0x004150d6
//
// Verbatim decompilation:
//   iVar3 = -1;
//   fVar4 = (float10)FUN_00442cc0(param_1);
//   if (fVar4 == (float10)DAT_005d757c) {                 // last place (==0.0)
//     for (iVar2=0; iVar2<4; iVar2++)
//       if (FUN_0040e470(iVar2)==1) iVar3 = iVar2;        // last active leader
//     if (iVar3 != -1) {
//       fVar5 = (float10)FUN_00442cc0(iVar3);
//       fVar4 = (float10)_DAT_005cd088;
//       param_1 = param_1 * 0x74;
//       *(float*)(&DAT_0089a4e8 + param_1) = (float)fVar5;          // FST (keeps ST0)
//       if (fVar4 < fVar5 &&
//           (iVar3 = *(int*)(&DAT_0089a4e4+param_1) + DAT_007f1008,
//            *(int*)(&DAT_0089a4e4+param_1) = iVar3, 72000 < iVar3)) return 1;
//       if ((float10)_DAT_005cc320 <= fVar5) return 0;
//       *(undefined4*)(&DAT_0089a4e4+param_1) = 0; return 0;
//     }
//   }
//   *(undefined4*)(&DAT_0089a4e4 + param_1*0x74) = 0; return 0;
//
// 72000 decimal = 0x11940. Stride 0x74 bytes. Observable: 0x0089a4e8 (car 0).
// ref: re/analysis/ai_update_d2/0x00415020.md
// ===========================================================================

// 0x00415020
extern "C" __declspec(dllexport) std::uint32_t __cdecl AiLastPlaceFrustration(int param_1)
{
    int iVar3 = -1;
    float fVar4 = s_F00442cc0(param_1);
    if (fVar4 == *reinterpret_cast<float*>(k_005d757c)) {
        for (int iVar2 = 0; iVar2 < 4; iVar2++) {
            if (s_F0040e470(iVar2) == 1) {
                iVar3 = iVar2;
            }
        }
        if (iVar3 != -1) {
            float fVar5 = s_F00442cc0(iVar3);
            float thresh = *reinterpret_cast<float*>(k_005cd088);
            int off = param_1 * 0x74;
            *reinterpret_cast<float*>(k_0089a4e8 + off) = fVar5;
            if (thresh < fVar5) {
                int t = *reinterpret_cast<int*>(k_0089a4e4 + off) +
                        *reinterpret_cast<int*>(k_007f1008);
                *reinterpret_cast<int*>(k_0089a4e4 + off) = t;
                if (72000 < t) {
                    return 1;
                }
            }
            if (*reinterpret_cast<float*>(k_005cc320) <= fVar5) {
                return 0;
            }
            *reinterpret_cast<std::uint32_t*>(k_0089a4e4 + off) = 0;
            return 0;
        }
    }
    *reinterpret_cast<std::uint32_t*>(k_0089a4e4 + param_1 * 0x74) = 0;
    return 0;
}

RH_ScopedInstall(AiLastPlaceFrustration, 0x00415020);

// ===========================================================================
// CarEventTrigger  --  0x004922e0
//
// Original: void FUN_004922e0(int param_1,u4 param_2,u4 param_3,u4 param_4)
//           body 0x004922e0..0x00492331
//
// Verbatim decompilation:
//   if (FUN_0040e470(param_1) == 1) {
//     if (FUN_0040e350() == 6) {
//       iVar1 = (&DAT_007f1a14)[param_1*4] * 0x4c;
//       if ((&DAT_007f105c)[(&DAT_007f1a14)[param_1*4] * 0x13] != 0) {
//         *(u4*)(&DAT_007f1068 + iVar1) = param_2;
//         *(u4*)(&DAT_007f1058 + iVar1) = 1;
//         *(u4*)(&DAT_007f1064 + iVar1) = param_3;
//         *(u4*)(&DAT_007f106c + iVar1) = param_4;
//       }
//     }
//   }
//
// (&DAT_007f1a14)[i*4] indexes a dword table with element-stride 4 (i.e. the C
// expression on an undefined1* base — &DAT_007f1a14 + param_1*4 read as int).
// Likewise (&DAT_007f105c)[slot*0x13] is a byte-strided guard read, and the
// four writes are at &DAT_007f1058/64/68/6c + slot*0x4c. Observable: 0x007f1058.
// ref: re/analysis/bucket_vehicle_004922e0_0057c500/0x004922e0.md
// ===========================================================================

// 0x004922e0
extern "C" __declspec(dllexport) void __cdecl CarEventTrigger(int param_1,
                                                              std::uint32_t param_2,
                                                              std::uint32_t param_3,
                                                              std::uint32_t param_4)
{
    if (s_F0040e470(param_1) == 1) {
        if (s_F0040e350() == 6) {
            int slot = *reinterpret_cast<int*>(k_007f1a14 + param_1 * 4);
            int iVar1 = slot * 0x4c;
            if (*reinterpret_cast<std::int8_t*>(k_007f105c + slot * 0x13) != 0) {
                *reinterpret_cast<std::uint32_t*>(k_007f1068 + iVar1) = param_2;
                *reinterpret_cast<std::uint32_t*>(k_007f1058 + iVar1) = 1;
                *reinterpret_cast<std::uint32_t*>(k_007f1064 + iVar1) = param_3;
                *reinterpret_cast<std::uint32_t*>(k_007f106c + iVar1) = param_4;
            }
        }
    }
}

RH_ScopedInstall(CarEventTrigger, 0x004922e0);

// ===========================================================================
// VehicleDamageAccum  --  0x00422b50
//
// Original: void FUN_00422b50(int param_1,int param_2)  body 0x00422b50..0x00422b9a
//
// Verbatim decompilation:
//   iVar1 = FUN_0040e470(param_1);
//   if (iVar1 != 1) {                              // non-player: random subtract
//     FUN_00472650(0,0x43960000);                  // random float [0,300.0f)
//     iVar1 = FUN_004a2c48();                       // -> int (consumes ST0)
//     param_2 = param_2 - iVar1;
//     if (param_2 < 0) param_2 = 0;
//   }
//   (&DAT_008995bc)[param_1*0x4e] = (&DAT_008995bc)[param_1*0x4e] + param_2;
//
// Listing: IMUL EDI,EDI,0x138 then LEA/MOV [EDI+0x8995bc] (int array, 0x4e
// dwords = 0x138 bytes). 0x43960000 = 300.0f. For a player car (FUN_0040e470==1)
// the accumulator gets the full delta (deterministic). The harness pins
// param_1=0 (player) so the random branch is NOT taken, and SEEDS the slot at
// 0x008995bc (sentinel) so the readback = sentinel + delta. Observable: 0x008995bc.
// ref: re/analysis/ai_update_d6/0x00422b50.md
// ===========================================================================

// 0x00422b50
extern "C" __declspec(dllexport) void __cdecl VehicleDamageAccum(int param_1, int param_2)
{
    int iVar1 = s_F0040e470(param_1);
    if (iVar1 != 1) {
        s_F00472650(0, 0x43960000);       // random float [0, 300.0f) -> ST0
        iVar1 = s_F004a2c48();            // ST0 -> int (rounding)
        param_2 = param_2 - iVar1;
        if (param_2 < 0) {
            param_2 = 0;
        }
    }
    int* slot = reinterpret_cast<int*>(k_008995bc + static_cast<std::uintptr_t>(param_1) * 0x138);
    *slot = *slot + param_2;
}

RH_ScopedInstall(VehicleDamageAccum, 0x00422b50);

// ===========================================================================
// CupSpinSpeedAndColor  --  0x00401340
//
// Original: void FUN_00401340(void)  body 0x00401340..0x004013e1
//
// Verbatim decompilation:
//   DAT_00636574 = (float)-DAT_007f0ff0 * _DAT_005cc328;
//   iVar1 = FUN_004b4080(DAT_00636564);
//   iVar2 = FUN_0042f6a0();
//   if (iVar2 == 3) { *(u4*)(iVar1+4) = 0xff0a66d8; return; }
//   iVar2 = FUN_0042f6a0();
//   if (iVar2 == 4) { *(u4*)(iVar1+4) = 0xfff5f0f0; return; }
//   *(u4*)(iVar1+4) = 0xff14c5ff;
//
// Listing detail (0x00401343..0x00401360): MOV EAX,[0x7f0ff0]; NEG EAX (integer
// negate); MOV [ESP+8],EAX; FILD [ESP+8] (int->float); FMUL [0x5cc328]; FSTP
// [0x636574]. So DAT_007f0ff0 is read as a 32-bit INTEGER, negated, converted to
// float, multiplied by the float DAT_005cc328. The three colors are ARGB
// constants written to *(FUN_004b4080(handle)+4). FUN_0042f6a0 is called twice
// (re-read), matched here. Observable: 0x00636574.
// ref: re/analysis/boot_hud_promote_ae1/0x00401340.md
// ===========================================================================

// 0x00401340
extern "C" __declspec(dllexport) void __cdecl CupSpinSpeedAndColor(void)
{
    std::int32_t srcInt = *reinterpret_cast<std::int32_t*>(k_007f0ff0);
    float mul = *reinterpret_cast<float*>(k_005cc328);
    *reinterpret_cast<float*>(k_00636574) =
        static_cast<float>(-srcInt) * mul;

    int iVar1 = s_F004b4080(*reinterpret_cast<int*>(k_00636564));
    int iVar2 = s_F0042f6a0();
    if (iVar2 == 3) {
        *reinterpret_cast<std::uint32_t*>(iVar1 + 4) = 0xff0a66d8u;
        return;
    }
    iVar2 = s_F0042f6a0();
    if (iVar2 == 4) {
        *reinterpret_cast<std::uint32_t*>(iVar1 + 4) = 0xfff5f0f0u;
        return;
    }
    *reinterpret_cast<std::uint32_t*>(iVar1 + 4) = 0xff14c5ffu;
}

RH_ScopedInstall(CupSpinSpeedAndColor, 0x00401340);

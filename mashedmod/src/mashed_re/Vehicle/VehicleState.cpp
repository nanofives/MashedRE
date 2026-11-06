// Mashed RE — Vehicle state accessor functions.
// Five pure-leaf C2->C3 promotions from the vehicle_promote_c2_* plates.
// All originals are trivial getters / predicates reading from per-vehicle
// global arrays in .bss.  No callees except FUN_00468b40's scan loop.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include "VehicleRecordBase.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// Global array layout — all in .bss, stride 0x341 DWORDs (0xD04 bytes)
// ─────────────────────────────────────────────────────────────────────────────
//   0x008815a4  VehicleAliveField    (+0x00 of per-vehicle block)
//   0x008815a8  VehicleRacePosition  (+0x04 of per-vehicle block)
//   0x008815b0  VehicleDestrState    (+0x0C of per-vehicle block)
//   0x008820b0  EntitySlotField0     (separate entity table, stride 0xD04)

static constexpr std::uintptr_t kVehicleBase_8815a4 = 0x008815a4u;
static constexpr std::uintptr_t kVehicleBase_8815a8 = 0x008815a8u;
static constexpr std::uintptr_t kVehicleBase_8815b0 = 0x008815b0u;
static constexpr std::uintptr_t kEntityBase_8820b0  = 0x008820b0u;
static constexpr std::uint32_t  kDWordStride         = 0x341u;
static constexpr std::uint32_t  kByteStride          = 0xD04u;

static constexpr std::uintptr_t kVehicleBase_881f68 = 0x00881f68u;
static constexpr std::uintptr_t kVehicleBase_881f6c = 0x00881f6cu;
static constexpr std::uintptr_t kVehicleBase_881f70 = 0x00881f70u;
static constexpr std::uintptr_t kVehicleBase_881f90 = 0x00881f90u;
static constexpr std::uintptr_t kVehicleBase_881f94 = 0x00881f94u;

// ─────────────────────────────────────────────────────────────────────────────
// [U-9186 leg H1a 2026-10-08] Record-base rebinding for the STANDALONE target.
//
// The constants above are the ORIGINAL's absolute addresses. Under
// /DMASHED_STANDALONE the per-vehicle record array is not there: it is
// Vehicle::g_vehicleArrayBase (= g_records, set at VehiclePhysicsRun.cpp:249),
// so every absolute read above returns blank image-pad. Measured in
// verify/d3_u9186_20261008/RESULT_G1.md: the alive field reads 0 at the absolute
// and 1 on the port's own record, on 100% of 53,992 rows.
//
// Same shape as Vehicle/LaunchRevCharge.cpp:77-83, which already does this for
// the same array. The #else branch keeps the absolute so the .asi is unchanged
// and its Frida evidence (taken on the absolute form) still applies.
//
// The base resolution, the offsets and the OFFSET CONVENTION note now live in
// Vehicle/VehicleRecordBase.h, shared with the exe-only body of FUN_0046d4a0
// (Vehicle/VehicleRecordPtr.cpp) so the two cannot drift apart.
// PREREG: verify/d3_u9186_20261008/PREREG_H1.md
using mashed_re::Vehicle::VehRecord;
using mashed_re::Vehicle::kOff_Alive;
using mashed_re::Vehicle::kOff_State;
using mashed_re::Vehicle::kOff_Secondary;

// NOT rebound in this leg, deliberately — PREREG_H1.md section 1:
//   0x0046c770 / 0x0046d700 index the SAME record (+0x10, +0x9C8/9CC/9D0) and
//     carry the identical defect, but each currently returns
//     pad-zero to its own consumers, so each rebind is a separate potential
//     behaviour change that deserves its own measurement.
//   0x0046c6d0 MUST NOT be rebound: 0x008820b0 is a separate entity table
//     (see the header block above) and 0x008820b0 - 0x008815a0 = 0x1110, past
//     the 0xd04 stride, so it is not a field of this record.
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046c7b0  VehicleSlotGetter
// Ghidra decomp:
//   if (0xf < param_1) return 0xffffffff;
//   return (&DAT_008815a4)[param_1 * 0x341];
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx) {
    if (vehicleIdx > 0xfu) return 0xffffffffu;
    // [H1a] record-base rebind; .asi resolves to the original absolute.
    const char* rec = VehRecord(vehicleIdx);
    if (!rec) return 0u;                    // standalone pre-race: no array yet
    return *reinterpret_cast<const std::uint32_t*>(rec + kOff_Alive);
}
RH_ScopedInstall(VehicleSlotGetter, 0x0046c7b0);  // re-enabled 2026-05-24 c3-vehicle

// MATCH-S11: the body of 0x0046c770 (VehicleDestructionStateGetter) moved to Match/MatchS11_0046c770.cpp,
// which compiles the byte-matched source re/match/s11/0046c770.c.

// MATCH-S11: the body of 0x0046c6d0 (VehicleEntitySlotRead) moved to Match/MatchS11_0046c6d0.cpp,
// which compiles the byte-matched source re/match/s11/0046c6d0.c.

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046dbe0  VehicleRacePositionGet
// Ghidra decomp:
//   return (&DAT_008815a8)[param_1 * 0x341];
// No bounds guard.  Race position: 0=1st, 1=2nd, 2=3rd, 3=4th.
// Disassembly 0x0046dbe0..0x0046dbf0: mov eax,[esp+4]; imul eax,eax,0xd04; mov eax,[eax+0x8815a8]; ret.
// [dcleaf 2026-10-22] The ONE body for 0x0046dbe0 in both targets. Record base through VehRecord (the
// standalone's g_vehicleArrayBase, the .asi's absolute 0x008815a0), so the exe reads its own record
// (+0x08) instead of image pad. Its one exe consumer is Vehicle/ForceIntegratorLeaves.cpp:RubberBandGrip
// (0x00442d28 / 0x00442d35), whose former private copy CarContactCount read g_vehicleArrayBase + 8 +
// car*0xd04, the same word. No null guard, as the original (a null base faults in both forms).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleRacePositionGet(int param_1) {
    return *reinterpret_cast<const std::uint32_t*>(VehRecord(static_cast<std::uint32_t>(param_1)) + 0x08);
}
RH_ScopedInstall(VehicleRacePositionGet, 0x0046dbe0);  // re-enabled 2026-05-24 c3-vehicle

// MATCH-S11: the body of 0x00468b40 (VehicleContactHistoryLookup) moved to Match/MatchS11_00468b40.cpp,
// which compiles the byte-matched source re/match/s11/00468b40.c.

// MATCH-S11: the body of 0x0046d700 (VehicleVec3At9C8Get) moved to Match/MatchS11_0046d700.cpp,
// which compiles the byte-matched source re/match/s11/0046d700.c.

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046cbb0  VehicleCarStateRead
// Ghidra decomp:
//   if (0xf < param_1) return 0;
//   *param_2 = (&DAT_00881f90)[param_1 * 0x341];
//   *param_3 = *(undefined4*)(&DAT_00881f94 + param_1 * 0xd04);
//   return 1;
// param_1 = car index 0..15; param_2 = out: state field (0=alive, 2=slide per callers);
// param_3 = out: secondary field (U-1856: purpose unconfirmed)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleCarStateRead(std::uint32_t carIdx, std::uint32_t* outState, std::uint32_t* outSecondary) {
    if (carIdx > 0xfu) return 0;
    // [H1a] record-base rebind; .asi resolves to the original absolutes.
    const char* rec = VehRecord(carIdx);
    if (!rec) { *outState = 0u; *outSecondary = 0u; return 1; }  // pre-race
    *outState     = *reinterpret_cast<const std::uint32_t*>(rec + kOff_State);
    *outSecondary = *reinterpret_cast<const std::uint32_t*>(rec + kOff_Secondary);
    return 1;
}
RH_ScopedInstall(VehicleCarStateRead, 0x0046cbb0);  // re-enabled 2026-05-24 phase-a2 GREEN (9/9 2 distinct)


// ─────────────────────────────────────────────────────────────────────────────
// 0x00417730  VehicleRaceAngleGet
// Ghidra decomp:
//   EAX = [ESP+4];  ST0 = *(float*)(EAX*4 + 0x0089a880);  RET
// Reads a per-car float from flat array DAT_0089a880 (stride 4 bytes, indexed by carIdx).
// Called by ai_path_following functions as a "race angle" metric.
// U-2173: float semantic unknown.  U-2174: array element count unknown.
// ─────────────────────────────────────────────────────────────────────────────
// Body = the matched source re/match/s03/h1_00417730.c (byte-identical under re/tools/match/match.py,
// cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1; MATCH-S03). MATCH_PORT maps its names onto the port.
#define MATCH_PORT 1
#define DAT_0089a880 (reinterpret_cast<float*>(0x0089a880u))
#define FUN_00417730 VehicleRaceAngleGet
extern "C" __declspec(dllexport) float __cdecl VehicleRaceAngleGet(int carIdx);
extern "C" {
// 0x00417730  VehicleRaceAngleGet
#include "../../../../re/match/s03/h1_00417730.c"
}
#undef FUN_00417730
#undef DAT_0089a880
#undef MATCH_PORT
RH_ScopedInstall(VehicleRaceAngleGet, 0x00417730);  // re-enabled 2026-05-24 c3-vehicle

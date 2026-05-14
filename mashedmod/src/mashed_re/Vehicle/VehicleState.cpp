// Mashed RE — Vehicle state accessor functions.
// Five pure-leaf C2->C3 promotions from the vehicle_promote_c2_* plates.
// All originals are trivial getters / predicates reading from per-vehicle
// global arrays in .bss.  No callees except FUN_00468b40's scan loop.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
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
// 0x0046c7b0  VehicleSlotGetter
// Ghidra decomp:
//   if (0xf < param_1) return 0xffffffff;
//   return (&DAT_008815a4)[param_1 * 0x341];
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx) {
    if (vehicleIdx > 0xfu) return 0xffffffffu;
    return reinterpret_cast<const std::uint32_t*>(kVehicleBase_8815a4)[vehicleIdx * kDWordStride];
}
RH_ScopedInstall(VehicleSlotGetter, 0x0046c7b0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046c770  VehicleDestructionStateGetter
// Ghidra decomp:
//   if (0xf < param_1) return 0xffffffff;
//   return (&DAT_008815b0)[param_1 * 0x341];
// U-3605: full value domain for this field is unconfirmed; 0=alive observed.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleDestructionStateGetter(std::uint32_t vehicleIdx) {
    if (vehicleIdx > 0xfu) return 0xffffffffu;
    return reinterpret_cast<const std::uint32_t*>(kVehicleBase_8815b0)[vehicleIdx * kDWordStride];
}
RH_ScopedInstall(VehicleDestructionStateGetter, 0x0046c770);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046c6d0  VehicleEntitySlotRead
// Ghidra decomp:
//   if (0xf < param_1) return 0;
//   *param_2 = *(undefined4*)(&DAT_008820b0 + param_1 * 0xd04);
//   return 1;
// Note: param_1 is signed int; OOB is param_1 > 15 (not unsigned guard).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleEntitySlotRead(int slotIdx, std::uint32_t* outVal) {
    if (slotIdx > 0xf) return 0;
    *outVal = *reinterpret_cast<const std::uint32_t*>(kEntityBase_8820b0 + slotIdx * kByteStride);
    return 1;
}
RH_ScopedInstall(VehicleEntitySlotRead, 0x0046c6d0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046dbe0  VehicleRacePositionGet
// Ghidra decomp:
//   return (&DAT_008815a8)[param_1 * 0x341];
// No bounds guard.  Race position: 0=1st, 1=2nd, 2=3rd, 3=4th.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl VehicleRacePositionGet(int param_1) {
    return reinterpret_cast<const std::uint32_t*>(kVehicleBase_8815a8)[param_1 * kDWordStride];
}
RH_ScopedInstall(VehicleRacePositionGet, 0x0046dbe0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00468b40  VehicleContactHistoryLookup
// Ghidra decomp:
//   piVar1 = (int*)(param_2 + 0xbfc);
//   while (piVar1[0x20]==0 || *(int*)(param_1+0x34) != *piVar1) {
//     ++iVar2; ++piVar1;
//     if (0x1f < iVar2) return 0;
//   }
//   return 1;
// param_1 = geometry entry ptr (contact ID at byte offset +0x34)
// param_2 = vehicle struct ptr
// Contact table: vehicle+0xBFC, 32 int slots; active flags at +0x80 per slot.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleContactHistoryLookup(int param_1, int param_2) {
    const int* piVar1 = reinterpret_cast<const int*>(param_2 + 0xBFC);
    int iVar2 = 0;
    while (piVar1[0x20] == 0 || *reinterpret_cast<const int*>(param_1 + 0x34) != *piVar1) {
        ++iVar2;
        ++piVar1;
        if (iVar2 > 0x1f) return 0;
    }
    return 1;
}
RH_ScopedInstall(VehicleContactHistoryLookup, 0x00468b40);

// ─────────────────────────────────────────────────────────────────────────────
// 0x0046d700  VehicleVec3At9C8Get
// Ghidra decomp:
//   if (0xf < param_2) return 0;
//   param_1[0] = (&DAT_00881f68)[param_2 * 0x341];
//   param_1[1] = (&DAT_00881f6c)[param_2 * 0x341];
//   param_1[2] = (&DAT_00881f70)[param_2 * 0x341];
//   return 1;
// param_1 = out buffer (3 DWORDs at +0x9C8/+0x9CC/+0x9D0 in per-vehicle block)
// param_2 = vehicle index 0..15  (U-1748: direction type unconfirmed)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl VehicleVec3At9C8Get(std::uint32_t* outVec, std::uint32_t vehicleIdx) {
    if (vehicleIdx > 0xfu) return 0;
    outVec[0] = reinterpret_cast<const std::uint32_t*>(kVehicleBase_881f68)[vehicleIdx * kDWordStride];
    outVec[1] = reinterpret_cast<const std::uint32_t*>(kVehicleBase_881f6c)[vehicleIdx * kDWordStride];
    outVec[2] = reinterpret_cast<const std::uint32_t*>(kVehicleBase_881f70)[vehicleIdx * kDWordStride];
    return 1;
}
RH_ScopedInstall(VehicleVec3At9C8Get, 0x0046d700);

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
    *outState     = reinterpret_cast<const std::uint32_t*>(kVehicleBase_881f90)[carIdx * kDWordStride];
    *outSecondary = *reinterpret_cast<const std::uint32_t*>(kVehicleBase_881f94 + carIdx * kByteStride);
    return 1;
}
RH_ScopedInstall(VehicleCarStateRead, 0x0046cbb0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x00417730  VehicleRaceAngleGet
// Ghidra decomp:
//   EAX = [ESP+4];  ST0 = *(float*)(EAX*4 + 0x0089a880);  RET
// Reads a per-car float from flat array DAT_0089a880 (stride 4 bytes, indexed by carIdx).
// Called by ai_path_following functions as a "race angle" metric.
// U-2173: float semantic unknown.  U-2174: array element count unknown.
// ─────────────────────────────────────────────────────────────────────────────
static constexpr std::uintptr_t kCarRaceArrayBase = 0x0089a880u;

extern "C" __declspec(dllexport) float __cdecl VehicleRaceAngleGet(int carIdx) {
    return *reinterpret_cast<const float*>(kCarRaceArrayBase + carIdx * 4u);
}
RH_ScopedInstall(VehicleRaceAngleGet, 0x00417730);

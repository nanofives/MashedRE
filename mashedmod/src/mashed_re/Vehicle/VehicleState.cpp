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

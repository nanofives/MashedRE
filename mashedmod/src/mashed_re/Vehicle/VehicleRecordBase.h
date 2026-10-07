#pragma once
// Mashed RE — per-vehicle record base resolution.
//
// [U-9186 leg H1 2026-10-08] One place that answers "where is the per-vehicle
// record array?", so the standalone and .asi answers cannot drift apart.
//
// The original's array is at 0x008815a0, 16 records of stride 0xd04 (stride
// witness `imul eax,eax,0xd04` at 0x0046d78e; the base is independently recorded
// as `base_va` in every .msd capture provenance, LaunchRevCharge.cpp:16-17).
// Under /DMASHED_STANDALONE that address is blank image-pad: the port's records
// are Vehicle::g_vehicleArrayBase (= g_records, set at VehiclePhysicsRun.cpp:249).
// Measured in verify/d3_u9186_20261008/RESULT_G1.md — the alive field reads 0 at
// the absolute and 1 on the port's record, on 100% of 53,992 rows.
//
// OFFSET CONVENTION: every kOff_* below is a difference from the ARRAY BASE
// 0x008815a0. VehicleState.cpp's older header block states offsets relative to
// 0x008815a4, so its "+0x00" is "+0x04" here. Mixing the two is the obvious way
// to get this wrong.
#include <cstddef>
#include <cstdint>

namespace mashed_re { namespace Vehicle { extern int* g_vehicleArrayBase; } }

namespace mashed_re {
namespace Vehicle {

constexpr std::uintptr_t kVehRecordArray   = 0x008815a0u;
constexpr std::size_t    kVehRecordStride  = 0xD04u;

constexpr std::size_t    kOff_Alive        = 0x004u;  // 0x008815a4
constexpr std::size_t    kOff_WheelMatrices= 0x928u;  // 0x00881ec8, VehicleStruct.h:99
constexpr std::size_t    kOff_WheelSetSel  = 0x9A8u;  // 0x00881f48, VehicleStruct.h:101
constexpr std::size_t    kOff_State        = 0x9F0u;  // 0x00881f90
constexpr std::size_t    kOff_Secondary    = 0x9F4u;  // 0x00881f94

// Returns nullptr standalone before the array is allocated (pre-race). A prior
// AV from exactly that window is recorded at TrackRenderer.cpp:3094, so callers
// must check rather than assume.
inline const char* VehRecord(std::uint32_t idx) {
#ifdef MASHED_STANDALONE
    const char* base = reinterpret_cast<const char*>(g_vehicleArrayBase);
    if (!base) return nullptr;
#else
    const char* base = reinterpret_cast<const char*>(kVehRecordArray);
#endif
    return base + static_cast<std::size_t>(idx) * kVehRecordStride;
}

}  // namespace Vehicle
}  // namespace mashed_re

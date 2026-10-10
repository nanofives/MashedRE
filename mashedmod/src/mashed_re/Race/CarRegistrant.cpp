// Mashed RE — mode 7 leg M2b: the car registrant FUN_0041f710 (exe-only TU).
//
// FUN_0041f710 (EAX = the car's visual record 0x0063d9e0 + v*0x2ac) registers
// {rec+0x258, +0x25c, +0x260 (sphere centre), +0x264 (radius), +0x290 + 0xff00, 0, 0, 0} via
// FUN_00484cf0. Verbatim naked (spec re/tools/naked_specs/cars_m2b.json -> CarRegistrant_naked.inc).
// The record itself is a RenderWare-side object the exe does not build, so a fake 0x2ac record per
// car carries the three fields the body reads:
//   +0x258..+0x260  = VehRecord(v) + 0x958..+0x960, BITWISE. Measured 1,600 / 1,600 on o_cm1; the
//                     mechanism: FUN_0040da50 hands FUN_00420870 the body matrix (0x0040dac2), which
//                     replaces the part-19 frame with it (FUN_004c1480, 0x004208ae) and copies its LTM
//                     translation to +0x258 (0x004208ba..0x004208d5).
//   +0x264          = 0x3f088c2d, MEASURED on every original Training frame for all 4 cars; the writer
//                     of this culling-sphere radius is not found [UNCERTAIN U-9218].
//   +0x290          = v (FUN_00420420 at 0x0042047c stores the player index).
// Gate: FUN_0040da50's per-car predicate, as PathProg_FrameTick mirrors it ([UNCERTAIN U-9210]).
// PREREG: verify/d3_mode7_m2_20261010/PREREG_M2.md section 5.
#include "CarRegistrant.h"
#include "WorldRegistry.h"
#include "../Vehicle/VehicleRecordBase.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" void RG_484cf0();                                                           // Race/WorldRegistry_naked.inc
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);          // 0x0046c7b0
extern "C" std::uint32_t __cdecl IsCarSlotActive(int param_1);                         // 0x0040e370

#include "CarRegistrant_naked.inc"

namespace mashed_re {
namespace Race {

namespace {
alignas(4) unsigned char s_rec[4][0x2ac];
constexpr std::uint32_t kRadiusBits = 0x3f088c2du;   // measured, U-9218

void CallRegistrant(unsigned char* rec) {
    __asm {
        mov eax, rec
        call CR_41f710
    }
}
}  // namespace

bool CarsReg_Enabled() {
    static const bool s_on = [] {
        const char* s = std::getenv("MASHED_REG_CARS");
        return s && s[0] && s[0] != '0';
    }();
    return s_on && Registry_Enabled();
}

void CarsReg_FrameTick() {
    if (!CarsReg_Enabled()) return;
    for (int v = 0; v < 4; ++v) {
        if (IsCarSlotActive(v) == 0) continue;                                     // *(PTR_005f2770 + 0x34 + v*4)
        if (VehicleSlotGetter(static_cast<std::uint32_t>(v)) != 1u) continue;      // 0x0040daa0 (FUN_0046c770 arm: U-9210)
        const char* r = Vehicle::VehRecord(static_cast<std::uint32_t>(v));
        if (!r) continue;
        unsigned char* rec = s_rec[v];
        std::memcpy(rec + 0x258, r + 0x958, 12);
        std::memcpy(rec + 0x264, &kRadiusBits, 4);
        const std::int32_t idx = v;
        std::memcpy(rec + 0x290, &idx, 4);
        CallRegistrant(rec);
    }
}

}  // namespace Race
}  // namespace mashed_re

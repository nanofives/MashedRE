// Mashed RE — mode 7 leg M2b: the car registrant FUN_0041f710. Exe-only TU.
// PREREG: verify/d3_mode7_m2_20261010/PREREG_M2.md section 5.
#pragma once

namespace mashed_re {
namespace Race {

// MASHED_REG_CARS (default OFF) AND MASHED_REGISTRY.
bool CarsReg_Enabled();

// FUN_0040da50's FUN_00420870 -> FUN_0041f710 per car (0x0040dad0), before the pickup tick and the publish.
void CarsReg_FrameTick();

}  // namespace Race
}  // namespace mashed_re

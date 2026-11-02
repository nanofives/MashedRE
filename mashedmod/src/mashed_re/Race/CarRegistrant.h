// Mashed RE — mode 7 leg M2b: the car registrant FUN_0041f710. Exe-only TU.
// PREREG: verify/d3_mode7_m2_20261010/PREREG_M2.md section 5.
#pragma once

namespace mashed_re {
namespace Race {

// Always on: MASHED_REG_CARS was deleted by D5-KNOBS-E4-MODE-REG.
// FUN_0040da50's FUN_00420870 -> FUN_0041f710 per car (0x0040dad0), before the pickup tick and the publish.
void CarsReg_FrameTick();

// [MATCH-PORTSTATE-RECORDS] base of the four 0x2ac records (0x0063d9e0 with MASHED_PORTSTATE_RECORDS, else the
// exe's own store). Witness only.
const unsigned char* CarsReg_RecordBase();

}  // namespace Race
}  // namespace mashed_re

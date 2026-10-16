// Mashed RE — [d2posmodel 2026-10-16] fix B knobs (verify/d2posmodel_20261016/PREREG_d2posmodel.md).
//
// Fix B makes the exe's car position model the original's, in three parts, each behind its
// own knob so it can be measured alone (RESULT_d2settle.md section 2 / section 5.1):
//   part 1 MASHED_POS_SEED=1    FUN_0046baa0's wheel seeds (states 1, fv 0, +0x9e0 = 4.0)
//                               before the first solver call (VehiclePhysicsRun.cpp, spawn settle)
//   part 2 MASHED_POS_WHEELS=1  real suspension points: the four g_suspScratch points through the
//                               ring matrix into DAT_0088e620 (WheelContactSolver.cpp), with the
//                               body basis published into the ring before the solver
//   part 3 MASHED_POS_TRANS=1   the solver's ring +0x30 translation is the car's position (x, y, z)
// [d2engage 2026-10-16] (verify/d2engage_20261016/PREREG_d2engage.md section 1):
//   part 4 MASHED_POS_ORDER=1   the spawn settle is a pre-release hold tick: slot 0's settle runs before
//                               LaunchRev_Release on the GO frame, with FUN_0040e350 = 5, and the release
//                               tick's own FUN_0046baa0 (0x0041043a) runs after it
//   part 5 MASHED_POS_CONTACTS=1 no ReassertContacts between A4 and the substep loop (FUN_00470c70
//                               0x0047109f..0x00471106 writes no wheel state / normal / +0x9e0)
//   MASHED_POS_MODEL=1          all five
// Header-only so VehiclePhysicsRun.cpp, WheelContactSolver.cpp and TrackRenderer.cpp read the
// same knobs without a new cross-TU extern (all three are exe-only, exe_sources.rsp).
#pragma once

#include <cstdlib>

namespace mashed_re {
namespace Vehicle {

inline bool PosModelEnvOn(const char* name) {
    const char* e = std::getenv(name);
    return e && e[0] == '1';
}
inline bool PosModelAll()    { static const bool s = PosModelEnvOn("MASHED_POS_MODEL");  return s; }
inline bool PosModelSeed()   { static const bool s = PosModelAll() || PosModelEnvOn("MASHED_POS_SEED");   return s; }
inline bool PosModelWheels() { static const bool s = PosModelAll() || PosModelEnvOn("MASHED_POS_WHEELS"); return s; }
inline bool PosModelTrans()  { static const bool s = PosModelAll() || PosModelEnvOn("MASHED_POS_TRANS");  return s; }
inline bool PosModelOrder()  { static const bool s = PosModelAll() || PosModelEnvOn("MASHED_POS_ORDER");  return s; }
inline bool PosModelContacts() { static const bool s = PosModelAll() || PosModelEnvOn("MASHED_POS_CONTACTS"); return s; }

}  // namespace Vehicle
}  // namespace mashed_re

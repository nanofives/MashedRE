// Mashed RE — port-state migration 2/3 (L-78 (c)): the original's car record block kept alive in the exe.
// Exe only. PREREG and arms: verify/portstaterecords_20261101/PREREG_MATCH-PORTSTATE-RECORDS.md, RESULT_*.md.
#pragma once

namespace mashed_re {
namespace Vehicle {

// MASHED_PORTSTATE_RECORDS (default OFF, read once). ON: the port's vehicle records (VehiclePhysicsRun g_records)
// live at the original's 0x008815a0 (16 x 0xd04) and the car visual records (Race/CarRegistrant s_rec) at
// 0x0063d9e0 (4 x 0x2ac), so the original addresses hold the port's state. OFF: the exe's own static stores.
bool PortStateRecords_On();

// Knob ON only: cumulative witness over every race frame, one [portstate-records] line per 600 frames
// (PREREG "Witness"). Call once per race frame after every car's physics.
void PortStateRecords_FrameWitness();

} // namespace Vehicle
} // namespace mashed_re

// LibRw/RwCarFx.h — [rtex 2026-10-16] the original's per-car material set-up, for the librw
// race renderer (U-9693). Two halves, both keyed on the PART CODE each vehicle geometry
// carries as rpUSERDATA "0.tv_part_id" (FUN_004b5190 @ 0x004b5190, Track/MatFxExt.h):
//
//  dirt  FUN_0040cea0 @ 0x0040cea0 reads two textures out of toastart/vehicles/dirt.piz,
//        named by the course (FUN_00404e00 @ 0x00404e00) and by course x vehicle
//        (FUN_00404e20 @ 0x00404e20) through the tables FUN_00404830 @ 0x00404830 builds,
//        and FUN_0041ecc0 @ 0x0041ecc0 makes them the MatFX DUAL texture of every
//        effect-4 material: wheel parts get the wheel texture, the rest the body texture.
//  env   FUN_00420420 @ 0x00420420 turns the materials of the parts FUN_0041f360 @
//        0x0041f360 lists into MatFX ENVMAP (texture = the vehicle table's env name,
//        `cargloss`); FUN_00420340 @ 0x00420340 refreshes the coefficient per frame.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace LibRw {

// 0x00404e00: the body dirt texture name for a course id (COURSE.LUA Course_Id, the value the
// original keeps at DAT_00644158), or nullptr when the table holds -1.
const char* CarDirtBodyName(int course_id);
// 0x00404e20: the wheel dirt texture name for (course id, vehicle id), or nullptr.
const char* CarDirtWheelName(int course_id, int vehicle_id);
// FUN_0041ecc0's selector: parts 0..3 and 0x21..0x24 take the wheel texture.
bool CarDirtIsWheelPart(int part);
// FUN_0041f360 with DAT_00636ad0 == 0 (read live as 0): the parts that get the env map.
bool CarEnvIsEnvPart(int part);
// The vehicle id of a vehicle piz base name (the table at 0x005f37a8, stride 0x84: name at
// +0, id at +0x80; ids run car*6 + variant) for variant 0, or -1.
int CarVehicleGroupId(const char* piz_base);

// MASHED_CAR_DIRT=0 turns the dirt half off (default ON). The env half is OPT-IN,
// MASHED_CAR_ENV=1 (default OFF; RwCarFx.cpp says why).
bool CarDirtEnabled();
bool CarEnvEnabled();

}  // namespace LibRw
}  // namespace mashed_re

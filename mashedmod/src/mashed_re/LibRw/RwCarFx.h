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

// MASHED_CAR_DIRT=0 turns the dirt half off (default ON). The env half follows the per-part
// visibility below (default ON; MASHED_CAR_ENV=0 turns it off, =1 forces it on; RwCarFx.cpp).
bool CarDirtEnabled();
bool CarEnvEnabled();

// [carlod 2026-10-16] per-part visibility (U-9079), the third half of the car set-up. Each frame
// FUN_00420870 @ 0x00420870 sets or clears every part atomic's RENDER bit (FUN_004b5240 @
// 0x004b5240) from the record's LOD state S (+0x298) and flags F (+0x294); the standalone used
// to draw every LOD at once. PREREG_carlod.md section 1 has the table and the live witness.
//
// The switch at 0x00420a33..0x00420b16: the bool FUN_00420870 hands FUN_004b5240 for `part`.
bool CarPartVisible(int state, std::uint32_t flags, int part);
// FUN_00446520 @ 0x00446520 (0x00446952..0x00446975): S = 1 when |part 0x13 - camera|^2 is below
// [0x005ce1d0] = 144.0, else 2. `d2` is FUN_0041f120 @ 0x0041f120's third output, unclamped.
int CarLodState(double d2);
// F as FUN_0041eeb0 @ 0x0041eeb0 builds it for an undamaged car: 0x8 (slot active), 0x40 | 0x80
// when FUN_0040da50 @ 0x0040da50 sets DAT_0063bc44 (course id 0, 0x1a, 0x21, 0x24), and 0x800 when
// the controller's brake byte is set (`brake`; the standalone has no such byte for AI cars, see
// RwCarFx.cpp).
std::uint32_t CarVisFlags(int course_id, bool brake);
// MASHED_CAR_VIS=0 turns the per-part visibility off (every atomic draws, the old behaviour).
bool CarVisEnabled();
// The glass atomics FUN_00420870 leaves without a RENDER bit are drawn by FUN_00420050 @
// 0x00420050 right after the clump render (`call 0x4e6680`, 0x00420162), one by one through
// the atomic's own render callback (`call [eax+0x48]`, 0x0042019d / 0x004201d1). `which` 0 is the
// first (part 0xe at state 0, 0x2f at state 1), 1 the second (0xf / 0x30); -1 when it is not
// drawn (state 2, or F's 0x400 / 0x10 / 0x1000 / 0x20 / 0x2000 bits).
int CarGlassPart(int state, std::uint32_t flags, int which);

}  // namespace LibRw
}  // namespace mashed_re

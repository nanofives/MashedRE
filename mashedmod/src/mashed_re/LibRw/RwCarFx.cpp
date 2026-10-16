// LibRw/RwCarFx.cpp — [rtex 2026-10-16] see RwCarFx.h. The three lookups call the single
// bodies in Vehicle/CarMaterialTables.cpp (shared with the .asi), never a copy.

#include "RwCarFx.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" std::uint32_t __cdecl IdxPtr404e00(std::uint32_t p1);                    // 0x00404e00
extern "C" std::uint32_t __cdecl IdxPtr404e20(std::uint32_t p1, std::uint32_t p2);  // 0x00404e20
extern "C" std::uint32_t __cdecl GatedSwitch636ad0(std::uint32_t a);                // 0x0041f360

namespace mashed_re {
namespace LibRw {
namespace {

// Vehicle table at 0x005f37a8 (stride 0x84): piz base name at +0 of entries 0, 6, 12, ...
// (ids 0..77 at +0x80 equal the entry index; FUN_0041f330 @ 0x0041f330 searches it).
const char* const kVehicleGroups[13] = {
    "Shuriken", "Advantag", "Atmos", "Stallion", "Bullet", "Sputter", "Cooler",
    "Shorty", "Crusader", "Creeper", "BamBam", "Kustom", "Formula"};

bool EnvOff(const char* name) {
    const char* e = std::getenv(name);
    return e && e[0] == '0' && e[1] == '\0';
}

}  // namespace

const char* CarDirtBodyName(int course_id) {
    if (course_id < 0) return nullptr;
    return reinterpret_cast<const char*>(
        static_cast<std::uintptr_t>(IdxPtr404e00(static_cast<std::uint32_t>(course_id))));
}

const char* CarDirtWheelName(int course_id, int vehicle_id) {
    if (course_id < 0 || vehicle_id < 0) return nullptr;
    return reinterpret_cast<const char*>(static_cast<std::uintptr_t>(
        IdxPtr404e20(static_cast<std::uint32_t>(course_id), static_cast<std::uint32_t>(vehicle_id))));
}

// FUN_0041ecc0 @ 0x0041ecc0's selector: switch(iVar5) { case 0..3, 0x21..0x24: param_3 (the
// wheel texture); default: param_2 (the body texture) }.
bool CarDirtIsWheelPart(int part) {
    return (part >= 0 && part <= 3) || (part >= 0x21 && part <= 0x24);
}

bool CarEnvIsEnvPart(int part) {
    return part >= 0 && GatedSwitch636ad0(static_cast<std::uint32_t>(part)) != 0;
}

int CarVehicleGroupId(const char* piz_base) {
    if (!piz_base) return -1;
    for (int g = 0; g < 13; ++g)
        if (_stricmp(piz_base, kVehicleGroups[g]) == 0) return g * 6;
    return -1;
}

bool CarDirtEnabled() {
    static const bool on = !EnvOff("MASHED_CAR_DIRT");
    return on;
}

// OPT-IN (MASHED_CAR_ENV=1), default OFF: PREREG_rtex rule 2 fails while the standalone still
// draws every car LOD (C02, U-9079) -- the env second pass then also runs on the duplicate
// glass atomics the original never draws (an uncited EXTRA CarGloss row), and the newly
// matched blended CarGloss draws add a C21 ORDER row. verify/rtex_20261016/RESULT_rtex.md.
bool CarEnvEnabled() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_CAR_ENV");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    return on;
}

}  // namespace LibRw
}  // namespace mashed_re

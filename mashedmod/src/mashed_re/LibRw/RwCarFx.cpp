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

bool CarVisEnabled() {
    static const bool on = !EnvOff("MASHED_CAR_VIS");
    return on;
}

// FUN_00420870 @ 0x00420870, the per-part loop at 0x00420a0d..0x00420b27: part ebp = 0..0x7f, the
// atomic is [esi+ebp*4] (esi = the car record), skipped when null (`je 0x420b20`, 0x00420a1d).
// S = *(rec+0x298) (&DAT_0063dc78), F = *(rec+0x294) (&DAT_0063dc74). The result starts as 0
// (`xor eax,eax` 0x00420a19); parts above 0x67 take it straight to 0x00420af8 (`ja`, 0x00420a26)
// and the jump table (0x00420a33, byte index 0x00420c94, targets 0x00420c3c) sends every unlisted
// part to the same 0, so those are never visible. Each handler's entry is cited on its case;
// all but 0x17 / 0x32 then pass 0x00420af8 / 0x00420b02..0x00420b14 (`and eax, ~-(F & 0x400 != 0)`),
// and the call is `push eax; push ecx; call FUN_004b5240` (0x00420b16..0x00420b18).
bool CarPartVisible(int state, std::uint32_t flags, int part) {
    const bool f10 = (flags & 0x10u) != 0, f20 = (flags & 0x20u) != 0;
    const bool f400 = (flags & 0x400u) != 0;
    bool v = false;
    switch (part) {
    case 0: case 1: case 2: case 3: case 8: case 9: case 0x10: case 0x12: case 0x14:
        v = state == 0; break;                                          // 0x00420a3a
    case 0x21: case 0x22: case 0x23: case 0x24: case 0x29: case 0x2a:
        v = state == 1; break;                                          // 0x00420a4c
    case 0x46: v = state == 0; break;                                   // 0x00420a5e
    case 0x31: case 0x47: v = state == 1; break;                        // 0x00420b52
    case 4: case 6:   v = state == 0 && !f10; break;                    // 0x00420a70
    case 0x25: case 0x27: v = state == 1 && !f10; break;                // 0x00420a87
    case 0xa: case 0xc:   v = state == 0 && !f20; break;                // 0x00420a9a
    case 0x2b: case 0x2d: v = state == 1 && !f20; break;                // 0x00420aae
    case 5: case 7:   v = state == 0 && f10; break;                     // 0x00420ac0
    case 0x26: case 0x28: v = state == 1 && f10; break;                 // 0x00420ad3
    case 0xb: case 0xd:   v = state == 0 && f20; break;                 // 0x00420ae4
    case 0x2c: case 0x2e: v = state == 1 && f20; break;                 // 0x00420b4a
    case 0x15: v = (flags & 0x40u) != 0 && !f10; break;                 // 0x00420b61
    case 0x18: v = (flags & 0x80u) != 0 && !f20; break;                 // 0x00420b6f
    case 0x11: v = (flags & 0x800u) != 0 && !f20; break;                // 0x00420b7d, no state test
    case 0x17: return state == 0 && f400;                               // 0x00420b88 -> 0x00420b16, no mask
    case 0x32: return state == 1 && f400;                               // 0x00420b92 -> 0x00420b16, no mask
    case 0x64: v = state == 2 && (flags & 0x30u) == 0; break;           // 0x00420bb3
    case 0x65: v = state == 2 && f10 && !f20; break;                    // 0x00420bcc
    case 0x66: v = state == 2 && !f10 && f20; break;                    // 0x00420bea
    case 0x67: v = state == 2 && f10 && f20; break;                     // 0x00420c10
    default: v = false; break;                                          // 0x00420a19 / 0x00420a26
    }
    return v && !f400;
}

// FUN_00420050 @ 0x00420050 after `call 0x4e6680` (0x00420162): `test ah,4; jne 0x4201d7`
// (F & 0x400, 0x00420170) skips both; the first atomic needs F & 0x10 and F & 0x1000 clear
// (0x00420175, 0x00420179), the second F & 0x20 and F & 0x2000 clear (0x004201a9, 0x004201ad).
// The atomic is the record's dword [esi+0x38] (part 0xe) or [esi+0xbc] (0x2f) for the first,
// [esi+0x3c] (0xf) or [esi+0xc0] (0x30) for the second, chosen by S = 0 / 1 (0x00420184..0x00420198,
// 0x004201b8..0x004201cc); any other S draws neither.
int CarGlassPart(int state, std::uint32_t flags, int which) {
    if (flags & 0x400u) return -1;
    if (which == 0) {
        if ((flags & 0x10u) || (flags & 0x1000u)) return -1;
        return state == 0 ? 0xe : state == 1 ? 0x2f : -1;
    }
    if ((flags & 0x20u) || (flags & 0x2000u)) return -1;
    return state == 0 ? 0xf : state == 1 ? 0x30 : -1;
}

// FUN_00446520 @ 0x00446520: `fld [ebp-0x1ac]; fcomp [0x005ce1d0]; fnstsw ax; test ah,5; jp`
// (0x00446952..0x00446963) -- `push 1` (0x00446965) when d2 < 144.0, `push 2` (0x00446975)
// otherwise, including equal and unordered.
int CarLodState(double d2) {
    return d2 < 144.0 ? 1 : 2;
}

// FUN_0040da50 @ 0x0040da50: DAT_0063bc44[slot] = 1 for DAT_00644158 in {0, 0x1a, 0x21, 0x24},
// else 0; FUN_0041eeb0 @ 0x0041eeb0 ORs 0x40 and 0x80 from it (0x0041eec5..0x0041ef1c). Bit 3 is
// FUN_0041eda0's. 0x10 / 0x20 (FUN_0041ede0) and 0x400 (FUN_0041ef80) are zero here: the
// standalone has no damage or wreck state. 0x800 is FUN_0041eeb0's fourth argument,
// `*(char*)(0x007f103c + ctrl*0x4c + 1) != 0` (FUN_0040da50, the controller's brake byte `[5]`);
// the standalone's default AI cars produce no such byte, so `brake` is false for them unless the
// matched-pose instrument supplies it (TrackRenderer MASHED_CAR_BRAKE_ROWS). [UNCERTAIN U-9698]
std::uint32_t CarVisFlags(int course_id, bool brake) {
    const bool lit = course_id == 0 || course_id == 0x1a || course_id == 0x21 || course_id == 0x24;
    return 0x8u | (lit ? 0xc0u : 0u) | (brake ? 0x800u : 0u);
}

// [carlod 2026-10-16] DEFAULT ON together with the per-part visibility (MASHED_CAR_VIS), opt-out
// MASHED_CAR_ENV=0; MASHED_CAR_ENV=1 forces it on even with MASHED_CAR_VIS=0. rtex held it opt-in:
// with every car LOD drawn the env second pass also ran on the duplicate glass atomics the original
// never draws (an uncited EXTRA CarGloss row, Arctic 1051 pixels +0.60). With CarPartVisible and
// the glass pass it passes PREREG_carlod rules 1-6 (verify/carlod_20261016/RESULT_carlod.md).
bool CarEnvEnabled() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_CAR_ENV");
        if (e && e[0] == '1' && e[1] == '\0') return true;
        if (e && e[0] == '0' && e[1] == '\0') return false;
        return CarVisEnabled();
    }();
    return on;
}

}  // namespace LibRw
}  // namespace mashed_re

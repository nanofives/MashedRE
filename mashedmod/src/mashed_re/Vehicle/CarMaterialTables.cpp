// Mashed RE — car dirt / env-map lookup functions, ONE body per RVA for both targets.
// [rtex 2026-10-16, U-9693]
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// FUN_00404e00 / FUN_00404e20 moved here unchanged from Util/PromoLoop_round74.cpp and
// FUN_0041f360 from Util/PromoLoop_round66.cpp (promote-round C3; same export names, same
// expressions in the .asi), so the standalone's car material set-up (LibRw/RwCarFx.cpp) calls
// the same bodies the .asi hooks instead of a second copy (build.bat "ONE RVA, ONE BODY").
// Under MASHED_STANDALONE (the exe) the MASHED absolute tables resolve to named standalone
// data, the Save\GameSaveBuffer.cpp pattern:
//   DAT_00639cc8[0x28] / DAT_00636c08[0xc30]  -> the tables FUN_00404830 @ 0x00404830 builds
//                                               (CarMaterialTables_dirt.inc, generated from
//                                               its decompilation by
//                                               verify/rtex_20261016/gen_dirt_tables.py)
//   PTR 0x005ea0b8[4] / 0x005ea0c8[24]        -> the name strings, read out of the anchored
//                                               binary (verify/rtex_20261016/evidence)
//   DAT_00636ad0                              -> 0, the value read live on TRAINING, Arctic and
//                                               City (PREREG_rtex amendment 1). [UNCERTAIN
//                                               U-9697] its writers FUN_00401000 /
//                                               FUN_0040cda0 / FUN_00499400 are not ported.

#include "../Core/HookSystem.h"

#include <cstdint>

#ifdef MASHED_STANDALONE
namespace {

// 0x00404830 — FUN_00404830: fills DAT_00639cc8[0x28] and DAT_00636c08[0xc30] with -1, then
// the stores of CarMaterialTables_dirt.inc.
struct DirtTables {
    std::int32_t body[0x28];
    std::int32_t wheel[0xc30];
    DirtTables() {
        for (std::int32_t& v : body) v = -1;
        for (std::int32_t& v : wheel) v = -1;
#include "CarMaterialTables_dirt.inc"
    }
};

const DirtTables& Tables() {
    static const DirtTables t;
    return t;
}

const char* const kBodyNames[4] = {"DirtBody", "EarthBody", "GrimeBody", "SandBody"};
const char* const kWheelNames[24] = {
    "DirtAdWheel", "EarthAdWheel", "GrimeAdWheel", "SandAdWheel",
    "DirtAtmWheel", "EarthAtmWheel", "GrimeAtmWheel", "SandAtmWheel",
    "DirtShWheel", "EarthShWheel", "GrimeShWheel", "SandShWheel",
    "DirtBuWheel", "EarthBuWheel", "GrimeBuWheel", "SandBuWheel",
    "DirtStWheel", "EarthStWheel", "GrimeStWheel", "SandStWheel",
    "DirtSpWheel", "EarthSpWheel", "GrimeSpWheel", "SandSpWheel"};

const std::int32_t g_dat_00636ad0 = 0;

std::int32_t Tab639cc8(std::uint32_t i) { return i < 0x28u ? Tables().body[i] : -1; }
std::int32_t Tab636c08(std::uint32_t i) { return i < 0xc30u ? Tables().wheel[i] : -1; }
std::uint32_t Name5ea0b8(std::uint32_t i) {
    return i < 4u ? static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(kBodyNames[i])) : 0u;
}
std::uint32_t Name5ea0c8(std::uint32_t i) {
    return i < 24u ? static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(kWheelNames[i])) : 0u;
}

}  // namespace
#define TAB_639CC8(i)  Tab639cc8(i)
#define TAB_636C08(i)  Tab636c08(i)
#define NAME_5EA0B8(i) Name5ea0b8(i)
#define NAME_5EA0C8(i) Name5ea0c8(i)
#define DAT_636AD0     g_dat_00636ad0
#else
#define TAB_639CC8(i)  (*reinterpret_cast<std::int32_t*>(0x00639cc8u + (i) * 4u))
#define TAB_636C08(i)  (*reinterpret_cast<std::int32_t*>(0x00636c08u + (i) * 4u))
#define NAME_5EA0B8(i) (*reinterpret_cast<std::uint32_t*>(0x005ea0b8u + (i) * 4u))
#define NAME_5EA0C8(i) (*reinterpret_cast<std::uint32_t*>(0x005ea0c8u + (i) * 4u))
#define DAT_636AD0     (*reinterpret_cast<std::int32_t*>(0x00636ad0u))
#endif

// 0x00404e00 / 0x00404e20: MATCH-S01 (2026-10-31). Both bodies are re/match/s01/00404e00.c, byte-identical to the
// original under re/tools/match/match.py (verify/matchs01_20261031/RESULT_MATCH-S01.md), compiled here unchanged.
// With MASHED_RE_PORT the file takes its tables from this TU: the original addresses in the .asi, the standalone
// tables above in the exe (the replaced bodies read the same storage; they also bounds-checked the index, which the
// original does not).
#define MASHED_RE_PORT 1
extern "C" {
#define FUN_00404e00 IdxPtr404e00
#define FUN_00404e20 IdxPtr404e20
__declspec(dllexport) char* __cdecl IdxPtr404e00(int body);
__declspec(dllexport) char* __cdecl IdxPtr404e20(int car, int wheel);
#ifdef MASHED_STANDALONE
#define DAT_00639cc8 (Tables().body)
#define DAT_00636c08 (Tables().wheel)
#define PTR_s_DirtBody_005ea0b8 ((char**)kBodyNames)
#define PTR_s_DirtAdWheel_005ea0c8 ((char**)kWheelNames)
#else
#define DAT_00639cc8 ((int*)0x00639cc8u)
#define DAT_00636c08 ((int*)0x00636c08u)
#define PTR_s_DirtBody_005ea0b8 ((char**)0x005ea0b8u)
#define PTR_s_DirtAdWheel_005ea0c8 ((char**)0x005ea0c8u)
#endif
#include "../../../../re/match/s01/00404e00.c"
}  // extern "C"
RH_ScopedInstall(IdxPtr404e00, 0x00404e00);
RH_ScopedInstall(IdxPtr404e20, 0x00404e20);

// 0x0041f360
extern "C" __declspec(dllexport) std::uint32_t __cdecl GatedSwitch636ad0(std::uint32_t a) {
    if (DAT_636AD0 == 0) {                                                         // gate
        switch (a) {
        case 4: case 5: case 8: case 10: case 0xb: case 0xe: case 0xf:
        case 0x25: case 0x26: case 0x29: case 0x2b: case 0x2c: case 0x2f: case 0x30:
            return 1;
        }
    }
    return 0;
}
RH_ScopedInstall(GatedSwitch636ad0, 0x0041f360);

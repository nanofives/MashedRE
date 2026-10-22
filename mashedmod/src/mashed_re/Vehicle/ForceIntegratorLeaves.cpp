// Mashed RE — the standalone's ports of three leaves A5 (0x0046ddb0) calls: 0x00442ce0, 0x00442c80, 0x0046dbe0.
// [dcphys2 2026-10-22] moved verbatim out of Vehicle/ForceIntegrator.cpp, which is now shared by both targets; this TU is
// exe only (the .asi's A5 calls the originals at their RVAs).
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include <cstdlib>
#include "ForceIntegrator.h"

namespace mashed_re {
namespace Vehicle {

using namespace fi;

// [u9772] MASHED_A5_FD0_LIVE=1 (default OFF): A5 (0x0046ddb0), FUN_00442ce0 and FUN_00442c80 test the live
// DAT_007f0fd0 word, as the original does, instead of g_playerCount (= the car count, VehiclePhysics_Init).
static int Fd0Word() {
    static const bool s = [] { const char* e = std::getenv("MASHED_A5_FD0_LIVE"); return e && e[0] == '1'; }();
    return s ? *reinterpret_cast<const int*>(0x007f0fd0u) : g_playerCount;
}

// [u9777] MASHED_A5_RUBBER_LIVE=1 (default ON since rbflip 2026-10-21 through the FAITH seed, MASHED_NO_A5_RUBBER_LIVE=1 opts out; verify/u9777_20261020/PREREG_u9777.md): the two leaves read the live
// FUN_00442a60 outputs, refdist 0x008989b0[car] (0x00442ca2, 0x00442d0f) and the reference car 0x008989c8 (0x00442d2f), as
// the original does, instead of g_rubberBand / g_rubberRefCar (all zero, no writer). Race/SpectatorDistances.cpp fills both.
static bool A5RubberLive() {
    static const bool s = [] { const char* e = std::getenv("MASHED_A5_RUBBER_LIVE"); return e && e[0] == '1'; }();
    return s;
}
static float RubberRefDist(int car) {
    return A5RubberLive() ? reinterpret_cast<const float*>(0x008989b0u)[car] : g_rubberBand[car];
}
static int RubberRefCar() {
    return A5RubberLive() ? *reinterpret_cast<const int*>(0x008989c8u) : g_rubberRefCar;
}

// ---- 0x00442ce0: catch-up / rubber-band grip multiplier (resolves U-2687) ----
float RubberBandGrip(int car, float current, float band)
{
    const int fd0 = Fd0Word();                                // DAT_007f0fd0
    if ((fd0 == 4) || (fd0 == 9) || (fd0 == 8)) return kOne;
    Fi_GameModeTick();   // FUN_0040e340 (observed side-effect retained)
    (void)current;
    if (RubberRefDist(car) == kZero) {                        // 0x00442d0f
        int i1 = CarContactCount(car);
        int i2 = CarContactCount(RubberRefCar());             // 0x00442d2f
        if (i1 == 0) {
            if (i2 == 3) return k0p6 * band + kOne;          // _DAT_005cc318
            if (i2 == 2) return kHalf * band + kOne;         // _DAT_005cc32c
            if (i2 == 1) return kQuarter * band + kOne;      // _DAT_005cc564
        } else if (i1 == 1) {
            if (i2 == 3) return kHalf * band + kOne;
            if (i2 == 2) return kQuarter * band + kOne;
        } else if ((i1 == 2) && (i2 == 3)) {
            return kQuarter * band + kOne;
        }
    }
    return 0.0f * band + kOne;
}

// ---- 0x00442c80: rubber-band activation gate (resolves U-3563) ----
int RubberBandGate(int car)
{
    int mode = Fi_GameMode();
    const int fd0 = Fd0Word();                                // DAT_007f0fd0
    if ((mode == 6) && (fd0 != 4) && (fd0 != 9) &&
        (fd0 != 8) && (kRubberThr < RubberRefDist(car))) {   // 0x00442ca2..0x00442cb4
        return 1;
    }
    return 0;
}

// ---- 0x0046dbe0: per-car contact-count getter (DAT_008815a8) ----
int CarContactCount(int car)
{
    return *reinterpret_cast<int*>(reinterpret_cast<char*>(g_vehicleArrayBase) + 8 + car * 0xd04);
}

}  // namespace Vehicle
}  // namespace mashed_re

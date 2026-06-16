// Mashed RE — WS-A5 force-integrator structural smoke test.
//
// Exercises the full FUN_0046ddb0 port end-to-end on a populated 4-car vehicle
// array: verifies it reads the per-car contact summaries + per-wheel contact
// flags correctly (grounded count), runs every phase without OOB/crash, and that
// the rubber-band callees (FUN_00442ce0/00442c80/0046dbe0) match the decompiler.
// (Exact physics fidelity is gated on installed-hook scenario telemetry, not this
// test — this proves the transcription is structurally sound + the consumption of
// the WS-B contact arrays is wired right.)
//
// Build (from mashedmod/src/mashed_re/Vehicle):
//   cl /nologo /EHsc /Fe:..\..\..\build\forceint_selftest.exe forceint_selftest.cpp ^
//      ForceIntegrator.cpp ForceIntegratorStubs.cpp ..\Collision\CarWorldContacts.cpp ^
//      ..\Collision\ContactStubs.cpp ..\Collision\ContactProducer.cpp
#include "ForceIntegrator.h"
#include <cstdio>
#include <cstring>
#include <vector>

using namespace mashed_re::Vehicle;

int main()
{
    const int N = 4;
    std::vector<int> arr(N * 0xd04 / 4 + 16, 0);   // 4 cars, int-indexed
    g_vehicleArrayBase = arr.data();
    int* self = arr.data();                         // car 0

    self[0] = 0;                                    // car index
    // 3 of 4 wheels in contact (active flags at 0x66/0x97/0xc8/0xf9):
    self[0x66] = 1; self[0x97] = 1; self[0xc8] = 1; /* self[0xf9] left 0 */
    // give wheels a non-degenerate suspension load (pf[6] at wheel+? ) and speed:
    reinterpret_cast<float*>(self)[0x279] = 30.0f;  // speed
    reinterpret_cast<float*>(self)[0x14]  = 1000.0f;// mass
    reinterpret_cast<float*>(self)[0x54]  = 1.0f;   // gear products
    reinterpret_cast<float*>(self)[0x55]  = 1.0f;
    reinterpret_cast<float*>(self)[0x56]  = 1.0f;
    // car-summary fields for the other 3 cars: active@+4, count@+8
    auto setCar = [&](int c, int active, int count) {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(g_vehicleArrayBase) + 4 + c * 0xd04) = active;
        *reinterpret_cast<int*>(reinterpret_cast<char*>(g_vehicleArrayBase) + 8 + c * 0xd04) = count;
    };
    setCar(0, 1, 3);  setCar(1, 1, 2);  setCar(2, 0, 0);  setCar(3, 1, 1);

    // runtime globals (stubs default 0; give plausible values so phases run)
    g_playerCount = 2; g_raceTimer = 100; g_suspScale = 1.0f; g_suspDtTerm = 0.01f;
    g_gravScale = 9.8f; g_gravY = -1.0f;

    unsigned char xform[64] = {0};
    VehicleWheelForceIntegrate(self, 0.016f, xform);

    float grounded = reinterpret_cast<float*>(self)[0x278];
    std::printf("grounded count        : %.1f (expect 3.0)\n", grounded);

    // rubber-band callees (FUN_0046dbe0 / 00442c80 / 00442ce0):
    int cc0 = CarContactCount(0), cc1 = CarContactCount(1);
    std::printf("CarContactCount 0,1   : %d %d (expect 3 2)\n", cc0, cc1);

    // gate: mode!=6 (stub Fi_GameMode==0) -> 0; band below thresh -> 0
    g_rubberBand[0] = 8.0f;   // > 7.0 thresh, but mode stub=0 so gate stays 0
    int gate = RubberBandGate(0);
    std::printf("RubberBandGate(0)     : %d (expect 0; game-mode stub != 6)\n", gate);

    // grip: playerCount 2 (not 4/9/8); band[0]!=0 -> returns 0*band+1 = 1.0
    float grip = RubberBandGrip(0, 1.0f, 0.5f);
    std::printf("RubberBandGrip        : %.3f (expect 1.000; band[0]!=0 path)\n", grip);

    bool pass = (grounded == 3.0f) && (cc0 == 3) && (cc1 == 2) && (gate == 0) && (grip == 1.0f);
    std::printf("RESULT: %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
}

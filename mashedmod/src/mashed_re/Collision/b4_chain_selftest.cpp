// Mashed RE — WS-B4 chain integration test: producer -> wheel solver -> integrator.
//
// Drives the FULL ported contact->force pipeline end-to-end on one vehicle:
//   WheelContactSolver (0x0046f6c0) runs the broadphase (ProduceTerrainBatch over
//   a ground triangle) + the classifier + the per-wheel 3-state machine, which
//   writes the wheel STATES; then VehicleWheelForceIntegrate (0x0046ddb0) reads
//   those states as the grounded count. Proves the producer/classifier output now
//   flows through the orchestrator into the integrator — the heart of WS-B4 —
//   WITHOUT the (still-unported) vehicle init / control integrator.
//
// Build (from mashedmod/src/mashed_re/Collision):
//   cl /nologo /EHsc /Fe:..\..\..\build\b4_chain_selftest.exe b4_chain_selftest.cpp ^
//      CarWorldContacts.cpp ContactProducer.cpp WheelContactSolver.cpp ContactStubs.cpp ^
//      ..\Vehicle\ForceIntegrator.cpp ..\Vehicle\ForceIntegratorStubs.cpp
#include "ContactConstants.h"
#include "ContactDeps.h"
#include "ContactSolvers.h"
#include "../Vehicle/ForceIntegrator.h"
#include <cstdio>
#include <cstring>
#include <vector>

using namespace mashed_re;

int main()
{
    std::vector<int> arr(4 * 0xd04 / 4 + 16, 0);
    int* self = arr.data();
    Vehicle::g_vehicleArrayBase = arr.data();
    auto F = [&](int i) -> float& { return *reinterpret_cast<float*>(self + i); };

    self[0] = 0;            // car index
    self[0x26b] = 0;        // wheel-ring selector -> iVar12 = self + 0x24a
    F(0x129) = 5.0f;        // query radius (byte 0x4a4)
    F(0x256) = -2.5f; F(0x257) = 0.0f; F(0x258) = -2.5f;   // query centre (iVar12+0x30..)
    F(0x272) = 0.0f; F(0x273) = 1.0f; F(0x274) = 0.0f;     // approach vec (up)
    F(0x275) = 0.0f; F(0x276) = 0.0f; F(0x277) = 1.0f;     // forward (z)
    F(0x26c) = 0.0f; F(0x26d) = 0.0f; F(0x26e) = 5.0f;     // velocity
    F(0x279) = 5.0f;        // speed (avoid /0 in the state machine)
    F(0x14)  = 1000.0f;     // mass

    // 4 wheels PENETRATING the y=0 ground by 0.1 (depth -0.1 in band, < 0 -> state 2),
    // inside the triangle's XZ projection (x+z < 0):
    float wheels[12] = {
        -2.0f, -0.1f, -2.0f,
        -3.0f, -0.1f, -2.0f,
        -2.0f, -0.1f, -3.0f,
        -4.0f, -0.1f, -4.0f,
    };
    std::memcpy(Collision::g_wheelContactPos, wheels, sizeof(wheels));

    // ground triangle (CCW so (v1-v0)x(v2-v0) = +Y), big, surfaceKey not the sentinel:
    static Collision::CollTriangle ground;
    float v0[3] = {-100, 0, -100}, v1[3] = {-100, 0, 100}, v2[3] = {100, 0, -100};
    std::memcpy(ground.v0, v0, sizeof(v0));
    std::memcpy(ground.v1, v1, sizeof(v1));
    std::memcpy(ground.v2, v2, sizeof(v2));
    ground.normal[0]=0; ground.normal[1]=1; ground.normal[2]=0;
    ground.material = 7; ground.surfaceKey = 1;
    Collision::g_worldTris = &ground;
    Collision::g_worldTriCount = 1;

    // --- run the orchestrator (producer + classifier + wheel state machine) ---
    unsigned char world[64] = {0};
    Collision::WheelContactSolver(self, world, 1);

    int states[4] = { self[0x66], self[0x97], self[0xc8], self[0xf9] };
    float groundedAfterSolver = F(0x278);
    std::printf("classifier active contacts : %d (expect 4)\n", Collision::g_activeContactCount);
    std::printf("wheel states               : %d %d %d %d (expect mostly 2; one may be dropped to 0)\n",
                states[0], states[1], states[2], states[3]);
    std::printf("grounded after wheel solver: %.1f\n", groundedAfterSolver);

    // --- run the integrator; it reads the wheel states as the grounded count ---
    Vehicle::g_playerCount = 2; Vehicle::g_suspScale = 1.0f; Vehicle::g_suspDtTerm = 0.01f;
    Vehicle::g_gravScale = 9.8f; Vehicle::g_gravY = -1.0f;
    unsigned char xform[64] = {0};
    Vehicle::VehicleWheelForceIntegrate(self, 0.016f, xform);
    float groundedInt = F(0x278);
    int nonzero = (states[0]!=0)+(states[1]!=0)+(states[2]!=0)+(states[3]!=0);
    std::printf("grounded read by integrator: %.1f (matches non-zero states = %d)\n", groundedInt, nonzero);

    bool pass = (Collision::g_activeContactCount == 4) &&
                (nonzero >= 3) &&
                (groundedInt == (float)nonzero);
    std::printf("RESULT: %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
}

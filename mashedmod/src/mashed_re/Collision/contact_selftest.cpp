// Mashed RE — WS-B2/B4 contact-source self-test.
//
// Behavioral check of the ported PRODUCER (ContactProducer / LAB_00468b80) +
// CLASSIFIER (WheelTerrainContactClassifier, 0x0046cc40) on controlled geometry:
// 4 wheels just above one ground triangle, inside its XZ projection, with an
// approach vector into the surface -> the classifier must register 4 contacts
// (g_activeContactCount==4) and latch each wheel's contact normal+depth. This is
// the contact-source half of WS-B4, exercising exactly what the captured original
// baseline measures (terrain batch count + active wheel contacts) WITHOUT needing
// the WS-A consumer or the vehicle-struct init.
//
// Build (standalone console; from mashedmod/src/mashed_re):
//   cl /nologo /EHsc /Fe:..\..\build\contact_selftest.exe contact_selftest.cpp ^
//      CarWorldContacts.cpp ContactStubs.cpp ContactProducer.cpp
#include "ContactConstants.h"
#include "ContactDeps.h"
#include "ContactSolvers.h"
#include <cstdio>
#include <cstring>

using namespace mashed_re::Collision;

int main()
{
    // --- fake vehicle (the 0xd04 record), zeroed -> history lookup returns "new" ---
    static int veh[0xd04];          // >= 0xd04 bytes; int-indexed like the decompiler
    std::memset(veh, 0, sizeof(veh));
    // approach-velocity vector veh[0x272..0x274]: align with the (up) face normal so
    // normal·approach = 1.0 > _DAT_005cc99c (0.3).  (classifier approach-speed gate)
    reinterpret_cast<float*>(veh)[0x272] = 0.0f;
    reinterpret_cast<float*>(veh)[0x273] = 1.0f;
    reinterpret_cast<float*>(veh)[0x274] = 0.0f;

    // --- one large ground triangle in the y=0 plane, CCW so (v1-v0)x(v2-v0) = +Y ---
    CollTriangle tri;
    float v0[3] = { -100.0f, 0.0f, -100.0f };
    float v1[3] = { -100.0f, 0.0f,  100.0f };
    float v2[3] = {  100.0f, 0.0f, -100.0f };
    std::memcpy(tri.v0, v0, sizeof(v0));
    std::memcpy(tri.v1, v1, sizeof(v1));
    std::memcpy(tri.v2, v2, sizeof(v2));
    tri.normal[0] = 0.0f; tri.normal[1] = 1.0f; tri.normal[2] = 0.0f;  // unit up
    tri.material = 7;
    tri.surfaceKey = 1;     // not the first-frame sentinel -> classifier takes the contact path

    // --- 4 wheels just above the plane (depth 0.1 in band [-0.25,0.25)), inside (x+z<0) ---
    float wheels[12] = {
        -2.0f, 0.1f, -2.0f,
        -3.0f, 0.1f, -2.0f,
        -2.0f, 0.1f, -3.0f,
        -4.0f, 0.1f, -4.0f,
    };
    std::memcpy(g_wheelContactPos, wheels, sizeof(wheels));
    for (int i = 0; i < 4; ++i) g_wheelSkipFlags[i] = 0;
    g_activeContactCount = 0;   // (FUN_0046f6c0 resets DAT_0088e650 each tick; we stand in)

    // --- producer: fill the terrain batch from the triangle near the car centre ---
    float center[3] = { -2.0f, 0.0f, -2.0f };
    int produced = ProduceTerrainBatch(center, 5.0f, &tri, 1);

    // --- classifier: consume the batch, fill per-wheel contacts ---
    WheelTerrainContactClassifier(veh, reinterpret_cast<float*>(g_terrainBatch));

    std::printf("produced batch entries : %d (terrainEntryCount=%d)\n", produced, g_terrainEntryCount);
    std::printf("active wheel contacts  : %d (expect 4)\n", g_activeContactCount);
    std::printf("skip flags             : %d %d %d %d\n",
                g_wheelSkipFlags[0], g_wheelSkipFlags[1], g_wheelSkipFlags[2], g_wheelSkipFlags[3]);
    // wheel 0 contact entry (vehicle int-word 0x65 + 0*0x31): normal f[0x1b..0x1d], depth f[0]
    float* w0 = reinterpret_cast<float*>(veh) + 0x65;
    std::printf("wheel0 latched normal  : (%.3f, %.3f, %.3f)  depth=%.4f\n",
                w0[0x1b], w0[0x1c], w0[0x1d], w0[0]);

    bool pass = (produced == 1) && (g_activeContactCount == 4) &&
                (w0[0x1b] == 0.0f && w0[0x1c] == 1.0f && w0[0x1d] == 0.0f);
    std::printf("RESULT: %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
}

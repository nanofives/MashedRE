// Mashed RE — port-state migration 2/3 (L-78 (c)): the original's car record block kept alive in the exe.
// Exe only. PREREG: verify/portstaterecords_20261101/PREREG_MATCH-PORTSTATE-RECORDS.md.
//
// The storage move itself is in VehiclePhysicsRun.cpp (g_records) and Race/CarRegistrant.cpp (s_rec): with the
// knob ON both point at the original addresses, so there is no copy and no mirror. This TU holds the knob and the
// witness the T13 re-check reads (the values the blocked matched bodies would see).

#include "RecordBlockState.h"
#include "VehicleRecordBase.h"
#include "../Race/CarRegistrant.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Vehicle {
namespace {

std::int32_t I32(std::uintptr_t a) { return *reinterpret_cast<const std::int32_t*>(a); }
std::uint32_t U32(std::uintptr_t a) { return *reinterpret_cast<const std::uint32_t*>(a); }

struct Counts {
    long frames = 0, samples = 0;
    long dbe0_bad = 0;       // 0x0046dbe0: I32(0x008815a8 + v*0xd04) vs VehRecord(v)+8
    long f030_bad = 0;       // 0x0041f030: 16 B at 0x0063d9e0 + v*0x2ac + 0x258 vs VehRecord(v)+0x958 + radius bits
    long d510_bad = 0;       // 0x0046d510: `at` row of the +0x928 matrix (sel +0x9a8) vs +0x9d4, bitwise
    long d510_bad0 = 0;      // same with the matrix at +0x928 itself (the a2 source's +0x48 of 0x00881e80)
    long f030_live = 0;      // samples where the car visual record was filled (+0x290 == v and radius set)
    std::int32_t score_last[4] = {0, 0, 0, 0};   // 0x008995bc + v*0x138 (no exe producer)
    std::uint32_t held_last[4] = {0, 0, 0, 0};   // 0x0088fc88 + v*0xb4 (not migrated)
};
Counts s_c;

}  // namespace

bool PortStateRecords_On() {
    // L-88 = a (2026-11-02): decided flip, default ON. MASHED_NO_PORTSTATE_RECORDS=1 reproduces the old default
    // (exe-own static stores, G-CTRL gates.csv ba407a16). MASHED_PORTSTATE_RECORDS is still accepted (no-op).
    static const bool s_on = (std::getenv("MASHED_NO_PORTSTATE_RECORDS") == nullptr);
    return s_on;
}

void PortStateRecords_FrameWitness() {
    if (!PortStateRecords_On()) return;
    const char* r0 = VehRecord(0);
    if (!r0) return;
    ++s_c.frames;
    for (std::uint32_t v = 0; v < 4; ++v) {
        const char* r = VehRecord(v);
        const std::uintptr_t vb = static_cast<std::uintptr_t>(v) * kVehRecordStride;
        ++s_c.samples;
        if (I32(0x008815a8u + vb) != *reinterpret_cast<const std::int32_t*>(r + 0x8)) ++s_c.dbe0_bad;

        const std::uintptr_t vis = reinterpret_cast<std::uintptr_t>(Race::CarsReg_RecordBase()) +
                                   static_cast<std::uintptr_t>(v) * 0x2acu;
        if (I32(vis + 0x290u) == static_cast<std::int32_t>(v) && U32(vis + 0x264u) == 0x3f088c2du) {
            ++s_c.f030_live;
            if (std::memcmp(reinterpret_cast<const void*>(vis + 0x258u), r + 0x958, 12) != 0) ++s_c.f030_bad;
        }

        // FUN_004c3df0(out = rec+0x9d4, in = &DAT_00614708 (0,0,1), 1, matrix) = the matrix's `at` row (m[8..10]).
        const std::int32_t sel = *reinterpret_cast<const std::int32_t*>(r + 0x9a8);
        const char* m = r + 0x928 + static_cast<std::uintptr_t>(sel & 1) * 0x40u;
        if (std::memcmp(m + 0x20, r + 0x9d4, 12) != 0) ++s_c.d510_bad;
        if (std::memcmp(r + 0x928 + 0x20, r + 0x9d4, 12) != 0) ++s_c.d510_bad0;

        s_c.score_last[v] = I32(0x008995bcu + static_cast<std::uintptr_t>(v) * 0x138u);
        s_c.held_last[v] = U32(0x0088fc88u + static_cast<std::uintptr_t>(v) * 0xb4u);
    }
    if (s_c.frames % 600 != 0) return;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[portstate-records] frames=%ld samples=%ld base=%08x vis0=%08x dbe0_bad=%ld f030_live=%ld "
                        "f030_bad=%ld d510_bad=%ld d510_bad0=%ld score=%d,%d,%d,%d held=%08x,%08x,%08x,%08x\n",
                     s_c.frames, s_c.samples,
                     static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(r0)),
                     static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(Race::CarsReg_RecordBase())), s_c.dbe0_bad,
                     s_c.f030_live, s_c.f030_bad, s_c.d510_bad, s_c.d510_bad0,
                     s_c.score_last[0], s_c.score_last[1], s_c.score_last[2], s_c.score_last[3],
                     s_c.held_last[0], s_c.held_last[1], s_c.held_last[2], s_c.held_last[3]);
        std::fclose(f);
    }
}

} // namespace Vehicle
} // namespace mashed_re

// Mashed RE — port-state migration 1/3 (L-78 (c)): the original's course table kept alive in the exe.
// Exe only. PREREG: verify/portstatecourse_20261101/PREREG_MATCH-PORTSTATE-COURSE.md.
//
// The four ranges below are file-backed .data in the original (.data VA 0x005ea000, RawSize 0x4d000) that no
// original code writes. The exe's image pad (exe_main.cpp B17) holds them zero-filled, so a matched body that
// reads them in the exe reads 0 (U-9783, MATCH-TOPDOWN-T04 / T05 / T13). The tables are the bytes of
// original\MASHED.exe.unpatched at those addresses, transcribed once (the RESULT carries the FNV-1a check).
//
// The hook slot dwords are the original's .text addresses. In the exe a call through one is valid only where
// B16 put a thunk at that address, so nothing here calls through a slot.
//
// 0x005f2770 (= 0x005f2728) is seeded by Ai_BridgeLoad (TrackRenderer.cpp, H2, default ON) and is not touched
// here. The slot-state cells 0x005f275c..0x005f276b (+0x34 of 0x005f2728) are runtime state and lie outside
// every seeded range. 0x005f2674..0x005f2727 (nine more 0x14 records) has no reference in the original and
// is left alone.

#include "CourseTableState.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Match/MatchS04_0041e870.cpp: the byte-matched selector 0x0041e870 (re/match/s04/0041e870.c).
extern "C" void TrackNodeRecordScan(int param_1);

namespace mashed_re {
namespace Track {
namespace {

struct HookRec {
    char          name[0x10];
    std::uint32_t course_id;    // +0x10, the key 0x0041e870 compares
    std::uint32_t slot[13];     // +0x14 .. +0x44, called through the thunks 0x0041e8b0 + 0x10*k in the original
};
static_assert(sizeof(HookRec) == 0x48, "hook record stride 0x48");

// 0x005f2570..0x005f2673: 13 records x 5 dwords
const std::uint32_t kCourseRecs[65] = {
    0x0000001eu, 0x00000006u, 0x00000007u, 0x00000008u, 0x00000009u,   // 0x005f2570
    0x00000000u, 0x00000012u, 0x00000013u, 0x00000014u, 0x00000017u,   // 0x005f2584
    0x00000002u, 0x0000002au, 0x0000002bu, 0x0000002cu, 0x0000002du,   // 0x005f2598
    0x00000019u, 0x00000024u, 0x00000025u, 0x00000026u, 0x00000029u,   // 0x005f25ac
    0x0000000bu, 0x00000000u, 0x00000001u, 0x00000002u, 0x00000003u,   // 0x005f25c0
    0x00000021u, 0x00000042u, 0x00000043u, 0x00000044u, 0x00000045u,   // 0x005f25d4
    0x00000022u, 0x0000003cu, 0x0000003du, 0x0000003eu, 0x0000003fu,   // 0x005f25e8
    0x00000003u, 0x0000001eu, 0x0000001fu, 0x00000020u, 0x00000021u,   // 0x005f25fc
    0x0000001au, 0x00000048u, 0x00000049u, 0x0000004au, 0x0000004bu,   // 0x005f2610
    0x00000024u, 0x00000036u, 0x00000037u, 0x00000038u, 0x0000003bu,   // 0x005f2624
    0x00000025u, 0x0000000cu, 0x0000000du, 0x0000000eu, 0x00000011u,   // 0x005f2638
    0x00000026u, 0x00000018u, 0x00000019u, 0x0000001au, 0x0000001du,   // 0x005f264c
    0x00000027u, 0x00000030u, 0x00000031u, 0x00000032u, 0x00000033u,   // 0x005f2660
};
// 0x005f2728..0x005f275b: 13 record pointers
const std::uint32_t kCoursePtrs[13] = {
    0x005f2570u, 0x005f25fcu, 0x005f2660u, 0x005f2584u, 0x005f264cu, 0x005f25c0u, 0x005f25acu,
    0x005f2598u, 0x005f2624u, 0x005f25e8u, 0x005f2638u, 0x005f25d4u, 0x005f2610u,
};
// 0x005f33f8..0x005f379f: hook table, 13 x 0x48 {name[0x10], Course_Id, slot[13]}
const HookRec kHookRecs[13] = {
    { "training", 30, { 0x00451280u, 0x004513b0u, 0x00451460u, 0x00000000u, 0x0045b350u, 0x00451520u, 0x00000000u, 0x0045b350u, 0x00000000u, 0x00000000u, 0x004514f0u, 0x00000000u, 0x00000000u } },   // 0x005f33f8
    { "superg", 25, { 0x00450f00u, 0x00450f80u, 0x00450fc0u, 0x00000000u, 0x00451090u, 0x00451000u, 0x00000000u, 0x0045b350u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u } },   // 0x005f3440
    { "arctic", 0, { 0x00448940u, 0x00448ef0u, 0x00449030u, 0x00449010u, 0x004492b0u, 0x004490d0u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00448f60u, 0x00000000u, 0x00000000u } },   // 0x005f3488
    { "rouabout", 2, { 0x0044cb60u, 0x0044d3f0u, 0x0044d6f0u, 0x00449010u, 0x00000000u, 0x0044d830u, 0x0044d360u, 0x0044d3d0u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x0044ca70u } },   // 0x005f34d0
    { "sands", 11, { 0x00450ca0u, 0x004507b0u, 0x00450910u, 0x00000000u, 0x0045b350u, 0x00450a80u, 0x00450460u, 0x00450460u, 0x00000000u, 0x00000000u, 0x00450a20u, 0x00000000u, 0x0045b350u } },   // 0x005f3518
    { "warzone", 33, { 0x004527f0u, 0x00452b20u, 0x00452d10u, 0x00000000u, 0x0045b350u, 0x00452e00u, 0x00452af0u, 0x00452ad0u, 0x00000000u, 0x00000000u, 0x00452da0u, 0x00000000u, 0x00000000u } },   // 0x005f3560
    { "forest", 34, { 0x0044dea0u, 0x0044ded0u, 0x0044df00u, 0x00000000u, 0x0045b350u, 0x0044df30u, 0x00000000u, 0x0045b350u, 0x00000000u, 0x00000000u, 0x0045b350u, 0x00000000u, 0x00000000u } },   // 0x005f35a8
    { "egypt", 3, { 0x00449810u, 0x00449820u, 0x00449830u, 0x00000000u, 0x00000000u, 0x00449850u, 0x0045b350u, 0x004497b0u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00449780u } },   // 0x005f35f0
    { "storm", 36, { 0x0044d930u, 0x0044da40u, 0x0044db40u, 0x00000000u, 0x00000000u, 0x0044dbd0u, 0x0045b350u, 0x0045b350u, 0x00000000u, 0x00000000u, 0x0044dbb0u, 0x00000000u, 0x00000000u } },   // 0x005f3638
    { "dump", 37, { 0x00449500u, 0x00449510u, 0x00449520u, 0x00000000u, 0x0045b350u, 0x004495a0u, 0x0045b350u, 0x0045b350u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u } },   // 0x005f3680
    { "highway", 38, { 0x0044f970u, 0x0044eeb0u, 0x0044e470u, 0x00000000u, 0x00000000u, 0x0044e590u, 0x0044ee80u, 0x0044eea0u, 0x00000000u, 0x00000000u, 0x00000000u, 0x0044f7d0u, 0x00000000u } },   // 0x005f36c8
    { "neustein", 39, { 0x0044ac70u, 0x0044aec0u, 0x0044af10u, 0x00000000u, 0x0045b350u, 0x0044af90u, 0x0044aea0u, 0x0044aea0u, 0x00000000u, 0x0044aeb0u, 0x0044af70u, 0x00000000u, 0x00000000u } },   // 0x005f3710
    { "city", 26, { 0x0044b3c0u, 0x0044b600u, 0x0044b8f0u, 0x00000000u, 0x00000000u, 0x0044b9e0u, 0x0044b3a0u, 0x0044b380u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u } },   // 0x005f3758
};
const std::uint32_t kHookCount = 13u;   // 0x005f37a0

constexpr std::uintptr_t kRecsAddr  = 0x005f2570u;
constexpr std::uintptr_t kPtrsAddr  = 0x005f2728u;
constexpr std::uintptr_t kHookAddr  = 0x005f33f8u;
constexpr std::uintptr_t kCountAddr = 0x005f37a0u;
constexpr std::uintptr_t kSelAddr   = 0x0063d7e4u;   // written only by 0x0041e870

std::uint32_t Fnv1a(std::uintptr_t a, std::size_t n) {
    std::uint32_t h = 2166136261u;
    for (std::size_t i = 0; i < n; ++i) {
        h ^= *reinterpret_cast<const unsigned char*>(a + i);
        h *= 16777619u;
    }
    return h;
}

std::int32_t I32(std::uintptr_t a) { return *reinterpret_cast<const std::int32_t*>(a); }

} // namespace

void CourseTableState_OnCourseLoad(int course_id) {
    static const bool s_on = (std::getenv("MASHED_PORTSTATE_COURSE") != nullptr);
    if (!s_on) return;

    std::memcpy(reinterpret_cast<void*>(kRecsAddr), kCourseRecs, sizeof(kCourseRecs));
    std::memcpy(reinterpret_cast<void*>(kPtrsAddr), kCoursePtrs, sizeof(kCoursePtrs));
    std::memcpy(reinterpret_cast<void*>(kHookAddr), kHookRecs, sizeof(kHookRecs));
    *reinterpret_cast<std::uint32_t*>(kCountAddr) = kHookCount;

    TrackNodeRecordScan(course_id);   // original call site 0x00426eaf (in 0x00426e10)

    // Witness only: what the blocked T04 / T05 / T13 rows would read now (PREREG re-check table).
    const std::uintptr_t sel = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(I32(kSelAddr)));
    const std::uintptr_t tab = static_cast<std::uintptr_t>(static_cast<std::uint32_t>(I32(0x005f2770u)));
    const std::int32_t e9fc = I32(0x0067e9fcu);
    const std::int32_t v = (e9fc == 10) ? I32(0x0067ea94u) : I32(0x0067f17cu);   // 0x0046b56c..0x0046b585
    const std::int32_t ba78 = I32(0x0063ba78u);
    std::int32_t ce80 = -1;
    if (tab && v >= 0 && v < 13) ce80 = I32(static_cast<std::uint32_t>(I32(tab + 4u * v)) + 4u);
    std::uint32_t rec78 = 0; std::int32_t rec78d0 = -1;
    if (tab && ba78 >= 0 && ba78 < 13) {
        rec78 = static_cast<std::uint32_t>(I32(tab + 4u * ba78));
        if (rec78) rec78d0 = I32(rec78);
    }
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[portstate-course] course=%d fnv recs=%08x ptrs=%08x hook=%08x count=%08x "
                        "sel=%08x name=%s slot14=%08x slot44=%08x tab=%08x e9fc=%d v=%d ce80=%d ba78=%d rec78=%08x "
                        "rec78d0=%d\n",
                     course_id, Fnv1a(kRecsAddr, sizeof(kCourseRecs)), Fnv1a(kPtrsAddr, sizeof(kCoursePtrs)),
                     Fnv1a(kHookAddr, sizeof(kHookRecs)), Fnv1a(kCountAddr, 4),
                     static_cast<unsigned>(sel), sel ? reinterpret_cast<const char*>(sel) : "-",
                     sel ? static_cast<unsigned>(I32(sel + 0x14u)) : 0u,
                     sel ? static_cast<unsigned>(I32(sel + 0x44u)) : 0u,
                     static_cast<unsigned>(tab), e9fc, v, ce80, ba78, rec78, rec78d0);
        std::fclose(f);
    }
}

} // namespace Track
} // namespace mashed_re

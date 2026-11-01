// Mashed RE — Vehicle replay/race-state cluster (wave1-s1 c3-sweep).
// One C2->C3 promotion from re/analysis/promote_c2_vehicle_lowrva/0x00432080.md.
//
// Session notes:
//   0x00411350 Replay::TimeFormat     — DEFERRED: implicit-ST0 FPU input; refused
//                                        in Replay.cpp (c3-batch-j-s3).
//   0x00411530 Replay::GetTimeAtIdx   — DEFERRED: 5-arg + FPU passthrough; refused
//                                        in Replay.cpp (c3-batch-j-s3).
//   0x00411d60 Replay::CheckTimer     — ALREADY AUTHORED in SmallLeaves_q4.cpp
//                                        (c3-batch-q-s4). No duplicate needed.
//   0x004248b0 PerCarSnapshot::Init   — ALREADY AUTHORED in SmallLeaves_q4.cpp
//                                        (c3-batch-q-s4). No duplicate needed.
//   0x004299d0 TimeRecord::WriteTrackBest — DEFERRED: no analysis note (
//                                        re/analysis/leaderboard/0x004299d0.md
//                                        does not exist).
//
// Authored this session:
//   0x00432080  RaceEnd::CheckFinish  — race-end dispatch (cases 1/2/0x1000).
//
// Stubs forwarded to original RVAs (passthrough pattern, already resolved):
//   S-0493: FUN_0040e470 — alive predicate (alive check per car index, case 2)
//   S-1041: FUN_0042d3a0 — RaceTransitionBufZero (already authored in SmallLeaves_o5.cpp;
//           forwarded here as passthrough to keep hook independence)
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include <cstdint>

namespace {

// ---------------------------------------------------------------------------
// Globals cited by RaceEnd::CheckFinish (0x00432080)
// Source: re/analysis/promote_c2_vehicle_lowrva/0x00432080.md
// ---------------------------------------------------------------------------
// DAT_0067eca4 — race-finalised flag (0x00432080 +0x0f: cmp against 0)
// DAT_0067f19c — return-value scratch zeroed at entry, set to 2 on finalise
// DAT_007f1a0c — mode dispatch value (1 / 2 / 0x1000)
// DAT_007f1a14 — single-target car index (cases 1,2)
// DAT_0067ecac — receives param_1 on finalise path
// DAT_0067e844, DAT_0067e9f8, DAT_0067e914 — zeroed on finalise path
//
// Two stride-0x4c byte arrays:
//   A array base: 0x007f1041 (entry at idx*0x4c must be != 0)
//   B array base: 0x007f1501 (entry at idx*0x4c must be == 0)
//
// Case 2 loop sentinel: 0x7f1a54 (= DAT_007f1a14 + 4 cars × 0x10)
//
// Case 0x1000: 8 hard-coded absolute (A, B) byte pairs (verbatim from decomp).

constexpr std::uintptr_t kDat_0067eca4 = 0x0067eca4u;
constexpr std::uintptr_t kDat_0067f19c = 0x0067f19cu;
constexpr std::uintptr_t kDat_007f1a0c = 0x007f1a0cu;
constexpr std::uintptr_t kDat_007f1a14 = 0x007f1a14u;
constexpr std::uintptr_t kDat_0067ecac = 0x0067ecacu;
constexpr std::uintptr_t kDat_0067e844 = 0x0067e844u;
constexpr std::uintptr_t kDat_0067e9f8 = 0x0067e9f8u;
constexpr std::uintptr_t kDat_0067e914 = 0x0067e914u;

constexpr std::uint32_t  kStride_4c    = 0x4cu;
constexpr std::uintptr_t kABase        = 0x007f1041u;
constexpr std::uintptr_t kBBase        = 0x007f1501u;
constexpr std::uintptr_t kCase2Sent    = 0x7f1a54u;
constexpr std::uintptr_t kSlotStride_10 = 0x10u;  // 0x004321ad add esi,0x10

// Case 0x1000: 8 absolute row addresses (sourced verbatim from decomp)
static const std::uintptr_t kC1000_A[8] = {
    0x007f1041u, 0x007f108du, 0x007f10d9u, 0x007f1125u,
    0x007f1171u, 0x007f11bdu, 0x007f1209u, 0x007f1255u
};
static const std::uintptr_t kC1000_B[8] = {
    0x007f1501u, 0x007f154du, 0x007f1599u, 0x007f15e5u,
    0x007f1631u, 0x007f167du, 0x007f16c9u, 0x007f1715u
};

// FUN_0040e470 — alive predicate (S-0493; forwarded to original RVA)
constexpr std::uintptr_t kFn_FUN_0040e470 = 0x0040e470u;
// FUN_0042d3a0 — bulk-zero 832 bytes (S-1041; also in SmallLeaves_o5.cpp but
//               referenced here as an independent passthrough)
constexpr std::uintptr_t kFn_FUN_0042d3a0 = 0x0042d3a0u;

using Fn_int_int_t   = int  (__cdecl*)(int);
using Fn_void_void_t = void (__cdecl*)();

template <typename T>
inline T as_fn(std::uintptr_t rva) { return reinterpret_cast<T>(rva); }

inline std::uint32_t  rd32(std::uintptr_t a) { return *reinterpret_cast<const std::uint32_t*>(a); }
inline std::uint32_t& rw32(std::uintptr_t a) { return *reinterpret_cast<std::uint32_t*>(a); }
inline std::uint8_t   rd8 (std::uintptr_t a) { return *reinterpret_cast<const std::uint8_t*>(a); }

} // namespace

// MATCH-S07: the body of 0x00432080 (RaceEndCheckFinish) moved to Match/MatchS07_00432080.cpp,
// which compiles the byte-matched source re/match/s07/00432080.c.

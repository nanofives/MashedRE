// Mashed RE — Vehicle small-leaf C2->C3 promotions (c3-batch-q-s4).
// Session: vehicle_lowrva_continuation (second vehicle session).
//
// Promoted (2 of 5 candidates):
//   0x004248b0  PerCarSnapshotInit  — 4-car × 0x138 stride snapshot copy + lap+1
//   0x00411d60  ReplayCheckTimer    — conditional tick-zero guard in Time Trial mode
//
// Deferred (3 candidates):
//   0x00432080  FUN_00432080   — 416 bytes (>200B STOP-AND-ASK; bigger-function session)
//   0x00411350  Replay::TimeFormat    — REFUSED c3-batch-j-s3: implicit-ST0 FPU input;
//                                       no fpu_st0_input arg_type in harness.
//   0x00411530  Replay::GetTimeAtIdx  — REFUSED c3-batch-j-s3: 5-arg (int int ptr ptr ptr)
//                                       + FPU pass-through; no three_out_ptr arg_type.
//
// Binary anchor: MASHED.exe size=2,846,720
//   sha256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

namespace {

// ── Callee helpers ────────────────────────────────────────────────────────────
//   GetRaceSubMode (0x0042f6a0, C3): returns DAT_0067e9fc (race sub-mode).
constexpr std::uintptr_t kFn_GetRaceSubMode = 0x0042f6a0u;
using Fn_int_void_t = int (__cdecl*)();

inline int GetRaceSubMode_() {
    return reinterpret_cast<Fn_int_void_t>(kFn_GetRaceSubMode)();
}

// ── Global addresses ──────────────────────────────────────────────────────────
//
//   0x008994c0  — live car-block base (src, 4×0x138 = 0x4E0 bytes)
//     + 0x00   DAT_008994c0  (lap counter source for car 0)
//     + 0x80   DAT_00899540  (float field 0 source)
//     + 0x84   DAT_00899544
//     + 0x88   DAT_00899548
//     + 0xd0   DAT_00899590
//     + 0xd4   DAT_008994c0+0xd4  = DAT_00899594
//     + 0xd8   DAT_00899598
//     + 0xdc   DAT_0089959c
//   Decompiler offsets from named globals:
//     DAT_00899560 → 0x008994c0 + 0x80  (DAT_00899560 = base+0x80)
//     DAT_00899564 → base + 0x84
//     DAT_00899568 → base + 0x88
//     DAT_008995b0 → base + 0xf0  [note: 0x008995b0 - 0x008994c0 = 0xf0? actually = 0xd0+0x40 -- see below]
//
//   Ghidra decompiler (verbatim, iVar1 = 0..3×0x138):
//     *(&DAT_00899a94 + iVar1) = *(&DAT_008995b4 + iVar1);   // field +0xd4
//     *(&DAT_00899a40 + iVar1) = *(&DAT_00899560 + iVar1);   // field +0x80
//     *(&DAT_00899a98 + iVar1) = *(&DAT_008995b8 + iVar1);   // field +0xd8
//     *(&DAT_00899a44 + iVar1) = *(&DAT_00899564 + iVar1);   // field +0x84
//     *(&DAT_00899a90 + iVar1) = *(&DAT_008995b0 + iVar1);   // field +0xd0
//     *(&DAT_00899a48 + iVar1) = *(&DAT_00899568 + iVar1);   // field +0x88
//     *(&DAT_00899a9c + iVar1) = *(&DAT_008995bc + iVar1);   // field +0xdc
//     *(&DAT_008999a0 + iVar1) = *(&DAT_008994c0 + iVar1) + 1;  // lap+1 at +0x00
//
//   Snapshot block (dst) at 0x00899900 (= 0x008994c0 + 0x4E0):
//     DAT_008999a0  = 0x008994c0 + 0x4E0 + 0x00  → lap counter dst for car 0
//     DAT_00899a40  = DAT_008999a0 + 0x1A0  (= 0x008994c0 + 0x4E0 + 0x1A0 + 0x80)
//                   Actually: 0x00899a40 - 0x008994c0 = 0x5E0; per-car offset 0x80 from snapshot-start
//     ... (all 7 dst offsets are src_base shifted by a fixed amount)
//
//   Computed strides (from Ghidra address arithmetic):
//     kLiveBase  = 0x008994c0  (live car array, 4×0x138)
//     kSnapBase  = 0x008994c0 + 0x4E0  = 0x008999a0  (snapshot array, same layout)
//
//     At iVar1=0 (car 0):
//       LAP dst: 0x008999a0   LAP src: 0x008994c0   (offset 0x00 in car struct)
//       F0  dst: 0x00899a40   F0  src: 0x00899560   offset 0x80
//       F1  dst: 0x00899a44   F1  src: 0x00899564   offset 0x84
//       F2  dst: 0x00899a48   F2  src: 0x00899568   offset 0x88
//       F3  dst: 0x00899a90   F3  src: 0x008995b0   offset 0xd0
//       F4  dst: 0x00899a94   F4  src: 0x008995b4   offset 0xd4
//       F5  dst: 0x00899a98   F5  src: 0x008995b8   offset 0xd8
//       F6  dst: 0x00899a9c   F6  src: 0x008995bc   offset 0xdc
//
//     Verifying offsets relative to live base and snap base:
//       src 0x00899560 - 0x008994c0 = 0xA0  — [UNCERTAIN vs note saying 0x80: note says +0x80]
//         CORRECTION: the note says "iVar1 offset +0x80" from snapshot-block-base at 0x008999a0
//         That is confusing; the decompiler names are the ground truth:
//         DAT_00899560 is the literal Ghidra global name → src[0x80] for car 0
//         0x00899560 - 0x008994c0 = 0x008994c0 + X → X = 0x00899560 - 0x008994c0 = 0xA0  [not 0x80]
//         Wait: 0x00899560 - 0x008994c0 = 0x4A0? Let me recompute:
//           0x00899560 - 0x008994c0 = 0x00899560 - 0x008994c0 = 0x6A0? No:
//           0x008994c0 + 0xA0 = 0x00899560  ✓ (0x8994c0 + 0xa0 = 0x899560)
//         So field offset 0x80 in the NOTE is inconsistent; Ghidra DAT names are authoritative.
//         We use the literal Ghidra DAT addresses as-is (no offset arithmetic needed):
//         src_addr = &DAT_00899560 + iVar1 (same as &(0x00899560) + iVar1).
//
//   Final implementation:
//     8 copy operations per iteration (7 dword copies + 1 lap+1 write)
//     4 iterations: iVar1 = 0, 0x138, 0x270, 0x3a8

//   Live block sources (using Ghidra DAT addresses directly):
constexpr std::uintptr_t kSrcLap = 0x008994c0u;  // +0x00 lap counter src (car 0)
constexpr std::uintptr_t kSrcF0  = 0x00899560u;  // +iVar1
constexpr std::uintptr_t kSrcF1  = 0x00899564u;
constexpr std::uintptr_t kSrcF2  = 0x00899568u;
constexpr std::uintptr_t kSrcF3  = 0x008995b0u;
constexpr std::uintptr_t kSrcF4  = 0x008995b4u;
constexpr std::uintptr_t kSrcF5  = 0x008995b8u;
constexpr std::uintptr_t kSrcF6  = 0x008995bcu;

//   Snapshot block destinations:
constexpr std::uintptr_t kDstLap = 0x008999a0u;  // +0x00 lap dst (car 0)
constexpr std::uintptr_t kDstF0  = 0x00899a40u;
constexpr std::uintptr_t kDstF1  = 0x00899a44u;
constexpr std::uintptr_t kDstF2  = 0x00899a48u;
constexpr std::uintptr_t kDstF3  = 0x00899a90u;
constexpr std::uintptr_t kDstF4  = 0x00899a94u;
constexpr std::uintptr_t kDstF5  = 0x00899a98u;
constexpr std::uintptr_t kDstF6  = 0x00899a9cu;

//   Per-car stride and loop count:
constexpr std::uint32_t kCarStride  = 0x138u;
constexpr std::uint32_t kLoopTotal  = 0x4e0u;   // 4 × 0x138 = loop bound

//   Replay CheckTimer globals:
constexpr std::uintptr_t kDat_0063bb14 = 0x0063bb14u;  // current-lap replay-obj ptr
constexpr std::uintptr_t kDat_007f0ff4 = 0x007f0ff4u;  // global tick counter
constexpr std::ptrdiff_t kOffset_0x194  = 0x194;         // first-frame-written flag

inline std::uint32_t  rd32(std::uintptr_t a) { return *reinterpret_cast<const std::uint32_t*>(a); }
inline std::uint32_t& rw32(std::uintptr_t a) { return *reinterpret_cast<std::uint32_t*>(a);       }

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// 0x004248b0  PerCarSnapshotInit
// Body (0x004248b0..0x0042491f): 111 bytes, pure leaf (callees_depth1: []).
//
// Copies 7 dword fields from the live car block (0x008994c0 region) into the
// snapshot block (0x008999a0 region) for 4 cars (stride 0x138), and increments
// the lap counter (field +0x00) by 1 in the snapshot.
//
// Ghidra decomp (iVar1 = 0; loop stride 0x138; bound 0x4E0):
//   *(&DAT_00899a94 + iVar1) = *(&DAT_008995b4 + iVar1);
//   *(&DAT_00899a40 + iVar1) = *(&DAT_00899560 + iVar1);
//   *(&DAT_00899a98 + iVar1) = *(&DAT_008995b8 + iVar1);
//   *(&DAT_00899a44 + iVar1) = *(&DAT_00899564 + iVar1);
//   *(&DAT_00899a90 + iVar1) = *(&DAT_008995b0 + iVar1);
//   *(&DAT_00899a48 + iVar1) = *(&DAT_00899568 + iVar1);
//   *(&DAT_00899a9c + iVar1) = *(&DAT_008995bc + iVar1);
//   *(&DAT_008999a0 + iVar1) = *(&DAT_008994c0 + iVar1) + 1;
//   iVar1 += 0x138; while (iVar1 < 0x4e0);
//
// Caller: FUN_004331a0 (race-end init, C2). Pure leaf — callee-gate exempt.
// U-1510: meanings of 7 dwords (non-blocking). U-1511: D-3117 carried.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl PerCarSnapshotInit(void) {
    for (std::uint32_t off = 0u; off < kLoopTotal; off += kCarStride) {
        rw32(kDstF4 + off) = rd32(kSrcF4 + off);   // field +0xd4
        rw32(kDstF0 + off) = rd32(kSrcF0 + off);   // field +0x80 (per DAT_00899560)
        rw32(kDstF5 + off) = rd32(kSrcF5 + off);   // field +0xd8
        rw32(kDstF1 + off) = rd32(kSrcF1 + off);   // field +0x84
        rw32(kDstF3 + off) = rd32(kSrcF3 + off);   // field +0xd0
        rw32(kDstF2 + off) = rd32(kSrcF2 + off);   // field +0x88
        rw32(kDstF6 + off) = rd32(kSrcF6 + off);   // field +0xdc
        rw32(kDstLap + off) = rd32(kSrcLap + off) + 1u; // lap+1
    }
}
RH_ScopedInstall(PerCarSnapshotInit, 0x004248b0);  // re-enabled 2026-05-24 c3-vehicle

// ─────────────────────────────────────────────────────────────────────────────
// 0x00411d60  ReplayCheckTimer
// Body (0x00411d60..0x00411d87): 39 bytes, no callees except GetRaceSubMode (C3).
//
// Ghidra decomp:
//   if (FUN_0042f6a0() == 2 &&
//       DAT_0063bb14 != 0 &&
//       *(int *)(DAT_0063bb14 + 0x194) == 0) {
//       DAT_007f0ff4 = 0;
//   }
//
// Purpose: In Time Trial mode (mode==2), if the current-lap replay buffer exists
// and its first-frame-written flag (+0x194) is not yet set, zero the global tick
// counter. Effect: holds tick=0 until the first replay frame is written.
//
// At quiescent main menu: mode != 2 → first gate fails → no write.
// Callee: GetRaceSubMode (0x0042f6a0, C3).  Not a live-state mutator at menu.
// Anti-island: caller chain through replay-record path (Replay.cpp, C3).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl ReplayCheckTimer(void) {
    if (GetRaceSubMode_() != 2) return;
    const std::uint32_t cur_ptr = rd32(kDat_0063bb14);
    if (cur_ptr == 0u) return;
    if (*reinterpret_cast<const std::int32_t*>(cur_ptr + kOffset_0x194) != 0) return;
    rw32(kDat_007f0ff4) = 0u;
}
RH_ScopedInstall(ReplayCheckTimer, 0x00411d60);  // re-enabled 2026-05-24 c3-vehicle

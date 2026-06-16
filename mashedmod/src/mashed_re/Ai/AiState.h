// Mashed RE — AI driver cluster: shared state-record + global-address map.
//
// Verbatim-port support header for the FUN_00418860 opponent-AI cluster
// (WS-C2). RVA-cited from re/analysis/ai_controller.md (C1) and this session's
// pool5 decompilations (2026-06-16). NO-GUESSING: every address below is cited
// to the function whose listing references it.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// These are ORIGINAL-image absolute addresses. In the dev .asi they resolve to
// live game memory (hooks installed at the RVAs below). In the standalone exe
// the image-pad owns the RVA range, but the .bss tuning constants (DAT_005ccXXX)
// are zeroed there and must be harvested (ai_controller.md §9 step 8 / WS-A7).
#pragma once

#include <cstdint>

namespace Ai {

// ---------------------------------------------------------------------------
// Control-output / cooked-input block.  Base 0x007f1038, stride 0x4c B
// (= 0x13 DWORDs), indexed by output slot.  Byte→axis map RESOLVED 2026-06-16
// (re/analysis/ai_ctrl_byte_map_RESOLVED_2026-06-16.md):
//   [0],[1] = steer cmd pair   (FUN_00416250 from FUN_00415e20 angle error)
//   [3]     = fire / powerup    (FUN_00415220)
//   [4]     = accel  0/0x40/0xff (consumer FUN_00467650 *(blk+4))
//   [5]     = brake  0/0x40/0xff (consumer FUN_00467650 *(blk+5), neg force)
//   [6],[7] = zeroed scratch
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kCtrlBlockBase   = 0x007f1038u;  // FUN_00418560 0x00418575
static constexpr std::uint32_t  kCtrlBlockStride = 0x4cu;        // FUN_00418560 0x00418572
// Output-slot id table: slot = *(int*)(0x007f1a14 + v*0x10).
static constexpr std::uintptr_t kSlotTableBase   = 0x007f1a14u;  // FUN_00418560 0x0041856c
static constexpr std::uint32_t  kSlotTableStride = 0x10u;

// ---------------------------------------------------------------------------
// Per-vehicle behavior/path record.  Base 0x0089a4cc, stride 0x74 B
// (= 0x1d DWORDs); the decomp indexes it as (&DAT_0089a4cc)[v*0x1d].
// Field offsets (from ai_controller.md §3a, cited there):
//   +0x00 (0x0089a4cc) AI line TYPE   0=race 1=inside 2=slow 3=cheat
//   +0x04 (0x0089a4d0) spline INDEX   0..2 within type
//   +0x1c (0x0089a4e4) frustration timer
//   +0x2c (0x0089a4f8) mode-5 startup countdown  (FUN_00418560)
//   +0x30 (0x0089a4fc) input-override countdown   (FUN_00418560 replay)
//   +0x40..0x4c (0x0089a50c/510/514/518) stored ctrl replay bytes
//   +0x60 (0x0089a52c) behavior MODE  (FUN_00416250 commit)
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kAiStateBase   = 0x0089a4ccu;
static constexpr std::uint32_t  kAiStateDwords = 0x1du;   // == 0x74 bytes

static constexpr std::uintptr_t kAiLineType        = 0x0089a4ccu; // +0x00
static constexpr std::uintptr_t kAiSplineIndex     = 0x0089a4d0u; // +0x04
static constexpr std::uintptr_t kAiMode5Countdown  = 0x0089a4f8u; // +0x2c
static constexpr std::uintptr_t kAiOverrideTimer   = 0x0089a4fcu; // +0x30
static constexpr std::uintptr_t kAiReplayB0        = 0x0089a50cu; // +0x40 (steer[0])
static constexpr std::uintptr_t kAiReplayB1        = 0x0089a510u; // +0x44 (steer[1])
static constexpr std::uintptr_t kAiReplayB4        = 0x0089a514u; // +0x48 (accel[4])
static constexpr std::uintptr_t kAiReplayB5        = 0x0089a518u; // +0x4c (brake[5])

// ---------------------------------------------------------------------------
// AI-line spline arrays.  Each line-type: <=3 splines, stride 0x204 B,
// count field at +0x200.  (FUN_00418560 / ai_controller.md §4.)
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kSplineRace   = 0x00801aa0u; // type 0
static constexpr std::uintptr_t kSplineRaceCnt= 0x00801ca0u; // type 0 count (+0x200)
static constexpr std::uintptr_t kSplineInside = 0x008020acu; // type 1
static constexpr std::uintptr_t kSplineInsideCnt = 0x008022acu;
static constexpr std::uintptr_t kSplineSlow   = 0x008026b8u; // type 2
static constexpr std::uintptr_t kSplineSlowCnt= 0x008028b8u;
static constexpr std::uintptr_t kSplineCheat  = 0x00802cc4u; // type 3
static constexpr std::uintptr_t kSplineCheatCnt = 0x00802ec4u;
static constexpr std::uint32_t  kSplineStride = 0x204u;

// Globals referenced by the tick loop / step (cited at use sites).
static constexpr std::uintptr_t kGameModeFd0   = 0x007f0fd0u; // FUN_00418560 dispatch selector
static constexpr std::uintptr_t kDbgSplineEn   = 0x007f1a50u; // FUN_00418560 debug override enable
static constexpr std::uintptr_t kDbgSplineType = 0x007f1a68u; // FUN_00418560 debug line type
static constexpr std::uintptr_t kDbgSplineIdx  = 0x007f1a6cu; // FUN_00418560 debug spline index
static constexpr std::uintptr_t kFrame0ff8     = 0x007f0ff8u; // FUN_00418560 mode-5 zero
static constexpr std::uintptr_t kOverrideStep  = 0x007f1008u; // FUN_00418560 override decrement

// 4th arg to the control step: float 100.0 == bits 0x42c80000 (FUN_00418560).
static constexpr std::uint32_t  kCtrlStepArg4  = 0x42c80000u;

// ---- small typed memory accessors (faithful to the decomp's raw casts) -----
inline std::uint8_t&  U8 (std::uintptr_t a) { return *reinterpret_cast<std::uint8_t*>(a); }
inline std::int32_t&  I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }
inline std::uint32_t& U32(std::uintptr_t a) { return *reinterpret_cast<std::uint32_t*>(a); }
inline float&         F32(std::uintptr_t a) { return *reinterpret_cast<float*>(a); }

} // namespace Ai

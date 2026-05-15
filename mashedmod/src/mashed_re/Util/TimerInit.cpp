// Mashed RE — timer/slot initialisation cluster (timer_d3_cont1_b).
// Four pure-leaf or thunk functions called from FUN_004111c0 (large init fn).
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x00422b10  TimerArrayZero
// Pure leaf; zeroes 0x138 (312) consecutive dwords = 0x4e0 (1248) bytes
// at 0x008994c0 via counted pointer loop (iVar1 from 0x138 down to 0).
// No branches (excluding loop), no external calls.
// Caller: FUN_004111c0 (0x004111c0).
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kTimerArrayBase  = 0x008994c0;
static constexpr std::size_t    kTimerArrayBytes = 0x138u * sizeof(std::uint32_t); // 0x4e0

extern "C" __declspec(dllexport) void __cdecl TimerArrayZero() {
    std::memset(reinterpret_cast<void*>(kTimerArrayBase), 0, kTimerArrayBytes);
}

RH_ScopedInstall(TimerArrayZero, 0x00422b10);

// ---------------------------------------------------------------------------
// 0x00425b10  PlayerSlotZero
// Pure leaf; writes 0 to exactly 8 globals at a uniform stride of 0x4c (76)
// bytes, starting at 0x008992a0.
// Addresses in decomp order:
//   0x008992a0, 0x008992ec, 0x00899338, 0x00899384,
//   0x008993d0, 0x0089941c, 0x00899468, 0x008994b4
// No branches, no calls.
// Caller: FUN_004111c0 (0x004111c0).
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void __cdecl PlayerSlotZero() {
    *reinterpret_cast<std::uint32_t*>(0x008992a0u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008992ecu) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00899338u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00899384u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008993d0u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x0089941cu) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x00899468u) = 0u;
    *reinterpret_cast<std::uint32_t*>(0x008994b4u) = 0u;
}

RH_ScopedInstall(PlayerSlotZero, 0x00425b10);

// ---------------------------------------------------------------------------
// 0x004222c0  TimerInitThunk
// Confirmed Ghidra thunk (thunk=true) of FUN_00422120 (0x00422120).
// 4-byte body: single unconditional branch to 0x00422120.
// Callee 0x00422120 is C2 (promote_c2_util_timer-20260513).
// Caller: FUN_004111c0 (0x004111c0).
// ---------------------------------------------------------------------------

// Call the original FUN_00422120 directly via its fixed address.
// 0x00422120 is C2 but not yet authored as a hook export; we forward
// the thunk to the original binary as the thunk itself does.
typedef void (__cdecl *TimerInitFn)();
static constexpr std::uintptr_t kTimerInitRva = 0x00422120u;

extern "C" __declspec(dllexport) void __cdecl TimerInitThunk() {
    reinterpret_cast<TimerInitFn>(kTimerInitRva)();
}

RH_ScopedInstall(TimerInitThunk, 0x004222c0);

// ---------------------------------------------------------------------------
// 0x0041cbc0  FloatTableInit
// Pure leaf; copies 12 hardcoded float values (as raw dwords) to
// 0x005f337c via counted pointer loop (iVar1 from 0xc down to 0).
// Also writes 0 to DAT_0063d270.
// No branches, no external calls.
// Caller: FUN_004111c0 (0x004111c0).
//
// The 12 values form 4 groups of 3 floats:
//   Group 0: {0x3ef5c28f, 0x3e800000, 0x3f800000}  (~0.480, 0.250, 1.000)
//   Group 1: {0x3ef5c28f, 0x3e19999a, 0x3f800000}  (~0.480, ~0.150, 1.000)
//   Group 2: {0x3ef5c28f, 0x3d4ccccd, 0x3f800000}  (~0.480, ~0.050, 1.000)
//   Group 3: {0x3ef5c28f, 0xbd4ccccd, 0x3f800000}  (~0.480, ~-0.050, 1.000)
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kFloatTableDst = 0x005f337cu;
static constexpr std::uintptr_t kDat0063d270   = 0x0063d270u;

static const std::uint32_t kFloatTable[12] = {
    0x3ef5c28fu, 0x3e800000u, 0x3f800000u,  // group 0
    0x3ef5c28fu, 0x3e19999au, 0x3f800000u,  // group 1
    0x3ef5c28fu, 0x3d4ccccdu, 0x3f800000u,  // group 2
    0x3ef5c28fu, 0xbd4ccccdu, 0x3f800000u,  // group 3
};

extern "C" __declspec(dllexport) void __cdecl FloatTableInit() {
    std::memcpy(reinterpret_cast<void*>(kFloatTableDst),
                kFloatTable,
                sizeof(kFloatTable));
    *reinterpret_cast<std::uint32_t*>(kDat0063d270) = 0u;
}

RH_ScopedInstall(FloatTableInit, 0x0041cbc0);

// Mashed RE — Util subsystem Wave 3 mixed-candidate cluster.
// Session: wave3-s4   Branch: c3-sweep/wave3-s4
//
// One util function authored to C2 quality:
//   0x004299d0  TimeRecord_WriteTrackBest  — write per-track best-lap time triple
//
// Analysis note: re/analysis/leaderboard/0x004299d0.md
//
// Binary anchor:
//   original\MASHED.exe  size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee: 0x00430790 — get current track index (DAT_0067f17c getter).
// Cited 3× in TimeRecord_WriteTrackBest at 0x004299e8, 0x004299f9, 0x00429a0a.
// ---------------------------------------------------------------------------
typedef int (__cdecl* FnGetTrackIdx)();
static inline int CallGetTrackIdx() {
    return reinterpret_cast<FnGetTrackIdx>(0x00430790u)();
}

// ===========================================================================
// 0x004299d0  TimeRecord_WriteTrackBest
//
// Writes a decoded time triple (fractional-seconds, seconds, minutes) into
// three parallel per-track arrays, indexed by the current track index.
//
// param_1 = fractional seconds (float, passed as uint32_t raw bits)
// param_2 = integer seconds (0..59)
// param_3 = integer minutes
//
// Array bases (cited in leaderboard/0x004299d0.md):
//   minutes:   DAT_007f0db4  (index = int32, stride 4)
//   seconds:   DAT_007f0de8  (index = int32, stride 4)
//   frac secs: DAT_007f0e1c  (index = float/uint32, stride 4)
//
// FUN_00430790 is called 3× (once per array write) to get the live track idx.
//
// Intermediate globals written before the array writes:
//   _DAT_0067d9a0 = param_1  (fractional seconds)
//    DAT_0067d998 = param_2  (seconds)
//    DAT_0067d990 = param_3  (minutes)
//
// Body 83 bytes (0x004299d0..0x00429a23).
// Callers: Replay::CreateOrLoad (0x00411d90) when prior best-time exists.
// Subsystem: util (see hooks.csv row)
//
// [UNCERTAIN U-2227]: intermediary globals 0x0067d9a0/d998/d990 not read back
// within this function; purpose of side-write unclear (possible HUD display).
// ===========================================================================

// 0x004299d0
extern "C" __declspec(dllexport)
void __cdecl TimeRecord_WriteTrackBest(std::uint32_t param_1,
                                       std::uint32_t param_2,
                                       std::uint32_t param_3)
{
    // Write parameters to intermediate globals (possible HUD-display side-effect)
    *reinterpret_cast<std::uint32_t*>(0x0067d9a0u) = param_1; // @0x004299d2 _DAT_0067d9a0 = frac secs
    *reinterpret_cast<std::uint32_t*>(0x0067d998u) = param_2; // @0x004299d7  DAT_0067d998 = seconds
    *reinterpret_cast<std::uint32_t*>(0x0067d990u) = param_3; // @0x004299dc  DAT_0067d990 = minutes

    // Write minutes into per-track array (call 1 @ 0x004299e8)
    {
        int idx = CallGetTrackIdx();
        reinterpret_cast<std::uint32_t*>(0x007f0db4u)[idx] = param_3; // @0x004299f0 minutes
    }
    // Write seconds into per-track array (call 2 @ 0x004299f9)
    {
        int idx = CallGetTrackIdx();
        reinterpret_cast<std::uint32_t*>(0x007f0de8u)[idx] = param_2; // @0x00429a01 seconds
    }
    // Write fractional seconds into per-track array (call 3 @ 0x00429a0a)
    {
        int idx = CallGetTrackIdx();
        reinterpret_cast<std::uint32_t*>(0x007f0e1cu)[idx] = param_1; // @0x00429a12 frac secs
    }
}

RH_ScopedInstall(TimeRecord_WriteTrackBest, 0x004299d0);

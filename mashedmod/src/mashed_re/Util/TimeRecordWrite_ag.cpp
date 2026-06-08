// Mashed RE - c3_batch_ag harness-ext: TimeRecord::WriteTrackBest.
// Calls FUN_00430790 (GetDat0067f17c, C3) to resolve the current track index, then
// writes a decoded (min,sec,frac) time triple into three parallel per-track arrays.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x004299d0  FUN_004299d0 (83 bytes)  void(u32 frac, u32 sec, u32 min)
// Decomp (pool13 2026-06-08, matches re/analysis/leaderboard/0x004299d0.md):
//   _DAT_0067d9a0 = frac;  DAT_0067d998 = sec;  DAT_0067d990 = min;
//   i = FUN_00430790();  (&DAT_007f0db4)[i] = DAT_0067d990;   // minutes
//   i = FUN_00430790();  (&DAT_007f0de8)[i] = DAT_0067d998;   // seconds
//   i = FUN_00430790();  (&DAT_007f0e1c)[i] = _DAT_0067d9a0;  // fractional secs
// Arrays are int32-indexed (base + i*4). The three writes re-read the globals just
// set, and re-fetch the index from FUN_00430790 each time (transcribed verbatim).
extern "C" __declspec(dllexport) void __cdecl TimeRecordWriteTrackBest(
        uint32_t frac, uint32_t sec, uint32_t min) {
    typedef int (__cdecl *GetIdxFn)();
    auto* const getIdx = reinterpret_cast<GetIdxFn>(0x00430790u);

    *reinterpret_cast<uint32_t*>(0x0067d9a0u) = frac;
    *reinterpret_cast<uint32_t*>(0x0067d998u) = sec;
    *reinterpret_cast<uint32_t*>(0x0067d990u) = min;

    int i = getIdx();
    reinterpret_cast<uint32_t*>(0x007f0db4u)[i] = *reinterpret_cast<uint32_t*>(0x0067d990u);
    i = getIdx();
    reinterpret_cast<uint32_t*>(0x007f0de8u)[i] = *reinterpret_cast<uint32_t*>(0x0067d998u);
    i = getIdx();
    reinterpret_cast<uint32_t*>(0x007f0e1cu)[i] = *reinterpret_cast<uint32_t*>(0x0067d9a0u);
}

RH_ScopedInstall(TimeRecordWriteTrackBest, 0x004299d0);  // c3_batch_ag harness-ext 2026-06-08

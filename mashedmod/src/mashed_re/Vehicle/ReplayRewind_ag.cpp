// Mashed RE - c3_batch_ag harness-ext: Replay::Rewind leaf.
// Pure single-store leaf, no callees — safe to A/B-diff via Frida.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x00483a30  FUN_00483a30  (10 bytes)
// Replay::Rewind — resets the read/write cursor (+0x1c) to the ring head (+0x18).
// Decomp (pool13 2026-06-08, matches re/analysis/bucket_vehicle_004820e0_00485420/00483a30.md):
//   void FUN_00483a30(int param_1) { *(param_1+0x1c) = *(param_1+0x18); }
// Constants cited: +0x1c (cursor, @0x00483a32), +0x18 (ring head, @0x00483a35).
extern "C" __declspec(dllexport) void __cdecl ReplayRewind(void* p) {
    char* const base = reinterpret_cast<char*>(p);
    *reinterpret_cast<uint32_t*>(base + 0x1c) = *reinterpret_cast<uint32_t*>(base + 0x18);
}

RH_ScopedInstall(ReplayRewind, 0x00483a30);  // c3_batch_ag harness-ext 2026-06-08

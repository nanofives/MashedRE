// Mashed RE - c3_batch_ag s1: inline memset leaf (Lua-VM alloc paths).
// Pure transform, no callees, no global side effects — safe to A/B-diff via Frida.
//
// Source RVA + decomp cited per function below (NO-GUESSING; bit-for-bit port).
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x004b64e0  FUN_004b64e0  (57 bytes, body 0x004b64e0..0x004b6519)
// CRT-style inline memset. Three params: dst (undefined4*), fill byte
// (undefined1, low byte of param_2), count (uint). Returns dst.
//
// Decomp (re/analysis/bucket_input_luajoy_004b64e0_004c06c0/0x004b64e0.md):
//   if (param_3 == 0) return param_1;
//   for (uVar1 = param_3 >> 2; uVar1 != 0; uVar1--)         // word-fill loop
//       *puVar2++ = CONCAT22(CONCAT11(p2,p2),CONCAT11(p2,p2)); // byte broadcast x4
//   for (param_3 &= 3; param_3 != 0; param_3--)             // tail byte-fill loop
//       *(byte*)puVar2++ = param_2;
//   return param_1;
//
// Constants cited: 0x4 (param_3>>2 word stride), 0x3 (param_3&3 tail mask).
extern "C" __declspec(dllexport) void* __cdecl MemsetInline(
        void* dst, unsigned int fillByte, unsigned int count) {
    if (count == 0u) return dst;                          // 0x004b64e0 early-out
    const uint8_t b = static_cast<uint8_t>(fillByte);
    const uint32_t word = static_cast<uint32_t>(b)
                        | (static_cast<uint32_t>(b) << 8)
                        | (static_cast<uint32_t>(b) << 16)
                        | (static_cast<uint32_t>(b) << 24);   // byte broadcast to 4 lanes
    uint32_t* pw = static_cast<uint32_t*>(dst);
    for (uint32_t w = count >> 2; w != 0u; --w) {         // word-fill loop
        *pw++ = word;
    }
    uint8_t* pb = reinterpret_cast<uint8_t*>(pw);
    for (uint32_t t = count & 3u; t != 0u; --t) {         // tail byte-fill loop
        *pb++ = b;
    }
    return dst;
}

RH_ScopedInstall(MemsetInline, 0x004b64e0);  // c3_batch_ag s1 2026-06-08

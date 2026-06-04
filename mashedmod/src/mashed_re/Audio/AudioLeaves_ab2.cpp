// Mashed RE — Audio pure-leaf cluster (c3-batch-ab session 2).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Candidates promoted in this file (2 of the 6 session-2 candidates):
//   0x005b3580  AudioListHeaderInit       — intrusive circular list header init
//   0x005b4060  AudioFmtConvertByteLength — format-conversion output byte length
//
// The other four session-2 candidates were SKIPPED (not harness-expressible
// without a new arg_type — see re/PROMOTION_QUEUE.md):
//   0x005b2fd0  5-arg projector, dereferences nested pointer tables in a ctx
//               struct (*(ctx+0x10/+0x18/+0x20)); no harness seeds nested ptrs.
//   0x005b35a0  push-back; dereferences header[1] as a live sentinel pointer
//               (*(*(hdr+4)+4)=...). Needs an initialised list + an allocated
//               owning object; no harness builds this list layout.
//   0x005b3670  find-by-field; walks the list and the match KEY is a scalar
//               arg2. The buffer-seeding harnesses cannot control a scalar arg2.
//   0x005b36b0  index-by-position; position is a scalar arg2 (same wall as
//               0x005b3670); only the empty-list early-out is reachable.
//
// Source analysis (read end-to-end):
//   re/analysis/bucket_audio_005b2220_005b8570/0x005b3580.md
//   re/analysis/bucket_audio_005b2220_005b8570/0x005b4060.md

#include "../Core/HookSystem.h"
#include <cstdint>
#include <xmmintrin.h>   // _mm_set_ss / _mm_cvtss_si32 — round-to-nearest-even (matches cvtss2si / x87 fistp default)

// Round-to-nearest-even of a float32 → int32, identical to the CPU conversion
// Ghidra renders as ROUND() (default rounding mode). MSVC's (int)roundf() would
// be round-half-AWAY and is NOT bit-identical, so we use the SSE intrinsic.
static inline int RoundNE(float x) { return _mm_cvtss_si32(_mm_set_ss(x)); }

// ─────────────────────────────────────────────────────────────────────────────
// 0x005b3580  FUN_005b3580  AudioListHeaderInit  (18 bytes, leaf)
// Intrusive circular doubly-linked-list header initializer ([[U-6906]]).
//
// param_1 = list header. Layout: [0]=count, [1]=sentinel.next, [2]=sentinel.prev.
//           The embedded sentinel node is the 8 bytes at header+4 (header[1..2]).
//
// Decomp (0x005b3580):
//   puVar1 = param_1 + 1;     // &header[1] — address of the sentinel node
//   *param_1   = 0;           // count = 0
//   param_1[2] = puVar1;      // sentinel.prev = &sentinel  (empty circular list)
//   *puVar1    = puVar1;      // sentinel.next = &sentinel
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioListHeaderInit(std::uint32_t* param_1) {
    std::uint32_t* puVar1 = param_1 + 1;                 // &header[1]
    param_1[0] = 0u;                                     // count = 0
    param_1[2] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(puVar1));  // prev = self
    *puVar1    = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(puVar1));  // next = self
}

RH_ScopedInstall(AudioListHeaderInit, 0x005b3580);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005b4060  FUN_005b4060  AudioFmtConvertByteLength  (225 bytes, leaf)
// Computes the output-buffer byte length for converting a buffer from format
// descriptor param_1 ("in") to format descriptor param_2 ("out"), scaling the
// input data size by the sample-rate, bits-per-sample and channel-count ratios,
// then rounding up to the output sample-frame alignment. Format descriptors
// [[U-6910]] (field roles confirmed). FP arithmetic + ROUND only; no callees.
//
// Field layout (both descriptors):
//   +0x00 u32  sample rate
//   +0x08 i32  input data size (read from param_1 only, = param_1[2])
//   +0x0c u8   bits per sample (low byte of dword at +0x0c)
//   +0x0d u8   channel count
//
// Decomp (0x005b4060), operator precedence preserved (+ binds tighter than &):
//   uVar1 = (uint)(byte)((byte)param_2[3] >> 3);       // output bytes-per-sample
//   return (int)ROUND((float)(
//            (int)ROUND( ((float)*param_2            / (float)*param_1)              *
//                        ((float)(byte)param_2[3]    / (float)*(byte*)(param_1+3))   *
//                        ((float)*(byte*)((int)param_2+0xd) / (float)*(byte*)((int)param_1+0xd)) *
//                        (float)param_1[2] )
//            + -1 + uVar1 & ~(uVar1 - 1) ));
// i.e. inner = ROUND(rate_ratio * bits_ratio * chan_ratio * size_in);
//      aligned = (uint)( (inner - 1) + uVar1 ) & ~(uVar1 - 1);   // round up to bytes/sample
//      return ROUND((float)aligned);
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl AudioFmtConvertByteLength(int* param_1, unsigned int* param_2) {
    // uVar1 = output bytes-per-sample = (out bits-per-sample) >> 3
    const unsigned int uVar1 =
        static_cast<unsigned int>(static_cast<unsigned char>(
            static_cast<unsigned char>(param_2[3]) >> 3));

    const float product =
        ( static_cast<float>(*param_2) / static_cast<float>(*param_1) ) *
        ( static_cast<float>(static_cast<unsigned char>(param_2[3])) /
          static_cast<float>(*reinterpret_cast<const unsigned char*>(param_1 + 3)) ) *
        ( static_cast<float>(*(reinterpret_cast<const unsigned char*>(param_2) + 0xd)) /
          static_cast<float>(*(reinterpret_cast<const unsigned char*>(param_1) + 0xd)) ) *
        static_cast<float>(param_1[2]);

    const int inner = RoundNE(product);

    // (inner - 1 + uVar1) computed in unsigned, then masked down to alignment.
    const unsigned int aligned =
        (static_cast<unsigned int>(inner + -1) + uVar1) & ~(uVar1 - 1u);

    return RoundNE(static_cast<float>(aligned));
}

RH_ScopedInstall(AudioFmtConvertByteLength, 0x005b4060);

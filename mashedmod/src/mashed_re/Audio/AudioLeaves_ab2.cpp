// Mashed RE — Audio pure-leaf cluster (c3-batch-ab session 2).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Candidates promoted in this file (2 of the 6 session-2 candidates):
//   (0x005b3580 AudioListHeaderInit removed 2026-10-16; the body is AudioListInit in
//    Audio/AudioList_ag.cpp)
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

// ─────────────────────────────────────────────────────────────────────────────
// FUN_005b3580 (intrusive circular list header init, [[U-6906]]) was ported here first as
// AudioListHeaderInit; its one-body-per-RVA copy is AudioListInit in Audio/AudioList_ag.cpp,
// the body hooks.csv's evidence cites (diff_audio_list_init.csv, count_header_list_ring).
// The duplicate that sat here was removed 2026-10-16 (dupinstall).
// ─────────────────────────────────────────────────────────────────────────────

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
//
// audiolink 2026-10-23, from the bytes (verify/audiolink_20261023/disasm_slice1.txt). The Ghidra ROUND() above
// is wrong twice:
//   - Both conversions TRUNCATE: fnstcw / or ah,0xc / fldcw around each fistp (0x005b40e7, 0x005b4125). The
//     first one matters: in {rate 3, size 1636, 16-bit mono} -> out {rate 2, 16-bit mono} gives 1090.67, so
//     the original returns 1090 and the old round-to-nearest port 1092.
//   - The product order is ((chOut/chIn) * (bitsOut/bitsIn)) * (rateOut/rateIn) * sizeIn (0x005b4087..0x005b40d2),
//     every operand an exact integer load (fild / fidiv / fimul; rateOut by fild qword, zero-extended).
// Each step is done in double from exact integer operands and rounded to float, which matches the x87 at the
// game's PC_24 (one rounding to 24 bits per op; 53 >= 2*24+2, so the double step adds no second rounding while
// every operand fits 24 bits). [UNCERTAIN U-9789] a rate or size of 2^24 or more is not covered by that argument.
// The middle fstp to m32 (0x005b40d6) is exact on a value already rounded to 24 bits.
extern "C" __declspec(dllexport) int __cdecl AudioFmtConvertByteLength(int* param_1, unsigned int* param_2) {
    const unsigned char* in  = reinterpret_cast<const unsigned char*>(param_1);
    const unsigned char* out = reinterpret_cast<const unsigned char*>(param_2);
    // esi = output bytes-per-sample = (out bits-per-sample) >> 3   (0x005b406c..0x005b4076)
    const unsigned int uVar1 = static_cast<unsigned int>(out[0xc]) >> 3;

    float st = static_cast<float>(static_cast<double>(out[0xd]) / static_cast<double>(in[0xd]));       // 0x005b4098
    const float bits = static_cast<float>(static_cast<double>(out[0xc]) / static_cast<double>(in[0xc])); // 0x005b40b2
    st = static_cast<float>(static_cast<double>(bits) * static_cast<double>(st));                        // 0x005b40b6
    const float rate = static_cast<float>(static_cast<double>(*param_2) / static_cast<double>(param_1[0])); // 0x005b40c4
    st = static_cast<float>(static_cast<double>(rate) * static_cast<double>(st));                        // 0x005b40d0
    st = static_cast<float>(static_cast<double>(st) * static_cast<double>(param_1[2]));                  // 0x005b40d2

    const int inner = static_cast<int>(st);                                                              // 0x005b40f3

    // (inner - 1 + uVar1) & ~(uVar1 - 1)   (0x005b4103..0x005b410a)
    const unsigned int aligned =
        (static_cast<unsigned int>(inner + -1) + uVar1) & ~(uVar1 - 1u);

    // fild qword (zero-extended) / fstp m32 at the caller's rounding / truncating fistp (0x005b4110..0x005b4131)
    return static_cast<int>(static_cast<float>(static_cast<double>(aligned)));
}

RH_ScopedInstall(AudioFmtConvertByteLength, 0x005b4060);

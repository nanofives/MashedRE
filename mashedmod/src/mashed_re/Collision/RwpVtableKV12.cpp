// Mashed RE — D4-WSB-PORT-SOLV-KV (2026-10-29): the scene +0xf8 PGS iteration chain.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (capstone over original/MASHED.exe.unpatched, re/tools/disasm_fn.py; decomp from
// re/tools/decomp_pc.py, pool10). Completes the four rows RwpVtableKV1.cpp deferred:
//
//   0x0056adb0 -> 0x0056ac40 (twice per pass), 0x0056aae0 -> 0x0056a7a0
//
// WHY DISASSEMBLY, NOT DECOMP. The decompiler mis-models every frame in this chain (it
// shows FUN_0056ac40() called with no args, FUN_0056a7a0 with 3, FUN_0056aae0 with 14).
// Every frame below was recovered from the call-site pushes and the caller's `add esp`:
//
//   caller -> 0x0056adb0   69 words. 0x0056b310 builds it at 0x0056b57c..0x0056b630:
//                          `sub esp,0x10c` + `rep movsd` 0x43 words + 2 pushes, then
//                          `call [esp+0x384]`, cleanup `add esp,0x114` (0x0056b645).
//                          W1/W2 are pointers into 0x0056b310's frame (lea at 0x0056b610 /
//                          0x0056b625). The deepest read here is [ebp+0x118] = W69.
//   0x0056adb0 -> 0x0056ac40  15 words, built twice (0x0056aedf..0x0056af63 and
//                          0x0056b0dd..0x0056b167), cleanup `add esp,0x3c` (0x0056af84 /
//                          0x0056b185). The `0x56af69` / `0x56b16d` the decomp shows as
//                          locals are these two return addresses.
//   0x0056adb0 -> 0x0056aae0  56 words (0x0056b000..0x0056b0d7), cleanup `add esp,0xe0`
//                          (0x0056b0e3): 14 pushed/stored words, W3..W36 by `rep movsd`
//                          0x22 from [ebp+0x10], then 8 pushed words.
//   0x0056aae0 -> 0x0056a7a0  39 words (0x0056abc8..0x0056abf8), cleanup inside the
//                          `add esp,0x9c` at 0x0056ac13: 3 stored words, 34 copied by
//                          `rep movsd` 0x22 from [entry+0x3c] (= w15..w48), then the two
//                          pushes (delta, idx). This settles U-9589 / U-9590: the decomp's
//                          in_stack_00000098 / in_stack_0000009c are v38 = delta and
//                          v39 = idx, exactly as the RESULT_D4-WSB-PORT-SOLV.md word count said.
//
// Words are 1-based in comments (W1 = first arg = f.w[0]), matching RwpVtableKV1.cpp.
// A frame passed by value under __cdecl is ABI-identical to these hand-built blocks
// (the same idiom as Kv11Frame / Kv13Frame in RwpSolverPartition13.cpp).
//
// CALLS. Each port calls its callees THROUGH THE ORIGINAL RVA, so the live code runs: the
// original when only one row is installed (hook-alone shadow A/B), the port when the
// callee row is installed too. Same rule as the Lane-2 raw-RVA thunks.
//
// FLOAT MODEL. The .asi compiles with MSVC's default /arch:SSE2 (build.bat:246), so every
// `float` op below is one single-precision rounding. The x87 originals run under MASHED's
// PC_24 control word, which also rounds each op to a 24-bit mantissa; the two agree except
// where an x87 intermediate leaves the float exponent range (no store between ops) or two
// NaN operands meet (x87 and SSE pick different payloads). [UNCERTAIN U-9589..U-9592 carry
// the frame question only; this residue is the generic one every SSE2 port of an x87 body
// carries, not specific to these rows.] The SSE parts (0x0056a7a0's mulps/addps, 0x0056ac40's
// cmpltps/andps/orps, 0x0056adb0's movaps/cmpneqps) use the same intrinsics, lane for lane.
//
// fld m32 / fstp m32 pairs that only MOVE a float (0x0056af87..0x0056afbd, 0x0056b188..
// 0x0056b1bb) are not bit copies on x87: a signalling NaN comes back quiet. X87Move()
// reproduces that. `mov` copies (0x0056b209/0x0056b215, 0x0056b277..0x0056b281,
// 0x0056b2ac, 0x0056b2c3, 0x0056ad72) stay bit copies.
//
// Constants (all .rdata, read from MASHED.exe.unpatched 2026-10-29, embedded as bits so
// the TU does not read the image):
//   0x005e5a40  0x3f800000 x4   (1.0f x4)    lane-index increment, 0x0056ac5c
//   0x005e5a70  0x80000000 x4   (-0.0f x4)   sign mask, 0x0056ac90
//   0x005d757c  0x00000000      (0.0f)       sum seed, 0x0056ad2c
//   0x005cc32c  0x3f000000      (0.5f)       0x0056b219, 0x0056b28d
//   0x005cc320  0x3f800000      (1.0f)       0x0056b22b, 0x0056b23f
//   0x005cc568  0x42c80000      (100.0f)     0x0056b239
//
// .asi only (asi_sources.rsp, not exe_sources.rsp), like RwpVtableKV1.cpp. The rows have no
// A/B yet, so they register under the L2_ prefix: opt-in through MASHED_HOOK_ONLY or
// MASHED_HOOK_LANE2=1 (Core/HookSystem.cpp:244-253); a default .asi run does not install them.
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>
#include <xmmintrin.h>

namespace mashed_re {
namespace Collision {

struct Kv12Frame  { int w[69]; };   // caller 0x0056b310 -> 0x0056adb0
struct PgsAc4Frame { int w[15]; };  // 0x0056adb0 -> 0x0056ac40
struct PgsAaeFrame { int w[56]; };  // 0x0056adb0 -> 0x0056aae0
struct PgsA7aFrame { int w[39]; };  // 0x0056aae0 -> 0x0056a7a0

namespace {

inline float F32(std::uint32_t u) { float f; std::memcpy(&f, &u, 4); return f; }
inline std::uint32_t U32(float f) { std::uint32_t u; std::memcpy(&u, &f, 4); return u; }
inline float FW(int w) { return F32(static_cast<std::uint32_t>(w)); }

// fld m32 + fstp m32: exact for every value except a signalling NaN, which x87 quiets
// (sets mantissa bit 22) under the masked invalid exception.
inline std::uint32_t X87Move(std::uint32_t b) {
    if ((b & 0x7f800000u) == 0x7f800000u && (b & 0x007fffffu) != 0 && (b & 0x00400000u) == 0)
        b |= 0x00400000u;
    return b;
}

// fld m32 + fistp m32 at the current x87 rounding mode (0x0056b260 / 0x0056b264).
inline int X87Fistp(float v) {
    int r;
    __asm {
        fld   dword ptr [v]
        fistp dword ptr [r]
    }
    return r;
}

const float kHalf    = F32(0x3f000000u);   // 0x005cc32c
const float kOne     = F32(0x3f800000u);   // 0x005cc320
const float kHundred = F32(0x42c80000u);   // 0x005cc568
const float kSumSeed = F32(0x00000000u);   // 0x005d757c

inline void CallAae0(PgsAaeFrame f) {
    reinterpret_cast<void (__cdecl*)(PgsAaeFrame)>(0x0056aae0u)(f);   // 0x0056b0d8
}
inline float CallAc40(PgsAc4Frame f) {
    return reinterpret_cast<float (__cdecl*)(PgsAc4Frame)>(0x0056ac40u)(f);   // 0x0056af64 / 0x0056b168
}
inline void CallA7a0(PgsA7aFrame f) {
    reinterpret_cast<void (__cdecl*)(PgsA7aFrame)>(0x0056a7a0u)(f);   // 0x0056abfb
}

}  // namespace

// ---------------------------------------------------------------------------------------
// 0x0056a7a0  (callee of 0x0056aae0 only, call 0x0056abfb). 39-word frame:
//   v1 = float* acc (vec4 rows)  v4 = jacobian base (0x60-byte blocks)
//   v8 = int* row start          v11 = int* acc row index   v20 = int* row count
//   v29 = int* pair table (2 ints per idx>>2)   v32 = int* block table (2 ints per idx>>2)
//   v38 = float delta   v39 = int idx
// ---------------------------------------------------------------------------------------
static void __cdecl FUN_0056a7a0_impl(PgsA7aFrame g)
{
    char* const acc   = reinterpret_cast<char*>(g.w[0]);    // [ebp+0x08]
    char* const jac   = reinterpret_cast<char*>(g.w[3]);    // [ebp+0x14]
    const int*  start = reinterpret_cast<const int*>(g.w[7]);    // [ebp+0x24]
    const int*  arow  = reinterpret_cast<const int*>(g.w[10]);   // [ebp+0x30]
    const int*  cnt   = reinterpret_cast<const int*>(g.w[19]);   // [ebp+0x54]
    const int*  pair  = reinterpret_cast<const int*>(g.w[28]);   // [ebp+0x78]
    const int*  blk   = reinterpret_cast<const int*>(g.w[31]);   // [ebp+0x84]
    const float delta = FW(g.w[37]);                             // [ebp+0x9c]
    const int   idx   = g.w[38];                                 // [ebp+0xa0]

    const int r = idx >> 2;                 // sar edx,2   0x0056a7bb
    const int c = idx & 3;                  // and ecx,3   0x0056a7be
    const int a = pair[r * 2];              // 0x0056a7c1
    const int b = pair[r * 2 + 1];          // 0x0056a7c4

    // Lanes 3 / 7 of the original's stack vectors are never written; only lanes 0..2 of
    // each half are broadcast (shufps 0x00/0x55/0xaa), so they cannot reach a result.
    float A[8] = {}, B[8] = {};
    if (a != -1) {                                                     // 0x0056a7c8
        const float* base = reinterpret_cast<const float*>(blk[r * 2] * 0x60 + jac);
        A[0] = delta * base[c];      A[1] = delta * base[c + 4];  A[2] = delta * base[c + 8];
        A[4] = delta * base[c + 12]; A[5] = delta * base[c + 16]; A[6] = delta * base[c + 20];
    }
    if (b != -1) {                                                     // 0x0056a83e
        const int r2 = idx / 4;               // cdq / and edx,3 / add / sar 2   0x0056a847
        const float* base = reinterpret_cast<const float*>(blk[r2 * 2 + 1] * 0x60 + jac);
        B[0] = delta * base[c];      B[1] = delta * base[c + 4];  B[2] = delta * base[c + 8];
        B[4] = delta * base[c + 12]; B[5] = delta * base[c + 16]; B[6] = delta * base[c + 20];
    }

    // 0x0056a8d3 / 0x0056a9d7: acc[arow[k]] += J0*s0 + J1*s1 + ... + J5*s5, added left to
    // right onto the accumulator (addps order 0x0056a9a9..0x0056a9bb).
    auto apply = [&](int grp, const float* s) {
        if (cnt[grp] == 0) return;
        const __m128 s0 = _mm_set1_ps(s[0]), s1 = _mm_set1_ps(s[1]), s2 = _mm_set1_ps(s[2]);
        const __m128 s3 = _mm_set1_ps(s[4]), s4 = _mm_set1_ps(s[5]), s5 = _mm_set1_ps(s[6]);
        int j = 0;
        do {
            const int k = start[grp] + j;
            const float* J = reinterpret_cast<const float*>(k * 0x60 + jac);
            float* t = reinterpret_cast<float*>(arow[k] * 0x10 + acc);
            const __m128 m5 = _mm_mul_ps(_mm_load_ps(J + 20), s5);
            const __m128 m4 = _mm_mul_ps(_mm_load_ps(J + 16), s4);
            const __m128 m3 = _mm_mul_ps(_mm_load_ps(J + 12), s3);
            const __m128 m2 = _mm_mul_ps(_mm_load_ps(J + 8),  s2);
            const __m128 m1 = _mm_mul_ps(_mm_load_ps(J + 4),  s1);
            const __m128 m0 = _mm_mul_ps(_mm_load_ps(J),      s0);
            __m128 v = _mm_load_ps(t);
            v = _mm_add_ps(v, m0);
            v = _mm_add_ps(v, m1);
            v = _mm_add_ps(v, m2);
            v = _mm_add_ps(v, m3);
            v = _mm_add_ps(v, m4);
            v = _mm_add_ps(v, m5);
            _mm_store_ps(t, v);
            ++j;
        } while (j != cnt[grp]);            // re-read every pass (0x0056a9c1)
    };
    if (a != -1) apply(a, A);
    if (b != -1) apply(b, B);
}

// ---------------------------------------------------------------------------------------
// 0x0056aae0  (callee of 0x0056adb0 only, call 0x0056b0d8). 56-word frame:
//   w1 = int* i   w2 = int* j   (idx = *j + *i*4, re-read at every use)
//   w3..w5 -> v1..v3 of 0x0056a7a0 (w3 = float* acc)
//   w6 = int* lo-flag   w9 = int* hi-flag   w12 = float* clamped value
//   w15..w48 -> v4..v37 of 0x0056a7a0
//   w49 x   w50 lo   w51 hi   w52 target   w53 / w54 / w55 coefficients   w56 unused
// ---------------------------------------------------------------------------------------
static void __cdecl FUN_0056aae0_impl(PgsAaeFrame e)
{
    const int* pi = reinterpret_cast<const int*>(e.w[0]);      // [esp+0x14] after 4 pushes
    const int* pj = reinterpret_cast<const int*>(e.w[1]);      // [esp+0x18]
    auto at = [&]() { return *pj + *pi * 4; };                  // lea [ecx+eax*4]

    const float x   = FW(e.w[48]);    // entry+0xc4
    const float lo  = FW(e.w[49]);    // entry+0xc8
    const float hi  = FW(e.w[50]);    // entry+0xcc
    const float tgt = FW(e.w[51]);    // entry+0xd0
    const float k53 = FW(e.w[52]);    // entry+0xd4
    const float k54 = FW(e.w[53]);    // entry+0xd8
    const float k55 = FW(e.w[54]);    // entry+0xdc

    float v = tgt - x;                                   // 0x0056aae0 / 0x0056aae7
    if (v < lo) v = lo;                                  // fcom / test ah,5 / jp  0x0056aaf2
    if (hi < v) v = hi;                                  // fcomp st(1)            0x0056ab10
    reinterpret_cast<float*>(e.w[11])[at()] = v;         // 0x0056ab38
    reinterpret_cast<int*>(e.w[5])[at()] = (v == lo) ? 0 : -1;   // test ah,0x44  0x0056ab44
    reinterpret_cast<int*>(e.w[8])[at()] = (v == hi) ? 0 : -1;   // 0x0056ab7a
    const float delta = v - tgt;                         // fsub entry+0xd0, fstp entry+0xc8

    PgsA7aFrame g;
    g.w[0] = e.w[2]; g.w[1] = e.w[3]; g.w[2] = e.w[4];   // 0x0056abf3..0x0056abf8
    std::memcpy(&g.w[3], &e.w[14], 34 * sizeof(int));     // rep movsd 0x22 from entry+0x3c
    g.w[37] = static_cast<int>(U32(delta));              // push edx  0x0056abc9
    g.w[38] = at();                                      // push ecx  0x0056abc8
    CallA7a0(g);

    float* acc = reinterpret_cast<float*>(e.w[2]);
    const int i3 = at();                                 // re-read after the call, 0x0056ac0e
    acc[i3] = (k53 * k55 + k54) * delta + acc[i3];       // 0x0056ac00..0x0056ac33
}

// ---------------------------------------------------------------------------------------
// 0x0056ac40  (callee of 0x0056adb0, calls 0x0056af64 and 0x0056b168). 15-word frame:
//   v1 = int* out index   v2 = int* out lane   v3 = float* x (n vec4)   v4 = n
//   v9 = mask A (n vec4)   v12 = mask B (n vec4)   v15 = float threshold
// Returns the x87 sum of squares of all 4n floats of x (st0, stored as float by both callers).
// ---------------------------------------------------------------------------------------
static float __cdecl FUN_0056ac40_impl(PgsAc4Frame v)
{
    const float* x  = reinterpret_cast<const float*>(v.w[2]);    // [ebp+0x10]
    const unsigned n = static_cast<unsigned>(v.w[3]);            // [ebp+0x14]
    const float* mA = reinterpret_cast<const float*>(v.w[8]);    // [ebp+0x28]
    const float* mB = reinterpret_cast<const float*>(v.w[11]);   // [ebp+0x34]
    const float thr = FW(v.w[14]);                               // [ebp+0x40]

    const __m128 inc  = _mm_set1_ps(F32(0x3f800000u));   // 0x005e5a40
    const __m128 sign = _mm_set1_ps(F32(0x80000000u));   // 0x005e5a70
    __m128 best = _mm_set1_ps(thr);         // movss + shufps 0   0x0056ac58 / 0x0056ac78
    __m128 lane = _mm_setzero_ps();         // xmm3               0x0056ac6b
    __m128 ctr  = _mm_setzero_ps();         // xmm4               0x0056ac81
    if (n != 0) {                           // jbe                0x0056ac84
        for (unsigned g = 0; g < n; ++g) {
            const __m128 xv = _mm_load_ps(x + g * 4);
            const __m128 nb = _mm_xor_ps(sign, _mm_and_ps(xv, _mm_load_ps(mB + g * 4)));
            const __m128 cB = _mm_cmplt_ps(best, nb);                        // 0x0056acca
            const __m128 cA = _mm_cmplt_ps(best, _mm_and_ps(xv, _mm_load_ps(mA + g * 4)));
            const __m128 m  = _mm_or_ps(cA, cB);                             // 0x0056acde
            if (_mm_movemask_ps(m) != 0) {                                   // 0x0056ace1
                const __m128 ax = _mm_andnot_ps(sign, xv);                   // |x|
                best = _mm_or_ps(_mm_and_ps(m, ax), _mm_andnot_ps(m, best));
                lane = _mm_or_ps(_mm_and_ps(m, ctr), _mm_andnot_ps(m, lane));
            }
            ctr = _mm_add_ps(ctr, inc);                                      // 0x0056ad15
        }
    }

    float sum = kSumSeed;                                                    // 0x0056ad2c
    for (unsigned g = 0; g < n; ++g)
        for (int q = 0; q < 4; ++q) {
            const float xi = x[g * 4 + q];
            sum = sum + xi * xi;                                             // 0x0056ad4c..0x0056ad56
        }

    alignas(16) float bl[4], ln[4];
    _mm_store_ps(bl, best);                                                  // 0x0056ad3f
    _mm_store_ps(ln, lane);                                                  // 0x0056ad3c
    std::uint32_t top = U32(thr);           // [esp+0x10], seeded from [ebp+0x40]
    int outLane = -1, outIdx = -1;
    for (int q = 0; q < 4; ++q) {
        if (bl[q] > F32(top)) {             // fcomp / and 0x4100 / jne   0x0056ad65
            top = U32(bl[q]);               // mov                        0x0056ad72
            outLane = q;
        }
    }
    if (outLane != -1)
        outIdx = _mm_cvtss_si32(_mm_load_ss(&ln[outLane]));                 // cvtss2si 0x0056ad91
    *reinterpret_cast<int*>(v.w[0]) = outIdx;                                // 0x0056ad9b
    *reinterpret_cast<int*>(v.w[1]) = outLane;                               // 0x0056ad9e
    return sum;
}

// ---------------------------------------------------------------------------------------
// 0x0056adb0  scene +0xf8 callback (installed at 0x0055fda6 when [scene+0x58] == 2), called
// only by 0x0056b310 through `call [esp+0x384]` at 0x0056b630. 69-word frame:
//   W1 = int* p1 (p1[0..5] array pointers, p1[6] written: pass count)
//   W2 = int* p2 (p2[0..5] array pointers)
//   W3..W36 -> w15..w48 of 0x0056aae0     W37 / W40 / W43 = float arrays
//   W46 = vec4 source  W47 = vec4 count  W49 = vec4 source
//   W64 / W65 / W66 -> w54..w56 of 0x0056aae0 (W66 also v15 of 0x0056ac40)
//   W68 = pass limit   W69 = adaptive-stop flag
// ---------------------------------------------------------------------------------------
static void __cdecl FUN_0056adb0_impl(Kv12Frame f)
{
    int* const p1 = reinterpret_cast<int*>(f.w[0]);    // [ebp+0x08]
    const int* p2 = reinterpret_cast<const int*>(f.w[1]);   // [ebp+0x0c]
    const int P1_0 = p1[0], P1_1 = p1[1], P1_2 = p1[2];
    const int P1_3 = p1[3], P1_4 = p1[4], P1_5 = p1[5];
    const int P2_0 = p2[0], P2_1 = p2[1], P2_2 = p2[2];
    const int P2_3 = p2[3], P2_4 = p2[4], P2_5 = p2[5];
    const int W37 = f.w[36], W40 = f.w[39], W43 = f.w[42];
    const int W64 = f.w[63], W65 = f.w[64], W66 = f.w[65];
    const unsigned W68 = static_cast<unsigned>(f.w[67]);   // [ebp+0x114]
    const int W69 = f.w[68];                                // [ebp+0x118]

    // 0x0056ae2c..0x0056aedd: copy W46 -> p1[3] and W49 -> p1[0], and write the two
    // not-equal masks W49 != W37 -> p2[0], W49 != W40 -> p2[3], one vec4 per pass.
    if (f.w[46] != 0) {                                     // [ebp+0xc0]
        const int src = f.w[45];                            // [ebp+0xbc]
        const int s49 = f.w[48];                            // [ebp+0xc8]
        unsigned n = static_cast<unsigned>(f.w[46]);
        int off = 0;
        do {
            const __m128 x0 = _mm_load_ps(reinterpret_cast<const float*>(s49 + off));
            const __m128 x1 = _mm_load_ps(reinterpret_cast<const float*>(src + off));
            _mm_store_ps(reinterpret_cast<float*>(P1_3 + off), x1);
            _mm_store_ps(reinterpret_cast<float*>(P1_0 + off), x0);
            const __m128 y37 = _mm_load_ps(reinterpret_cast<const float*>(W37 + off));
            _mm_store_ps(reinterpret_cast<float*>(P2_0 + off), _mm_cmpneq_ps(x0, y37));
            const __m128 y40 = _mm_load_ps(reinterpret_cast<const float*>(W40 + off));
            _mm_store_ps(reinterpret_cast<float*>(P2_3 + off), _mm_cmpneq_ps(x0, y40));
            off += 0x10;
        } while (--n != 0);
    }

    int outI = -1, outJ = -1;    // [esp+0x18] / [esp+0x1c], written by 0x0056ac40
    auto ac40Frame = [&]() {
        PgsAc4Frame a;
        a.w[0] = reinterpret_cast<int>(&outI); a.w[1] = reinterpret_cast<int>(&outJ);
        a.w[2] = P1_3; a.w[3] = P1_4; a.w[4] = P1_5;
        a.w[5] = P1_0; a.w[6] = P1_1; a.w[7] = P1_2;
        a.w[8] = P2_0; a.w[9] = P2_1; a.w[10] = P2_2;
        a.w[11] = P2_3; a.w[12] = P2_4; a.w[13] = P2_5;
        a.w[14] = W66;
        return a;
    };

    // Gather at k = *j + *i*4 (0x0056af69..0x0056afbd / 0x0056b16d..0x0056b1bb), fld/fstp.
    std::uint32_t g30, g34, g38, g3c, g40;
    auto gather = [&]() {
        const int k = outJ + outI * 4;
        g30 = X87Move(reinterpret_cast<const std::uint32_t*>(P1_3)[k]);
        g34 = X87Move(reinterpret_cast<const std::uint32_t*>(P1_0)[k]);
        g38 = X87Move(reinterpret_cast<const std::uint32_t*>(W37)[k]);
        g3c = X87Move(reinterpret_cast<const std::uint32_t*>(W40)[k]);
        g40 = X87Move(reinterpret_cast<const std::uint32_t*>(W43)[k]);
    };

    const float r0 = CallAc40(ac40Frame());                 // 0x0056af64
    std::uint32_t best = U32(r0);                           // fst  [esp+0x44]  0x0056af71
    gather();
    std::uint32_t lo  = U32(r0);                            // fstp [esp+0x24]  0x0056afd2
    std::uint32_t hi  = 0;                                  // [esp+0x20]       0x0056afa8
    std::uint32_t avg = 0;                                  // [esp+0x10]       0x0056af97
    unsigned pass   = 0;                                    // [esp+0x28]       0x0056af93
    unsigned window = 0;                                    // [esp+0x2c]       0x0056afe5
    const float nf  = static_cast<float>(static_cast<double>(static_cast<unsigned>(P1_4)));  // fild qword 0x0056afd6
    unsigned limit  = static_cast<unsigned>(P1_4) + 1;      // [esp+0x48]       0x0056afda

    if (W68 == 0) { p1[6] = 0; return; }                    // jbe 0x0056b2ee

    for (;;) {
        if (outI == -1) { p1[6] = static_cast<int>(pass); return; }   // 0x0056affa -> 0x0056b2fa

        PgsAaeFrame e;                                      // 0x0056b000..0x0056b0d7
        e.w[0] = reinterpret_cast<int>(&outI); e.w[1] = reinterpret_cast<int>(&outJ);
        e.w[2] = P1_3; e.w[3] = P1_4; e.w[4] = P1_5;
        e.w[5] = P2_0; e.w[6] = P2_1; e.w[7] = P2_2;
        e.w[8] = P2_3; e.w[9] = P2_4; e.w[10] = P2_5;
        e.w[11] = P1_0; e.w[12] = P1_1; e.w[13] = P1_2;
        std::memcpy(&e.w[14], &f.w[2], 34 * sizeof(int));   // rep movsd from [ebp+0x10]
        e.w[48] = static_cast<int>(g30); e.w[49] = static_cast<int>(g38);
        e.w[50] = static_cast<int>(g3c); e.w[51] = static_cast<int>(g34);
        e.w[52] = static_cast<int>(g40);
        e.w[53] = W64; e.w[54] = W65; e.w[55] = W66;
        CallAae0(e);

        const float cur = CallAc40(ac40Frame());            // 0x0056b168, fstp [esp+0x14]
        gather();

        if (W69 != 0) {                                     // [ebp+0x118]  0x0056b1bf
            ++window;                                       // 0x0056b1cf
            bool check;
            if (window > limit) { window = 0; check = true; }   // jbe 0x0056b1d6
            else check = (window == 0);                     // 0x0056b1de
            if (check) {
                if (!(F32(avg) >= F32(best))) {             // fcomp / and 0x100  0x0056b1ea
                    best = avg;                                             // mov 0x0056b215
                    const float a2 = (F32(avg) + F32(avg)) * kHalf;         // 0x0056b203..0x0056b21f
                    avg = U32(a2);
                    const float span = F32(hi) - F32(lo);                   // 0x0056b223
                    const float rcp  = kOne / F32(avg);                     // 0x0056b22b
                    const float est  = rcp * span;                          // 0x0056b237
                    const float test = est * kHundred + kOne;               // 0x0056b239 / 0x0056b23f
                    if (static_cast<double>(limit) > static_cast<double>(test)) {   // fcompp, and 0x4100
                        const float scaled = est * nf;                      // 0x0056b254 / 0x0056b256
                        limit = static_cast<unsigned>(X87Fistp(scaled) + 1);    // 0x0056b264..0x0056b26d
                    }
                    hi = U32(cur);                                          // 0x0056b27d
                    lo = U32(cur);                                          // 0x0056b281
                } else {
                    outI = -1;                                              // 0x0056b1f7
                }
            }
            avg = U32((cur - F32(avg)) * kHalf + F32(avg));                // 0x0056b285..0x0056b297
            if (cur > F32(hi)) hi = U32(cur);                              // 0x0056b29f..0x0056b2b0
            if (cur < F32(lo)) lo = U32(cur);                              // 0x0056b2b8..0x0056b2c7
        }

        ++pass;                                                             // 0x0056b2d5
        if (!(pass < W68)) { p1[6] = static_cast<int>(pass); return; }      // jb 0x0056aff7
    }
}

// --- registrations (.asi only; L2_ prefix = opt-in, see file header) --------------------
extern "C" void __cdecl L2_FUN_0056a7a0(PgsA7aFrame g) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0056a7a0", 0x0056a7a0u, ShadowAB::kPhaseAny);
    ShadowAB::RunTracked(ab, FUN_0056a7a0_impl, SHADOW_STACK_WINDOW(), g);
}
extern "C" void __cdecl L2_FUN_0056aae0(PgsAaeFrame e) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0056aae0", 0x0056aae0u, ShadowAB::kPhaseAny);
    ShadowAB::RunTracked(ab, FUN_0056aae0_impl, SHADOW_STACK_WINDOW(), e);
}
extern "C" float __cdecl L2_FUN_0056ac40(PgsAc4Frame v) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0056ac40", 0x0056ac40u, ShadowAB::kPhaseAny);
    return ShadowAB::RunTracked(ab, FUN_0056ac40_impl, SHADOW_STACK_WINDOW(), v);
}
extern "C" void __cdecl L2_FUN_0056adb0(Kv12Frame f) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0056adb0", 0x0056adb0u, ShadowAB::kPhaseAny);
    ShadowAB::RunTracked(ab, FUN_0056adb0_impl, SHADOW_STACK_WINDOW(), f);
}
RH_ScopedInstall(L2_FUN_0056a7a0, 0x0056a7a0);
RH_ScopedInstall(L2_FUN_0056aae0, 0x0056aae0);
RH_ScopedInstall(L2_FUN_0056ac40, 0x0056ac40);
RH_ScopedInstall(L2_FUN_0056adb0, 0x0056adb0);

} // namespace Collision
} // namespace mashed_re

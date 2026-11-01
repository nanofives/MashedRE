// ============================================================================
//  L2_0055b940.cpp -- Lane 2 port, HAND-WRITTEN (decomp2port.py refuses it: UNKNOWN_TYPE ret float10), 2026-11-01
//  Transcription of Ghidra's decompilation of MASHED.exe (anchor in CLAUDE.md), checked line by line
//  against the disassembly. A row moves to C3 only through the shadow A/B plus the caller/callee gate
//  in re-classify. Globals are read through their absolute addresses (.asi target only; NOT in
//  exe_sources.rsp). Unported callees are raw-RVA thunks.
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x0055b940  FUN_0055b940  -- hand-written from the Ghidra decompilation and the disassembly of
// MASHED.exe.unpatched (verify/d4wsb_portbroad2_20261101/disasm_lin.py 0055b940 0055ba89). Size 329 B.
// Plate: hooks.csv row 0x0055b940. decomp2port.py refuses it (UNKNOWN_TYPE: ret float10). The return is
// ST0; MSVC returns a float in ST0 too. Every compare follows the fnstsw / test / jp form, which sends NaN
// down a fixed side; three of Ghidra's compares send NaN the other way and are rewritten (cited below).
// ---------------------------------------------------------------------------
#define DAT_005d757c (*(const float*)0x005d757cu)   // .rdata 00000000 (0.0f)
#define DAT_005ceabc (*(const float*)0x005ceabcu)   // .rdata 00800000 (FLT_MIN); Ghidra: PTR_DAT_005ceabc
#define DAT_005ceac0 (*(const float*)0x005ceac0u)   // .rdata 7f7fffff (FLT_MAX)
// 0x0055bbf0 returns ST0, read back as a float: its last load is `fld dword ptr [esp+0x38]` at 0x0055bd61.
static inline float L2T_FUN_0055bbf0(unsigned int a0, float * a1, float * a2) { return reinterpret_cast<float(__cdecl*)(unsigned int, float *, float *)>(0x0055bbf0u)(a0, a1, a2); }   // 0x0055bbf0 (original thunk)

extern "C" float __cdecl L2_FUN_0055b940(unsigned int * param_1, float param_2, int param_3);
static float __cdecl L2_FUN_0055b940_impl(unsigned int * param_1, float param_2, int param_3)
{
  // [esp+0..0x24] after `sub esp,0x28` at 0x0055b940: L[0..2] is 0x0055bbf0's param_3 (`lea eax,[esp]`),
  // L[3..9] its param_2 (`lea ecx,[esp+0xc]`). One block, as on the original stack.
  float L[10];
  const float r = L2T_FUN_0055bbf0(*param_1, &L[3], &L[0]);         // 0x0055b94c..0x0055b955
  // fcomp / test ah,5 / jp at 0x0055b95e..0x0055b96c: only an ordered `param_2 < 0` takes it.
  // The original writes the result into its own param_2 arg slot (`fstp [esp+0x34]` at 0x0055b976). No caller reads
  // that slot after the call (the 7 sites pop ST0 and read [esp+0x30] or higher), so a local suffices.
  if (param_2 < DAT_005d757c) {
    param_2 = -(param_2 * r);                                        // 0x0055b96e..0x0055b976
  }
  *(float *)(param_1 + 4) = param_2;                                 // 0x0055b98f
  *(float *)(param_1 + 1) = L[0];                                    // 0x0055b996
  *(float *)(param_1 + 2) = L[1];                                    // 0x0055b99c
  *(float *)(param_1 + 3) = L[2];                                    // 0x0055b9a5
  // fcomp dword ptr [0x5ceabc] at 0x0055b986, flags read at 0x0055b9a8; `and eax,0x4100 / jne` skips on <, == or NaN.
  if (param_3 != 0) {                                                // test eax,eax / je at 0x0055b9a3..0x0055b9aa
    if (param_2 > DAT_005ceabc) {                                    // 0x0055b9ac..0x0055b9b1
      L[3] = L[3] * param_2;                                         // 0x0055b9b3..0x0055b9bb
      L[4] = L[4] * param_2;                                         // 0x0055b9bf..0x0055b9c7
      L[5] = L[5] * param_2;                                         // 0x0055b9cb..0x0055b9d3
    }
    *(float *)(param_1 + 5) = L[3];                                  // 0x0055b9e2
    *(float *)(param_1 + 6) = L[4];                                  // 0x0055b9e9
    *(float *)(param_1 + 7) = L[5];                                  // 0x0055b9ef
    *(float *)(param_1 + 8) = L[6];                                  // 0x0055b9fa
    *(float *)(param_1 + 9) = L[7];                                  // 0x0055ba00
    *(float *)(param_1 + 10) = L[8];                                 // 0x0055ba07
    *(float *)(param_1 + 0xb) = L[9];                                // 0x0055ba0f
    param_1[0x12] = param_1[0x12] | 2;                               // 0x0055ba0a..0x0055ba0d, 0x0055ba64
  }
  else if (param_2 > DAT_005ceabc) {                                 // 0x0055ba14..0x0055ba19 (Ghidra's `<=` exit would let NaN through)
    // max of L[3..5] as the three fcomp / jp tests pick it (0x0055ba1b..0x0055ba54); NaN goes the jp-taken way.
    float m;
    if (L[3] < L[4]) m = (L[5] < L[4]) ? L[4] : L[5];
    else             m = (L[3] < L[5]) ? L[5] : L[3];
    const unsigned int fl = param_1[0x12] & 0xfffffffdu;             // 0x0055ba5c..0x0055ba5f
    *(float *)(param_1 + 0xc) = m * param_2;                         // 0x0055ba58, 0x0055ba61
    param_1[0x12] = fl;                                              // 0x0055ba64
  }
  // fcom dword ptr [0x5ceabc] / test ah,5 / jp at 0x0055ba67..0x0055ba73: an ordered r < FLT_MIN returns FLT_MAX,
  // anything else (NaN included) divides. Ghidra's `FLT_MIN <= r` would send NaN to FLT_MAX.
  if (r < DAT_005ceabc) {
    return DAT_005ceac0;                                             // 0x0055ba75..0x0055ba77
  }
  return param_2 / r;                                                // fdivr dword ptr [esp+0x30] at 0x0055ba81
}
extern "C" float __cdecl L2_FUN_0055b940(unsigned int * param_1, float param_2, int param_3) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0055b940", 0x0055b940u, ShadowAB::kPhaseAny);
    return ShadowAB::RunTracked(ab, L2_FUN_0055b940_impl, SHADOW_STACK_WINDOW(), param_1, param_2, param_3);
}
RH_ScopedInstall(L2_FUN_0055b940, 0x0055b940);
#undef DAT_005d757c
#undef DAT_005ceabc
#undef DAT_005ceac0

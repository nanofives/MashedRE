// ============================================================================
//  L2_0055bbf0.cpp -- Lane 2 port, HAND-WRITTEN (decomp2port.py refuses it: INDIRECT_CALL), 2026-11-01
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

typedef unsigned int   undefined4;

// ---------------------------------------------------------------------------
// 0x0055bbf0  FUN_0055bbf0  -- hand-written from the Ghidra decompilation and the disassembly of
// MASHED.exe.unpatched (verify/d4wsb_portbroad2_20261101/disasm_lin.py 0055bbf0 0055bd6b). Size 379 B.
// Plate: hooks.csv row 0x0055bbf0. decomp2port.py refuses it (INDIRECT_CALL), so every line below
// cites the instruction it reproduces. Float sums follow the x87 order, not Ghidra's text order.
// ---------------------------------------------------------------------------
// 0x00546b10 is bound to its port (Collision/RwpSolverMath2.cpp), as decomp2port.py binds it in L2_0055b080.
extern "C" undefined4 __cdecl FUN_00546b10(float *param_1, float *param_2);   // 0x00546b10 (ported)

// `call dword ptr [eax+0xc]` at 0x0055bc07: three pushed args (esi, edi, &local), `add esp, 0xc` at 0x0055bc0d
// (caller cleans, cdecl), result in ST0 stored with `fstp dword ptr [esp+0x38]` at 0x0055bc10 (rounded to float).
static inline float L2_vcallf(unsigned int fn, float *a0, int a1, float *a2)
{ return reinterpret_cast<float(__cdecl*)(float *, int, float *)>(fn)(a0, a1, a2); }

extern "C" float __cdecl L2_FUN_0055bbf0(float * param_1, int param_2, float * param_3);
static float __cdecl L2_FUN_0055bbf0_impl(float * param_1, int param_2, float * param_3)
{
  // [esp+8..0x20] after `push edi` at 0x0055bbf9: loc[0..2] = the slot call's output (`lea ecx,[esp+8]` at 0x0055bc00),
  // loc[3..6] = the output of 0x00546b10 (`lea eax,[esp+0x14]` at 0x0055bc1e). One block, as on the original stack.
  float loc[7];
  // [esi+0x5c] is a pointer dword (`mov eax,[esi+0x5c]` at 0x0055bbfd); Ghidra's `(int)param_1[0x17]` would convert a float.
  const float r = L2_vcallf(*(unsigned int *)(*(unsigned int *)(param_1 + 0x17) + 0xc), param_1, param_2, loc);
  // Not reproduced: the original keeps r in its own param_1 arg slot (`fstp [esp+0x38]` at 0x0055bc10). Neither caller
  // reads that slot after the call (0x0055b95a reads [esp+0x40], 0x005743df reads [esp+0x28]).
  if ((*(unsigned char *)(*(unsigned int *)(param_1 + 0x17) + 0x40) & 2) == 0) {    // 0x0055bc0a..0x0055bc18
    FUN_00546b10(&loc[3], param_1);                                                  // 0x0055bc1e..0x0055bc24
    float *p = (float *)param_2;
    const float p3 = p[3], p4 = p[4], p5 = p[5], p6 = p[6];   // [edi+0xc..0x18]; all read before the first store (0x0055bcd5)
    const float q0 = loc[3], q1 = loc[4], q2 = loc[5], q3 = loc[6];
    const float w  = q3 * p6 - ((p4 * q1 + p3 * q0) + p5 * q2);    // 0x0055bc29..0x0055bc50
    const float x1 = p5 * q1 - p4 * q2;                             // 0x0055bc54..0x0055bc62
    const float y1 = p3 * q2 - p5 * q0;                             // 0x0055bc64..0x0055bc71
    const float z1 = p4 * q0 - p3 * q1;                             // 0x0055bc73..0x0055bc82
    const float n3 = (p3 * q3 + x1) + p6 * q0;                      // 0x0055bc86..0x0055bc8e, 0x0055bcaa..0x0055bcb5
    const float n4 = (p4 * q3 + y1) + p6 * q1;                      // 0x0055bc92..0x0055bc9b, 0x0055bcb9..0x0055bcc6
    const float n5 = (p5 * q3 + z1) + p6 * q2;                      // 0x0055bc9f..0x0055bca6, 0x0055bcca..0x0055bce0
    p[3] = n3;                                                      // 0x0055bcd5
    p[4] = n4;                                                      // 0x0055bcd7
    p[5] = n5;                                                      // 0x0055bcf0
    p[6] = w;                                                       // 0x0055bcf3
    const float v0 = loc[0], v1 = loc[1], v2 = loc[2];
    const float s0 = (v2 * param_1[8]  + v1 * param_1[4]) + v0 * param_1[0];   // 0x0055bcec..0x0055bd0c
    const float s1 = (v2 * param_1[9]  + v1 * param_1[5]) + v0 * param_1[1];   // 0x0055bd0e..0x0055bd25
    const float s2 = (v2 * param_1[10] + v1 * param_1[6]) + v0 * param_1[2];   // 0x0055bd27..0x0055bd40
    param_3[0] = param_1[0xc] + s0;                                 // 0x0055bd44..0x0055bd49
    param_3[1] = param_1[0xd] + s1;                                 // 0x0055bd4b..0x0055bd50
    param_3[2] = param_1[0xe] + s2;                                 // 0x0055bd57..0x0055bd5e
  }
  return r;                                                         // fld dword ptr [esp+0x38] at 0x0055bd61
}
extern "C" float __cdecl L2_FUN_0055bbf0(float * param_1, int param_2, float * param_3) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0055bbf0", 0x0055bbf0u, ShadowAB::kPhaseAny);
    return ShadowAB::RunTracked(ab, L2_FUN_0055bbf0_impl, SHADOW_STACK_WINDOW(), param_1, param_2, param_3);
}
RH_ScopedInstall(L2_FUN_0055bbf0, 0x0055bbf0);

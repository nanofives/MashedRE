// RwpSolverCore12.cpp — B5e solver-island cluster K12 (single fn, the island's largest).
// Member (rva:size): 00570090:10500 — 1 fn / 10,500 B. __cdecl (verified: epilogue @0x0057298a
// = pop esi/ebp/ebx; add esp,0x320; RET(0xC3) — caller cleans args, frame 0x320).
//
// 0x00570090  Per-joint constraint-ROW GENERATOR (the K10/f350 analogue for the full 6-DOF
//   joint/contact: normal + 2 friction + twist + swing + linear/angular limits + motors). For the
//   two constraint bodies it resolves the contact-frame basis (FUN_0056fb90 x2, K10), the relative
//   orientation double-cover (FUN_0056fea0, K10) and the swing magnitude (FUN_005667c0, K1); then a
//   long sequence of config-flag-gated blocks each (a) memcpy the 27-word DAT_005e5738 row template
//   into a row buffer, (b) fill the Jacobian rows + clamp limits, (c) emit via FUN_0056f1f0 (K9);
//   ends with FUN_0056f0a0 (K9). Verbatim transcription of the C1 decomp.
//
// ---- DE-RISK / transcription notes (NO-GUESSING; every claim cited to decomp/disasm/memory_read):
//  A. Ghidra `float10 fVar19/fVar20` + `extraout_ST1` are x87-liveness ARTIFACTS, not returns:
//     fVar19=(float10)_DAT_005ceac0 (memory_read 0x005ceac0 = 0x7f7fffff = +FLT_MAX) and
//     fVar20=(float10)_DAT_005e5050 (0x005e5050 = 0xff7fffff = -FLT_MAX) are the two default clamp
//     limits held resident in ST0/ST1 across the void FUN_0056f1f0 calls. Every `(float)fVar19` is
//     inlined to  3.4028235e+38f  and every `(float)fVar20` to  -3.4028235e+38f. The lone REAL
//     float10 is FUN_005667c0 @decomp-421 (a magnitude): `float fVar1 = (float)FUN_005667c0(...)`.
//  B. The three row buffers local_29c / local_d8 / local_6c are each 27-word CONTIGUOUS buffers
//     that FUN_0056f1f0 indexes [0..0x1a]; the ~150 "named" Ghidra locals between them are their
//     interior slots. Mapped to float row0[27] / row1[27] / row2[27]; template fill = memcpy
//     (LOAD_ROW). The "int-bit" slot stores are all exactly representable float literals
//     (0x3f4ccccd == 0.8f, 0xff7fffff == -FLT_MAX, 0 == 0.0f), so the buffers stay float[].
//       row0(local_29c): [0..2]=29c[0..2] [4]=28c [5]=288 [6]=284 [8]=27c [9]=278 [10]=274
//         [12]=26c [13]=268 [14]=264 [16]=25c [17]=258 [18]=254 [19]=250 [20]=24c [21]=248
//         [22]=244 [23]=240 [24]=23c
//       row1(local_d8): [0]=d8 [4]=c8 [5]=c4 [6]=c0 [12]=a8 [13]=a4 [14]=a0 [16]=98 [17]=94
//         [18]=90 [19]=8c [20]=88 [21]=84 [22]=80 [23]=7c [24]=78
//       row2(local_6c): [4]=5c [5]=58 [6]=54 [12]=3c [13]=38 [14]=34 [16]=2c [17]=28 [18]=24
//         [19]=20 [20]=1c [21]=18 [22]=14 [23]=10 [24]=c
//  C. Helper output layouts (verified vs my committed K10/K9/K1 ports): FUN_0056fb90 writes
//     param_3=4-float quat, param_4=1 body-position POINTER, param_5=15-float [3x4 rot + 3 center
//     @idx 12/13/14]. FUN_0056fea0 writes param_3=4-float quat product, param_4=12-float matrix.
//     FUN_005667c0 reads a 3-vec, writes a 3-vec, returns |v|. So the receiving frame arrays are:
//       qA[4],qB[4]  = fb90 quats (local_2c0.., local_2b0..)      — dead after fea0, then slot reuse
//       qp[4]        = fea0 quat  (local_2fc.. copied out to named local_2fc/2f8/2f4/2f0)
//       mC[12]       = fea0 matrix (local_218.. copied out to local_218..local_1ec)
//       mA[15],mB[15]= fb90 matrices (bodyA @local_19c, bodyB @local_1dc; center at idx 12/13/14).
//                      mB stays an addressable array (loop1/loop2 walk it via pfVar13=&mB[1], +=4).
//       rel[3]/nrm[3]= FUN_005667c0 in/out (local_1e8.. / local_224..).
//  D. Ghidra "float* holding a float value" slot reuse (K10-class) split into distinct typed vars:
//     local_2a0 = float* Bpos (deref only @168-178) THEN float scratch (199+); local_2ec = float*
//     Apos THEN float scratch (934+); pfVar13 = float* mB-walker THEN float pf13_f (1028+).
//     local_300 = int bitmask 8/16/32 in loop1 THEN float (999+); local_318 = int loop index THEN
//     float f318 (936+). Values never overlap in lifetime, so separate C vars are bit-faithful.
//  E. x87 FADD associativity under the <=1-ULP floor -> U-9020 (settle at lane-end per-field diff);
//     the mixed-sign sums were printed already-parenthesized by Ghidra and are kept verbatim.
// ---------------------------------------------------------------------------
#include "../Core/HookSystem.h"
#include <cmath>       // sqrtf — x87 FSQRT floor
#include <cstring>     // memcpy — the 27-word row-template copy

namespace mashed_re {
namespace Collision {

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef long double    float10;     // x87 80-bit return of FUN_005667c0 (read as (float))

#define _DAT_005cc320  (*(const float*)0x005cc320u)   // 1.0f
#define _DAT_005cc32c  (*(const float*)0x005cc32cu)   // 0.5f
#define _DAT_005cc31c  (*(const float*)0x005cc31cu)   // 3.0f
#define _DAT_005cc574  (*(const float*)0x005cc574u)   // 2.0f
#define _DAT_005cc34c  (*(const float*)0x005cc34cu)   // -2.0f
#define _DAT_005cc35c  (*(const float*)0x005cc35cu)   // 4.0f
#define _DAT_005cc328  (*(const float*)0x005cc328u)   // 0.01f
#define _DAT_005cc56c  (*(const float*)0x005cc56cu)   // 0.1f
#define _DAT_005e57dc  (*(const float*)0x005e57dcu)   // -0.9f
#define _DAT_005ceae4  (*(const float*)0x005ceae4u)   // 0.99999893f (0x3f7fffef)
#define DAT_005d757c   (*(const float*)0x005d757cu)   // 0.0f

// The 27-word "unset" constraint-row template (see K9 note 3). Copy = the 0x1b-count decomp loop.
#define LOAD_ROW(buf)  memcpy((buf), (const void*)0x005e5738u, 27u * sizeof(float))

// field accessors matching decomp bases
#define PB(off)  (*(float*)(param_2 + (off)))   // manifold/body field (param_2 base)
#define CF(off)  (*(float*)(iVar10 + (off)))    // joint-config field (iVar10 = *(int*)(param_2+0x4c))

// --- extern callees (earlier clusters) ---
extern "C" int * __cdecl FUN_0056fb90(uint *param_1,float *param_2,float *param_3,int *param_4,
                                      float *param_5);                    // K10
extern "C" void  __cdecl FUN_0056fea0(float *param_1,float *param_2,float *param_3,float *param_4); // K10
extern "C" void  __cdecl FUN_0056f1f0(int *param_1,undefined4 *param_2); // K9
extern "C" void  __cdecl FUN_0056f0a0(int param_1);                      // K9
extern "C" float10 __cdecl FUN_005667c0(float *param_1,float *param_2);  // K1

// ---------------------------------------------------------------------------
// 0x00570090
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00570090(int param_1,int param_2,float param_3,float param_4)
{
  // constraint-row output buffers (note B)
  float row0[27], row1[27], row2[27];
  // helper-I/O frames (note C)
  float qA[4], qB[4], qp[4], mC[12], mA[15], mB[15], rel[3], nrm[3];
  float *Apos, *Bpos;                       // fb90 body-position pointers (note D)
  float  local_ec[3];
  int    iVar10, iVar12, iVar14;
  int    local_2e4, local_2e8, local_31c;
  uint   uVar3, uVar11;

  param_3 = param_3 * param_4;
  local_31c = 0;
  local_2e4 = (int)FUN_0056fb90((uint*)(param_2 + 0x50),
                                (float*)(*(int*)(param_2 + 0x4c) + 4),
                                qA, (int*)&Apos, mA);
  local_2e8 = (int)FUN_0056fb90((uint*)(param_2 + 0x58),
                                (float*)(*(int*)(param_2 + 0x4c) + 0x20),
                                qB, (int*)&Bpos, mB);

  // lever arms: bodyA-center - contact (Apos), bodyB-center - contact (Bpos), and A-center - B-center
  float local_2dc = mA[12] - *Apos;
  float local_2d8 = mA[13] - Apos[1];
  float local_2d4 = mA[14] - Apos[2];
  float local_2d0 = mA[12] - *Bpos;
  float local_2cc = mA[13] - Bpos[1];
  float local_2c8 = mA[14] - Bpos[2];
  rel[0] = mA[12] - mB[12];
  rel[1] = mA[13] - mB[13];
  rel[2] = mA[14] - mB[14];

  *(undefined4*)(*(int*)(param_1 + 0xb8) + *(int*)(param_1 + 0xf8) * 4) = 0;
  *(int*)(param_1 + 0xbc) = *(int*)(param_1 + 0xbc) + 1;
  if (local_2e4 == 0) iVar10 = -1; else iVar10 = (int)*(short*)(local_2e4 + 0x20);
  *(int*)(*(int*)(param_1 + 0xac) + *(int*)(param_1 + 0xf8) * 8) = iVar10;
  if (local_2e8 == 0) iVar10 = -1; else iVar10 = (int)*(short*)(local_2e8 + 0x20);
  *(int*)(*(int*)(param_1 + 0xac) + 4 + *(int*)(param_1 + 0xf8) * 8) = iVar10;

  FUN_0056fea0(qA, qB, qp, mC);
  // copy fea0 outputs into readable named locals (not re-passed to any helper)
  float local_2fc = qp[0], local_2f8 = qp[1], local_2f4 = qp[2], local_2f0 = qp[3];
  float local_218 = mC[0],  local_214 = mC[1],  local_210 = mC[2],  local_20c = mC[3];
  float local_208 = mC[4],  local_204 = mC[5],  local_200 = mC[6],  local_1fc = mC[7];
  float local_1f8 = mC[8],  local_1f4 = mC[9],  local_1f0 = mC[10], local_1ec = mC[11];

  // expand the orientation quaternion (qp) into the 3x3 used to rotate contact tangents
  float local_2a0f = local_2fc * local_2fc;           // (float)local_2a0, live to decomp-1018
  float local_228  = local_2f8 * local_2f8;
  float local_dc   = local_2f4 * local_2f4;
  float local_2b0  = local_2f0 * local_2fc;           // qB-slot scratch reuse
  float local_2ac  = local_2f0 * local_2f8;
  float local_11c  = _DAT_005cc320 - (local_dc + local_228 + local_dc + local_228);
  float fVar1, fVar2, fVar4, fVar5, fVar6, fVar7, fVar8, fVar9;
  fVar1 = local_2f0 * local_2f4 + local_2f8 * local_2fc; float local_118 = fVar1 + fVar1;
  fVar1 = local_2f4 * local_2fc - local_2ac;            float local_114 = fVar1 + fVar1;
  fVar1 = local_2f8 * local_2fc - local_2f0 * local_2f4; float local_10c = fVar1 + fVar1;
  float local_108 = _DAT_005cc320 - (local_dc + local_2a0f + local_dc + local_2a0f);
  fVar1 = local_2b0 + local_2f4 * local_2f8; float local_104 = fVar1 + fVar1;
  fVar1 = local_2ac + local_2f4 * local_2fc; float local_fc = fVar1 + fVar1;
  fVar1 = local_2f4 * local_2f8 - local_2b0; float local_f8 = fVar1 + fVar1;
  float local_f4 = _DAT_005cc320 - (local_228 + local_2a0f + local_228 + local_2a0f);
  (void)local_118; (void)local_114; (void)local_108; (void)local_104; (void)local_f8; (void)local_f4;

  local_ec[0] = mB[2] * rel[2] + mB[0] * rel[0] + mB[1] * rel[1];
  local_ec[1] = mB[6] * rel[2] + mB[4] * rel[0] + mB[5] * rel[1];
  local_ec[2] = mB[10] * rel[2] + mB[8] * rel[0] + mB[9] * rel[1];

  // ================= loop 1: the 3 rotational-DOF rows (config flags bit 3/4/5) =================
  {
    float *pfVar13 = &mB[1];       // local_1dc + 1
    uint  mask1 = 8;               // local_300 = 1.12104e-44f == int-bits 8
    int   idx   = 0;               // local_318
    do {
      if ((*(uint*)(*(int*)(param_2 + 0x4c) + 0x3c) & mask1) != 0) {
        fVar1 = local_ec[idx];
        LOAD_ROW(row0);
        if (local_2e4 != 0) {
          row0[0] = -pfVar13[-1];
          row0[1] = -pfVar13[0];
          row0[2] = -pfVar13[1];
          row0[4] = pfVar13[0] * local_2d4 - pfVar13[1] * local_2d8;
          row0[5] = pfVar13[1] * local_2dc - pfVar13[-1] * local_2d4;
          row0[6] = pfVar13[-1] * local_2d8 - pfVar13[0] * local_2dc;
        }
        if (local_2e8 != 0) {
          row0[8]  = pfVar13[-1];
          row0[12] = pfVar13[1] * local_2cc - pfVar13[0] * local_2c8;
          row0[9]  = pfVar13[0];
          row0[10] = pfVar13[1];
          row0[13] = pfVar13[-1] * local_2c8 - pfVar13[1] * local_2d0;
          row0[14] = pfVar13[0] * local_2d0 - pfVar13[-1] * local_2cc;
        }
        row0[23] =  3.4028235e+38f;   // (float)fVar19
        row0[22] = -3.4028235e+38f;   // (float)fVar20
        row0[18] = 0.0f;
        row0[19] = 0.0f;
        row0[21] =  3.4028235e+38f;   // (float)fVar19
        row0[24] = 0.8f;              // 0x3f4ccccd
        row0[20] = -3.4028235e+38f;   // (float)fVar20
        row0[16] = -fVar1;
        FUN_0056f1f0((int*)param_1, (undefined4*)row0);
        local_31c = local_31c + 1;
      }
      idx = idx + 1;
      pfVar13 = pfVar13 + 4;
      mask1 = mask1 * 2;
    } while ((uint)idx < 3);
  }

  // ================= linear-DOF constraint rows (config flags bit 0 / 1,2 / 6) =================
  if ((*(byte*)(*(int*)(param_2 + 0x4c) + 0x3c) & 1) != 0) {
    LOAD_ROW(row0);
    if (local_2e4 != 0) { row0[4] = local_20c;  row0[5] = local_208;  row0[6] = local_204; }
    if (local_2e8 != 0) { row0[12] = -local_20c; row0[13] = -local_208; row0[14] = -local_204; }
    row0[23] =  3.4028235e+38f;
    row0[22] = -3.4028235e+38f;
    row0[18] = 0.0f;
    row0[19] = 0.0f;
    row0[21] =  3.4028235e+38f;
    row0[24] = 0.8f;
    row0[20] = -3.4028235e+38f;
    row0[16] = -local_2fc;
    FUN_0056f1f0((int*)param_1, (undefined4*)row0);
    local_31c = local_31c + 1;
  }
  uVar11 = *(uint*)(*(int*)(param_2 + 0x4c) + 0x3c) & 6;
  if (uVar11 == 2) {
    fVar1 = mA[1] * mB[6];
    fVar2 = mA[2] * mB[5];
    LOAD_ROW(row0);
    fVar1 = fVar1 - fVar2;
    row0[16] = local_10c;
    fVar2 = mA[2] * mB[4] - mA[0] * mB[6];
    fVar4 = mA[0] * mB[5] - mA[1] * mB[4];
    if (local_2e4 != 0) { row0[4] = fVar1;  row0[5] = fVar2;  row0[6] = fVar4; }
    if (local_2e8 != 0) { row0[12] = -fVar1; row0[13] = -fVar2; row0[14] = -fVar4; }
    row0[23] =  3.4028235e+38f;
    row0[20] = -3.4028235e+38f;
    row0[18] = 0.0f;
    row0[22] = -3.4028235e+38f;
    row0[19] = 0.0f;
    row0[24] = 0.8f;
    row0[21] =  3.4028235e+38f;
    FUN_0056f1f0((int*)param_1, (undefined4*)row0);
    local_31c = local_31c + 1;
  }
  else if (uVar11 == 4) {
    fVar1 = mA[1] * mB[10];
    fVar2 = mA[2] * mB[9];
    LOAD_ROW(row0);
    fVar1 = fVar1 - fVar2;
    row0[16] = local_fc;
    fVar2 = mA[2] * mB[8] - mA[0] * mB[10];
    fVar4 = mA[0] * mB[9] - mA[1] * mB[8];
    if (local_2e4 != 0) { row0[4] = fVar1;  row0[5] = fVar2;  row0[6] = fVar4; }
    if (local_2e8 != 0) { row0[12] = -fVar1; row0[13] = -fVar2; row0[14] = -fVar4; }
    row0[23] =  3.4028235e+38f;
    row0[20] = -3.4028235e+38f;
    row0[18] = 0.0f;
    row0[22] = -3.4028235e+38f;
    row0[19] = 0.0f;
    row0[24] = 0.8f;
    row0[21] =  3.4028235e+38f;
    FUN_0056f1f0((int*)param_1, (undefined4*)row0);
    local_31c = local_31c + 1;
  }
  else if (uVar11 == 6) {
    LOAD_ROW(row0);
    LOAD_ROW(row1);
    if (local_2e4 != 0) {
      row0[4] = local_200; row0[5] = local_1fc; row0[6] = local_1f8;
      row1[4] = local_1f4; row1[5] = local_1f0; row1[6] = local_1ec;
    }
    if (local_2e8 != 0) {
      row0[12] = -local_200; row0[13] = -local_1fc; row0[14] = -local_1f8;
      row1[12] = -local_1f4; row1[13] = -local_1f0; row1[14] = -local_1ec;
    }
    row0[23] =  3.4028235e+38f;
    row0[22] = -3.4028235e+38f;
    row1[20] = -3.4028235e+38f;   // 0xff7fffff
    row0[18] = 0.0f;
    row0[21] =  3.4028235e+38f;
    row0[19] = 0.0f;
    row0[24] = 0.8f;
    row0[20] = -3.4028235e+38f;
    row1[18] = 0.0f;
    row1[19] = 0.0f;
    row1[23] =  3.4028235e+38f;
    row1[24] = 0.8f;              // 0x3f4ccccd
    row1[22] = -3.4028235e+38f;
    row0[16] = -local_2f8;
    row1[16] = -local_2f4;
    FUN_0056f1f0((int*)param_1, (undefined4*)row0);
    FUN_0056f1f0((int*)param_1, (undefined4*)row1);
    local_31c = local_31c + 2;
  }

  // ================= penetration / distance-limit row (uses swing magnitude) =================
  {
    fVar1 = (float)FUN_005667c0(nrm, rel);     // real float10 magnitude
    float local_224 = nrm[0], local_220 = nrm[1], local_21c = nrm[2];
    iVar10 = *(int*)(param_2 + 0x4c);
    if (((*(byte*)(iVar10 + 0x40) & 0x38) == 0) ||
        (fVar2 = fVar1 - CF(0x44), fVar2 <= DAT_005d757c)) {
      if (((*(byte*)(iVar10 + 0x54) & 0x38) != 0) &&
          (fVar2 = fVar1 - CF(0x58), DAT_005d757c < fVar2)) {
        iVar14 = 1;
        goto LAB_00570b99;
      }
      goto LAB_00570e2d;
    }
    iVar14 = 2;
  LAB_00570b99:
    {
      LOAD_ROW(row0);
      float local_2bc = local_224 * local_2d4 - local_21c * local_2dc;
      float local_2b8 = local_220 * local_2dc - local_224 * local_2d8;
      local_2b0        = local_21c * local_2cc - local_220 * local_2c8;
      local_2ac        = local_224 * local_2c8 - local_21c * local_2d0;
      float local_2a8  = local_220 * local_2d0 - local_224 * local_2cc;
      if (local_2e4 != 0) {
        row0[0] = -local_224;
        row0[1] = -local_220;
        row0[2] = -local_21c;
        row0[4] = -(local_21c * local_2d8 - local_220 * local_2d4);
        row0[5] = -local_2bc;
        row0[6] = -local_2b8;
      }
      if (local_2e8 != 0) {
        row0[8]  = local_224;
        row0[9]  = local_220;
        row0[10] = local_21c;
        row0[12] = local_2b0;
        row0[13] = local_2ac;
        row0[14] = local_2a8;
      }
      if ((((iVar14 != 2) || ((*(byte*)(iVar10 + 0x54) & 0x38) == 0)) ||
           (fVar1 - CF(0x58) <= DAT_005d757c)) ||
          (*(int*)(iVar10 + 0x68) == 0x7f7fffff)) {
        row0[22] = 0.0f;
      } else {
        row0[22] = (CF(0x44) - CF(0x58)) * CF(0x68);
      }
      row0[23] = 3.4028235e+38f;
      row0[20] = 0.0f;
      row0[21] = 3.4028235e+38f;
      row0[16] = -fVar2;
      if (iVar14 == 1) {
        fVar1 = param_4 * CF(0x68) + CF(0x6c);
        if (fVar1 <= DAT_005d757c) goto LAB_00570e2d;
        fVar1 = _DAT_005cc320 / fVar1;
        if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
        else                                  row0[18] = param_3 * _DAT_005e57dc;
        row0[19] = fVar1 * param_4 * CF(0x68);
      }
      row0[24] = 0.8f;
      FUN_0056f1f0((int*)param_1, (undefined4*)row0);
      local_31c = local_31c + 1;
    }
  }
LAB_00570e2d:

  // ================= loop 2: the 3 motor/limit rows (config bit clear + 0x8c walk) =================
  {
    float *pfVar13 = &mB[1];              // local_1dc + 1
    float *local_304 = (float*)(param_2 + 0x7c);
    uVar11 = 8;
    int idx = 0;                          // local_318
    do {
      if (((*(uint*)(*(int*)(param_2 + 0x4c) + 0x3c) & uVar11) == 0) &&
          (uVar3 = *(uint*)(*(int*)(param_2 + 0x4c) + 0x8c), (uVar3 & uVar11 * 0x101) != 0)) {
        LOAD_ROW(row0);
        if ((uVar11 & uVar3) != 0)        row0[16] = local_304[-3] - local_ec[idx];
        if ((uVar3 & uVar11 << 8) != 0)   row0[17] = -local_304[0];
        if ((uVar3 & uVar11 << 0x10) == 0) { row0[23] = 3.4028235e+38f; row0[22] = -3.4028235e+38f; }
        else {
          row0[23] =  *(float*)(*(int*)(param_2 + 0x4c) + 0x98);
          row0[22] = -*(float*)(*(int*)(param_2 + 0x4c) + 0x98);
        }
        if (local_2e4 != 0) {
          row0[0] = -pfVar13[-1];
          row0[1] = -pfVar13[0];
          row0[2] = -pfVar13[1];
          row0[4] = pfVar13[0] * local_2d4 - pfVar13[1] * local_2d8;
          row0[5] = pfVar13[1] * local_2dc - pfVar13[-1] * local_2d4;
          row0[6] = pfVar13[-1] * local_2d8 - pfVar13[0] * local_2dc;
        }
        if (local_2e8 != 0) {
          row0[8]  = pfVar13[-1];
          row0[12] = pfVar13[1] * local_2cc - pfVar13[0] * local_2c8;
          row0[9]  = pfVar13[0];
          row0[10] = pfVar13[1];
          row0[13] = pfVar13[-1] * local_2c8 - pfVar13[1] * local_2d0;
          row0[14] = pfVar13[0] * local_2d0 - pfVar13[-1] * local_2cc;
        }
        iVar10 = *(int*)(param_2 + 0x4c);
        fVar1 = param_4 * CF(0x90) + CF(0x94);
        if (DAT_005d757c < fVar1) {
          fVar1 = _DAT_005cc320 / fVar1;
          if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
          else                                  row0[18] = param_3 * _DAT_005e57dc;
          row0[24] = 0.8f;
          row0[19] = fVar1 * param_4 * CF(0x90);
          FUN_0056f1f0((int*)param_1, (undefined4*)row0);
          local_31c = local_31c + 1;
        }
      }
      idx = idx + 1;
      local_304 = local_304 + 1;
      pfVar13 = pfVar13 + 4;
      uVar11 = uVar11 * 2;
    } while ((uint)idx < 3);
  }

  // ================= angular motor / cone block (config 0x8c mask 0x707) =================
  iVar10 = *(int*)(param_2 + 0x4c);
  uVar11 = *(uint*)(iVar10 + 0x8c);
  if ((uVar11 & 0x707) != 0) {
    float local_2a4 = ((-(PB(0x60) * PB(0x8c)) - PB(0x64) * PB(0x90)) - PB(0x68) * PB(0x94)) * _DAT_005cc32c;
    local_2b0       = ((PB(0x8c) * PB(0x6c) + PB(0x68) * PB(0x90)) - PB(0x64) * PB(0x94)) * _DAT_005cc32c;
    local_2ac       = (PB(0x60) * PB(0x94) + (PB(0x90) * PB(0x6c) - PB(0x68) * PB(0x8c))) * _DAT_005cc32c;
    float local_2a8 = (PB(0x94) * PB(0x6c) + (PB(0x64) * PB(0x8c) - PB(0x60) * PB(0x90))) * _DAT_005cc32c;
    if (((*(uint*)(iVar10 + 0x3c) & 7) == 0) && ((uVar11 & 0x7000000) != 0)) {
      // ---- block A: full 3-row angular motor ----
      fVar1 = PB(0x6c);
      float local_2c0 = PB(0x6c) * local_2fc + PB(0x60) * local_2f0 + (PB(0x68) * local_2f8 - PB(0x64) * local_2f4);
      float local_2bc = PB(0x6c) * local_2f8 + PB(0x64) * local_2f0 + (PB(0x60) * local_2f4 - PB(0x68) * local_2fc);
      float local_2b8 = PB(0x6c) * local_2f4 + PB(0x68) * local_2f0 + (PB(0x64) * local_2fc - PB(0x60) * local_2f8);
      float local_2d0b = local_2a4 * local_2fc + local_2b0 * local_2f0 + (local_2a8 * local_2f8 - local_2ac * local_2f4);
      float local_2ccb = local_2a4 * local_2f8 + local_2ac * local_2f0 + (local_2b0 * local_2f4 - local_2a8 * local_2fc);
      float local_2c8b = local_2a4 * local_2f4 + local_2a8 * local_2f0 + (local_2ac * local_2fc - local_2b0 * local_2f8);
      float local_15c = (local_200 * PB(0x68) + local_218 * PB(0x60) + local_20c * fVar1) - local_1f4 * PB(0x64);
      float local_158 = (local_1fc * PB(0x68) + local_214 * PB(0x60) + local_208 * fVar1) - local_1f0 * PB(0x64);
      float local_154 = (local_1f8 * PB(0x68) + local_210 * PB(0x60) + local_204 * fVar1) - local_1ec * PB(0x64);
      float local_150 = local_1f4 * PB(0x60) + fVar1 * local_200 + (local_218 * PB(0x64) - local_20c * PB(0x68));
      float local_14c = local_1f0 * PB(0x60) + fVar1 * local_1fc + (local_214 * PB(0x64) - local_208 * PB(0x68));
      float local_148 = local_1ec * PB(0x60) + fVar1 * local_1f8 + (local_210 * PB(0x64) - local_204 * PB(0x68));
      float local_144 = fVar1 * local_1f4 + ((local_218 * PB(0x68) + local_20c * PB(0x64)) - local_200 * PB(0x60));
      float local_140 = fVar1 * local_1f0 + ((local_214 * PB(0x68) + local_208 * PB(0x64)) - local_1fc * PB(0x60));
      fVar4 = local_204 * PB(0x64);
      fVar5 = local_210 * PB(0x68);
      fVar2 = PB(0x60);
      LOAD_ROW(row0);
      float local_13c = fVar1 * local_1ec + ((fVar5 + fVar4) - local_1f8 * fVar2);
      LOAD_ROW(row1);
      LOAD_ROW(row2);
      if ((uVar11 & 7) != 0)     { row0[16] = -local_2c0; row1[16] = -local_2bc; row2[16] = -local_2b8; }
      if ((uVar11 & 0x700) != 0) { row0[17] = local_2d0b; row1[17] = local_2ccb; row2[17] = local_2c8b; }
      if ((uVar11 & 0x70000) == 0) { row2[22] = -3.4028235e+38f; row2[23] = 3.4028235e+38f; }
      else                         { row2[23] = CF(0xb0);        row2[22] = -CF(0xb0); }
      row0[20] = 0.0f;
      row0[21] = 0.0f;
      row1[20] = 0.0f;   // 0
      row1[21] = 0.0f;
      row2[20] = 0.0f;   // 0
      row2[21] = 0.0f;   // 0
      if (local_2e4 != 0) {
        row0[4] = local_15c; row0[5] = local_158; row0[6] = local_154;
        row1[4] = local_150; row1[5] = local_14c; row1[6] = local_148;
        row2[4] = local_144; row2[5] = local_140; row2[6] = local_13c;
      }
      if (local_2e8 != 0) {
        row0[12] = -local_15c; row0[13] = -local_158; row0[14] = -local_154;
        row1[12] = -local_150; row1[13] = -local_14c; row1[14] = -local_148;
        row2[12] = -local_144; row2[13] = -local_140; row2[14] = -local_13c;
      }
      fVar1 = param_4 * CF(0xa8) + CF(0xac);
      row0[22] = row2[22];
      row0[23] = row2[23];
      row1[22] = row2[22];
      row1[23] = row2[23];
      if (fVar1 <= DAT_005d757c) {
        row0[21] = 0.0f;   // LAB_005721a5
        row0[20] = 0.0f;
      } else {
        fVar1 = _DAT_005cc320 / fVar1;
        if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
        else                                  row0[18] = param_3 * _DAT_005e57dc;
        row0[24] = 0.8f;
        row1[24] = 0.8f;
        row0[19] = fVar1 * CF(0xa8) * param_4;
        row2[24] = 0.8f;
        row1[18] = row0[18];
        row1[19] = row0[19];
        row2[18] = row0[18];
        row2[19] = row0[19];
        FUN_0056f1f0((int*)param_1, (undefined4*)row0);
        FUN_0056f1f0((int*)param_1, (undefined4*)row1);
        FUN_0056f1f0((int*)param_1, (undefined4*)row2);
        local_31c = local_31c + 3;
      }
    }
    else {
      // ---- block B: twist row + two swing-cone rows ----
      if (((*(uint*)(iVar10 + 0x3c) & 1) == 0) && ((uVar11 & 0x101) != 0)) {
        fVar1 = local_11c + _DAT_005cc320;
        LOAD_ROW(row0);
        fVar1 = _DAT_005cc320 / sqrtf(fVar1 * _DAT_005cc32c);
        fVar5 = _DAT_005cc320 / sqrtf(PB(0x60) * PB(0x60) + PB(0x6c) * PB(0x6c));
        fVar2 = fVar5 * fVar1;
        fVar4 = (local_2f0 * PB(0x6c) - local_2fc * PB(0x60)) * fVar2;
        if ((uVar11 & 1) != 0)     row0[16] = (local_2fc * PB(0x6c) + local_2f0 * PB(0x60)) * fVar2;
        if ((uVar11 & 0x100) != 0) row0[17] = -((local_2b0 * PB(0x6c) - local_2a4 * PB(0x60)) * fVar4 * fVar5);
        if ((uVar11 & 0x10000) == 0) { row0[22] = -3.4028235e+38f; row0[23] = 3.4028235e+38f; }
        else                         { row0[23] = CF(0xa4);        row0[22] = -CF(0xa4); }
        fVar4 = fVar4 * fVar1;
        fVar1 = local_2fc * fVar4;
        row0[20] = 0.0f;
        row0[21] = 0.0f;
        fVar2 = -(fVar4 * local_2f0);
        fVar5 = local_20c * fVar2 + local_218 * fVar1;
        fVar4 = local_208 * fVar2 + local_214 * fVar1;
        fVar1 = fVar2 * local_204 + fVar1 * local_210;
        if (local_2e4 != 0) { row0[4] = fVar5;  row0[5] = fVar4;  row0[6] = fVar1; }
        if (local_2e8 != 0) { row0[12] = -fVar5; row0[13] = -fVar4; row0[14] = -fVar1; }
        fVar1 = param_4 * CF(0x9c) + CF(0xa0);
        if (DAT_005d757c < fVar1) {
          fVar1 = _DAT_005cc320 / fVar1;
          if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
          else                                  row0[18] = param_3 * _DAT_005e57dc;
          row0[24] = 0.8f;
          row0[19] = fVar1 * CF(0x9c) * param_4;
          FUN_0056f1f0((int*)param_1, (undefined4*)row0);
          local_31c = local_31c + 1;
        }
      }
      iVar10 = *(int*)(param_2 + 0x4c);
      uVar11 = ~(*(uint*)(iVar10 + 0x3c) & 6);
      if ((uVar11 != 0) && ((*(uint*)(iVar10 + 0x8c) & 0x606) != 0)) {
        fVar1 = local_11c + _DAT_005cc320;
        float local_2e0 = PB(0x68);
        fVar2 = fVar1 + fVar1;
        fVar6 = (PB(0x60) * PB(0x60) + PB(0x6c) * PB(0x6c)) * _DAT_005cc35c;
        fVar4 = sqrtf(fVar2);
        fVar7 = sqrtf(fVar6);
        fVar5 = _DAT_005cc320 / (fVar4 + fVar1);
        fVar8 = fVar6 * _DAT_005cc32c;
        fVar9 = _DAT_005cc320 / (fVar7 + fVar8);
        float local_22c = (_DAT_005cc320 + fVar4) / ((fVar2 + fVar2 + (local_11c + _DAT_005cc31c) * fVar4) * fVar1);
        float local_230 = (fVar7 + _DAT_005cc320) / ((fVar6 + fVar6 + (fVar8 + _DAT_005cc574) * fVar7) * fVar8);
        fVar1 = PB(0x64);
        fVar4 = local_2e0 * local_2e0 + fVar1 * fVar1;
        float local_15c = _DAT_005cc320 - (fVar4 + fVar4);
        float local_158 = PB(0x6c) * local_2e0 + fVar1 * PB(0x60);
        local_158 = local_158 + local_158;
        float local_154 = local_2e0 * PB(0x60) - PB(0x6c) * fVar1;
        local_154 = local_154 + local_154;
        local_2b0 = local_154 * PB(0x90) - local_158 * PB(0x94);
        local_2ac = PB(0x94) * local_15c - local_154 * PB(0x8c);
        local_2a8 = local_158 * PB(0x8c) - PB(0x90) * local_15c;
        float local_2c0 = mA[1] * mB[2] - mA[2] * mB[1];
        float local_2bc = mA[2] * mB[0] - mA[0] * mB[2];
        float local_2b8 = mA[0] * mB[1] - mA[1] * mB[0];
        local_2d0        = mA[1] * mB[6] - mA[2] * mB[5];
        local_2cc        = mA[2] * mB[4] - mA[0] * mB[6];
        local_2c8        = mA[0] * mB[5] - mA[1] * mB[4];
        local_2dc        = mA[1] * mB[10] - mA[2] * mB[9];
        local_2d8        = mA[2] * mB[8]  - mA[0] * mB[10];
        local_2d4        = mA[0] * mB[9]  - mA[1] * mB[8];
        if (((uVar11 & 2) != 0) && (_DAT_005cc328 < fVar2)) {
          uVar3 = *(uint*)(iVar10 + 0x8c);
          LOAD_ROW(row0);
          if ((uVar3 & 2) != 0)     row0[16] = fVar5 * local_10c - local_158 * fVar9;
          if ((uVar3 & 0x200) != 0) row0[17] = local_2ac * fVar9 - local_2b0 * local_158 * local_230;
          fVar1 = local_22c * local_10c;
          local_2d0 = local_2c0 * fVar1 + local_2d0 * fVar5;
          local_2cc = local_2bc * fVar1 + local_2cc * fVar5;
          local_2c8 = local_2b8 * fVar1 + local_2c8 * fVar5;
          if (local_2e4 != 0) { row0[4] = local_2d0;  row0[5] = local_2cc;  row0[6] = local_2c8; }
          if (local_2e8 != 0) { row0[12] = -local_2d0; row0[13] = -local_2cc; row0[14] = -local_2c8; }
          if ((uVar3 & 0x20000) == 0) { row0[22] = -3.4028235e+38f; row0[23] = 3.4028235e+38f; }
          else                        { row0[23] = CF(0xb0);        row0[22] = -CF(0xb0); }
          row0[20] = 0.0f;
          row0[21] = 0.0f;
          fVar1 = param_4 * CF(0xa8) + CF(0xac);
          if (DAT_005d757c < fVar1) {
            fVar1 = _DAT_005cc320 / fVar1;
            if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
            else                                  row0[18] = param_3 * _DAT_005e57dc;
            row0[24] = 0.8f;
            row0[19] = fVar1 * CF(0xa8) * param_4;
            FUN_0056f1f0((int*)param_1, (undefined4*)row0);
            local_31c = local_31c + 1;
          }
        }
        if (((uVar11 & 4) != 0) && (_DAT_005cc328 < fVar2)) {
          LOAD_ROW(row0);
          iVar10 = *(int*)(param_2 + 0x4c);
          uVar11 = *(uint*)(iVar10 + 0x8c);
          if ((uVar11 & 4) != 0)     row0[16] = fVar5 * local_fc - local_154 * fVar9;
          if ((uVar11 & 0x400) != 0) row0[17] = local_2a8 * fVar9 - local_2b0 * local_154 * local_230;
          fVar1 = local_22c * local_fc;
          local_2dc = local_2c0 * fVar1 + local_2dc * fVar5;
          local_2d8 = local_2bc * fVar1 + local_2d8 * fVar5;
          local_2d4 = local_2b8 * fVar1 + local_2d4 * fVar5;
          if (local_2e4 != 0) { row0[4] = local_2dc;  row0[5] = local_2d8;  row0[6] = local_2d4; }
          if (local_2e8 != 0) { row0[12] = -local_2dc; row0[13] = -local_2d8; row0[14] = -local_2d4; }
          if ((uVar11 & 0x40000) == 0) { row0[22] = -3.4028235e+38f; row0[23] = 3.4028235e+38f; }
          else                         { row0[23] = CF(0xb0);        row0[22] = -CF(0xb0); }
          row0[20] = 0.0f;
          row0[21] = 0.0f;
          fVar1 = param_4 * CF(0xa8) + CF(0xac);
          if (fVar1 <= DAT_005d757c) { row0[21] = 0.0f; row0[20] = 0.0f; }   // LAB_005721a5 inline
          else {
            fVar1 = _DAT_005cc320 / fVar1;
            if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
            else                                  row0[18] = param_3 * _DAT_005e57dc;
            row0[24] = 0.8f;
            row0[19] = fVar1 * CF(0xa8) * param_4;
            FUN_0056f1f0((int*)param_1, (undefined4*)row0);
            local_31c = local_31c + 1;
          }
        }
      }
    }
  }

  // ================= elliptic (0x48/0x5c) twist-limit row =================
  iVar10 = *(int*)(param_2 + 0x4c);
  fVar2 = CF(0x48) * CF(0x48);
  fVar1 = CF(0x5c) * CF(0x5c);
  float local_2ec_f = _DAT_005cc320 - fVar1;
  float f318 = 0.0f;
  if (((*(byte*)(iVar10 + 0x40) & 1) == 0) ||
      (f318 = local_2fc * fVar2 * local_2fc - (_DAT_005cc320 - fVar2) * local_2f0 * local_2f0,
       f318 <= DAT_005d757c)) {
    if (((*(byte*)(iVar10 + 0x54) & 1) != 0) &&
        (f318 = fVar1 * local_2fc * local_2fc - local_2ec_f * local_2f0 * local_2f0,
         DAT_005d757c < f318)) {
      iVar14 = 1;
      goto LAB_0057224e;
    }
  }
  else {
    iVar14 = 2;
    fVar1 = fVar2;
  LAB_0057224e:
    {
      fVar2 = (_DAT_005cc320 - fVar1) * local_2f0;
      LOAD_ROW(row0);
      fVar2 = fVar2 + fVar2;
      row0[16] = f318;
      fVar1 = fVar1 * local_2fc * _DAT_005cc34c;
      fVar5 = local_20c * fVar1 + local_218 * fVar2;
      fVar4 = local_208 * fVar1 + local_214 * fVar2;
      fVar1 = fVar1 * local_204 + fVar2 * local_210;
      if (local_2e4 != 0) { row0[4] = fVar5;  row0[5] = fVar4;  row0[6] = fVar1; }
      if (local_2e8 != 0) { row0[12] = -fVar5; row0[13] = -fVar4; row0[14] = -fVar1; }
      row0[22] = -3.4028235e+38f;
      row0[23] = 0.0f;
      if (iVar14 == 1) {
        fVar1 = param_4 * CF(0x74) + CF(0x78);
        if (fVar1 <= DAT_005d757c) goto LAB_0057240f;
        fVar1 = _DAT_005cc320 / fVar1;
        if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
        else                                  row0[18] = param_3 * _DAT_005e57dc;
        row0[20] = 0.0f;
        row0[19] = fVar1 * param_4 * CF(0x74);
      } else {
        row0[20] = -3.4028235e+38f;
      }
      row0[21] = 0.0f;
      row0[24] = 0.8f;
      FUN_0056f1f0((int*)param_1, (undefined4*)row0);
      local_31c = local_31c + 1;
    }
  }
LAB_0057240f:

  // ================= final swing-cone limit row (0x40/0x54 mask 6) =================
  iVar10 = *(int*)(param_2 + 0x4c);
  float local_2c0, local_2bc, local_2b8;   // hoisted (skipped by goto LAB_0057290f) — see C2362
  float local_300, local_320;
  if (_DAT_005ceae4 <= CF(0x60)) local_300 = 0.999999f; else local_300 = CF(0x60);
  fVar1 = _DAT_005ceae4;
  if (CF(0x64) < _DAT_005ceae4) fVar1 = CF(0x64);
  if (_DAT_005ceae4 <= CF(0x4c)) local_320 = 0.999999f; else local_320 = CF(0x4c);
  fVar2 = _DAT_005ceae4;
  if (CF(0x50) < _DAT_005ceae4) fVar2 = CF(0x50);
  fVar4 = sqrtf(local_2f0 * local_2f0 + local_2a0f);
  float local_2e0 = sqrtf(local_228 + local_dc);
  float local_230 = (_DAT_005cc320 + fVar4) * fVar4;
  local_230 = local_230 + local_230;
  local_230 = local_230 * local_230;
  fVar5 = _DAT_005cc320 / (_DAT_005cc320 + fVar4);
  fVar4 = local_2e0 * fVar4 + local_2e0 * fVar4;
  local_228 = ((local_320 + _DAT_005cc320) * local_10c) / (_DAT_005cc320 - local_320);
  fVar6 = ((local_300 + _DAT_005cc320) * local_10c) / (_DAT_005cc320 - local_300);
  float local_2a0v = ((_DAT_005cc320 + fVar2) * local_fc) / (_DAT_005cc320 - fVar2);   // local_2a0
  float pf13_f     = ((_DAT_005cc320 + fVar1) * local_fc) / (_DAT_005cc320 - fVar1);   // pfVar13
  float local_22c = local_228 * local_10c + local_2a0v * local_fc;
  fVar1 = fVar6 * local_10c + pf13_f * local_fc;
  if (((*(byte*)(iVar10 + 0x40) & 6) == 0) || (local_22c <= local_230)) {
    local_2ec_f = pf13_f;
    if (((*(byte*)(iVar10 + 0x54) & 6) == 0) || (fVar1 <= local_230)) goto LAB_0057290f;
    local_2ec_f = _DAT_005cc320 / fVar1;
    iVar14 = 1;
    fVar1 = sqrtf(local_2ec_f);
    fVar2 = fVar5 * local_2e0 - fVar4 * fVar1;
    local_2a0v = local_2ec_f;
    local_230 = fVar6;
    local_228 = fVar1;
  }
  else {
    local_2ec_f = _DAT_005cc320 / local_22c;
    iVar14 = 2;
    fVar1 = sqrtf(local_2ec_f);
    fVar2 = fVar5 * local_2e0 - fVar4 * fVar1;
    pf13_f = local_2a0v;
    local_230 = local_228;
    local_22c = fVar1;
  }
  local_2ec_f = local_2ec_f * fVar1 * fVar4;
  fVar4 = (fVar1 * local_11c - fVar5 * _DAT_005cc32c) / fVar4;
  fVar5 = local_2ec_f * local_230;
  fVar1 = local_2ec_f * pf13_f;
  fVar6 = mB[8] * fVar1 + mB[4] * fVar5 + mB[0] * fVar4;
  fVar7 = mB[9] * fVar1 + mB[5] * fVar5 + fVar4 * mB[1];
  fVar1 = mB[10] * fVar1 + mB[6] * fVar5 + fVar4 * mB[2];
  local_2c0 = mA[2] * fVar7 - mA[1] * fVar1;
  local_2bc = mA[0] * fVar1 - mA[2] * fVar6;
  local_2b8 = mA[1] * fVar6 - mA[0] * fVar7;
  LOAD_ROW(row0);
  row0[16] = -fVar2;
  if (local_2e4 != 0) { row0[4] = local_2c0;  row0[5] = local_2bc;  row0[6] = local_2b8; }
  if (local_2e8 != 0) { row0[12] = -local_2c0; row0[13] = -local_2bc; row0[14] = -local_2b8; }
  row0[22] = 0.0f;
  row0[23] = 3.4028235e+38f;
  if (iVar14 == 1) {
    fVar1 = param_4 * CF(0x80) + CF(0x84);
    if (fVar1 <= DAT_005d757c) goto LAB_0057290f;
    fVar1 = _DAT_005cc320 / fVar1;
    if (param_3 * _DAT_005cc56c <= fVar1) row0[18] = fVar1 - param_3;
    else                                  row0[18] = param_3 * _DAT_005e57dc;
    row0[21] = 0.0f;
    row0[19] = fVar1 * CF(0x80) * param_4;
  } else {
    row0[21] = 3.4028235e+38f;
  }
  row0[20] = 0.0f;
  row0[24] = 0.8f;
  FUN_0056f1f0((int*)param_1, (undefined4*)row0);
  local_31c = local_31c + 1;
LAB_0057290f:
  if (local_31c == 0) {
    LOAD_ROW(row0);
    row0[22] = -3.4028235e+38f;
    row0[23] =  3.4028235e+38f;
    row0[20] = 0.0f;
    row0[21] = 0.0f;
    row0[24] = 0.0f;   // 0
    FUN_0056f1f0((int*)param_1, (undefined4*)row0);
  }
  FUN_0056f0a0(param_1);
  (void)iVar12;
  return;
}

// --- gta-reversed-style hook registration — CLUSTER 12. ---
RH_ScopedInstall(FUN_00570090, 0x00570090);

}  // namespace Collision
}  // namespace mashed_re

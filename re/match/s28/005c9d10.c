// FLAGS: /O2 /Oy- /Ob1
/* MATCH-S28 h1 0x005c9d10 : 11.8% (regs 16, ops 45) with /O2 /Oy- /Ob1 (the original has an ebp frame; /O2 /Ob1 /G5 gives 9.0, /O2 /GS /Ob1 9.9). Not hand-matched: the raw converter text made compilable and C++-safe (ROUND/CARRY4/__aullshr rewritten: CARRY4 as an unsigned add compare, the __aullshr call as >> 0x20 of piVar5[0x19:0x18] (asm: eax=[esi+0x60], edx=[esi+0x64], ecx=0x20), the float quotient as (float)((long double)a / (long double)b); these three spellings are my reading, not byte-verified). Voice resampler with 64-bit fixed-point position (piVar5[0x18..0x19]), 16-bit linear interpolation in four-sample unrolled loops. Residual class (b), ran out of budget (1472 B). */

/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */

typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned __int64 undefined8;
typedef unsigned int uint;
typedef unsigned char byte;
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
typedef long double float10;
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
longlong __stdcall __allmul(uint p0, int p1, uint p2, int p3);
undefined8 __stdcall __aullrem(uint p0, uint p1, uint p2, uint p3);
#define CARRY4(a,b) ((unsigned int)((unsigned int)(a) + (unsigned int)(b)) < (unsigned int)(a))
// MATCH: 0x005c9d10 FUN_005c9d10
/* WARNING: Removing unreachable block (ram,0x005c9d97) */
/* [C2 2026-06-02] Voice resampler / sample-fetch stage that fills the source buffer the
   FUN_005c98d0 mixer then reads. `piVar5 =
 */

int FUN_005c9d10(int param_1)

{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  bool bVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  undefined4 *puVar18;
  short *psVar19;
  short *psVar20;
  bool bVar21;
  longlong lVar22;
  undefined8 uVar23;
  uint local_a4;
  int local_a0;
  uint local_74;
  int local_70;
  uint local_1c;
  uint local_10;
  short *local_c;
  
  piVar5 = *(int **)(param_1 + 0x20);
  if (*piVar5 != 4) {
    return 0;
  }
  iVar6 = piVar5[0x14];
  uVar13 = (uint)*(byte *)(piVar5[0x1c] + 0x39);
  psVar20 = (short *)piVar5[0x15];
  local_c = psVar20;
  for (local_1c = *(uint *)(piVar5[0x1d] + 0x28); local_1c != 0; local_1c = local_1c - local_10) {
    bVar10 = false;
    local_10 = local_1c;
    if (piVar5[0x16] != 0 || piVar5[0x17] != 0) {
      iVar15 = -piVar5[0x18];
      uVar11 = (piVar5[2] - piVar5[0x19]) - (uint)(piVar5[0x18] != 0);
      lVar22 = __aullrem(iVar15,uVar11,piVar5[0x16],piVar5[0x17]);
      uVar11 = (uint)(float)((long double)(longlong)CONCAT44(uVar11,iVar15) / (long double)*(longlong *)(piVar5 + 0x16));
      iVar15 = (int)uVar11 >> 0x1f;
      if (lVar22 != 0) {
        bVar21 = 0xfffffffe < uVar11;
        uVar11 = uVar11 + 1;
        iVar15 = iVar15 + (uint)bVar21;
      }
      local_10 = local_1c;
      if ((iVar15 == 0) && (local_10 = local_1c, uVar11 <= local_1c)) {
        bVar10 = true;
        local_10 = uVar11;
      }
    }
    if ((piVar5[0x11] == 0) && (piVar5[0x12] == 0)) {
      psVar20 = psVar20 + local_10 * uVar13;
      if (piVar5[0x17] == 0) {
        lVar22 = (ulonglong)(uint)piVar5[0x16] * (ulonglong)local_10;
      }
      else {
        lVar22 = (ulonglong)(uint)piVar5[0x16] * (ulonglong)local_10;
        lVar22 = CONCAT44((int)((ulonglong)lVar22 >> 0x20) + piVar5[0x17] * local_10,(int)lVar22);
      }
      local_74 = (uint)lVar22;
      uVar11 = piVar5[0x18];
      piVar5[0x18] = uVar11 + local_74;
      local_70 = (int)((ulonglong)lVar22 >> 0x20);
      piVar5[0x19] = piVar5[0x19] + local_70 + (uint)CARRY4(uVar11,local_74);
      local_c = psVar20;
    }
    else {
      uVar11 = piVar5[0x16];
      iVar15 = piVar5[0x17];
      if ((uVar11 == 0) && (iVar15 == 1)) {
        uVar11 = local_10 * uVar13 * 2;
        iVar15 = (int)(*(ulonglong *)(piVar5 + 0x18) >> 0x20);
        puVar18 = (undefined4 *)(iVar6 + iVar15 * uVar13 * 2);
        psVar19 = psVar20;
        for (uVar14 = uVar11 >> 2; uVar14 != 0; uVar14 = uVar14 - 1) {
          *(undefined4 *)psVar19 = *puVar18;
          puVar18 = puVar18 + 1;
          psVar19 = psVar19 + 2;
        }
        psVar20 = local_c + local_10 * uVar13;
        for (uVar11 = uVar11 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
          *(undefined1 *)psVar19 = *(undefined1 *)puVar18;
          puVar18 = (undefined4 *)((int)puVar18 + 1);
          psVar19 = (short *)((int)psVar19 + 1);
        }
        uVar23 = __allmul(local_10,0,0,1);
        uVar11 = piVar5[0x18];
        piVar5[0x18] = uVar11 + (uint)uVar23;
        piVar5[0x19] = piVar5[0x19] + (int)((ulonglong)uVar23 >> 0x20) +
                       (uint)CARRY4(uVar11,(uint)uVar23);
        local_c = psVar20;
      }
      else {
        uVar14 = local_10 >> 2;
        uVar7 = local_10;
        if (uVar13 == 1) {
          if (iVar15 == 0) {
            lVar22 = (ulonglong)uVar11 * 3;
          }
          else {
            lVar22 = CONCAT44((int)((ulonglong)uVar11 * 3 >> 0x20) + iVar15 * 3,
                              (int)((ulonglong)uVar11 * 3));
          }
          for (; uVar14 != 0; uVar14 = uVar14 - 1) {
            uVar7 = piVar5[0x18];
            iVar8 = piVar5[0x19];
            iVar12 = iVar8 + iVar15 + (uint)CARRY4(uVar7,uVar11);
            iVar16 = iVar8 + (iVar15 << 1 | uVar11 >> 0x1f) + (uint)CARRY4(uVar7,uVar11 * 2);
            local_a4 = (uint)lVar22;
            local_a0 = (int)((ulonglong)lVar22 >> 0x20);
            iVar17 = local_a0 + iVar8 + (uint)CARRY4(local_a4,uVar7);
            sVar1 = *(short *)(iVar6 + iVar8 * 2);
            sVar2 = *(short *)(iVar6 + iVar12 * 2);
            sVar3 = *(short *)(iVar6 + 2 + iVar12 * 2);
            *psVar20 = (short)((int)(((int)*(short *)(iVar6 + 2 + iVar8 * 2) - (int)sVar1) *
                                    (uVar7 >> 0x11)) >> 0xf) + sVar1;
            sVar1 = *(short *)(iVar6 + iVar16 * 2);
            sVar4 = *(short *)(iVar6 + 2 + iVar16 * 2);
            psVar20[1] = (short)((int)(((int)sVar3 - (int)sVar2) * (uVar7 + uVar11 >> 0x11)) >> 0xf)
                         + sVar2;
            sVar2 = *(short *)(iVar6 + iVar17 * 2);
            sVar3 = *(short *)(iVar6 + 2 + iVar17 * 2);
            psVar20[2] = (short)((int)(((int)sVar4 - (int)sVar1) * (uVar7 + uVar11 * 2 >> 0x11)) >>
                                0xf) + sVar1;
            uVar9 = piVar5[0x18];
            piVar5[0x18] = uVar9 + uVar11 * 4;
            piVar5[0x19] = piVar5[0x19] + (iVar15 << 2 | uVar11 >> 0x1e) +
                           (uint)CARRY4(uVar9,uVar11 * 4);
            local_c = psVar20 + 4;
            psVar20[3] = (short)((int)(((int)sVar3 - (int)sVar2) * (local_a4 + uVar7 >> 0x11)) >>
                                0xf) + sVar2;
            psVar20 = local_c;
          }
          for (uVar11 = local_10 & 3; uVar11 != 0; uVar11 = uVar11 - 1) {
            local_c = psVar20 + 1;
            sVar1 = *(short *)(iVar6 + piVar5[0x19] * 2);
            *psVar20 = (short)((int)(((int)*(short *)(iVar6 + 2 + piVar5[0x19] * 2) - (int)sVar1) *
                                    ((uint)piVar5[0x18] >> 0x11)) >> 0xf) + sVar1;
            uVar14 = piVar5[0x18];
            piVar5[0x18] = uVar14 + piVar5[0x16];
            piVar5[0x19] = piVar5[0x19] + piVar5[0x17] + (uint)CARRY4(uVar14,piVar5[0x16]);
            psVar20 = local_c;
          }
        }
        else {
          for (; uVar7 != 0; uVar7 = uVar7 - 1) {
            iVar15 = piVar5[0x19];
            uVar11 = piVar5[0x18];
            sVar1 = *(short *)(iVar6 + iVar15 * 4);
            sVar2 = *(short *)(iVar6 + 2 + iVar15 * 4);
            local_c = psVar20 + 2;
            sVar3 = *(short *)(iVar6 + 6 + iVar15 * 4);
            *psVar20 = (short)((int)(((int)*(short *)(iVar6 + 4 + iVar15 * 4) - (int)sVar1) *
                                    (uVar11 >> 0x11)) >> 0xf) + sVar1;
            psVar20[1] = (short)((int)(((int)sVar3 - (int)sVar2) * (uVar11 >> 0x11)) >> 0xf) + sVar2
            ;
            uVar11 = piVar5[0x18];
            piVar5[0x18] = uVar11 + piVar5[0x16];
            piVar5[0x19] = piVar5[0x19] + piVar5[0x17] + (uint)CARRY4(uVar11,piVar5[0x16]);
            psVar20 = local_c;
          }
        }
      }
    }
    if (bVar10) {
      if ((*(byte *)(piVar5 + 0x29) & 8) == 0) {
        *piVar5 = 1;
        local_1c = local_10;
      }
      else if ((uint)piVar5[2] <= (uint)piVar5[0x19]) {
        piVar5[0x18] = piVar5[0x18];
        piVar5[0x19] = piVar5[0x19] - (piVar5[2] - piVar5[1]);
      }
    }
  }
  return param_1;
}

// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00563c80: hand-matched (h1). 34.1% (regs 46.3%, ops 70.7%), 123/110 ins. Raw converter body (nearest-vertex hill climb), not worked past flags (/O2 /Ob1 /G5 best of 3). Class (b). */
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
typedef unsigned int uint;
typedef unsigned short ushort;
// MATCH: 0x00563c80 FUN_00563c80
void FUN_00563c80(int param_1,float *param_2,ushort param_3,float param_4,ushort *param_5)

{
  float *pfVar1;
  ushort uVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  float *pfVar6;
  ushort uVar7;
  ushort uVar8;
  ushort uVar9;
  ushort uVar10;
  ushort uVar11;
  ushort *puVar12;
  
  pfVar6 = param_2;
  iVar3 = *(int *)(param_1 + 4);
  uVar9 = *(ushort *)(param_1 + 0x1c) + 2;
  param_3 = -(ushort)(param_3 < *(ushort *)(param_1 + 0x1c)) & param_3;
  uVar8 = 0;
  pfVar1 = (float *)(iVar3 + (uint)param_3 * 0xc);
  fVar4 = pfVar1[2] * param_2[2] +
          *pfVar1 * *param_2 + *(float *)(iVar3 + 4 + (uint)param_3 * 0xc) * param_2[1];
  uVar10 = uVar9;
  uVar11 = param_3;
  if (uVar9 != 0) {
    do {
      if ((uVar10 == uVar11) || (param_4 < fVar4)) {
        *param_5 = uVar11;
        return;
      }
      uVar2 = *(ushort *)(*(int *)(param_1 + 8) + (uint)uVar11 * 2);
      uVar7 = *(short *)(*(int *)(param_1 + 8) + 2 + (uint)uVar11 * 2) - uVar2;
      param_3 = uVar11;
      if (uVar7 != 0) {
        param_2 = (float *)(uint)uVar7;
        puVar12 = (ushort *)(*(int *)(param_1 + 0x18) + (uint)uVar2 * 2);
        do {
          uVar2 = *(ushort *)(*(int *)(param_1 + 0x14) + 6 + (uint)*puVar12 * 0xc);
          if ((uVar2 != uVar10) &&
             (pfVar1 = (float *)(iVar3 + (uint)uVar2 * 0xc),
             fVar5 = pfVar1[2] * pfVar6[2] +
                     *pfVar1 * *pfVar6 + *(float *)(iVar3 + 4 + (uint)uVar2 * 0xc) * pfVar6[1],
             fVar4 < fVar5)) {
            fVar4 = fVar5;
            param_3 = uVar2;
          }
          puVar12 = puVar12 + 1;
          param_2 = (float *)((int)param_2 - 1);
        } while (param_2 != (float *)0x0);
      }
      uVar8 = uVar8 + 1;
      uVar10 = uVar11;
      uVar11 = param_3;
    } while (uVar8 < uVar9);
  }
  *param_5 = param_3;
  return;
}

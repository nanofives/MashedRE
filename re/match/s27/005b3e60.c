// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 h1: 43.6% (regs 50.9%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
int FUN_005b4060(int * p0, uint * p1);
uint FUN_005b4150(int p0, int p1, int p2);
// MATCH: 0x005b3e60 FUN_005b3e60
undefined4 FUN_005b3e60(int param_1,int param_2,int param_3,int param_4,int param_5,int *param_6)

{
  byte bVar4;
  byte *pbVar1;
  char *pcVar2;
  int iVar3;
  int iVar5;
  uint uVar8;
  undefined4 *puVar9;
  undefined4 uVar6;
  uint uVar7;
  undefined4 *puVar10;
  int local_4;
  
  if (param_6 == (int *)0x0) {
    param_6 = &local_4;
  }
  iVar5 = FUN_005b4150(param_1,param_3,param_4);
  *param_6 = iVar5;
  puVar10 = (undefined4 *)(param_2 + param_3);
  uVar8 = *(int *)(param_1 + 8) - param_3;
  uVar6 = FUN_005b4060((int *)param_1,(uint *)param_4);
  bVar4 = *(byte *)(param_1 + 0xc);
  uVar7 = uVar8 / (bVar4 >> 3);
  if ((param_5 != 0) && (puVar9 = (undefined4 *)(param_5 + *param_6), puVar9 != (undefined4 *)0x0))
  {
    if ((*(char *)(param_1 + 0xd) == '\x01') && (*(char *)(param_4 + 0xd) == '\x02')) {
      if (bVar4 == 8) {
        uVar8 = 0;
        if (uVar7 != 0) {
          do {
            *(undefined1 *)((int)puVar9 + 1) = *(undefined1 *)(uVar8 + (int)puVar10);
            *(undefined1 *)puVar9 = *(undefined1 *)(uVar8 + (int)puVar10);
            puVar9 = (undefined4 *)((int)puVar9 + 2);
            uVar8 = uVar8 + 1;
          } while (uVar8 < uVar7);
          return uVar6;
        }
      }
      else {
        uVar8 = 0;
        if (0 != uVar7) {
          do {
            puVar9 = puVar9 + 1;
            *(undefined2 *)puVar9 = *(undefined2 *)((int)puVar10 + uVar8 * 2);
            *(undefined2 *)((int)puVar9 + 2) = *(undefined2 *)((int)puVar10 + uVar8 * 2);
            uVar8 = uVar8 + 1;
          } while (uVar8 < uVar7);
          return uVar6;
        }
      }
    }
    else if (!(!(!((*(char *)(param_1 + 0xd) == '\x02') && (*(char *)(param_4 + 0xd) == '\x01'))))) {
      for (uVar7 = uVar8 >> 2; uVar7 != 0; uVar7 = uVar7 - 1) {
        *puVar9 = *puVar10;
        puVar10 = puVar10 + 1;
        puVar9 = puVar9 + 1;
      }
      for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
        *(undefined1 *)puVar9 = *(undefined1 *)puVar10;
        puVar10 = (undefined4 *)((int)puVar10 + 1);
        puVar9 = (undefined4 *)((int)puVar9 + 1);
      }
    } else {
      if (bVar4 == 8) {
        if (!((*(byte *)(param_1 + 0x18) & 1) == 0)) {
          uVar8 = 0;
          if (uVar7 != 0) {
            do {
              pbVar1 = (byte *)(uVar8 + (int)puVar10);
              iVar5 = uVar8 + 1;
              *(char *)puVar9 = (char)((uint)*pbVar1 + (uint)*(byte *)(iVar5 + (int)puVar10) >> 1);
              puVar9 = (undefined4 *)((int)puVar9 + 1);
              uVar8 = uVar8 + 2;
            } while (uVar8 < uVar7);
            return uVar6;
          }
        } else {
          uVar8 = 0;
          if (uVar7 != 0) {
            do {
              pcVar2 = (char *)(uVar8 + (int)puVar10);
              iVar5 = 1 + uVar8;
              uVar8 = uVar8 + 2;
              *(char *)puVar9 =
                   (char)((int)(short)((short)*pcVar2 + (short)*(char *)(iVar5 + (int)puVar10)) / 2)
              ;
              puVar9 = (undefined4 *)((int)puVar9 + 1);
            } while (uVar8 < uVar7);
            return uVar6;
          }
        }
      }
      else if (!((*(byte *)(param_1 + 0x18) & 1) == 0)) {
        uVar8 = 0;
        if (uVar7 != 0) {
          do {
            iVar5 = uVar8 * 2;
            iVar3 = uVar8 * 2;
            uVar8 = 2 + uVar8;
            *(short *)puVar9 =
                 (short)((uint)*(ushort *)((int)puVar10 + iVar5) +
                         (uint)*(ushort *)((int)puVar10 + iVar3 + 2) >> 1);
            puVar9 = (undefined4 *)((int)puVar9 + 2);
          } while (uVar8 < uVar7);
          return uVar6;
        }
      } else {
        uVar8 = 0;
        if (uVar7) {
          do {
            iVar3 = uVar8 * 2;
            uVar8 = uVar8 + 2;
            iVar5 = uVar8 * 2;
            *(short *)puVar9 =
                 (short)(((int)*(short *)((int)puVar10 + iVar5) +
                         (int)*(short *)((int)puVar10 + iVar3 + 2)) / 2);
            puVar9 = (undefined4 *)((int)puVar9 + 2);
          } while (uVar7 > uVar8);
          return uVar6;
        }
      }
    }
  }
  return uVar6;
}

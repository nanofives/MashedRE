// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f3e90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned int uint;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined4 FUN_004e8030(int * p0);
int * FUN_004e8120(undefined4 p0);
int FUN_004f3be0(int * p0, int p1);
// MATCH: 0x004f3e90 FUN_004f3e90
int * FUN_004f3e90(undefined4 param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int local_14;
  uint local_10;
  undefined1 local_c [4];
  undefined4 local_8;
  undefined4 local_4;
  
  iVar1 = FUN_004cc5e0(param_1,1,local_c,&local_10);
  if (iVar1 == 0) {
    return (int *)0x0;
  }
  if ((local_10 < 0x35000) || (0x37002 < local_10)) {
    local_8 = 2;
    local_4 = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&local_8);
    return (int *)0x0;
  }
  iVar2 = FUN_004cc790(param_1,&local_14,4);
  iVar1 = local_14;
  if (iVar2 == 0) {
    return (int *)0x0;
  }
  iVar2 = 0;
  param_2[2] = 0;
  *param_2 = 0;
  param_2[1] = 0;
  if (local_14 == 0) {
    return param_2;
  }
  if (local_14 < 1) {
LAB_004f3f9a:
    iVar1 = (**(code **)(DAT_007d3ff8 + 0x108))(local_14 * 4,0x10501);
    iVar4 = FUN_004cc790(param_1,iVar1,local_14 * 4);
    if (iVar4 != 0) {
      iVar2 = 0;
      if (0 < local_14) {
        do {
          iVar4 = *(int *)(iVar1 + iVar2 * 4);
          if (iVar4 < 0) {
            iVar4 = FUN_004cc5e0(param_1,7,0,&local_10);
            if (iVar4 == 0) {
              (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
              iVar1 = *param_2;
              if (iVar1 == 0) goto LAB_004f41b9;
              iVar2 = param_2[1];
              iVar4 = 0;
              if (0 < iVar2) {
                do {
                  FUN_004e8030(*(undefined4 *)(iVar1 + iVar4 * 4));
                  *(undefined4 *)(iVar1 + iVar4 * 4) = 0;
                  iVar4 = iVar4 + 1;
                } while (iVar4 < iVar2);
              }
              goto LAB_004f41a1;
            }
            if ((local_10 < 0x35000) || (0x37002 < local_10)) {
              local_8 = 2;
              local_4 = FUN_004d7ff0(0x80000004);
              FUN_004d8480(&local_8);
              (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
              iVar1 = *param_2;
              if (iVar1 == 0) goto LAB_004f41b9;
              iVar2 = param_2[1];
              iVar4 = 0;
              if (0 < iVar2) {
                do {
                  FUN_004e8030(*(undefined4 *)(iVar1 + iVar4 * 4));
                  *(undefined4 *)(iVar1 + iVar4 * 4) = 0;
                  iVar4 = iVar4 + 1;
                } while (iVar4 < iVar2);
              }
              goto LAB_004f41a1;
            }
            iVar4 = FUN_004e8120(param_1);
            if (iVar4 == 0) {
              (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
              iVar1 = *param_2;
              if (iVar1 != 0) {
                iVar2 = param_2[1];
                iVar4 = 0;
                if (0 < iVar2) {
                  do {
                    FUN_004e8030(*(undefined4 *)(iVar1 + iVar4 * 4));
                    *(undefined4 *)(iVar1 + iVar4 * 4) = 0;
                    iVar4 = iVar4 + 1;
                  } while (iVar4 < iVar2);
                }
                (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
                *param_2 = 0;
              }
              param_2[1] = 0;
              param_2[2] = 0;
              return (int *)0x0;
            }
          }
          else {
            iVar4 = *(int *)(*param_2 + iVar4 * 4);
            *(short *)(iVar4 + 0x18) = *(short *)(iVar4 + 0x18) + 1;
          }
          FUN_004f3be0(param_2,iVar4);
          FUN_004e8030(iVar4);
          iVar2 = iVar2 + 1;
        } while (iVar2 < local_14);
      }
      (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
      return param_2;
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
    iVar1 = *param_2;
    if (iVar1 == 0) goto LAB_004f41b9;
    iVar4 = param_2[1];
    if (0 < iVar4) {
      do {
        FUN_004e8030(*(undefined4 *)(iVar1 + iVar2 * 4));
        *(undefined4 *)(iVar1 + iVar2 * 4) = 0;
        iVar2 = iVar2 + 1;
      } while (iVar2 < iVar4);
    }
LAB_004f41a1:
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
  }
  else {
    iVar4 = local_14 * 4;
    iVar3 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar4,0x1030008);
    if (iVar3 != 0) {
      *param_2 = iVar3;
      param_2[2] = iVar1;
      goto LAB_004f3f9a;
    }
    local_8 = 2;
    local_4 = FUN_004d7ff0(0x80000013,iVar4);
    FUN_004d8480(&local_8);
    iVar1 = *param_2;
    if (iVar1 == 0) goto LAB_004f41b9;
    iVar4 = param_2[1];
    if (0 < iVar4) {
      do {
        FUN_004e8030(*(undefined4 *)(iVar1 + iVar2 * 4));
        *(undefined4 *)(iVar1 + iVar2 * 4) = 0;
        iVar2 = iVar2 + 1;
      } while (iVar2 < iVar4);
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(iVar1);
  }
  *param_2 = 0;
LAB_004f41b9:
  param_2[1] = 0;
  param_2[2] = 0;
  return (int *)0x0;
}

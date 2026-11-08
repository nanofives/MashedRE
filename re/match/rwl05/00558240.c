// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00558240: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
undefined4 FUN_004c7650(undefined4 p0);
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 * FUN_004cdca0(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004cdd00(uint * p0);
uint * FUN_004cdd60(uint * p0);
int FUN_004d5310(int p0, byte * p1);
undefined4 FUN_004d7ff0(undefined4 p0);
undefined4 * FUN_004d8480(undefined4 * p0);
undefined * FUN_00558180(undefined4 * p0, int * p1);
byte * FUN_00558400(byte * p0, int p1, byte * p2, int p3);
// MATCH: 0x00558240 FUN_00558240
int FUN_00558240(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int local_24;
  undefined4 local_20;
  undefined1 local_1c [8];
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  
  if ((param_1 == (undefined4 *)0x0) || (param_2 == (undefined4 *)0x0)) {
    local_24 = 0x180;
    local_20 = FUN_004d7ff0(0x80000016);
    FUN_004d8480(&local_24);
    return 0;
  }
  iVar4 = 0x10;
  do {
    iVar4 = iVar4 * 2;
  } while (iVar4 < ((iVar4 / 9 + 0x5a) / (iVar4 / 9)) * 0xf);
  iVar4 = FUN_004c77c0(iVar4,iVar4,0,4);
  if (iVar4 != 0) {
    iVar5 = 0x10;
    do {
      iVar5 = iVar5 * 2;
    } while (iVar5 < ((iVar5 / 9 + 0x5a) / (iVar5 / 9)) * 0xf);
    iVar5 = FUN_004cdca0(iVar5,iVar5,8);
    if (iVar5 != 0) {
      iVar2 = FUN_004cdd60(iVar5);
      iVar8 = 0;
      if (iVar2 != 0) {
        puVar1 = *(undefined4 **)(iVar5 + 0x18);
        *puVar1 = *param_2;
        puVar1[1] = *param_1;
        iVar8 = *(int *)(iVar5 + 0x14);
        iVar2 = *(int *)(iVar5 + 0x10);
        uVar3 = FUN_00558180(local_1c,&param_2);
        iVar7 = 0;
        param_2 = (undefined4 *)0x0;
        iVar6 = 0;
        if (0 < local_c) {
          do {
            local_24 = iVar6 + 1;
            uVar3 = FUN_00558400(uVar3,local_1c,
                                 local_14 * iVar7 + iVar8 + local_10 * (int)param_2 * iVar2,iVar2);
            iVar7 = iVar7 + 1;
            if (local_8 == iVar7) {
              iVar7 = 0;
              param_2 = (undefined4 *)((int)param_2 + 1);
            }
            iVar6 = local_24;
          } while (local_24 < local_c);
        }
        FUN_004d5310(iVar4,iVar5);
        iVar8 = iVar4;
      }
      FUN_004cdd00(iVar5);
      if (iVar8 != 0) {
        return iVar4;
      }
    }
    FUN_004c7650(iVar4);
  }
  return 0;
}

// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d9d00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
void FUN_004d9ee0(float * p0, float * p1, float * p2);
// MATCH: 0x004d9d00 FUN_004d9d00
void FUN_004d9d00(undefined4 *param_1,int *param_2,int param_3,int *param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  byte bVar5;
  uint uVar6;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  int local_8;
  int local_4;
  
  if (param_1 != (undefined4 *)0x0) {
    iVar1 = param_2[3];
    iVar2 = *param_2;
    iVar3 = param_2[1];
    iVar4 = param_2[2];
    local_4 = 1 << ((byte)param_3 & 0x1f);
    local_10 = local_4 + iVar2;
    local_c = local_4 + iVar3;
    local_8 = local_4 + iVar4;
    local_4 = local_4 + iVar1;
    if ((((local_10 != *param_4 && -1 < local_10 - *param_4) &&
         (local_c != param_4[1] && -1 < local_c - param_4[1])) &&
        (local_8 != param_4[2] && -1 < local_8 - param_4[2])) &&
       (((local_4 != param_4[3] && -1 < local_4 - param_4[3] && (iVar2 - param_4[4] < 0)) &&
        ((iVar3 - param_4[5] < 0 && ((iVar4 - param_4[6] < 0 && (iVar1 - param_4[7] < 0)))))))) {
      if ((-1 < iVar2 - *param_4) &&
         (((((-1 < iVar3 - param_4[1] && (-1 < iVar4 - param_4[2])) && (-1 < iVar1 - param_4[3])) &&
           ((-1 < param_4[4] - local_10 && (-1 < param_4[5] - local_c)))) &&
          ((-1 < param_4[6] - local_8 && (-1 < param_4[7] - local_4)))))) {
        FUN_004d9ee0(param_5,param_5,param_1);
        return;
      }
      if (0 < param_3) {
        uVar6 = 0;
        param_1 = (undefined4 *)((int)param_1 + 0x1c);
        do {
          bVar5 = (byte)(param_3 + -1);
          local_20 = (((int)uVar6 >> 3 & 1U) << (bVar5 & 0x1f)) + *param_2;
          local_1c = (((int)uVar6 >> 2 & 1U) << (bVar5 & 0x1f)) + param_2[1];
          local_18 = (((int)uVar6 >> 1 & 1U) << (bVar5 & 0x1f)) + param_2[2];
          local_14 = ((uVar6 & 1) << (bVar5 & 0x1f)) + param_2[3];
          FUN_004d9d00(*param_1,&local_20,param_3 + -1,param_4,param_5);
          uVar6 = uVar6 + 1;
          param_1 = param_1 + 1;
        } while ((int)uVar6 < 0x10);
      }
    }
  }
  return;
}

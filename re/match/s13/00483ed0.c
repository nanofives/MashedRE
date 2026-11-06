// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00483ed0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
void FUN_004c0740(int p0, int p1);
int FUN_004c0b30(void);
undefined4 FUN_004c0c20(int p0);
void FUN_004c15c0(int p0);
int FUN_004c1a40(int p0, undefined4 p1);
int FUN_004c1b10(int p0, undefined4 p1);
int FUN_004c1c10(int p0, int p1);
undefined1 * FUN_004c1d30(void);
undefined4 * FUN_004c5a00(undefined4 p0);
undefined4 FUN_004c7650(undefined4 p0);
int FUN_004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
// MATCH: 0x00483ed0 FUN_00483ed0
int FUN_00483ed0(int *param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar1 = FUN_004c77c0(param_2,param_3,0,param_4 | 5);
  if (iVar1 != 0) {
    iVar2 = FUN_004c77c0(param_2,param_3,0,1);
    if (iVar2 != 0) {
      iVar3 = FUN_004c0b30();
      if (iVar3 != 0) {
        iVar4 = FUN_004c1d30();
        if (iVar4 != 0) {
          FUN_004c1c10(iVar4,1);
          *(int *)(iVar4 + 0x60) = iVar1;
          *(int *)(iVar4 + 100) = iVar2;
          FUN_004c0740(iVar4,iVar3);
          FUN_004c15c0(iVar3);
          FUN_004c1a40(iVar4,0x3dcccccd);
          FUN_004c1b10(iVar4,0x43960000);
          iVar1 = FUN_004c5a00(iVar1);
          *(undefined1 *)(iVar1 + 0x50) = 2;
          *param_1 = iVar1;
          return iVar4;
        }
        FUN_004c0c20(iVar3);
      }
      FUN_004c7650(iVar2);
    }
    FUN_004c7650(iVar1);
  }
  return 0;
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047d330: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
MGLOBAL(undefined4, DAT_006c9a78, 0x006c9a78);
undefined4 FUN_004c0c20(int p0);
undefined4 FUN_004e43b0(undefined4 p0, int p1);
undefined4 FUN_004e4440(int p0);
undefined4 FUN_004e6920(int p0);
int FUN_004e7e30(int p0, undefined4 p1);
undefined4 * FUN_00559ee0(undefined4 * p0, int * p1);
undefined4 FUN_0057c210(int p0);
undefined4 FUN_0057c370(int p0);
// MATCH: 0x0047d330 FUN_0047d330
void FUN_0047d330(void)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = &DAT_006c9a78;
  do {
    if (*piVar2 != 0) {
      iVar1 = FUN_0057c210(*piVar2);
      if (iVar1 != 0) {
        if (*(int *)(iVar1 + 0x24) != 0) {
          FUN_00559ee0(*(int *)(iVar1 + 0x24),iVar1);
        }
        FUN_0057c370(iVar1);
      }
      iVar1 = *(int *)(*piVar2 + 4);
      if (iVar1 != 0) {
        FUN_004e7e30(*piVar2,0);
        FUN_004c0c20(iVar1);
      }
      iVar1 = FUN_004e4440(*piVar2);
      if (iVar1 != 0) {
        FUN_004e43b0(iVar1,*piVar2);
      }
      FUN_004e6920(*piVar2);
      *piVar2 = 0;
    }
    piVar2 = piVar2 + 1;
  } while ((int)piVar2 < 0x6c9a88);
  return;
}

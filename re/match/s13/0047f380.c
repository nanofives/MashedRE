// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f380: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined4, DAT_006ce274, 0x006ce274);
MGLOBAL(undefined4, DAT_006c6b90, 0x006c6b90);
MGLOBAL(undefined4, DAT_006c6eb8, 0x006c6eb8);
MGLOBAL(undefined4, DAT_006c71d8, 0x006c71d8);
void FUN_0047d330(void);
undefined4 FUN_004c0c20(int p0);
undefined4 FUN_004e43b0(undefined4 p0, int p1);
undefined4 FUN_004e4440(int p0);
undefined4 FUN_004e6920(int p0);
int FUN_004e7e30(int p0, undefined4 p1);
undefined4 * FUN_00559ee0(undefined4 * p0, int * p1);
undefined4 FUN_0055dec0(undefined4 * p0);
undefined4 FUN_0057c210(int p0);
undefined4 FUN_0057c370(int p0);
// MATCH: 0x0047f380 FUN_0047f380
void FUN_0047f380(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = FUN_0055dec0(DAT_006ce274);
  FUN_0047d330();
  iVar3 = 0;
  do {
    if (*(int *)((int)&DAT_006c71d8 + iVar3) != 0) {
      iVar2 = FUN_0057c210(*(int *)((int)&DAT_006c71d8 + iVar3));
      if (iVar2 != 0) {
        if ((iVar1 != 0) && (*(int *)(iVar2 + 0x24) == iVar1)) {
          FUN_00559ee0(iVar1,iVar2);
        }
        FUN_0057c370(iVar2);
      }
      iVar2 = FUN_004e4440(*(undefined4 *)((int)&DAT_006c71d8 + iVar3));
      if (iVar2 != 0) {
        FUN_004e43b0(iVar2,*(undefined4 *)((int)&DAT_006c71d8 + iVar3));
      }
      iVar2 = *(int *)(*(int *)((int)&DAT_006c71d8 + iVar3) + 4);
      if (iVar2 != 0) {
        FUN_004e7e30(*(int *)((int)&DAT_006c71d8 + iVar3),0);
        FUN_004c0c20(iVar2);
      }
      FUN_004e6920(*(undefined4 *)((int)&DAT_006c71d8 + iVar3));
    }
    *(undefined4 *)((int)&DAT_006c71d8 + iVar3) = 0;
    *(undefined4 *)((int)&DAT_006c6b90 + iVar3) = 0;
    *(undefined4 *)((int)&DAT_006c6eb8 + iVar3) = 1;
    iVar3 = iVar3 + 4;
  } while (iVar3 < 800);
  return;
}

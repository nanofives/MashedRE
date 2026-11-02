// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f290 (h7): 100% (regs 100%). Raw 85.3%: the range check is `>= 0 && <= 200` (test/jl, cmp 0xc8/jg). relocheck OK. */
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
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBAL(int, DAT_006c6eb0, 0x006c6eb0);
MGLOBALA(int, DAT_006c6b90, 201, 0x006c6b90);
MGLOBALA(int, DAT_006c6eb8, 201, 0x006c6eb8);
MGLOBALA(int, DAT_006c71d8, 201, 0x006c71d8);
void FUN_004c0c20(int p0);
void FUN_004e43b0(int p0, int p1);
int FUN_004e4440(int p0);
void FUN_004e6920(int p0);
void FUN_004e7e30(int p0, int p1);
void FUN_00559ee0(int p0, int p1);
int FUN_0055dec0(int p0);
int FUN_0057c210(int p0);
void FUN_0057c370(int p0);
// MATCH: 0x0047f290 FUN_0047f290
void FUN_0047f290(int param_1)
{
  int iVar1;
  int iVar2;

  iVar1 = FUN_0055dec0(DAT_006ce274);
  if ((param_1 >= 0) && (param_1 <= 200)) {
    if (DAT_006c71d8[param_1] != 0) {
      iVar2 = FUN_0057c210(DAT_006c71d8[param_1]);
      if (iVar2 != 0) {
        if ((iVar1 != 0) && (*(int *)(iVar2 + 0x24) == iVar1)) {
          FUN_00559ee0(iVar1, iVar2);
        }
        FUN_0057c370(iVar2);
      }
      iVar1 = FUN_004e4440(DAT_006c71d8[param_1]);
      if (iVar1 != 0) {
        FUN_004e43b0(iVar1, DAT_006c71d8[param_1]);
      }
      iVar1 = *(int *)(DAT_006c71d8[param_1] + 4);
      if (iVar1 != 0) {
        FUN_004e7e30(DAT_006c71d8[param_1], 0);
        FUN_004c0c20(iVar1);
      }
      FUN_004e6920(DAT_006c71d8[param_1]);
      DAT_006c6eb0 = DAT_006c6eb0 - 1;
    }
    DAT_006c71d8[param_1] = 0;
    DAT_006c6b90[param_1] = 0;
    DAT_006c6eb8[param_1] = 1;
  }
}

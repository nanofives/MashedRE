// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 64.9%. Same three fixes as 0x00558030. Supersedes the header below. */
/* MATCH-RWL05 0x00557ec0: hand-matched (h3). 64.9% (regs 75.3, ops 87.0). same lazy-init block as 0x00558030, same residue. The permuter variant that swapped piVar2[1]= with the +0x48 store was rejected (changes semantics). class (b). */
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
#pragma intrinsic(memset)
void *memset(void *, int, unsigned int);
typedef int (*Alloc)(int, int);
void FUN_00557a60(void);
void FUN_00557b70(void);
void FUN_004e8e90(int p0);
MGLOBAL(int, DAT_00913274, 0x00913274);
MGLOBAL(int, DAT_0091327c, 0x0091327c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005cc55c, 0x005cc55c);
void FUN_004e5f90(void);
void FUN_004e8ea0(int p0);
// MATCH: 0x00557ec0 FUN_00557ec0
int FUN_00557ec0(int param_1, int param_2, int param_3)
{
  int iVar1;
  int *piVar2;

  iVar1 = DAT_00913274;
  piVar2 = *(int **)(param_1 + iVar1);
  if (piVar2 == (int *)0) {
    piVar2 = (int *)(*(Alloc *)(DAT_007d3ff8 + 0x118))(DAT_0091327c, 0x30112);
    if (piVar2 != (int *)0) {
      int i;
      piVar2[3] = -0x40800000;
      piVar2[4] = -0x40800000;
      *piVar2 = 0;
      piVar2[1] = 0;
      piVar2[2] = (int)FUN_00557a60;
      piVar2[5] = -1;
      for (i = 0; i < 10; i++) piVar2[6 + i] = 0;
    }
    *(int **)(param_1 + iVar1) = piVar2;
    if (piVar2 != (int *)0) {
      iVar1 = *(int *)(param_1 + 0x18);
      if (iVar1 != 0) {
        FUN_004e8e90(iVar1);
      }
      piVar2[6] = iVar1;
    } else {
      return 0;
    }
  }
  if (param_3 != 0) {
    FUN_004e8e90(param_3);
  }
  if (piVar2[param_2 + 6] != 0) {
    FUN_004e8ea0(piVar2[param_2 + 6]);
  }
  piVar2[param_2 + 6] = param_3;
  if (*piVar2 == 0) {
    piVar2[1] = *(int *)(param_1 + 0x48);
    *(void (**)(void))(param_1 + 0x48) = FUN_00557b70;
    if (FUN_00557b70 == 0) {
      *(void (**)(void))(param_1 + 0x48) = FUN_004e5f90;
    }
    *piVar2 = 1;
  }
  return param_1;
}

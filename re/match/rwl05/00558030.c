// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 75.0%. Offset variable read first, if/else return shape, zeroing for loop instead of memset. Supersedes the header below. */
/* MATCH-RWL05 0x00558030: hand-matched (h3). 75.0% (regs 76.5, ops 85.3). memset intrinsic for the 10-dword clear, function-pointer null test (`mov ecx,0x557b70; test ecx,ecx`) kept; permuter swapped the DAT_00913274 load order. Residue: placement of the `return 0` block and table-offset register. class (b). */
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
// MATCH: 0x00558030 FUN_00558030
int FUN_00558030(int param_1, float param_2)
{
  int iVar1;
  float fVar2;
  int *piVar3;

  iVar1 = DAT_00913274;
  piVar3 = *(int **)(param_1 + iVar1);
  if (piVar3 == (int *)0) {
    piVar3 = (int *)(*(Alloc *)(DAT_007d3ff8 + 0x118))(DAT_0091327c, 0x30112);
    if (piVar3 != (int *)0) {
      int i;
      piVar3[3] = -0x40800000;
      piVar3[4] = -0x40800000;
      *piVar3 = 0;
      piVar3[1] = 0;
      piVar3[2] = (int)FUN_00557a60;
      piVar3[5] = -1;
      for (i = 0; i < 10; i++) piVar3[6 + i] = 0;
    }
    *(int **)(param_1 + iVar1) = piVar3;
    if (piVar3 != (int *)0) {
      iVar1 = *(int *)(param_1 + 0x18);
      if (iVar1 != 0) {
        FUN_004e8e90(iVar1);
      }
      piVar3[6] = iVar1;
    } else {
      return 0;
    }
  }
  fVar2 = DAT_005cc55c / param_2;
  piVar3[3] = *(int *)&param_2;
  *(float *)(piVar3 + 4) = fVar2;
  if (*piVar3 == 0) {
    piVar3[1] = *(int *)(param_1 + 0x48);
    *(void (**)(void))(param_1 + 0x48) = FUN_00557b70;
    if (FUN_00557b70 == 0) {
      *(void (**)(void))(param_1 + 0x48) = FUN_004e5f90;
    }
    *piVar3 = 1;
  }
  return param_1;
}

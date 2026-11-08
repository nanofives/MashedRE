// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005ad420 (287 B). Best 74.5% (regs 79.1%), residual class (b). memset intrinsic gives rep stosd/stosb; orig keeps a zero in ebp for the whole first half and loads p5 into ebp only before loop 2 (source shape unknown; 'int z=0' folds away); jbe-guarded for loops differ; permuter's 91.9% variant stores param_1+0x10 before the allocation (uninitialised), rejected. */
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
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
void FUN_004522d0(int p0);
int FUN_005adbe0(int p0);
void *FUN_005aea00(unsigned int p0, int p1);
// MATCH: 0x005ad420 FUN_005ad420
int FUN_005ad420(int param_1,int param_2,unsigned int param_3,int param_4,unsigned int param_5)
{
  int iVar1;
  int *puVar2;
  unsigned int uVar3;
  int iVar4;
  int z = 0;

  if ((*(int *)(param_1 + 0x10) != z) && (*(int *)(param_1 + 0xc) != z)) {
    if ((*(unsigned char *)(param_1 + 0x18) & 1) == 0) {
      FUN_004522d0(*(int *)(param_1 + 0x10));
    }
    *(int *)(param_1 + 0x10) = z;
    *(int *)(param_1 + 0xc) = z;
  }
  if ((param_2 != z) && (param_4 != z)) {
    uVar3 = 0;
    if (param_3 != z) {
      iVar4 = param_2 + 0xc;
      do {
        *(int *)(iVar4 + -4) = z;
        iVar1 = FUN_005adbe0(iVar4);
        if (iVar1 == 0) {
          return 0;
        }
        uVar3 = uVar3 + 1;
        iVar4 = iVar4 + 0x14;
      } while (uVar3 < param_3);
    }
    uVar3 = 0;
    if (param_5 != 0) {
      iVar4 = param_4 + 0xc;
      do {
        *(int *)(iVar4 + -4) = 0;
        iVar1 = FUN_005adbe0(iVar4);
        if (iVar1 == 0) {
          return 0;
        }
        uVar3 = uVar3 + 1;
        iVar4 = iVar4 + 0x14;
      } while (uVar3 < param_5);
    }
    *(unsigned short *)(param_1 + 0x14) = (unsigned short)param_3;
    *(int *)(param_1 + 0x10) = param_2;
    *(int *)(param_1 + 0xc) = param_4;
    *(unsigned int *)(param_1 + 0x18) = *(unsigned int *)(param_1 + 0x18) | 1;
    *(short *)(param_1 + 0x16) = (short)param_5;
    return param_1;
  }
  puVar2 = (int *)FUN_005aea00((param_5 + param_3) * 0x14,0x3080a);
  *(int **)(param_1 + 0x10) = puVar2;
  if (puVar2 == 0) {
    return 0;
  }
  *(int **)(param_1 + 0xc) = puVar2 + param_3 * 5;
  memset(puVar2, 0, (param_5 + param_3) * 0x14);
  *(unsigned int *)(param_1 + 0x18) = *(unsigned int *)(param_1 + 0x18) & 0xfffffffe;
  return param_1;
}

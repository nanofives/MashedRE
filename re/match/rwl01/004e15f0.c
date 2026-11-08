// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004e15f0: best 96.2% (regs 100%, ops 100%), class (a)/(b). 32-byte struct copy (rep movsd) via a B32 struct type; only the base/index register order of [ecx+eax-0x1c] and the lea differs (commutative add operand order, 9 spellings tried, permuter 386 compiles). */
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
int FUN_004e13f0(int p0);
typedef struct B32 { int f0, f4, f8, fc, f10, f14, f18, f1c; } B32;
// MATCH: 0x004e15f0 FUN_004e15f0
undefined4 FUN_004e15f0(undefined4 *param_1,int *param_2,undefined4 param_3)
{
  int *piVar5;
  B32 *blk;
  B32 *end;
  B32 *mid;
  int iVar4;
  int iVar6;
  piVar5 = (int *)FUN_004e13f0((int)param_3);
  if (piVar5 != (int *)0x0) {
    blk = (B32 *)*param_1;
    end = (B32 *)((int)blk + param_1[1] - 0x20);
    blk->f0 = 0;
    blk->f4 = 0;
    blk->f8 = 0;
    blk->fc = 0;
    mid = blk + 1;
    *end = *blk;
    blk->f4 = (int)mid;
    mid->f0 = (int)blk;
    mid->f4 = (int)end;
    end->f0 = (int)mid;
    mid->fc = (int)piVar5;
    mid->f8 = (int)end - (int)mid - 0x20;
    piVar5[1] = (int)mid;
    *piVar5 = mid->f8;
    if (param_2 != (int *)0x0) {
      iVar6 = param_2[1];
      iVar4 = *param_2;
      *(int *)(iVar6 + -0x1c + iVar4) = (int)blk;
      *(int *)blk = iVar6 + -0x20 + iVar4;
    }
    return 1;
  }
  return 0;
}

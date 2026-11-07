// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af690: 88.9% (regs 88.9%, ops 95.6%). Same instruction stream as the original (jbe/jns forms fixed with (0 < uint count) and (x & 0x80000000)); the only residual is stack cleanup: original emits add esp,0xc after each of the first two calls and add esp,0x10 after the third, the DDK defers all to one add esp,0x28. Tried nested call expression, fn-pointer arity casts, variadic proto, 7 flag sets. Class (a). cxx pending. */
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
typedef unsigned int uint;
int * FUN_005af700(int p0, int p1, uint p2, int p3, uint *p4);
typedef int * (*f4_t)(int p0, int p1, uint p2, int p3);
void FUN_005b10d0(int p0);
// MATCH: 0x005af690 FUN_005af690
void FUN_005af690(int param_1,int param_2,uint *param_3)
{
  uint uVar1;
  int *iVar2;
  int *uVar3;
  uint uVar4;

  uVar4 = 0;
  if (0 < *(uint *)(param_2 + 4)) {
    do {
      if (*param_3 & 0x80000000) {
        uVar1 = *param_3 & 0x7fffffff;
        *param_3 = uVar1;
        iVar2 = FUN_005af700(param_1,param_2,uVar1,1,param_3 + 1);
        uVar3 = ((f4_t)FUN_005af700)(param_1,param_2,*param_3,iVar2[0x58 / 4]);
        FUN_005b10d0((int)uVar3);
      }
      uVar4 = uVar4 + 1;
      param_3 = param_3 + 4;
    } while (uVar4 < *(uint *)(param_2 + 4));
  }
  return;
}

// FLAGS: /O1 /GS
/* MATCH-S28 0x005c31d0: 100% (regs 100%). _fltin2-shaped: /O1 /GS (cookie needs a char[12] local, not a struct; raw converter ran /O2 /G5, no cookie). Ghidra's INTRNCVT/_LDBL12 typedefs replaced by plain ints. Class: none. */
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
typedef struct { unsigned int flags; int nbytes; long lval; double dval; } FLT_s;
typedef struct { unsigned int lo; unsigned int hi; } DBLW;
int FUN_004b0609(void *p0, int *p1, int p2, int p3, int p4, int p5, int p6);
int FUN_004acc4b(void *p0, void *p1);
// MATCH: 0x005c31d0 FUN_005c31d0
void FUN_005c31d0(FLT_s *param_1, int param_2)
{
  unsigned int uVar1;
  int IVar2;
  unsigned int uVar3;
  char ld12[12];
  DBLW x;
  int end;

  uVar3 = 0;
  uVar1 = FUN_004b0609(ld12, &end, param_2, 0, 0, 0, 0);
  if ((uVar1 & 4) != 0) {
    uVar3 = 0x200;
    x.lo = 0;
    x.hi = 0;
  }
  else {
    IVar2 = FUN_004acc4b(ld12, &x);
    if (((uVar1 & 2) != 0) || (IVar2 == 1)) {
      uVar3 = 0x80;
    }
    if (((uVar1 & 1) != 0) || (IVar2 == 2)) {
      uVar3 = uVar3 | 0x100;
    }
  }
  param_1->flags = uVar3;
  param_1->nbytes = end - param_2;
  *(unsigned int *)((char *)param_1 + 0x10) = x.lo;
  *(unsigned int *)((char *)param_1 + 0x14) = x.hi;
}



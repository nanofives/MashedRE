// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bfd10 : 77.2% (regs 77, ops 86). By hand: cdecl 3 args; allocator call 0x005aea00(size,0x10800), init 0x005c0140 (cdecl 3), fail cleanup 0x004522d0, two vtable-pointer stores (0x635ab0 at +4, 0x635af8 at +0) and a rep stosd of 12 dwords (memset intrinsic). Remaining diff: the original pushes ebx/ebp/esi/edi and shares one epilogue between the two failure paths (the first fail jumps past the cleanup call); ours emits separate epilogues. Residual class (b). */

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
void *memset(void *d, int c, unsigned int n);
void *FUN_005aea00(int size, int flags);
int FUN_005c0140(int p0, int *p1, int p2);
void FUN_004522d0(int *p0);
// MATCH: 0x005bfd10 FUN_005bfd10
int *FUN_005bfd10(int param_1, int *param_2, int param_3)
{
  int *piVar2;
  int bVar3;

  bVar3 = 0;
  if (param_3 == 0) {
    param_3 = 0x178;
  }
  if (param_2 == (int *)0) {
    param_2 = (int *)FUN_005aea00(param_3, 0x10800);
    bVar3 = 1;
    if (param_2 == (int *)0) {
      return (int *)0;
    }
  }
  piVar2 = param_2 + 1;
  if (FUN_005c0140(param_1, piVar2, param_3) != 0) {
    piVar2[2] = bVar3;
    piVar2[4] = 0;
    piVar2[0] = 0x635ab0;
    param_2[0] = 0x635af8;
    param_2[0x50] = 0;
    memset(param_2 + 0x52, 0, 0x30);
    return param_2;
  }
  FUN_004522d0(param_2);
  return (int *)0;
}

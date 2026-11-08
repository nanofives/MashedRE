// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR04 2026-11-08: 100% (regs 100%), relocheck OK, C++ check OK, was 84.8%. 4-iteration zeroing for loop (unrolled with its own zero register). Supersedes the header below. */
/* MATCH-RWL05 0x005551d0: hand-matched (h1). 84.8% (regs 93.5%, ops 93.5%), 45/46 ins. Original zeroes dwords +0x14..+0x20 through a second xor edx,edx (separate zero value) while +8..+0x10 and +0x124.. use edi; tried float/pointer/short typed zero stores, same result. Class (b). */
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
MGLOBAL(int, DAT_00912a08, 0x00912a08);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_00554940(void);
typedef unsigned int * (__cdecl *fnalloc)(int, int);
int FUN_005c4ad0(int p0, unsigned int p1);
// MATCH: 0x005551d0 FUN_005551d0
unsigned int * FUN_005551d0(void)
{
  unsigned int *puVar1;
  int iVar3;
  unsigned int *puVar4;

  puVar1 = (*(fnalloc *)(DAT_007d3ff8 + 0x118))(DAT_00912a08,0x30199);
  if (puVar1 == 0) {
    return 0;
  }
  *puVar1 = 1;
  puVar1[1] = 0x3f800000;
  puVar1[2] = 0;
  puVar1[3] = 0;
  puVar1[4] = 0;
  for (iVar3 = 0; iVar3 < 4; iVar3++) {
    puVar1[5 + iVar3] = 0;
  }
  puVar1[0x4a] = 0;
  puVar1[0x4c] = 0;
  puVar1[0x49] = 0;
  puVar1[0x4b] = 0;
  puVar1[0x4d] = FUN_005c4ad0(0x20,0x30190);
  puVar1[0x4f] = 0;
  puVar1[0x50] = 0;
  puVar1[0x4e] = (unsigned int)FUN_00554940;
  puVar4 = puVar1 + 9;
  for (iVar3 = 0x40; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = 0xffffffff;
    puVar4 = puVar4 + 1;
  }
  return puVar1;
}

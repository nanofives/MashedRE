// FLAGS: /O2 /GS /Ob1
/* 0x00423630 (53 B). Best 100.0% exact (regs 100, ops 100).
   Raw g2s 86.7%: with three separate globals the compiler sank the 0x00801a9c zero store down next to the
   0x007f1a54 / 0x007f1a64 stores (tried arrays, loops, memset, file-static, __inline helpers: all 86.7%).
   100% once 0x007f1a9c is one record: two int[0x2000] arrays (0x007f1a9c, 0x007f9a9c) and the dword at
   0x00801a9c right after them, both fills intrinsic memset(.., 0xff, 0x8000). Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { int a[0x2000]; int b[0x2000]; int n; } Rec007f1a9c;
MGLOBAL(Rec007f1a9c, DAT_007f1a9c, 0x007f1a9c);
MGLOBAL(int, DAT_007f1a54, 0x007f1a54);
MGLOBAL(int, DAT_007f1a64, 0x007f1a64);
#ifdef __cplusplus
extern "C" void *memset(void *, int, unsigned int);
#else
void *memset(void *, int, unsigned int);
#endif
#pragma intrinsic(memset)

// MATCH: 0x00423630 FUN_00423630
void FUN_00423630(void)
{
  DAT_007f1a9c.n = 0;
  memset(DAT_007f1a9c.a, 0xff, sizeof(DAT_007f1a9c.a));
  memset(DAT_007f1a9c.b, 0xff, sizeof(DAT_007f1a9c.b));
  DAT_007f1a54 = 0;
  DAT_007f1a64 = 0;
}

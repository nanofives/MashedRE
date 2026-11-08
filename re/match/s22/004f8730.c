// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f8730 (h6): best 96.7% (regs 96.7%) under /O2 /Ob1 /G5. Hand form: 20-byte entry struct array at
   0x007d73b0, count 0x007d98b0, free-list head 0x007d98b4. Raw converter was 96.7% too. Tried: struct array /
   indexed / pointer-walk loops, head written through the pointer, permuter (6 rounds, no gain), flagsweep (all 5 sets
   96.7%). Residual: one `pop edi` scheduled after the last -1 store instead of between the 0x007d73b4 and 0x007d73b8
   stores. Class (a): pure epilogue scheduling, no source shape moved it. */
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
typedef struct E { int a; int b; void *p; int c; int d; } E;
MGLOBAL(int, DAT_007d98b0, 0x007d98b0);
MGLOBALA(E, DAT_007d73b0, 1, 0x007d73b0);
MGLOBAL(E *, DAT_007d98b4, 0x007d98b4);
void FUN_004cbb50(void *p0);
// MATCH: 0x004f8730 FUN_004f8730
void FUN_004f8730(void)
{
  int i;
  for (i = 0; i < DAT_007d98b0; i++) {
    if (DAT_007d73b0[i].p != 0) { FUN_004cbb50(DAT_007d73b0[i].p); DAT_007d73b0[i].p = 0; }
  }
  DAT_007d98b0 = 0;
  DAT_007d73b0[0].a = -1;
  DAT_007d98b4 = DAT_007d73b0;
  DAT_007d73b0[0].b = -1;
  DAT_007d73b0[0].p = (void *)-1;
  DAT_007d73b0[0].c = -1;
  DAT_007d73b0[0].d = -1;
}

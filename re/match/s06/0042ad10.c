// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042ad10 (118 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 10.8% (in_EAX). Static function: idx pointer in EAX, a in EDX, x/y/e on the stack, plain ret
   (pilot finding 1); caller_0042ad10 is a stand-in for the real callers at 0x0042bb2c.. (not matched). The
   0x34-byte records at 0x00898ac0 as one struct array; *idx is reloaded before every store because the int
   stores may alias it. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
/* 0x34-byte records at 0x00898ac0; fields named by offset. The element count is not established (N = 1 here). */
typedef struct {
  int f00;
  int f04;
  int f08;
  unsigned char col[4];
  int f10;
  float f14;
  float f18;
  float f1c;
  int f20;
  int f24;
  int f28;
  int f2c;
  int f30;
} Rec898ac0;
MGLOBALA(Rec898ac0, DAT_00898ac0, 1, 0x00898ac0);
// MATCH: 0x0042ad10 FUN_0042ad10
static void FUN_0042ad10(int *idx, int a, int x, int y, int e)
{
  DAT_00898ac0[*idx].f00 = a;
  DAT_00898ac0[*idx].col[0] = 0xff;
  DAT_00898ac0[*idx].col[1] = 0xff;
  DAT_00898ac0[*idx].col[2] = 0xff;
  DAT_00898ac0[*idx].col[3] = 0xff;
  DAT_00898ac0[*idx].f14 = 0.6f;
  DAT_00898ac0[*idx].f18 = (float)x;
  DAT_00898ac0[*idx].f1c = (float)y;
  DAT_00898ac0[*idx].f24 = e;
}

/* stand-in caller (not matched): the real callers sit at 0x0042bb2c and later */
void caller_0042ad10(int *idx, int a)
{
  FUN_0042ad10(idx, a, 64, 48, 0);
  FUN_0042ad10(idx, a + 1, 64, 48, 0);
}

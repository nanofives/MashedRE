// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042d3e0 (h5, by hand): 100.0% (regs 100.0%). 30 x 0x34-byte records at 0x00898ac0, 14 stores per record (+0x20 and +0x30 untouched), indexed loop. Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct Rec00898ac0 {
  int f00;
  int f04;
  int f08;
  char f0c;
  char f0d;
  char f0e;
  char f0f;
  int f10;
  int f14;
  int f18;
  int f1c;
  int f20;
  int f24;
  int f28;
  int f2c;
  int f30;
} Rec00898ac0;
MGLOBALA(Rec00898ac0, DAT_00898ac0, 30, 0x00898ac0);
// MATCH: 0x0042d3e0 FUN_0042d3e0
void FUN_0042d3e0(void)
{
  int i;

  for (i = 0; i < 30; i++) {
    DAT_00898ac0[i].f00 = 0;
    DAT_00898ac0[i].f04 = 0;
    DAT_00898ac0[i].f08 = 0;
    DAT_00898ac0[i].f0c = 0;
    DAT_00898ac0[i].f0d = 0;
    DAT_00898ac0[i].f0e = 0;
    DAT_00898ac0[i].f0f = 0;
    DAT_00898ac0[i].f10 = 0;
    DAT_00898ac0[i].f14 = 0;
    DAT_00898ac0[i].f18 = 0;
    DAT_00898ac0[i].f1c = 0;
    DAT_00898ac0[i].f24 = 0;
    DAT_00898ac0[i].f28 = 0;
    DAT_00898ac0[i].f2c = 0;
  }
}

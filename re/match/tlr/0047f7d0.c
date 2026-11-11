// FLAGS: /O2 /GS /Ob1
/* TL-MATCH-REST 0x0047f7d0 (no hooks.csv row before this slice; decomp_pc.py 2026-11-10). */
/* ---- MATCH-TLR preamble (same text in every re/match/tlr source; not part of any function body) ----
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
typedef struct { int f[7]; } Q_0047f7d0;
int FUN_0047f6d0(int a, int b);
void FUN_00538c80(int a, Q_0047f7d0 *q, int (*cb)(int, int), int d);
// MATCH: 0x0047f7d0 FUN_0047f7d0
int FUN_0047f7d0(int param_1, int param_2, int *param_3, int param_4, int param_5)
{
  Q_0047f7d0 q;
  *(int *)(param_5 + 4) = 0;
  *(int *)(param_5 + 0xc) = 0;
  q.f[3] = param_3[4];
  q.f[4] = param_3[5];
  q.f[5] = param_3[6];
  q.f[0] = param_3[0];
  q.f[1] = param_3[1];
  q.f[2] = param_3[2];
  q.f[6] = 4;
  FUN_00538c80(*(int *)(param_1 + 0x40), &q, FUN_0047f6d0, param_5);
  return param_1;
}

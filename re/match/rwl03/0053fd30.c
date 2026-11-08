// FLAGS: /O2 /Ob1 /G5
// h3: 77.2% regs 78.0%. Four stage lookups with byte counters (two variables), struct stride 0x18. Residual: out-of-line found/null block order of loops 2 and 3; class (b).
/* MATCH-RWL03 0x0053fd30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef struct { int a, b; float c; int d, e, f; } S53fd30;
MGLOBAL(S53fd30 *, DAT_007dc5dc, 0x007dc5dc);
void FUN_004c5a60(int p0);
// MATCH: 0x0053fd30 FUN_0053fd30
int FUN_0053fd30(int param_1,int param_2,int param_3,int param_4,float param_5)
{
  S53fd30 *pS;
  byte i;
  byte j;
  S53fd30 *e;

  pS = *(S53fd30 **)((char *)DAT_007dc5dc + param_1);
  for (i = 0; i < 2; i++) {
    if (pS[i].f == 2) {
      e = &pS[i];
      goto found1;
    }
  }
  e = 0;
found1:
  *(int *)(param_2 + 0x54) = *(int *)(param_2 + 0x54) + 1;
  if (e->b != 0) {
    FUN_004c5a60(e->b);
    e->b = 0;
  }
  e->b = param_2;
  if (param_1 == 0) {
    return 0;
  }
  pS = *(S53fd30 **)((char *)DAT_007dc5dc + param_1);
  for (j = 0; j < 2; j++) {
    if (pS[j].f == 2) {
      e = &pS[j];
      goto found2;
    }
  }
  e = 0;
found2:
  e->a = param_3;
  pS = *(S53fd30 **)((char *)DAT_007dc5dc + param_1);
  for (j = 0; j < 2; j++) {
    if (pS[j].f == 2) {
      e = &pS[j];
      goto found3;
    }
  }
  e = 0;
found3:
  e->d = param_4;
  pS = *(S53fd30 **)((char *)DAT_007dc5dc + param_1);
  for (j = 0; j < 2; j++) {
    if (pS[j].f == 2) {
      e = &pS[j];
      goto found4;
    }
  }
  e = 0;
found4:
  e->c = param_5;
  return param_1;
}

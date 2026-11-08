// FLAGS: /O2 /Ob1 /G5
// h3: 58.9% regs 69.9%. Pointer locals computed right before each call (matches lea order) but the original keeps all four callee-saved pushes at entry and reloads param_2; class (b).
/* MATCH-RWL03 0x005300d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef struct { unsigned int a0; unsigned int pad[8]; unsigned int f9; unsigned int f10; unsigned int f11; unsigned int f12; } S5300d0;
int FUN_004cbe80(int p0, unsigned int p1, int p2);
int FUN_004cc770(int p0, unsigned int *p1, int p2);
// MATCH: 0x005300d0 FUN_005300d0
int FUN_005300d0(int param_1, S5300d0 *param_2)
{
  unsigned int *pa = &param_2->f9;
  unsigned int *pb;
  unsigned int *pc;

  if (FUN_004cc770(param_1, pa, 4) == 0) {
    return 0;
  }
  pb = pa + 1;
  if (FUN_004cc770(param_1, pb, 4) == 0) {
    return 0;
  }
  pc = pa + 2;
  if (FUN_004cc770(param_1, pc, 4) == 0) {
    return 0;
  }
  if (0 < param_2->f10) {
    if (FUN_004cbe80(param_1, pa[3], param_2->a0 + (*pc + *pb) * 2) == 0) {
      return 0;
    }
  }
  return param_1;
}

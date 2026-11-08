// FLAGS: /O2 /Ob1 /G5
// h3: 100% regs 100%. Struct (base,rp,wp,end) at DAT+DAT, permuter statement order (shift after the second compare).
/* MATCH-RWL03 0x00534870: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef struct { unsigned int *base; unsigned int *rp; unsigned int *wp; unsigned int *end; } St534870;
MGLOBAL(int, DAT_007dc578, 0x007dc578);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
#define S534870 ((St534870 *)(DAT_007d3ff8 + DAT_007dc578))
// MATCH: 0x00534870 FUN_00534870
uint FUN_00534870(void)
{
  unsigned int *p;
  uint uVar3;

  *S534870->rp = *S534870->rp + *S534870->wp;
  p = S534870->rp;
  uVar3 = *p;
  uVar3 = uVar3 >> 1;
  S534870->rp = p + 1;
  if (S534870->rp >= S534870->end) {
    S534870->rp = S534870->base;
    S534870->wp = S534870->wp + 1;
    return uVar3;
  }
  S534870->wp = S534870->wp + 1;
  if (S534870->wp >= S534870->end) {
    S534870->wp = S534870->base;
  }
  return uVar3;
}

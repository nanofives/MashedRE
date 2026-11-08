// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR06 2026-11-08: offset read into a plain local (the compiler homes it in param_2's slot), typed entry array, returns param_2. Supersedes the header below. */
/* MATCH-RWL03 0x005333a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
// MATCH: 0x005333a0 FUN_005333a0
typedef struct { int ptr; int data[8]; } E533;
typedef struct { int a0; int num; int a8; int ac; E533 *ents; } S533;
S533 *FUN_005333a0(undefined4 param_1,S533 *param_2)
{
  E533 *base = param_2->ents;
  int i;
  uint off;

  for (i = 0; i < param_2->num; i++) {
    if (FUN_004cc790(param_1,(undefined4)base[i].data,0x20) == 0) return 0;
    if (FUN_004cc790(param_1,(undefined4)&off,4) == 0) return 0;
    base[i].ptr = (int)(base + off / sizeof(E533));
  }
  return param_2;
}

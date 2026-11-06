// FLAGS: /O2 /GS /Ob1
/* 0x0048aa20: 100% (regs 100%, whole symbol byte-identical 44 B). Raw 57.1%: FUN_0048a850 is called with 7
   args (its 7th is not read by 0x0048a850's body; the callers push 1.0f), typed prototypes. */
/* MATCH-S14 0x0048aa20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
typedef struct { uchar r, g, b, a; } Rgba;
void FUN_0048a850(float *pos, int count, float *vel, Rgba *color, float *accel, float size, float p7);
// MATCH: 0x0048aa20 FUN_0048aa20
void FUN_0048aa20(float *param_1, int param_2, float *param_3, Rgba *param_4, float *param_5, float param_6)
{
  FUN_0048a850(param_1, param_2, param_3, param_4, param_5, param_6, 1.0f);
}

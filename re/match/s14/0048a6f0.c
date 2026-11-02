// FLAGS: /O2 /GS /Ob1
/* 0x0048a6f0: 100% (regs 100%, whole symbol byte-identical 141 B). Raw converter 68.6%. By hand: a second
   float parameter (caller 0x0048a780 pushes 2 args; Ghidra missed it and showed the reused param_1 slot),
   alpha = (uchar)(255.0f - param_2 * 255.0f) through _ftol (0x004a2c48; 255.0f at 0x005cd04c), RGBA local,
   FUN_004c1520's third arg is float 10.0f (0x41200000). */
/* MATCH-S14 0x0048a6f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
void FUN_0048a4e0(int p0, float *p1);
int FUN_004b40f0(int p0);
int FUN_004c1340(int p0, float *p1, int p2);
int FUN_004c1520(int p0, float *p1, float p2, int p3);
float FUN_004c39b0(float *p0, float *p1);
// MATCH: 0x0048a6f0 FUN_0048a6f0
void FUN_0048a6f0(int *param_1, float param_2)
{
  int frame;
  int x;
  Rgba color;
  float v[3];

  frame = *(int *)(*param_1 + 4);
  FUN_004c39b0(v, (float *)(param_1 + 1));
  FUN_004c1340(frame, (float *)(param_1 + 1), 2);
  FUN_004c1520(frame, v, 10.0f, 1);
  x = FUN_004b40f0(*param_1);
  if (x != 0) {
    color.r = 0xff; color.g = 0xff; color.b = 0xff;
    color.a = (uchar)(255.0f - param_2 * 255.0f);
    *(Rgba *)(x + 4) = color;
  }
  FUN_0048a4e0(*param_1, 0);
}

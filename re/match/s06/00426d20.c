// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426d20: 100% by hand (h4), regs 100%. Walks the 4-pointer table DAT_00656f48..0x00656f58.
   The loop bound is a SIGNED compare (jl): `(int)p < (int)(DAT_00656f48 + 4)`; the plain pointer compare gave jb
   (97.2%). The {0, 1, 0} float triple is a 3-float local passed by address. relocheck OK. Residual: none. */
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
MGLOBAL(int, DAT_0066d720, 0x0066d720);
MGLOBALA(int, DAT_00656f48, 4, 0x00656f48);
int FUN_004c1340(int p0, int p1, int p2);
int FUN_004c1520(int p0, float *p1, int p2, int p3);
void FUN_004c15c0(int p0);
// MATCH: 0x00426d20 FUN_00426d20
void FUN_00426d20(int param_1)
{
  float axis[3];
  int *p;
  int frame;

  axis[0] = 0.0f;
  axis[1] = 1.0f;
  axis[2] = 0.0f;
  for (p = DAT_00656f48; (int)p < (int)(DAT_00656f48 + 4); p++) {
    if (*p != 0) {
      frame = *(int *)(*p + 4);
      FUN_004c15c0(frame);
      FUN_004c1520(frame, axis, DAT_0066d720, 0);
      FUN_004c1340(frame, param_1 + 0x30, 2);
    }
  }
}

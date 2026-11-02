// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00448e20: 100% (regs 100%) by hand (h9). Raw 50.6%. 4-entry twin of 0x00448cb0 without the angle
   advance: float globals, double literals 5.0/0.25/4.0 (.rdata 0x005cd030/0x005ce1c8/0x005ce1f8), sin/cos
   intrinsics.
   param_1 is unused: the caller 0x00448ef0 pushes one argument, so it is declared here too. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x, y, z; } MatchV3_00448e20;
MGLOBALA(int, DAT_00896480, 4, 0x00896480);
MGLOBALA(MatchV3_00448e20, DAT_005f95f0, 4, 0x005f95f0);
MGLOBALA(float, DAT_005f9620, 4, 0x005f9620);
MGLOBAL(float, DAT_00683258, 0x00683258);
MGLOBAL(float, DAT_0068325c, 0x0068325c);
MGLOBAL(float, DAT_00683260, 0x00683260);
double sin(double);
double cos(double);
#pragma intrinsic(sin, cos)
int FUN_004c1340(int p0, MatchV3_00448e20 *p1, int p2);
int FUN_004c1520(int p0, MatchV3_00448e20 *p1, float p2, int p3);
void FUN_004c15c0(int p0);
// MATCH: 0x00448e20 FUN_00448e20
void FUN_00448e20(int param_1)
{
  MatchV3_00448e20 v;
  int i;
  int m;

  v.x = 0.0f;
  v.y = 1.0f;
  v.z = 0.0f;
  for (i = 0; i < 4; i++) {
    m = *(int *)(DAT_00896480[i] + 4);
    FUN_004c15c0(m);
    FUN_004c1520(m, &v, DAT_005f9620[i], 0);
    FUN_004c1520(m, (MatchV3_00448e20 *)(m + 0x30), (float)(cos(DAT_00683260) * 5.0), 2);
    FUN_004c1520(m, (MatchV3_00448e20 *)(m + 0x10), (float)(sin(DAT_0068325c) * 5.0), 2);
    DAT_005f95f0[i].y = (float)(sin(DAT_00683258) * 0.25 - 4.0);
    FUN_004c1340(*(int *)(DAT_00896480[i] + 4), &DAT_005f95f0[i], 2);
  }
}

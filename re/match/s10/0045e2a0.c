// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045e2a0 (h4): 100% exact, regs 100%. Same shape as 0x0045e160: raw 45 scalar locals (14.8%);
   one local 15 x {x,y,z} float array assigned element by element. Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } V3_45e2a0;
MGLOBAL(int, DAT_0068f618, 0x0068f618);
int FUN_00546380(int p0, int p1, V3_45e2a0 *p2);
// MATCH: 0x0045e2a0 FUN_0045e2a0
void FUN_0045e2a0(void)
{
  V3_45e2a0 pts[15];
  pts[0].x = -43.0f;
  pts[0].y = -2.6f;
  pts[0].z = 22.0f;
  pts[1].x = -33.0f;
  pts[1].y = -2.6f;
  pts[1].z = 40.0f;
  pts[2].x = -16.0f;
  pts[2].y = -2.6f;
  pts[2].z = 53.0f;
  pts[3].x = 4.0f;
  pts[3].y = -2.6f;
  pts[3].z = 53.0f;
  pts[4].x = 22.0f;
  pts[4].y = -2.6f;
  pts[4].z = 51.0f;
  pts[5].x = 31.0f;
  pts[5].y = -2.6f;
  pts[5].z = 31.0f;
  pts[6].x = 30.0f;
  pts[6].y = -2.6f;
  pts[6].z = 10.0f;
  pts[7].x = 25.0f;
  pts[7].y = -2.6f;
  pts[7].z = -6.0f;
  pts[8].x = 27.0f;
  pts[8].y = -2.6f;
  pts[8].z = -31.0f;
  pts[9].x = 20.0f;
  pts[9].y = -2.6f;
  pts[9].z = -50.0f;
  pts[10].x = -2.0f;
  pts[10].y = -2.6f;
  pts[10].z = -54.0f;
  pts[11].x = -34.0f;
  pts[11].y = -2.6f;
  pts[11].z = -47.0f;
  pts[12].x = -43.0f;
  pts[12].y = -2.6f;
  pts[12].z = -35.0f;
  pts[13].x = -42.0f;
  pts[13].y = -2.6f;
  pts[13].z = -14.0f;
  pts[14].x = -37.0f;
  pts[14].y = -2.6f;
  pts[14].z = 6.1f;
  DAT_0068f618 = FUN_00546380(0xf, 2, pts);
}

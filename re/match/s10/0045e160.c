// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045e160 (h4): 100% exact, regs 100%. Raw had 33 scalar locals (19%); one local 11 x {x,y,z}
   float array assigned element by element gives the original store order and frame. Residual: none. */
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
typedef struct { float x; float y; float z; } V3_45e160;
MGLOBAL(int, DAT_0068f618, 0x0068f618);
int FUN_00546380(int p0, int p1, V3_45e160 *p2);
// MATCH: 0x0045e160 FUN_0045e160
void FUN_0045e160(void)
{
  V3_45e160 pts[11];
  pts[0].x = 21.0f;
  pts[0].y = 0.4f;
  pts[0].z = 75.0f;
  pts[1].x = 25.0f;
  pts[1].y = 0.4f;
  pts[1].z = 50.0f;
  pts[2].x = 25.0f;
  pts[2].y = 0.4f;
  pts[2].z = 25.0f;
  pts[3].x = 29.6f;
  pts[3].y = 0.4f;
  pts[3].z = 6.4f;
  pts[4].x = 26.0f;
  pts[4].y = 0.4f;
  pts[4].z = -20.5f;
  pts[5].x = 20.5f;
  pts[5].y = 0.4f;
  pts[5].z = -31.0f;
  pts[6].x = -8.0f;
  pts[6].y = 0.4f;
  pts[6].z = -41.0f;
  pts[7].x = -32.0f;
  pts[7].y = 0.4f;
  pts[7].z = -20.0f;
  pts[8].x = -37.0f;
  pts[8].y = 0.4f;
  pts[8].z = -18.7f;
  pts[9].x = -18.0f;
  pts[9].y = 0.4f;
  pts[9].z = 3.3f;
  pts[10].x = -22.5f;
  pts[10].y = 0.4f;
  pts[10].z = 30.0f;
  DAT_0068f618 = FUN_00546380(0xb, 1, pts);
}

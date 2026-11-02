// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00481810 (h10): 100.0% (regs 100%). Moved it: callee arities from the call sites (0x0055bb70
   3 args, 0x0055bde0 5 args, cdecl, one merged add esp,0x20); the 16-byte buffer typed float[4] (a char
   array drew a /GS cookie); the 0x20-byte result as a struct whose two V3 members are struct-copied out.
   Raw 40.8%. Residual: none. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Hit { int f0; V3 a; V3 b; int f2c; } Hit;
int FUN_0047d130(int p0);
void FUN_0055bb70(int p0, int p1, void *p2);
int FUN_0055bde0(int p0, int p1, float *p2, float *p3, Hit *p4);
int FUN_0057c210(int p0);
// MATCH: 0x00481810 FUN_00481810
int FUN_00481810(int param_1, float *param_2, float *param_3, V3 *param_4, V3 *param_5)
{
  int o;
  int g0;
  int g2;
  float buf[4];
  Hit hit;

  o = FUN_0057c210(FUN_0047d130(param_1));
  g0 = **(int **)(o + 0x10);
  g2 = (*(int **)(o + 0x10))[2];
  if (*(int *)(o + 0x24) != 0 &&
      (*(unsigned int *)(*(int *)(*(int *)(o + 0x24) + 100) + (*(unsigned short *)(o + 0x20) >> 5) * 4) &
       (1 << (*(unsigned short *)(o + 0x20) & 0x1f))) != 0) {
    FUN_0055bb70(g0, g2, buf);
    if (FUN_0055bde0(g0, g2, param_2, param_3, &hit) != 0) {
      *param_4 = hit.a;
      *param_5 = hit.b;
      return 1;
    }
  }
  return 0;
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b4550 (247 B): by hand (g2s output did not compile: C2059 from a stray header comment).
   Mean of param_3 vec3s: copy element 0, add elements 1..n-1, scale by DAT_005cc320 / n.
   Best 63.3% (regs 63.3%). Residual (a): the original unrolls the add loop x4, 9176 unrolls it x8 (as S01 0x00401690,
   0x00475ab0, 0x00499d90). Everything else (copy, first-iteration z operand order, remainder loop, scale) lines up.
   Tried: struct += / a = a + b / float* indexing / pointer walk / macro and __inline add; flags /G5, /G6, /Ox, /Op,
   /Ob2: every form unrolls x8 (or worse). */
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
typedef struct { float x, y, z; } V3_4b4550;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
// MATCH: 0x004b4550 FUN_004b4550
void FUN_004b4550(V3_4b4550 *param_1, V3_4b4550 *param_2, int param_3)
{
  int i;
  float s;

  *param_1 = *param_2;
  for (i = 1; i < param_3; i++) {
    param_1->x += param_2[i].x;
    param_1->y += param_2[i].y;
    param_1->z += param_2[i].z;
  }
  s = DAT_005cc320 / (float)param_3;
  param_1->x = s * param_1->x;
  param_1->y = s * param_1->y;
  param_1->z = s * param_1->z;
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b4e70: by hand, 100%. (57 B) Wrapper: query record, type = 1 before the 6-dword line copy -> FUN_004b4e20(p1, &rec, p3). */
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
typedef struct { float x, y, z; } V3_004b4e70;
typedef struct { V3_004b4e70 center; float radius; } Sphere_004b4e70;
typedef struct { V3_004b4e70 start; V3_004b4e70 end; } Line_004b4e70;
typedef struct { union { Line_004b4e70 line; Sphere_004b4e70 sphere; } t; int type; } Isect_004b4e70;
int FUN_004b4e20(int p0, void *p1, int p2);
// MATCH: 0x004b4e70 FUN_004b4e70
void FUN_004b4e70(int param_1, Line_004b4e70 *param_2, int param_3)
{
  Isect_004b4e70 is;

  is.type = 1;
  is.t.line = *param_2;
  FUN_004b4e20(param_1, &is, param_3);
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S18 h2 0x004b4d10: by hand, 100%. (66 B) Wrapper: query record {4-dword sphere copied, type = 3} -> FUN_004b4c80(p1, &rec, p3). */
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
typedef struct { float x, y, z; } V3_004b4d10;
typedef struct { V3_004b4d10 center; float radius; } Sphere_004b4d10;
typedef struct { V3_004b4d10 start; V3_004b4d10 end; } Line_004b4d10;
typedef struct { union { Line_004b4d10 line; Sphere_004b4d10 sphere; } t; int type; } Isect_004b4d10;
int FUN_004b4c80(int p0, void *p1, int p2);
// MATCH: 0x004b4d10 FUN_004b4d10
void FUN_004b4d10(int param_1, Sphere_004b4d10 *param_2, int param_3)
{
  Isect_004b4d10 is;

  is.t.sphere = *param_2;
  is.type = 3;
  FUN_004b4c80(param_1, &is, param_3);
}

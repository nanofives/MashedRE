// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005af860 (129 B). Best 58.1% (regs 83.7%), residual class (b). stack layout now exact (Frm struct 0x60 frame, cdecl FUN_005a9e40 w/ deferred add esp, S7 rep movsd copies); residual is reg alloc/load order (orig loads p5+0x10 into edx, p5+0x58 after rep movsd); permuter's 83.7% variant sinks the dead f.lc store after the call (not faithful), rejected. */
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
typedef struct S7 { int d[7]; } S7;
typedef struct Frm { int a[7]; S7 l44; S7 l28; int lc; int l8; int pad; } Frm;
int FUN_005a9e40(int p0, void *p1);
int * FUN_005aa560(int p0, int p1, unsigned int p2, void *p3, int p4, int p5);
// MATCH: 0x005af860 FUN_005af860
void FUN_005af860(int param_1,S7 *param_2,S7 *param_3,int param_4,int param_5)
{
  Frm f;
  int uVar1;
  int *q = (int *)param_5;
  FUN_005a9e40(param_1, &f);
  f.l8 = q[4];
  f.a[2] = q[3];
  f.lc = *(int *)(*(int *)(f.a[2] + 0x18) + 0x20 + param_4 * 0x28);
  uVar1 = q[0x14];
  f.l44 = *param_2;
  f.l28 = *param_3;
  FUN_005aa560(param_1, 0, 4, &f, uVar1, q[0x16]);
}

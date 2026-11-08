// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00559b50: hand-matched (h3). 100%. the three locals are one struct (S3) whose address goes to 0x005595d0; Ghidra prototypes were float/arity wrong. */
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
typedef struct { int a; int b; int c; } S3;
int FUN_00559560(int p0);
int *FUN_005595d0(S3 *p0, int p1, int p2, int p3, int p4);
int FUN_00564130(int p0, unsigned int p1, unsigned int p2);
// MATCH: 0x00559b50 FUN_00559b50
void FUN_00559b50(int param_1, int param_2, int param_3, int param_4)
{
  S3 s;

  s.b = FUN_00559560(param_1);
  s.a = FUN_00564130(s.b, 0x10, 0x30900);
  s.c = s.a;
  FUN_005595d0(&s, param_1, param_2, param_3, param_4);
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042c090 (187 B): 100% by hand (raw 20.0%). Ghidra's `&stack0x00000004` is the address of the
   function's own first stack argument: the function takes one 4-byte argument and passes &param_1 as the fifth
   argument of every FUN_004273e0 call. Float constants: 0x43020000 = 130.0f, 0x43ff0000 = 510.0f,
   0x42f00000 = 120.0f, 0x43220000 = 162.0f, 0x43a40000 = 328.0f, 0x43b40000 = 360.0f. The argument's type is
   [UNCERTAIN] (4 bytes, only its address is used here); int is used. Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
void FUN_004273e0(float p0, float p1, float p2, float p3, int *p4);
// MATCH: 0x0042c090 FUN_0042c090
void FUN_0042c090(int param_1)
{
  FUN_004273e0(130.0f, 120.0f, 510.0f, 120.0f, &param_1);
  FUN_004273e0(130.0f, 162.0f, 510.0f, 162.0f, &param_1);
  FUN_004273e0(130.0f, 328.0f, 510.0f, 328.0f, &param_1);
  FUN_004273e0(130.0f, 360.0f, 510.0f, 360.0f, &param_1);
  FUN_004273e0(130.0f, 120.0f, 130.0f, 360.0f, &param_1);
  FUN_004273e0(510.0f, 120.0f, 510.0f, 360.0f, &param_1);
}

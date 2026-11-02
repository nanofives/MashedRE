// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045da90 (h3): 100% (regs 100%). Raw converter was 29.6%: the four Ghidra locals are one
   16-byte struct passed by address to FUN_005a89d0 (2 stack args, caller cleans). Residual: none. */
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
typedef struct {
  int a;
  int b;
  int c;
  int d;
} S_0045da90;
MGLOBAL(int, DAT_0069045c, 0x0069045c);
void FUN_005a89d0(int p0, S_0045da90 *p1);
// MATCH: 0x0045da90 FUN_0045da90
void FUN_0045da90(int param_1,int param_2,int param_3,int param_4)
{
  S_0045da90 s;

  s.a = param_2;
  s.b = 0;
  s.c = 0;
  s.d = param_3;
  FUN_005a89d0((&DAT_0069045c)[param_1],&s);
  s.d = param_4;
  s.c = 1;
  FUN_005a89d0((&DAT_0069045c)[param_1],&s);
}

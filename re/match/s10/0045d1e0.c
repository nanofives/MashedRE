// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045d1e0 (h2): 100% (regs 100%). Raw did not compile. Natural && chain; the compiler itself keeps
   &DAT_0088f6a0[param_1] in EDI across the calls. Callees declared with the arity the original pushes. */
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
typedef struct { int f0; int f4; int f8; } Rec28;
MGLOBALA(int, DAT_0088f0c0, 4, 0x0088f0c0);
MGLOBALA(Rec28 *, DAT_0088f6a0, 4, 0x0088f6a0);
MGLOBALA(int, DAT_008aa2e0, 4, 0x008aa2e0);
void FUN_0045c880(int param_1, int param_2, int param_3);
void FUN_0045cbe0(int param_1, int param_2, int param_3);
void FUN_0045cd30(int param_1);
int FUN_0045d0e0(int param_1);
void FUN_004656e0(int param_1);
// MATCH: 0x0045d1e0 FUN_0045d1e0
void FUN_0045d1e0(int param_1)
{
  if (FUN_0045d0e0(param_1) != 0 && DAT_0088f0c0[param_1] == 1 && DAT_008aa2e0[param_1] == 0) {
    FUN_0045c880(DAT_0088f6a0[param_1]->f0, DAT_0088f6a0[param_1]->f4, DAT_0088f6a0[param_1]->f8);
    FUN_0045cd30(param_1);
    FUN_004656e0(param_1);
    FUN_0045cbe0(DAT_0088f6a0[param_1]->f0, DAT_0088f6a0[param_1]->f4, DAT_0088f6a0[param_1]->f8);
  }
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004656e0 (h8): 100% by hand (raw 70.6%). What moved it: two separate early `return 0` statements
   (a combined || test or the nested Ghidra form put the return-0 epilogue at the end, 84.3%); the descriptor
   passed to 0x005a86a0 is a local {ptr, count, 4-dword entry} with ptr pointing at its own entry. */
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
typedef struct { int v; int a; int b; int c; } MatchParm_4656e0;
typedef struct { MatchParm_4656e0 *p; int n; MatchParm_4656e0 parm; } MatchDesc_4656e0;
MGLOBALA(char *, DAT_0088f680, 1, 0x0088f680);
MGLOBALA(int, DAT_0069045c, 1, 0x0069045c);
MGLOBALA(int, DAT_008aa2e0, 1, 0x008aa2e0);
int FUN_00462760(void);
void FUN_00463150(int a);
void FUN_005a86a0(int h, MatchDesc_4656e0 *d);
void FUN_005a8960(int h, int a);
int FUN_005a89a0(int h);
// MATCH: 0x004656e0 FUN_004656e0
int FUN_004656e0(int param_1)
{
  MatchDesc_4656e0 d;

  if (DAT_0088f680[param_1] == 0) {
    return 0;
  }
  if (FUN_00462760() != 0) {
    return 0;
  }
  if (*(int *)(DAT_0088f680[param_1] + 0xc) == 0) {
    if (FUN_005a89a0(DAT_0069045c[param_1]) != 6) {
      d.p = &d.parm;
      d.n = 1;
      d.parm.a = 3;
      d.parm.c = 2;
      d.parm.b = 0;
      d.parm.v = *(int *)(DAT_0088f680[param_1] + 8);
      FUN_005a86a0(DAT_0069045c[param_1], &d);
      FUN_005a8960(DAT_0069045c[param_1], 2);
      *(int *)(DAT_0088f680[param_1] + 0xc) = 1;
      DAT_008aa2e0[param_1] = 1;
      FUN_00463150(0);
    }
  }
  return 1;
}

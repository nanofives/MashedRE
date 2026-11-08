// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005aa7a0 (h2): best 82.1% (regs 82.1%), class a. original duplicates pop/pop/ret inline for the early return, DDK shares the tail; 8 source forms + permuter. relocheck BAD only because the code differs from the original. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
uint FUN_005a9e60(uint p0, undefined4 p1, int p2);
void FUN_005a9f10(int p0, undefined4 p1);
// MATCH: 0x005aa7a0 FUN_005aa7a0
void FUN_005aa7a0(int param_1,uint *param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;

  uVar2 = *param_2;
  if (*(int *)(param_1 + 0x48) == 0) {
    if (FUN_005a9e60(param_1,param_4,uVar2 >> 2 & 1) == 0) {
      return;
    }
  }
  FUN_005a9f10(param_1,param_4);
}

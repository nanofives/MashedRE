// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a9f10 (h2): best 73.9% (regs 73.9%), class b. 2nd branch (1-arg callee) not emitted as tail jump, jns vs jge; flags test as uint. relocheck BAD only because the code differs from the original. */
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
void FUN_005ae800(undefined4 p0, undefined4 p1);
void FUN_005ae3a0(undefined4 p0);
void FUN_005aea00(undefined4 p0, undefined4 p1);
// MATCH: 0x005a9f10 FUN_005a9f10
void FUN_005a9f10(int param_1,undefined4 param_2)

{
  uint uVar1;

  uVar1 = *(uint *)(param_1 + 0xc);
  if ((uVar1 & 0x40) != 0) {
    FUN_005ae800(*(undefined4 *)(param_1 + 0x48),*(undefined4 *)(param_1 + 0x58));
  }
  else if ((char)uVar1 >= 0) {
    FUN_005aea00(param_2,*(undefined4 *)(param_1 + 0x58));
  }
  else {
    FUN_005ae3a0(*(undefined4 *)(param_1 + 0x48));
  }
}

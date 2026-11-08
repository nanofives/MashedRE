// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00578ec0: hand-matched (h2). 100.0% regs 100.0%. Fixed callee 0x00564190 prototype to 1 arg; guard 'uVar2 < (uint)n' (not != 0) gives jbe; --iVar1 form gives dec/jne. */
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
typedef int (__cdecl *code_fn)(int *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_0055bad0(int p0);
void FUN_00564190(int p0);
// MATCH: 0x00578ec0 FUN_00578ec0
undefined4 FUN_00578ec0(int *param_1)
{
  int iVar1;
  uint uVar2;

  iVar1 = param_1[0x19];
  param_1[0x19] = --iVar1;
  if (iVar1 == 0) {
    uVar2 = 0;
    if (uVar2 < (uint)param_1[1]) {
      iVar1 = 0;
      do {
        FUN_0055bad0(*param_1 + iVar1);
        uVar2 = uVar2 + 1;
        iVar1 = iVar1 + 0x60;
      } while (uVar2 < (uint)param_1[1]);
    }
    FUN_00564190(*param_1);
    param_1[1] = 0;
    (*(code_fn *)(DAT_007d3ff8 + 0x10c))(param_1);
  }
  return 1;
}

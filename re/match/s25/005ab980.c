// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005ab980 (h2): best 100.0% (regs 100.0%), class -. memcpy intrinsic, uVar1 >= count compare direction. relocheck. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int, DAT_007dd610, 0x007dd610);
MGLOBAL(int, DAT_007dd614, 0x007dd614);
MGLOBALA(undefined1, DAT_007dce08, 1, 0x007dce08);
void * __cdecl memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x005ab980 FUN_005ab980
void FUN_005ab980(int param_1)

{
  uint uVar1;
  int iVar3;

  iVar3 = *(int *)(param_1 + 0xc) - DAT_007dd610;
  uVar1 = DAT_007dd614 - iVar3;
  if (uVar1 >= *(uint *)(param_1 + 0x14)) {
    uVar1 = *(uint *)(param_1 + 0x14);
  }
  memcpy(*(void **)(param_1 + 0x18),&DAT_007dce08[iVar3],uVar1);
  *(uint *)(param_1 + 0x18) = *(int *)(param_1 + 0x18) + uVar1;
  *(uint *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) - uVar1;
  return;
}

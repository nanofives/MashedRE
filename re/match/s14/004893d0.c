// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004893d0 (h5): raw g2s body, condition rewritten `param_1[2] >= param_1[1]` (raw `param_1[1] <= param_1[2]`
   swapped cmp operands and branch sense, 93.2%). 100% (regs 100%). */
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
// MATCH: 0x004893d0 FUN_004893d0
void FUN_004893d0(int *param_1,undefined4 *param_2,undefined1 *param_3)

{
  undefined4 *puVar1;
  int iVar2;
  
  if (param_1[2] >= param_1[1]) {
    param_1[2] = param_1[1];
    return;
  }
  iVar2 = param_1[2] * 0x30;
  puVar1 = (undefined4 *)(*param_1 + 4 + iVar2);
  *puVar1 = *param_2;
  puVar1[1] = param_2[1];
  puVar1[2] = param_2[2];
  *(undefined4 *)(*param_1 + 0x1c + iVar2) = 0;
  *(undefined1 *)(iVar2 + *param_1) = *param_3;
  *(undefined1 *)(*param_1 + 1 + iVar2) = param_3[1];
  *(undefined1 *)(*param_1 + 2 + iVar2) = param_3[2];
  *(undefined1 *)(*param_1 + 3 + iVar2) = param_3[3];
  param_1[2] = param_1[2] + 1;
  param_1[0x15] = 1;
  return;
}

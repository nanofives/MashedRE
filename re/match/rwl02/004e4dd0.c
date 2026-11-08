// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e4dd0: best 100.0% (regs 100.0%), residual class -. permuter moved puVar3[2]=3 after the +0x3c store (same semantics) to get dx for the dec. */
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

MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d71a0, 0x007d71a0);
MGLOBAL(undefined4, DAT_0061862c, 0x0061862c);
void FUN_004d7ff0(void);
int FUN_004d8000(undefined4 *p0, undefined1 *p1);
// MATCH: 0x004e4dd0 FUN_004e4dd0
undefined1 * FUN_004e4dd0(undefined1 param_1)
{
  undefined1 *puVar1;
  short sVar2;
  undefined1 *puVar3;
  puVar3 = (undefined1 *)(**(int (**)(int,int))(DAT_007d3ff8 + 0x118))(*(int *)(DAT_007d71a0 + DAT_007d3ff8),0x30012);
  if (puVar3 == (undefined1 *)0) {
    return (undefined1 *)0;
  }
  *puVar3 = 3;
  puVar3[1] = param_1;
  *(undefined4 *)(puVar3 + 0x18) = 0x3f800000;
  *(undefined4 *)(puVar3 + 0x1c) = 0x3f800000;
  *(undefined4 *)(puVar3 + 0x20) = 0x3f800000;
  *(undefined4 *)(puVar3 + 0x24) = 0x3f800000;
  puVar1 = puVar3 + 0x2c;
  puVar3[2] = 0;
  *(undefined4 *)(puVar3 + 4) = 0;
  *(void (**)(void))(puVar3 + 0x10) = FUN_004d7ff0;
  *(undefined4 *)(puVar3 + 0x14) = 0;
  *(undefined4 *)(puVar3 + 0x28) = 0;
  puVar3[3] = 1;
  *(undefined1 **)puVar1 = puVar1;
  *(undefined1 **)(puVar3 + 0x30) = puVar1;
  *(undefined4 *)(puVar3 + 0x38) = 0;
  *(undefined4 *)(puVar3 + 0x34) = 0;
  sVar2 = *(short *)(DAT_007d3ff8 + 10);
  sVar2--;
  *(short *)(puVar3 + 0x3c) = sVar2;
  puVar3[2] = 3;
  FUN_004d8000(&DAT_0061862c,puVar3);
  return puVar3;
}

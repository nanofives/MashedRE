// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWR02 2026-11-08: 100% (relocheck OK). The +0x64 list head init (next then prev, both = head) moved after the +0x6c store; supersedes the header below. */
/* MATCH-RWL02 h1 0x004e67b0: best 97.1% (regs 97.1%), residual class b. permuter swapped two independent stores (+0x68 and *puVar1). The callback at +0x48 is stored, tested against zero and stored again as in the original (`if (FUN_004e5f90 == 0)` dummy test reproduces the duplicate store); remaining: push order of arg setup. */
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
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
void FUN_004e5f90(void);
void FUN_004e6880(void);
void FUN_004c0740(undefined1 *p0, int p1);
int FUN_004d8000(undefined4 *p0, undefined1 *p1);
// MATCH: 0x004e67b0 FUN_004e67b0
undefined1 * FUN_004e67b0(void)
{
  undefined1 *puVar1;
  undefined1 *puVar2;
  puVar2 = (undefined1 *)(**(int (**)(int,int))(DAT_007d3ff8 + 0x118))(*(int *)(DAT_007d7200 + DAT_007d3ff8),0x30014);
  if (puVar2 == (undefined1 *)0) {
    return (undefined1 *)0;
  }
  *puVar2 = 1;
  puVar2[1] = 0;
  *(undefined4 *)(puVar2 + 4) = 0;
  *(void (**)(void))(puVar2 + 0x10) = FUN_004e6880;
  *(undefined4 *)(puVar2 + 0x14) = 0;
  puVar2[2] = 5;
  puVar2[3] = 1;
  FUN_004c0740(puVar2,0);
  *(undefined4 *)(puVar2 + 0x18) = 0;
  puVar2[3] = puVar2[3] | 1;
  *(undefined4 *)(puVar2 + 0x28) = 0;
  *(undefined4 *)(puVar2 + 0x1c) = 0;
  *(undefined4 *)(puVar2 + 0x20) = 0;
  *(undefined4 *)(puVar2 + 0x24) = 0;
  *(undefined4 *)(puVar2 + 0x38) = 0;
  *(undefined4 *)(puVar2 + 0x2c) = 0;
  *(undefined4 *)(puVar2 + 0x30) = 0;
  *(undefined4 *)(puVar2 + 0x34) = 0;
  *(void (**)(void))(puVar2 + 0x48) = FUN_004e5f90;
  if (FUN_004e5f90 == 0) { *(void (**)(void))(puVar2 + 0x48) = FUN_004e5f90; }
  *(undefined4 *)(puVar2 + 0x54) = 0x3f800000;
  *(undefined4 *)(puVar2 + 0x58) = 0x3f800000;
  *(undefined2 *)(puVar2 + 0x50) = 0;
  *(undefined2 *)(puVar2 + 0x52) = 0;
  *(undefined4 *)(puVar2 + 0x5c) = 0;
  *(undefined4 *)(puVar2 + 0x4c) = 3;
  *(undefined4 *)(puVar2 + 0x44) = 0;
  *(undefined4 *)(puVar2 + 0x40) = 0;
  *(undefined4 *)(puVar2 + 0x3c) = 0;
  *(undefined4 *)(puVar2 + 0x6c) = 0;
  *(undefined1 **)(puVar2 + 100) = puVar2 + 100;
  *(undefined1 **)(puVar2 + 0x68) = puVar2 + 100;
  FUN_004d8000(&DAT_00618664,puVar2);
  return puVar2;
}

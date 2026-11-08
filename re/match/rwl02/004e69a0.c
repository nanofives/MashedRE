// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e69a0: best 100.0% (regs 100.0%), residual class -. 16- and 20-byte struct copies (S16/S20) give the lea/mov pairs and rep movsd. */
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

typedef struct S16 { int a, b, c, d; } S16;
typedef struct S20 { int a, b, c, d, e; } S20;
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
void FUN_004c0740(undefined1 *p0, int p1);
void FUN_004c0e50(int p0);
int FUN_004d8090(undefined4 *p0, undefined1 *p1, undefined1 *p2);
undefined4 FUN_004e4440(undefined1 *p0);
void FUN_004e8e90(int p0);
undefined4 FUN_004e8ea0(int p0);
undefined1 * FUN_004e67b0(void);
// MATCH: 0x004e69a0 FUN_004e69a0
undefined1 * FUN_004e69a0(undefined1 *param_1)
{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  puVar1 = FUN_004e67b0();
  if (puVar1 == (undefined1 *)0) {
    return (undefined1 *)0;
  }
  *puVar1 = *param_1;
  puVar1[1] = param_1[1];
  puVar1[2] = param_1[2];
  puVar1[3] = param_1[3];
  *(undefined4 *)(puVar1 + 4) = 0;
  puVar1[2] = param_1[2];
  FUN_004c0740(puVar1,0);
  puVar1[3] = puVar1[3] | 1;
  iVar3 = *(int *)(param_1 + 0x18);
  if (iVar3 != *(int *)(puVar1 + 0x18)) {
    if (iVar3 != 0) {
      FUN_004e8e90(iVar3);
    }
    if (*(int *)(puVar1 + 0x18) != 0) {
      FUN_004e8ea0(*(int *)(puVar1 + 0x18));
    }
    *(int *)(puVar1 + 0x18) = iVar3;
    if (iVar3 != 0) {
      iVar3 = *(int *)(iVar3 + 0x5c);
      *(S16 *)(puVar1 + 0x1c) = *(S16 *)(iVar3 + 4);
    }
    iVar3 = *(int *)(puVar1 + 4);
    if (iVar3 != 0) {
      iVar2 = FUN_004e4440(puVar1);
      if (iVar2 != 0) {
        FUN_004c0e50(iVar3);
      }
    }
  }
  *(S16 *)(puVar1 + 0x1c) = *(S16 *)(param_1 + 0x1c);
  *(undefined4 *)(puVar1 + 0x48) = *(undefined4 *)(param_1 + 0x48);
  *(undefined4 *)(puVar1 + 0x6c) = *(undefined4 *)(param_1 + 0x6c);
  *(S20 *)(puVar1 + 0x4c) = *(S20 *)(param_1 + 0x4c);
  FUN_004d8090(&DAT_00618664,puVar1,param_1);
  return puVar1;
}

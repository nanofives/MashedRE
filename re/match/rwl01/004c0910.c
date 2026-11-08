// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c0910: best 82.0% (regs 82%, ops 94.6%), class (b). 64-byte payload copy as a Blob64 struct assignment (rep movsd); residual is prologue/register scheduling of param_1/param_2 (permuter 219 compiles, no gain). */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3e70, 0x007d3e70);
MGLOBAL(undefined4, DAT_00617f78, 0x00617f78);
void FUN_004c0d70(int p0);
int FUN_004d8000(int p0, undefined1 * p1);
int FUN_004d8060(int p0, undefined1 * p1);
int FUN_004d8090(int p0, undefined1 * p1, undefined1 * p2);
typedef struct Blob64 { undefined4 a[16]; } Blob64;
// MATCH: 0x004c0910 FUN_004c0910
undefined1 * FUN_004c0910(undefined1 *param_1,undefined1 *param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  int iVar4;
  
  puVar2 = (undefined1 *)
           (**(code **)(DAT_007d3ff8 + 0x118))(*(undefined4 *)(DAT_007d3e70 + DAT_007d3ff8),0x3000e)
  ;
  if (puVar2 == (undefined1 *)0x0) {
    return (undefined1 *)0x0;
  }
  if (param_2 == (undefined1 *)0x0) {
    param_2 = puVar2;
  }
  *puVar2 = *param_1;
  puVar2[1] = param_1[1];
  puVar2[2] = param_1[2];
  puVar1 = puVar2 + 0x90;
  puVar2[3] = param_1[3];
  *(undefined1 **)(puVar2 + 0x94) = puVar1;
  *(undefined1 **)puVar1 = puVar1;
  *(undefined4 *)(puVar2 + 4) = 0;
  *(Blob64 *)(puVar2 + 0x10) = *(Blob64 *)(param_1 + 0x10);
  *(undefined4 *)(puVar2 + 0x98) = 0;
  *(undefined4 *)(puVar2 + 0x9c) = 0;
  *(undefined1 **)(puVar2 + 0xa0) = param_2;
  iVar4 = *(int *)(param_1 + 0x98);
  *(undefined1 **)(param_1 + 0xa0) = puVar2;
  while( true ) {
    if (iVar4 == 0) {
      FUN_004d8000((int)&DAT_00617f78,puVar2);
      FUN_004d8090((int)&DAT_00617f78,puVar2,param_1);
      return puVar2;
    }
    iVar3 = (int)FUN_004c0910((undefined1 *)iVar4,param_2);
    if (iVar3 == 0) break;
    *(undefined4 *)(iVar3 + 0x9c) = *(undefined4 *)(puVar2 + 0x98);
    *(int *)(puVar2 + 0x98) = iVar3;
    *(undefined1 **)(iVar3 + 4) = puVar2;
    iVar4 = *(int *)(iVar4 + 0x9c);
  }
  iVar4 = *(int *)(puVar2 + 0x98);
  while (iVar4 != 0) {
    iVar3 = *(int *)(iVar4 + 0x9c);
    FUN_004c0d70(iVar4);
    iVar4 = iVar3;
  }
  FUN_004d8060((int)&DAT_00617f78,puVar2);
  if ((puVar2[3] & 3) != 0) {
    **(undefined4 **)(puVar2 + 0xc) = *(undefined4 *)(puVar2 + 8);
    *(undefined4 *)(*(int *)(puVar2 + 8) + 4) = *(undefined4 *)(puVar2 + 0xc);
  }
  (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d3e70 + DAT_007d3ff8),puVar2);
  return (undefined1 *)0x0;
}

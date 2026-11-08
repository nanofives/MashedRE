// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x0053fbb0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef unsigned char byte;
#ifdef __cplusplus
typedef int (*codeptr)(...);
#else
typedef int (*codeptr)();
#endif
MGLOBAL(int, DAT_007dc5dc, 0x007dc5dc);
MGLOBAL(undefined4, DAT_007dc5e8, 0x007dc5e8);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d8560(int p0, int p1);
undefined4 * FUN_0053eca0(undefined4 * p0);
// MATCH: 0x0053fbb0 FUN_0053fbb0
int FUN_0053fbb0(uint param_1,int param_2)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  byte bVar4;
  undefined4 *puVar5;
  
  iVar1 = param_1;
  puVar2 = *(undefined4 **)(DAT_007dc5dc + param_1);
  if (puVar2 == (undefined4 *)0x0) {
    puVar2 = (undefined4 *)(**(codeptr *)(DAT_007d3ff8 + 0x118))(DAT_007dc5e8,0x30120);
    if (puVar2 == (undefined4 *)0x0) {
      return 0;
    }
    puVar5 = puVar2;
    for (iVar3 = 0xd; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = 0;
      puVar5 = puVar5 + 1;
    }
    *(undefined4 **)(DAT_007dc5dc + param_1) = puVar2;
  }
  if ((param_2 == 0) || ((puVar2[0xc] != 0 && (puVar2[0xc] != param_2)))) {
    FUN_0053eca0(puVar2);
  }
  puVar2[0xc] = param_2;
  switch(param_2) {
  case 1:
    puVar2[5] = 1;
    return iVar1;
  case 2:
    puVar2[5] = 2;
    return iVar1;
  case 3:
    puVar2[5] = 1;
    puVar2[0xb] = 2;
    return iVar1;
  case 4:
    bVar4 = 0;
    puVar2[5] = 4;
    *(byte *)&param_1 = 0;
    iVar3 = *(int *)(DAT_007dc5dc + iVar1);
    do {
      if (*(int *)(iVar3 + 0x14 + (param_1 & 0xff) * 0x18) == 4) goto LAB_0053fc99;
      bVar4 = bVar4 + 1;
      *(byte *)&param_1 = bVar4;
    } while (bVar4 < 2);
    break;
  case 5:
    puVar2[5] = 5;
    return iVar1;
  case 6:
    puVar2[5] = 5;
    puVar2[0xb] = 4;
    bVar4 = 0;
    iVar3 = *(int *)(DAT_007dc5dc + param_1);
    *(byte *)&param_1 = 0;
    do {
      if (*(int *)(iVar3 + 0x14 + (param_1 & 0xff) * 0x18) == 4) goto LAB_0053fc99;
      bVar4 = bVar4 + 1;
      *(byte *)&param_1 = bVar4;
    } while (bVar4 < 2);
    break;
  default:
    goto switchD_0053fc29_default;
  }
  iVar3 = 0;
LAB_0053fcec:
  *(undefined4 *)(iVar3 + 8) = 6;
  *(undefined4 *)(iVar3 + 4) = 5;
  FUN_004d8560(iVar3,10);
  FUN_004d8560(iVar3,0xb);
switchD_0053fc29_default:
  return iVar1;
LAB_0053fc99:
  iVar3 = iVar3 + (param_1 & 0xff) * 0x18;
  goto LAB_0053fcec;
}

// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d80d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 * FUN_004c4600(undefined4 * p0, undefined4 * p1, undefined4 * p2);
void FUN_004d8280(int p0, uint p1);
void FUN_004d8300(int p0);
// MATCH: 0x004d80d0 FUN_004d80d0
undefined4 FUN_004d80d0(void)

{
  byte bVar1;
  byte bVar2;
  undefined4 *puVar3;
  undefined4 *puVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  
  puVar4 = (undefined4 *)(DAT_007d3ff8 + 0xbc);
  for (puVar3 = (undefined4 *)*puVar4; puVar3 != puVar4; puVar3 = (undefined4 *)*puVar3) {
    bVar1 = *(byte *)((int)puVar3 + -5);
    if ((bVar1 & 1) == 0) {
      for (puVar6 = (undefined4 *)puVar3[0x22]; puVar6 != puVar3 + 0x22;
          puVar6 = (undefined4 *)*puVar6) {
        (*(code *)puVar6[2])(puVar6 + -2);
      }
      for (iVar5 = puVar3[0x24]; iVar5 != 0; iVar5 = *(int *)(iVar5 + 0x9c)) {
        for (puVar6 = *(undefined4 **)(iVar5 + 0x90); puVar6 != (undefined4 *)(iVar5 + 0x90);
            puVar6 = (undefined4 *)*puVar6) {
          (*(code *)puVar6[2])(puVar6 + -2);
        }
        *(byte *)(iVar5 + 3) = *(byte *)(iVar5 + 3) & 0xf7;
        FUN_004d8300(*(undefined4 *)(iVar5 + 0x98));
      }
    }
    else {
      if ((bVar1 & 4) != 0) {
        puVar6 = puVar3 + 2;
        puVar7 = puVar3 + 0x12;
        for (iVar5 = 0x10; iVar5 != 0; iVar5 = iVar5 + -1) {
          *puVar7 = *puVar6;
          puVar6 = puVar6 + 1;
          puVar7 = puVar7 + 1;
        }
      }
      for (puVar6 = (undefined4 *)puVar3[0x22]; puVar6 != puVar3 + 0x22;
          puVar6 = (undefined4 *)*puVar6) {
        (*(code *)puVar6[2])(puVar6 + -2);
      }
      for (iVar5 = puVar3[0x24]; iVar5 != 0; iVar5 = *(int *)(iVar5 + 0x9c)) {
        bVar2 = *(byte *)(iVar5 + 3);
        if ((bVar2 & 4) != 0 || (bVar1 & 4) != 0) {
          FUN_004c4600(iVar5 + 0x50,iVar5 + 0x10,*(int *)(iVar5 + 4) + 0x50);
        }
        for (puVar6 = *(undefined4 **)(iVar5 + 0x90); puVar6 != (undefined4 *)(iVar5 + 0x90);
            puVar6 = (undefined4 *)*puVar6) {
          (*(code *)puVar6[2])(puVar6 + -2);
        }
        *(byte *)(iVar5 + 3) = *(byte *)(iVar5 + 3) & 0xf3;
        FUN_004d8280(*(undefined4 *)(iVar5 + 0x98),bVar2 | bVar1 & 4);
      }
    }
    *(byte *)((int)puVar3 + -5) = bVar1 & 0xf0;
  }
  *(int *)(DAT_007d3ff8 + 0xbc) = DAT_007d3ff8 + 0xbc;
  *(int *)(DAT_007d3ff8 + 0xc0) = DAT_007d3ff8 + 0xbc;
  return 1;
}

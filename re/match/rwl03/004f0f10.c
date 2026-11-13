// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f0f10: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912118, 0x00912118);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
int FUN_004f3bd0(undefined4 p0, int p1);
// MATCH: 0x004f0f10 FUN_004f0f10
undefined4 * FUN_004f0f10(undefined4 param_1,char *param_2,undefined4 param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 *puVar3;
  int *piVar4;
  uint uVar5;
  uint uVar6;
  undefined2 *puVar7;
  int *piVar8;
  uint uVar9;
  struct { undefined4 a; int b; int c; } H;
  undefined4 *puStack_40c;
  struct { int a; undefined4 b; } P;
  undefined2 auStack_400 [512];
  
  iVar2 = FUN_004cc790(param_1,(undefined4)&H,0xc);
  if (iVar2 == 0) {
    return (undefined4 *)0x0;
  }
  iVar2 = (H.b + 1) * 0x10;
  if (((*param_2 == '\b') && ((*(uint *)(param_2 + 8) & 0x1000000) == 0)) ||
     ((*param_2 == '\a' && ((*(uint *)(param_2 + 8) & 0x1000000) == 0)))) {
    iVar2 = iVar2 + H.c * 2;
  }
  puVar3 = (undefined4 *)(**(code **)(DAT_007d3ff8 + 0x108))(iVar2,0x30502);
  if (puVar3 != (undefined4 *)0x0) {
    piVar8 = (int *)(puVar3 + 4);
    *puVar3 = H.a;
    *(undefined2 *)(puVar3 + 1) = (undefined2)H.b;
    piVar4 = piVar8 + H.b * 3;
    *(undefined2 *)((int)puVar3 + 6) = *(undefined2 *)(DAT_00912118 + DAT_007d3ff8);
    puVar3[2] = H.c;
    puVar3[3] = 0;
    *(short *)(DAT_00912118 + DAT_007d3ff8) = *(short *)(DAT_00912118 + DAT_007d3ff8) + 1;
    uVar5 = (uint)*(ushort *)(puVar3 + 1);
    puStack_40c = puVar3;
    while (uVar5 != 0) {
      uVar5 = uVar5 - 1;
      iVar2 = FUN_004cc790(param_1,(undefined4)&P,8);
      if (iVar2 == 0) {
        return (undefined4 *)0x0;
      }
      piVar8[1] = P.a;
      iVar2 = FUN_004f3bd0(param_3,P.b);
      piVar8[2] = iVar2;
      *piVar8 = (int)piVar4;
      if (((*param_2 == '\b') && ((*(uint *)(param_2 + 8) & 0x1000000) == 0)) ||
         ((*param_2 == '\a' && ((*(uint *)(param_2 + 8) & 0x1000000) == 0)))) {
        uVar6 = piVar8[1];
        while (uVar6 != 0) {
          puVar7 = auStack_400;
          uVar9 = uVar6;
          if (0xff < uVar6) {
            uVar9 = 0x100;
          }
          iVar2 = FUN_004cc790(param_1,(undefined4)auStack_400,uVar9 * 4);
          if (iVar2 == 0) {
            return (undefined4 *)0x0;
          }
          uVar6 = uVar6 - uVar9;
          for (; puVar3 = puStack_40c, uVar9 != 0; uVar9 = uVar9 - 1) {
            uVar1 = *puVar7;
            puVar7 = puVar7 + 2;
            *(undefined2 *)piVar4 = uVar1;
            piVar4 = (int *)((int)piVar4 + 2);
          }
        }
      }
      piVar8 = piVar8 + 3;
    }
  }
  return puVar3;
}

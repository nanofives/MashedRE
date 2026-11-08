// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x004f3440: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifdef __cplusplus
typedef int (__cdecl code)(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912118, 0x00912118);
// MATCH: 0x004f3440 FUN_004f3440
undefined4 FUN_004f3440(int *param_1,int param_2)

{
  int *piVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  int *piVar7;
  int *piVar8;
  
  if (*param_1 == 0) {
    return 0;
  }
  piVar4 = (int *)(**(code **)(DAT_007d3ff8 + 0x118))
                            (*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),0x30502);
  piVar4[1] = 0;
  piVar4[2] = 0;
  for (iVar5 = *param_1; iVar5 != 0; iVar5 = *(int *)(iVar5 + 0xc)) {
    piVar4[2] = piVar4[2] + *(int *)(iVar5 + 4) + 6;
  }
  iVar5 = (**(code **)(DAT_007d3ff8 + 0x108))(piVar4[2] << 2,0x30502);
  *piVar4 = iVar5;
  piVar8 = (int *)*param_1;
  uVar6 = 0;
  if (0 < (uint)piVar8[1]) {
    do {
      *(undefined2 *)(*piVar4 + piVar4[1] * 2) = *(undefined2 *)(*piVar8 + uVar6 * 2);
      piVar4[1] = piVar4[1] + 1;
      uVar6 = uVar6 + 1;
    } while (uVar6 < (uint)piVar8[1]);
  }
  (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar8);
  piVar7 = (int *)piVar8[3];
  *piVar8 = 0;
  (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),piVar8);
  do {
    if (piVar7 == (int *)0x0) {
      *param_1 = (int)piVar4;
      piVar4[3] = 0;
      return 1;
    }
    if (piVar7 != (int *)0x0) {
      uVar6 = piVar4[1];
      piVar8 = piVar7;
      do {
        if (*(short *)(*piVar4 + -2 + uVar6 * 2) == *(short *)*piVar8) {
          if (((uVar6 & 1) == 0) || (param_2 == 0)) goto LAB_004f3619;
          *(undefined2 *)(*piVar4 + uVar6 * 2) = *(undefined2 *)*piVar8;
          iVar5 = piVar4[1] + 1;
          goto LAB_004f3616;
        }
        piVar8 = (int *)piVar8[3];
      } while (piVar8 != (int *)0x0);
    }
    if (piVar7 != (int *)0x0) {
      uVar6 = piVar4[1];
      iVar5 = *piVar4;
      piVar8 = piVar7;
      do {
        if (*(short *)(iVar5 + -4 + uVar6 * 2) == *(short *)*piVar8) {
          if (((uVar6 & 1) != 0) || (param_2 == 0)) {
            *(undefined2 *)(iVar5 + uVar6 * 2) = *(undefined2 *)*piVar8;
            iVar5 = piVar4[1] + 1;
            goto LAB_004f3616;
          }
          *(undefined2 *)(iVar5 + uVar6 * 2) = *(undefined2 *)*piVar8;
          *(undefined2 *)(*piVar4 + 2 + piVar4[1] * 2) = *(undefined2 *)*piVar8;
          goto LAB_004f3610;
        }
        piVar8 = (int *)piVar8[3];
      } while (piVar8 != (int *)0x0);
    }
    uVar6 = piVar4[1];
    if (((uVar6 & 1) == 0) || (param_2 == 0)) {
      *(undefined2 *)(*piVar4 + 2 + piVar4[1] * 2) = *(undefined2 *)*piVar7;
      *(undefined2 *)(*piVar4 + uVar6 * 2) = *(undefined2 *)(*piVar4 + -2 + uVar6 * 2);
      piVar8 = piVar7;
LAB_004f3610:
      iVar5 = piVar4[1] + 2;
    }
    else {
      *(undefined2 *)(*piVar4 + uVar6 * 2) = *(undefined2 *)(*piVar4 + -2 + uVar6 * 2);
      *(undefined2 *)(*piVar4 + 2 + piVar4[1] * 2) = *(undefined2 *)*piVar7;
      *(undefined2 *)(*piVar4 + 4 + piVar4[1] * 2) = *(undefined2 *)*piVar7;
      iVar5 = piVar4[1] + 3;
      piVar8 = piVar7;
    }
LAB_004f3616:
    piVar4[1] = iVar5;
LAB_004f3619:
    uVar6 = 0;
    if (0 < (uint)piVar8[1]) {
      do {
        *(undefined2 *)(*piVar4 + piVar4[1] * 2) = *(undefined2 *)(*piVar8 + uVar6 * 2);
        piVar4[1] = piVar4[1] + 1;
        uVar6 = uVar6 + 1;
      } while (uVar6 < (uint)piVar8[1]);
    }
    (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar8);
    *piVar8 = 0;
    if (piVar7 == piVar8) {
      piVar7 = (int *)piVar7[3];
    }
    else {
      piVar3 = (int *)piVar7[3];
      piVar2 = piVar7;
      while (piVar1 = piVar3, piVar1 != piVar8) {
        piVar2 = piVar1;
        piVar3 = (int *)piVar1[3];
      }
      piVar2[3] = piVar8[3];
    }
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_00912118 + 4 + DAT_007d3ff8),piVar8);
  } while( true );
}

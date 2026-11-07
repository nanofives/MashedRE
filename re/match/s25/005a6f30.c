// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 NOTE: permuter best 65.4% (regs 70.9%, ops 80.2%, 223/237 ins), raw converter start. NOT port-ready: the VS2022 C++ check FAILS (callee prototypes of FUN_005a6dc0/005a6df0 are 4-arg vararg-style calls with float args passed by value via fld/fstp, FUN_005a6e10/005a7460 types, DAT_007dca18 is a list head pointer, ROUND() is an unresolved converter macro that the DDK compiles as an implicit call, and the goto LAB_005a71bf block drops the puVar5[8]/[0xb] stores). Residual (b): needs a hand rewrite of the float-arg calls and the loop; budget spent. */
/* MATCH-S25 0x005a6f30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char byte;
MGLOBAL(int, DAT_007dca34, 0x007dca34);
MGLOBAL(undefined1, DAT_007dca44, 0x007dca44);
MGLOBAL(undefined1, DAT_007dca48, 0x007dca48);
MGLOBAL(undefined1, DAT_007dca4c, 0x007dca4c);
MGLOBAL(undefined1, DAT_00632f08, 0x00632f08);
MGLOBAL(undefined4, DAT_007dca18, 0x007dca18);
MGLOBAL(int, DAT_007dca40, 0x007dca40);
MGLOBAL(int, DAT_007dca38, 0x007dca38);
MGLOBAL(int, DAT_007dca50, 0x007dca50);
MGLOBAL(undefined1, DAT_005cc558, 0x005cc558);
uint FUN_005a6d60(undefined4 * p0, int p1, undefined4 p2, undefined4 p3);
void FUN_005a6dc0(int p0, undefined4 p1, undefined4 p2);
void FUN_005a6df0(int p0);
undefined4 FUN_005a6e10(undefined4 p0, undefined4 p1, undefined4 p2, uint * p3);
void FUN_005a7460(int * p0);
uint FUN_005acda0(int p0, uint p1);
MGLOBAL(int, _DAT_007dca44, 0x007dca44);
MGLOBAL(int, _DAT_007dca48, 0x007dca48);
MGLOBAL(int, _DAT_007dca4c, 0x007dca4c);
MGLOBAL(int, _DAT_00632f08, 0x00632f08);
MGLOBAL(int, _DAT_005cc558, 0x005cc558);
// MATCH: 0x005a6f30 FUN_005a6f30
/* [C2 2026-06-01] Audio mixer flush / per-frame voice-allocation tick. `void FUN_005a6f30(void)`.
   Gated on the mixer object `DAT_007dca34
 */

void FUN_005a6f30(void)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  undefined4 *puVar5;
  uint uVar6;
  int local_14;
  
  if (DAT_007dca34 != 0) {
    iVar2 = FUN_005a6e10(DAT_007dca34,1,2,0);
    if (iVar2 != 0) {
      _DAT_007dca48 = *(undefined4 *)(iVar2 + 0x2c);
      _DAT_007dca44 = *(undefined4 *)(iVar2 + 0x28);
      _DAT_007dca4c = *(undefined4 *)(iVar2 + 0x30);
      _DAT_00632f08 = *(undefined4 *)(iVar2 + 100);
    }
    local_14 = 0;
    puVar5 = DAT_007dca18;
    if ((undefined4 **)DAT_007dca18 != &DAT_007dca18) {
      do {
        iVar2 = puVar5[2];
        puVar1 = (undefined4 *)*puVar5;
        if (local_14 < DAT_007dca40 - DAT_007dca38) {
          if (!((puVar5[3] & 0x10) == 0)) {
            FUN_005a7460(puVar5);
          } else {
            if (iVar2 != 0) {
              if (!((puVar5[3] & 8) == 0)) {
                puVar5[8] = puVar5[9];
              } else {
                uVar3 = FUN_005a6e10(iVar2,7,2,0);
                puVar5[8] = uVar3;
              }
              FUN_005a6dc0(iVar2,6,1,0);
              FUN_005a6df0(iVar2,9,1,0);
              puVar5[0xb] = DAT_007dca50;
              puVar5[2] = 0;
            }
          }
          if (((puVar5[3] & 1) != 0) && ((puVar5[3] & 8) == 0)) {
            uVar4 = FUN_005acda0(puVar5[10] + 0x10,*(undefined4 *)(puVar5[10] + 0x18));
            uVar6 = puVar5[8] +
                    (int)ROUND((float)(uint)(DAT_007dca50 - puVar5[0xb]) * (float)puVar5[5] *
                               _DAT_005cc558);
            puVar5[8] = uVar6;
            if (uVar4 <= uVar6) {
              if ((*(byte *)(puVar5 + 3) & 4) != 0) {
                do {
                  uVar6 = uVar6 - uVar4;
                } while (uVar4 <= uVar6);
                goto LAB_005a71bf;
                puVar5[8] = uVar6;
                puVar5[0xb] = DAT_007dca50;
              }
              FUN_005a7460(puVar5);
              puVar5[2] = 0;
            }
            puVar5[0xb] = DAT_007dca50;
          }
        }
        else if (iVar2 == 0) {
          iVar2 = FUN_005a6e10(DAT_007dca34,4,2,0);
          puVar5[2] = iVar2;
          if (iVar2 != 0) {
            FUN_005a6df0(iVar2,9,1,puVar5[10]);
            FUN_005a6dc0(iVar2,4,1,puVar5[5]);
            FUN_005a6dc0(iVar2,5,1,puVar5[6]);
            FUN_005a6dc0(iVar2,8,1,(uint)puVar5[3] >> 2 & 1);
            if ((*(byte *)(puVar5 + 3) & 8) == 0) {
              FUN_005a6dc0(iVar2,7,1,puVar5[8]);
            }
            FUN_005a6dc0(iVar2,10,1,puVar5[7]);
            FUN_005a6d60(iVar2,0,1,puVar5 + 0xc);
            FUN_005a6d60(iVar2,1,1,puVar5 + 0xf);
            FUN_005a6dc0(iVar2,2,1,puVar5[0x12]);
            FUN_005a6dc0(iVar2,3,1,puVar5[0x13]);
            FUN_005a6dc0(iVar2,0xb,1,puVar5[0x18]);
            FUN_005a6dc0(iVar2,6,1,1);
          }
        }
LAB_005a71bf:
        local_14 = local_14 + puVar5[4];
        puVar5 = puVar1;
      } while ((undefined4 **)puVar1 != &DAT_007dca18);
    }
  }
  return;
}

// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 28.3% (regs 65.8%), class b. Raw converter output with prototype/cast fixes for the C++ check only (ops 83% = same instruction multiset, order/register assignment differs). Above the 45 min cap before any rewrite was started; list-walk and COM-less alloc paths need structural work. */
/* MATCH-RWL01 0x004cdf20: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl01_20261107/results.tsv. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(undefined4, DAT_006182b4, 0x006182b4);
uint * FUN_004cdd60(uint * p0);
undefined4 FUN_004ceaf0();
undefined4 FUN_004ceba0(uint * p0, uint * p1);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
int FUN_004d8000(undefined4 * p0, uint * p1);
int FUN_004d8060(undefined4 * p0, uint * p1);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004cdf20 FUN_004cdf20
/* [C1 2026-05-19] Signature `RwImage* f(RwImage* dst, RwImage* src)`. */

uint * FUN_004cdf20(uint *param_1,int param_2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined1 *puVar7;
  byte *pbVar8;
  uint uVar9;
  uint *puVar10;
  undefined4 *puVar11;
  undefined4 *puVar12;
  int iVar13;
  undefined4 *puVar14;
  byte *pbVar15;
  int local_c;
  uint local_8;
  undefined4 local_4;
  
  uVar9 = param_1[1];
  if ((uVar9 != *(uint *)(param_2 + 4)) || (uVar2 = param_1[2], uVar2 != *(uint *)(param_2 + 8))) {
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x8000000a);
    FUN_004d8480(&local_8);
    return (uint *)0x0;
  }
  uVar3 = param_1[3];
  if ((uVar3 == 4) || (uVar3 == 8)) {
    puVar4 = (uint *)(**(code **)(DAT_007d3ff8 + 0x118))
                               (*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8),0x30018);
    if (puVar4 == (uint *)0x0) {
      return (uint *)0x0;
    }
    puVar4[1] = uVar9;
    puVar4[2] = uVar2;
    puVar4[3] = uVar3;
    puVar4[5] = 0;
    puVar4[6] = 0;
    *puVar4 = 0;
    FUN_004d8000(&DAT_006182b4,puVar4);
    iVar5 = (int)FUN_004cdd60(puVar4);
    if (iVar5 == 0) {
      if ((*puVar4 & 1) != 0) {
        (**(code **)(DAT_007d3ff8 + 0x10c))(puVar4[5]);
        puVar4[5] = 0;
        puVar4[6] = 0;
        *puVar4 = *puVar4 & 0xfffffffe;
      }
      FUN_004d8060(&DAT_006182b4,puVar4);
      (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8),puVar4);
      return (uint *)0x0;
    }
    if (puVar4[3] == param_1[3]) {
      iVar5 = FUN_004ceaf0();
    }
    else {
      iVar5 = FUN_004ceba0(puVar4,(uint *)param_1);
    }
    puVar10 = puVar4;
    if (iVar5 == 0) {
      puVar10 = (uint *)0x0;
    }
    uVar9 = *puVar10;
    *puVar10 = uVar9 & 0xfffffffd;
    *puVar10 = *param_1 & 2 | uVar9 & 0xfffffffd;
    if ((*param_1 & 1) != 0) {
      (**(code **)(DAT_007d3ff8 + 0x10c))(param_1[5]);
      param_1[5] = 0;
      param_1[6] = 0;
      *param_1 = *param_1 & 0xfffffffe;
    }
    param_1[3] = 0x20;
    FUN_004cdd60(param_1);
    uVar9 = puVar4[3];
    puVar10 = param_1;
    if (param_1[3] == uVar9) {
      if ((((undefined4 *)param_1[6] != (undefined4 *)0x0) &&
          ((undefined4 *)puVar4[6] != (undefined4 *)0x0)) && ((int)uVar9 < 9)) {
        puVar12 = (undefined4 *)puVar4[6];
        puVar6 = (undefined4 *)param_1[6];
        for (uVar9 = 1 << ((byte)uVar9 & 0x1f) & 0x3fffffff; uVar9 != 0; uVar9 = uVar9 - 1) {
          *puVar6 = *puVar12;
          puVar12 = puVar12 + 1;
          puVar6 = puVar6 + 1;
        }
        for (iVar5 = 0; iVar5 != 0; iVar5 = iVar5 + -1) {
          *(undefined1 *)puVar6 = *(undefined1 *)puVar12;
          puVar12 = (undefined4 *)((int)puVar12 + 1);
          puVar6 = (undefined4 *)((int)puVar6 + 1);
        }
      }
      puVar12 = (undefined4 *)puVar4[5];
      local_8 = ((int)(param_1[3] + 7) >> 3) * param_1[1];
      puVar6 = (undefined4 *)param_1[5];
      local_c = 0;
      if (0 < (int)param_1[2]) {
        do {
          puVar11 = puVar12;
          puVar14 = puVar6;
          for (uVar9 = local_8 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
            *puVar14 = *puVar11;
            puVar11 = puVar11 + 1;
            puVar14 = puVar14 + 1;
          }
          for (uVar9 = local_8 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
            *(undefined1 *)puVar14 = *(undefined1 *)puVar11;
            puVar11 = (undefined4 *)((int)puVar11 + 1);
            puVar14 = (undefined4 *)((int)puVar14 + 1);
          }
          puVar12 = (undefined4 *)((int)puVar12 + puVar4[4]);
          puVar6 = (undefined4 *)((int)puVar6 + param_1[4]);
          local_c = local_c + 1;
        } while (local_c < (int)param_1[2]);
      }
    }
    else {
      iVar5 = FUN_004ceba0((uint *)param_1,puVar4);
      if (iVar5 == 0) {
        puVar10 = (uint *)0x0;
      }
    }
    uVar9 = *puVar10;
    *puVar10 = uVar9 & 0xfffffffd;
    *puVar10 = *puVar4 & 2 | uVar9 & 0xfffffffd;
    (**(code **)(DAT_007d3ff8 + 0x10c))(puVar4[5]);
    puVar4[5] = 0;
    puVar4[6] = 0;
    *puVar4 = *puVar4 & 0xfffffffe;
    FUN_004d8060(&DAT_006182b4,puVar4);
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8),puVar4);
  }
  else if (uVar3 != 0x20) {
    local_8 = 1;
    local_4 = FUN_004d7ff0(0x80000009);
    FUN_004d8480(&local_8);
    return (uint *)0x0;
  }
  iVar5 = *(int *)(param_2 + 0x18);
  uVar9 = param_1[5];
  pbVar15 = *(byte **)(param_2 + 0x14);
  local_c = 0;
  if (0 < (int)param_1[2]) {
    do {
      iVar13 = *(int *)(param_2 + 0xc);
      if ((iVar13 == 4) || (iVar13 == 8)) {
        iVar13 = 0;
        if (0 < (int)param_1[1]) {
          pbVar8 = pbVar15;
          puVar7 = (undefined1 *)(uVar9 + 3);
          do {
            bVar1 = *pbVar8;
            pbVar8 = pbVar8 + 1;
            iVar13 = iVar13 + 1;
            *puVar7 = *(undefined1 *)(iVar5 + 3 + (uint)bVar1 * 4);
            puVar7 = puVar7 + 4;
          } while (iVar13 < (int)param_1[1]);
        }
      }
      else if ((iVar13 == 0x20) && (iVar13 = 0, 0 < (int)param_1[1])) {
        puVar7 = (undefined1 *)(uVar9 + 3);
        do {
          *puVar7 = puVar7[(int)pbVar15 - uVar9];
          puVar7 = puVar7 + 4;
          iVar13 = iVar13 + 1;
        } while (iVar13 < (int)param_1[1]);
      }
      pbVar15 = pbVar15 + *(int *)(param_2 + 0x10);
      uVar9 = uVar9 + param_1[4];
      local_c = local_c + 1;
    } while (local_c < (int)param_1[2]);
  }
  return param_1;
}

// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 50.7% (regs 57.5%), class b. Raw converter output with prototype/cast fixes for the C++ check only. Frame 0x98 vs original 0x9c: the error record (local_98/local_94) overlaps local_90[] in Ghidra's layout and needs a proper struct/array to keep both stores; not rewritten. */
/* MATCH-RWL01 0x004d8810: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
uint FUN_004cbd30(int p0, undefined4 * p1, uint p2);
undefined4 * FUN_004cc050(int p0, int p1);
undefined4 FUN_004cc400(undefined4 p0, int * p1, undefined4 * p2, uint * p3, uint * p4);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
undefined4 * FUN_004d8480(undefined4 * p0);
// MATCH: 0x004d8810 FUN_004d8810
/* [C1 2026-05-12] Mechanical behavior is clear: reads native raster data from stream, handles two
   data layouts. */

undefined1 * FUN_004d8810(undefined1 *param_1,undefined4 param_2)

{
  int iVar1;
  undefined1 *puVar2;
  uint uVar3;
  undefined1 *puVar4;
  uint uVar5;
  uint uVar6;
  uint local_9c;
  undefined1 *local_98;
  undefined4 local_94;
  uint local_90 [3];
  int local_84;
  undefined1 local_80 [128];
  
  iVar1 = FUN_004cc400(param_2,&local_84,&local_9c,local_90,0);
  if (iVar1 == 0) {
    return (undefined1 *)0x0;
  }
  while( true ) {
    if ((local_90[0] < 0x35000) || (0x37002 < local_90[0])) {
      local_98 = (undefined1 *)0x1;
      local_94 = FUN_004d7ff0(0x80000004);
      FUN_004d8480((undefined4 *)&local_98);
      return (undefined1 *)0x0;
    }
    if (local_84 == 2) break;
    if (local_84 == 0x13) {
      local_98 = (undefined1 *)0x0;
      if (param_1 == (undefined1 *)0x0) {
        param_1 = (undefined1 *)(**(code **)(DAT_007d3ff8 + 0x108))(local_9c,0x30002);
        if (param_1 == (undefined1 *)0x0) {
          local_90[1] = 1;
          local_90[2] = FUN_004d7ff0(0x80000013,local_9c);
          goto LAB_004d8922;
        }
        local_98 = (undefined1 *)0x1;
      }
      puVar4 = param_1;
      uVar6 = local_9c;
      if (local_9c == 0) {
        return param_1;
      }
      do {
        uVar5 = 0x80;
        if (uVar6 < 0x81) {
          uVar5 = uVar6;
        }
        uVar3 = FUN_004cbd30(param_2,(undefined4 *)local_80,uVar5);
        if (uVar3 != uVar5) {
          if (local_98 == (undefined1 *)0x0) {
            return (undefined1 *)0x0;
          }
          (**(code **)(DAT_007d3ff8 + 0x10c))(param_1);
          return (undefined1 *)0x0;
        }
        uVar6 = uVar6 - uVar5;
        uVar3 = 0;
        uVar5 = uVar5 >> 1;
        if (uVar5 != 0) {
          do {
            puVar4[uVar3] = local_80[uVar3 * 2];
            uVar3 = uVar3 + 1;
          } while (uVar3 < uVar5);
        }
        puVar4 = puVar4 + uVar5;
      } while (uVar6 != 0);
      return param_1;
    }
    iVar1 = (int)FUN_004cc050(param_2,local_9c);
    if (iVar1 == 0) {
      return (undefined1 *)0x0;
    }
    iVar1 = FUN_004cc400(param_2,&local_84,&local_9c,local_90,0);
    if (iVar1 == 0) {
      return (undefined1 *)0x0;
    }
  }
  local_98 = param_1;
  if ((param_1 != (undefined1 *)0x0) ||
     (local_98 = (undefined1 *)(**(code **)(DAT_007d3ff8 + 0x108))(local_9c,0x30002),
     local_98 != (undefined1 *)0x0)) {
    puVar4 = local_98;
    uVar6 = local_9c;
    if (local_9c == 0) {
      return local_98;
    }
    do {
      uVar5 = 0x40;
      if (uVar6 < 0x41) {
        uVar5 = uVar6;
      }
      uVar3 = FUN_004cbd30(param_2,(undefined4 *)local_80,uVar5);
      if (uVar3 != uVar5) {
        return (undefined1 *)0x0;
      }
      uVar6 = uVar6 - uVar5;
      if (uVar5 != 0) {
        puVar2 = puVar4;
        uVar3 = uVar5;
        do {
          *puVar2 = puVar2[(int)(local_80 + -(int)puVar4)];
          puVar2 = puVar2 + 1;
          uVar3 = uVar3 - 1;
        } while (uVar3 != 0);
      }
      puVar4 = puVar4 + uVar5;
    } while (uVar6 != 0);
    return local_98;
  }
  local_90[1] = 1;
  local_90[2] = FUN_004d7ff0(0x80000013,local_9c);
LAB_004d8922:
  FUN_004d8480(local_90 + 1);
  return (undefined1 *)0x0;
}

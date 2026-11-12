// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWC-NEAR (2026-11-12): 100%, BYTE-IDENTICAL 433 B under rw9044, relocheck OK. See the note above FL_004cc820. */
/* RWC-MATCH run 40 (2026-11-12): 99.3% (regs 99.3%) under rw9044. The tail is the rwLinkListAddLLLink order
   (next = head, prev = &list, head->prev = link, head = link) written on param_5 + 7 directly, which reloads the
   head into edx as the original does (load-before-store order kept). Residual: the param_4 test sits above the four
   header stores in the original (6 loop-head and store-order spellings, permuter 474 compiles: no gain).
   Earlier: RWC-MATCH-S01 (2026-11-12): 98.0% (regs 99.3%) under rw9044 (was 95.4%). Kept one permuter move (two independent stores in the block loop). Its second move (DAT_007d45cc load after the param_5[8] store) scored 98.7% but changes the order of a load and a store that can alias, so it was reverted. Residual: two loads hoisted earlier in the original (the param_4 test before the four header stores, and the DAT_007d45cc load before the lea of param_5+7). Not a match. */
/* h3 hand-match: best 74.5% (regs 78.4%), class b. Zeroing loops (rep stosd + rep stosb) are memset(p+2,0,n) with #pragma intrinsic(memset) (55.6 -> 74.5); alloc/realloc arms flipped (realloc arm inline); param_4 as uint with >0 test (jbe). Residual: original saves ebx/ebp/esi/edi at entry (ours pushes esi/ebp later) and a different copy of the a few scheduling choices. */
/* MATCH-RWL01 0x004cc820: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
MGLOBAL(int, DAT_006182b0, 0x006182b0);
MGLOBAL(uint *, DAT_007d45fc, 0x007d45fc);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_007d45cc, 0x007d45cc);
/* MATCH-RWC-NEAR (2026-11-12): the header stores go through a struct of the 0x24-byte block (fields named by offset),
   which lets c2 schedule the param_4 test after `shr ebp,3` as at 0x004cc8c8. With uint * indexing the test stays
   last before the jbe (99.3%). */
typedef struct LL_004cc820 { struct LL_004cc820 *next; struct LL_004cc820 *prev; } LL_004cc820;
typedef struct FL_004cc820 { uint f0; uint f4; uint f8; uint fc; LL_004cc820 f10; uint f18; LL_004cc820 f1c; } FL_004cc820;
// MATCH: 0x004cc820 FUN_004cc820
uint * FUN_004cc820(int param_1,uint param_2,uint param_3,uint param_4,uint *param_5,uint param_6)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  
  if (DAT_006182b0 == 0) {
    param_4 = 0;
  }
  if (param_3 == 0) {
    param_3 = 4;
  }
  if (param_5 == (uint *)0x0) {
    if (DAT_007d45fc != (uint *)0x0) {
      param_5 = (uint *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d45fc,param_6 & 0xff0000);
    }
    else {
      param_5 = (uint *)(**(code **)(DAT_007d3ff8 + 0x108))(0x24,param_6 & 0xff0000);
    }
    if (param_5 == (uint *)0x0) {
      return (uint *)0x0;
    }
    param_5[6] = 2;
  }
  else {
    param_5[6] = 3;
  }
  uVar2 = param_1 + -1 + param_3 & ~(param_3 - 1);
  puVar1 = param_5 + 4;
  ((FL_004cc820 *)param_5)->f0 = uVar2;
  ((FL_004cc820 *)param_5)->f4 = param_2;
  uVar5 = param_2 + 7 >> 3;
  ((FL_004cc820 *)param_5)->fc = param_3;
  ((FL_004cc820 *)param_5)->f8 = uVar5;
  ((FL_004cc820 *)param_5)->f10.next = (LL_004cc820 *)puVar1;
  ((FL_004cc820 *)param_5)->f10.prev = (LL_004cc820 *)puVar1;
  if (param_4 > 0) {
    do {
      puVar3 = (uint *)(**(code **)(DAT_007d3ff8 + 0x108))
                                 (uVar2 * param_2 + uVar5 + 7 + param_3,param_6);
      if (puVar3 == (uint *)0x0) {
        puVar3 = (uint *)*puVar1;
        while (puVar3 != puVar1) {
          *(uint *)puVar3[1] = *puVar3;
          *(uint *)(*puVar3 + 4) = puVar3[1];
          (**(code **)(DAT_007d3ff8 + 0x10c))(puVar3);
          puVar3 = (uint *)*puVar1;
        }
        if ((param_5[6] & 1) == 0) {
          if ((DAT_007d45fc != param_5) && (DAT_007d45fc != (uint *)0x0)) {
            (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d45fc,param_5);
            return (uint *)0x0;
          }
          (**(code **)(DAT_007d3ff8 + 0x10c))(param_5);
        }
        return (uint *)0x0;
      }
      puVar3[1] = 0;
      *puVar3 = 0;
      uVar4 = *puVar1;
      *puVar3 = uVar4;
      puVar3[1] = (uint)puVar1;
      *(uint **)(*puVar1 + 4) = puVar3;
      *puVar1 = (uint)puVar3;
      memset(puVar3 + 2, 0, uVar5);
      param_4 = param_4 + -1;
    } while (param_4 != 0);
  }
  param_5[7] = (uint)DAT_007d45cc;
  param_5[8] = (uint)&DAT_007d45cc;
  *(uint **)((int)DAT_007d45cc + 4) = param_5 + 7;
  DAT_007d45cc = (undefined4)(param_5 + 7);
  return param_5;
}

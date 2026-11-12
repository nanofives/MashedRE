// TOOLCHAIN rw9044
// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c32b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs19_20261102/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(undefined4 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_007d3fe0, 0x007d3fe0);
MGLOBAL(undefined1, DAT_007d3fe4, 0x007d3fe4);
MGLOBAL(undefined4, DAT_00617fe0, 0x00617fe0);
MGLOBAL(undefined4, DAT_007d3ec8, 0x007d3ec8);
void FUN_004d8430(void);
void FUN_004d8470(void);
void FUN_004d8530(void);
void FUN_004d8550(void);
void FUN_004d8a80(void);
void FUN_004d8b70(void);
void FUN_004d8fa0(void);
void FUN_004d9000(void);
void FUN_004d9030(void);
void FUN_004d9040(void);
void FUN_004c07b0(void);
void FUN_004c0830(void);
void FUN_004c1940(void);
void FUN_004c1980(void);
void FUN_004c35b0(void);
void FUN_004c35d0(void);
void FUN_004ccba0(void);
void FUN_004c3e20(void);
void FUN_004c3e90(void);
void FUN_004c4430(void);
void FUN_004c4470(void);
void FUN_004c5e00(void);
void FUN_004c5f60(void);
void FUN_004c78a0(void);
void FUN_004c78e0(void);
void FUN_004cbca0(void);
void FUN_004cbd00(void);
void FUN_004cca80(void);
void FUN_004cd810(void);
void FUN_004cd850(void);
void FUN_004cd900(void);
void FUN_004cdb60(void);
void FUN_0045b350(void);
void FUN_004cc7e0(undefined4 p0);
undefined4 FUN_004cce20(undefined4 * p0);
void FUN_004ccf20(void);
undefined4 FUN_004d7c60(void);
undefined4 FUN_004d7ca0(void);
int FUN_004d7de0(int * p0, int p1, int p2, undefined * p3, undefined * p4, undefined * p5);
undefined4 FUN_004d8560(void);
undefined4 FUN_004d8570(void);
undefined4 thunk_FUN_004cfa00(void);
MGLOBAL(int, _DAT_007d3fe0, 0x007d3fe0);
MGLOBAL(int, _DAT_007d3fe4, 0x007d3fe4);
// MATCH: 0x004c32b0 FUN_004c32b0
/* [C1 2026-05-23] Signature: `int FUN_004c32b0(undefined4 param_1, uint param_2, undefined4
   param_3)` - three params
 */

int FUN_004c32b0(undefined4 param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  uint uVar16;
  bool bVar17;
  
  DAT_007d3ff8 = &DAT_007d3ec8;
  bVar17 = (param_2 & 1) == 0;
  if (bVar17) {
    _DAT_007d3fe0 = (int)FUN_004cca80;
    _DAT_007d3fe4 = (int)FUN_004ccba0;
  }
  else {
    _DAT_007d3fe0 = (int)FUN_004c35b0;
    _DAT_007d3fe4 = (int)FUN_004c35d0;
  }
  FUN_004cc7e0(bVar17);
  DAT_007d3ff8[0x4a] = param_3;
  if ((DAT_007d3ff8[0x49] == 0) && (iVar1 = FUN_004d8570(), iVar1 != 0)) {
    iVar1 = FUN_004cce20((undefined4 *)param_1);
    if (iVar1 != 0) {
      iVar1 = FUN_004d7c60();
      if (iVar1 != 0) {
        uVar2 = FUN_004d7de0((int *)&DAT_00617fe0,8,0x40f,(undefined *)FUN_004d8430,(undefined *)FUN_004d8470,0);
        uVar3 = FUN_004d7de0((int *)&DAT_00617fe0,0x18,0x401,(undefined *)FUN_004c3e90,(undefined *)FUN_004c3e20,0);
        uVar4 = FUN_004d7de0((int *)&DAT_00617fe0,0,0x40d,(undefined *)FUN_004d9030,(undefined *)FUN_004d9040,0);
        uVar5 = FUN_004d7de0((int *)&DAT_00617fe0,0x18,0x402,(undefined *)FUN_004c4470,(undefined *)FUN_004c4430,0);
        uVar6 = FUN_004d7de0((int *)&DAT_00617fe0,4,0x403,(undefined *)FUN_004c07b0,(undefined *)FUN_004c0830,0);
        uVar7 = FUN_004d7de0((int *)&DAT_00617fe0,4,0x404,(undefined *)FUN_004cbca0,(undefined *)FUN_004cbd00,0);
        uVar8 = FUN_004d7de0((int *)&DAT_00617fe0,4,0x405,(undefined *)FUN_004c1980,(undefined *)FUN_004c1940,0);
        uVar9 = FUN_004d7de0((int *)&DAT_00617fe0,0x220,0x406,(undefined *)FUN_004cd900,(undefined *)FUN_004cdb60,0);
        uVar10 = FUN_004d7de0((int *)&DAT_00617fe0,100,0x407,(undefined *)FUN_004c78e0,(undefined *)FUN_004c78a0,0);
        uVar11 = FUN_004d7de0((int *)&DAT_00617fe0,0x34,0x408,(undefined *)FUN_004c5f60,(undefined *)FUN_004c5e00,0);
        uVar12 = FUN_004d7de0((int *)&DAT_00617fe0,0x60,0x409,(undefined *)FUN_004d8530,(undefined *)FUN_004d8550,0);
        uVar13 = FUN_004d7de0((int *)&DAT_00617fe0,4,0x412,(undefined *)FUN_004d8fa0,(undefined *)FUN_004d9000,0);
        uVar14 = FUN_004d8560();
        uVar15 = FUN_004d7de0((int *)&DAT_00617fe0,0x74,0x40a,(undefined *)FUN_004cd850,(undefined *)FUN_004cd810,0);
        uVar16 = FUN_004d7de0((int *)&DAT_00617fe0,0x28,0x40b,(undefined *)FUN_004d8a80,(undefined *)FUN_004d8b70,0);
        if ((-1 < (int)(uVar16 | uVar15 |
                       uVar14 | uVar13 | uVar12 | uVar11 | uVar10 | uVar9 | uVar8 | uVar7 | uVar6 |
                       uVar5 | uVar4 | uVar3 | uVar2)) && (iVar1 = thunk_FUN_004cfa00(), iVar1 != 0)
           ) {
          DAT_007d3ff8[0x49] = 1;
          return iVar1;
        }
        FUN_004d7ca0();
      }
      FUN_004ccf20();
    }
    FUN_0045b350();
  }
  return 0;
}

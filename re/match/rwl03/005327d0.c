// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x005327d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(uint, DAT_007dc540, 0x007dc540);
MGLOBAL(undefined4, DAT_007dc544, 0x007dc544);
MGLOBAL(uint, DAT_007dc548, 0x007dc548);
MGLOBAL(uint, DAT_007dc54c, 0x007dc54c);
MGLOBALA(char, DAT_00623c30, 1, 0x00623c30);
int FUN_004cbb60(void);
int FUN_004d4170(void);
void FUN_004d41e0(int p0);
int FUN_004d4380(int p0);
int FUN_004d4dd0(int p0);
int FUN_004d4f90(int p0, int p1, void *p2, int p3);
// MATCH: 0x005327d0 FUN_005327d0
/* [C1 2026-05-20] Constructs and registers the `nodeD3D9SkinAtomicAllInOne_csl` pipeline. */

int FUN_005327d0(void)

{
  int iVar1;
  int iVar2;
  int uVar3;

  iVar1 = FUN_004cbb60();
  DAT_007dc540 = *(byte *)(iVar1 + 0x1e) & 1;
  if ((!DAT_007dc540 || ((*(uint *)(iVar1 + 0xc4) & 0xffff) < 0x101)) ||
     ((*(uint *)(iVar1 + 0xcc) & 0xffff) < 0x101)) {
    DAT_007dc544 = 0;
    DAT_007dc548 = 0;
    DAT_007dc54c = 0;
  }
  else {
    DAT_007dc544 = 1;
    DAT_007dc54c = *(uint *)(iVar1 + 200);
    DAT_007dc548 = (DAT_007dc54c - 0xcU) / 3;
  }
  iVar1 = FUN_004d4170();
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x2c) = 0x116;
    *(undefined4 *)(iVar1 + 0x30) = 1;
    iVar2 = FUN_004d4dd0(iVar1);
    if (iVar2 != 0) {
      uVar3 = FUN_004d4f90(iVar2,0,DAT_00623c30,0);
      FUN_004d4380(uVar3);
      return iVar1;
    }
    FUN_004d41e0(iVar1);
    iVar1 = 0;
  }
  return iVar1;
}

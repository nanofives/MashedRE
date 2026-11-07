// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005a8f30: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
typedef int code(void);
MGLOBAL(undefined1, DAT_00617ff8, 0x00617ff8);
MGLOBAL(undefined1, DAT_005e617c, 0x005e617c);
MGLOBAL(undefined1, FUN_005a91b0, 0x005a91b0);
undefined4 FUN_005a9e10(int p0, undefined1 * p1, undefined4 p2);
undefined4 FUN_005aee20(uint p0);
// MATCH: 0x005a8f30 FUN_005a8f30
/* WARNING: Removing unreachable block (ram,0x005a8fc4) */
/* WARNING: Removing unreachable block (ram,0x005a8fa5) */
/* WARNING: Removing unreachable block (ram,0x005a8fcc) */
/* WARNING: Removing unreachable block (ram,0x005a9036) */
/* WARNING: Removing unreachable block (ram,0x005a9078) */
/* WARNING: Removing unreachable block (ram,0x005a904c) */
/* WARNING: Removing unreachable block (ram,0x005a8fd8) */
/* WARNING: Removing unreachable block (ram,0x005a901c) */
/* WARNING: Removing unreachable block (ram,0x005a8fee) */
/* [C2 2026-06-01] `void FUN_005a8f30(int param_1)` */

void FUN_005a8f30(int param_1)

{
  byte bVar1;
  int iVar2;
  char cVar4;
  uint uVar3;
  
  FUN_005a9e10(param_1,&DAT_005e617c,0);
  *(undefined4 *)(param_1 + 0x40) = 0x4c;
  bVar1 = FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) != 0) {
    FUN_005aee20(0x28);
  }
  bVar1 = FUN_005aee20(0x28);
  if ((short)(1 << (bVar1 & 0x1f)) == 0) {
    cVar4 = '\0';
  }
  else {
    bVar1 = FUN_005aee20(0x28);
    cVar4 = (char)((uint)(1 << (bVar1 & 0x1f)) >> 8);
  }
  if (!(cVar4 == '\0')) {
    bVar1 = FUN_005aee20(0x28);
    if (!((short)(1 << (bVar1 & 0x1f)) == 0)) {
      bVar1 = FUN_005aee20(0x28);
      iVar2 = (&DAT_00617ff8)[(int)(1 << (bVar1 & 0x1f) & 0xffffU) >> 8] + 7;
    } else {
      iVar2 = DAT_00617ff8 + 7;
    }
  } else {
    bVar1 = FUN_005aee20(0x28);
    if (!((short)(1 << (bVar1 & 0x1f)) == 0)) {
      bVar1 = FUN_005aee20(0x28);
      uVar3 = 1 << (bVar1 & 0x1f) & 0xffff;
    } else {
      uVar3 = 1;
    }
    iVar2 = (&DAT_00617ff8)[uVar3] + -1;
  }
  *(undefined2 *)(param_1 + 0x44) = 0;
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | iVar2 << 0x1c;
  *(code **)(param_1 + 0x28) = (code *)&FUN_005a91b0;
  *(undefined2 *)(param_1 + 0x46) = 1;
  return;
}

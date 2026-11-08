// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c2bf0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined4, DAT_00617f98, 0x00617f98);
void FUN_004c0740(int p0, int p1);
void FUN_004c1a70(int p0);
undefined1 * FUN_004c1d30(void);
int FUN_004d8090(int p0, undefined4 p1, undefined4 p2);
// MATCH: 0x004c2bf0 FUN_004c2bf0

int FUN_004c2bf0(int param_1)

{
  int iVar1;
  
  iVar1 = (int)FUN_004c1d30();
  if (iVar1 == 0) {
    return 0;
  }
  FUN_004c0740(iVar1,*(undefined4 *)(param_1 + 4));
  *(undefined4 *)(iVar1 + 0x18) = *(undefined4 *)(param_1 + 0x18);
  *(undefined4 *)(iVar1 + 0x1c) = *(undefined4 *)(param_1 + 0x1c);
  *(undefined4 *)(iVar1 + 0x68) = *(undefined4 *)(param_1 + 0x68);
  *(undefined4 *)(iVar1 + 0x6c) = *(undefined4 *)(param_1 + 0x6c);
  *(undefined4 *)(iVar1 + 0x70) = *(undefined4 *)(param_1 + 0x70);
  *(undefined4 *)(iVar1 + 0x74) = *(undefined4 *)(param_1 + 0x74);
  *(undefined4 *)(iVar1 + 0x78) = *(undefined4 *)(param_1 + 0x78);
  *(undefined4 *)(iVar1 + 0x7c) = *(undefined4 *)(param_1 + 0x7c);
  *(undefined4 *)(iVar1 + 0x80) = *(undefined4 *)(param_1 + 0x80);
  *(undefined4 *)(iVar1 + 0x84) = *(undefined4 *)(param_1 + 0x84);
  *(undefined4 *)(iVar1 + 0x88) = *(undefined4 *)(param_1 + 0x88);
  *(undefined4 *)(iVar1 + 0x60) = *(undefined4 *)(param_1 + 0x60);
  *(undefined4 *)(iVar1 + 100) = *(undefined4 *)(param_1 + 100);
  *(undefined4 *)(iVar1 + 0x14) = *(undefined4 *)(param_1 + 0x14);
  FUN_004c1a70(iVar1);
  FUN_004d8090((int)&DAT_00617f98,iVar1,param_1);
  return iVar1;
}

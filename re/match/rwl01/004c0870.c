// FLAGS: /O2 /Ob1 /G5
/* h3 hand-match: best 100% (regs 100%), class -. byte flag local widened to uint (gives xor ecx,ecx; mov cl) then narrowed on store */
/* MATCH-RWL01 0x004c0870: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char byte;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004c0910(undefined4 p0, int p1);
// MATCH: 0x004c0870 FUN_004c0870
void FUN_004c0870(undefined4 param_1)

{
  uint bVar1;
  int iVar2;
  
  iVar2 = FUN_004c0910(param_1,0);
  if (iVar2 != 0) {
    *(byte *)(iVar2 + 3) = *(byte *)(iVar2 + 3) & 0xfc;
    bVar1 = *(byte *)(*(int *)(iVar2 + 0xa0) + 3);
    if ((bVar1 & 3) == 0) {
      *(undefined4 *)(*(int *)(iVar2 + 0xa0) + 8) = *(undefined4 *)(DAT_007d3ff8 + 0xbc);
      *(int *)(*(int *)(iVar2 + 0xa0) + 0xc) = DAT_007d3ff8 + 0xbc;
      *(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(iVar2 + 0xa0) + 8;
      *(int *)(DAT_007d3ff8 + 0xbc) = *(int *)(iVar2 + 0xa0) + 8;
    }
    *(byte *)(*(int *)(iVar2 + 0xa0) + 3) = (byte)(bVar1 | 3);
    *(byte *)(iVar2 + 3) = *(byte *)(iVar2 + 3) | 0xc;
  }
  return;
}

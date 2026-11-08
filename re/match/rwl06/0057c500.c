// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0057c500: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_0055b860(undefined4 * p0, undefined4 p1);
undefined4 FUN_00564130(int p0, uint p1, uint p2);
undefined4 FUN_0057c1b0(undefined4 p0);
// MATCH: 0x0057c500 FUN_0057c500

int FUN_0057c500(void)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(DAT_007d3ff8 + 0x108))(0x54,0x30900);
  if (iVar1 != 0) {
    *(undefined4 *)(iVar1 + 0x50) = 1;
    uVar2 = FUN_00564130(0x60,0x10,0x30900);
    *(undefined4 *)(iVar1 + 0x4c) = uVar2;
    FUN_0055b860((undefined4 *)iVar1,uVar2);
    FUN_0057c1b0(*(undefined4 *)(iVar1 + 0x4c));
  }
  return iVar1;
}

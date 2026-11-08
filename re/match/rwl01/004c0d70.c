// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004c0d70: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3e70, 0x007d3e70);
MGLOBAL(undefined4, DAT_00617f78, 0x00617f78);
int FUN_004d8060(int p0, undefined4 p1);
// MATCH: 0x004c0d70 FUN_004c0d70
void FUN_004c0d70(int param_1)

{
  int iVar1;
  int iVar2;
  
  if (param_1 != 0) {
    iVar2 = *(int *)(param_1 + 0x98);
    while (iVar2 != 0) {
      iVar1 = *(int *)(iVar2 + 0x9c);
      FUN_004c0d70(iVar2);
      iVar2 = iVar1;
    }
    FUN_004d8060((int)&DAT_00617f78,param_1);
    if ((*(byte *)(param_1 + 3) & 3) != 0) {
      **(undefined4 **)(param_1 + 0xc) = *(undefined4 *)(param_1 + 8);
      *(undefined4 *)(*(int *)(param_1 + 8) + 4) = *(undefined4 *)(param_1 + 0xc);
    }
    (**(code **)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d3e70 + DAT_007d3ff8),param_1);
  }
  return;
}

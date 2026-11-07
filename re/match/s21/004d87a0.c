// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d87a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_005d8d70, 0x005d8d70);
undefined4 * FUN_004cbe80(undefined4 * p0, undefined4 * p1, uint p2);
undefined4 FUN_004cc580(undefined4 p0, undefined4 p1, undefined4 p2, uint p3, uint p4);
// MATCH: 0x004d87a0 FUN_004d87a0
uint FUN_004d87a0(undefined *param_1,undefined4 param_2)

{
  undefined *puVar1;
  int iVar2;
  uint uVar3;
  
  if (param_1 == (undefined *)0x0) {
    param_1 = &DAT_005d8d70;
  }
  puVar1 = param_1;
  if (param_1 == (undefined *)0x0) {
    puVar1 = &DAT_005d8d70;
  }
  iVar2 = (**(code **)(DAT_007d3ff8 + 0xf4))(puVar1);
  uVar3 = iVar2 + 4U & 0xfffffffc;
  iVar2 = FUN_004cc580(param_2,2,uVar3,0x37002,10);
  if (iVar2 == 0) {
    return 0;
  }
  iVar2 = FUN_004cbe80(param_2,param_1,uVar3);
  return -(uint)(iVar2 != 0) & (uint)param_1;
}

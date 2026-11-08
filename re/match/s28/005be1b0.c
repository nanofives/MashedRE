// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005be1b0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs28_20261107/results.tsv. */
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
MGLOBAL(undefined1, DAT_005e7170, 0x005e7170);
int FUN_005adf30(byte * p0, byte * p1);
void __stdcall FUN_005bf670(int p0);
undefined4 __stdcall FUN_005bf6d0(int * p0, undefined4 p1, undefined4 * p2);
// MATCH: 0x005be1b0 FUN_005be1b0
undefined4 __stdcall FUN_005be1b0(undefined4 param_1,undefined4 param_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_005adf30((byte *)param_2,(byte *)&DAT_005e7170);
  if (iVar1 == 0) {
    *param_3 = param_1;
    FUN_005bf670(param_1);
    return 0;
  }
  uVar2 = FUN_005bf6d0((int *)param_1,param_2,param_3);
  return uVar2;
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00422fd0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs05_20261031/results.tsv. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
undefined4 FUN_0040e470(int p0);
void FUN_004215c0(undefined4 p0, undefined4 p1, undefined4 p2);
void FUN_0045ba00(int p0, undefined4 p1);
undefined4 FUN_0046c5c0(uint p0);
undefined4 FUN_0046c790(uint p0, undefined4 p1);
void thunk_FUN_00419760(int p0, int p1);
// MATCH: 0x00422fd0 FUN_00422fd0
undefined4 FUN_00422fd0(int param_1)

{
  int iVar1;
  
  if (param_1 < 0x10) {
    FUN_0046c5c0(param_1);
    FUN_0046c790(param_1,1);
    FUN_004215c0(param_1,0x42480000,0);
    FUN_004215c0(param_1,0x42480000,1);
    FUN_0045ba00(param_1,2);
    if (DAT_007f0fd0 != 7) {
      iVar1 = FUN_0040e470(param_1);
      if (iVar1 == 1) {
        thunk_FUN_00419760(param_1,1);
      }
    }
  }
  return 0;
}

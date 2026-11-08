// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004cdca0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4628, 0x007d4628);
MGLOBAL(undefined4, DAT_006182b4, 0x006182b4);
int FUN_004d8000(int p0, undefined4 p1);
// MATCH: 0x004cdca0 FUN_004cdca0
undefined4 * FUN_004cdca0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)
           (**(code **)(DAT_007d3ff8 + 0x118))(*(undefined4 *)(DAT_007d4628 + DAT_007d3ff8),0x30018)
  ;
  if (puVar1 == (undefined4 *)0x0) {
    return (undefined4 *)0x0;
  }
  puVar1[1] = param_1;
  puVar1[2] = param_2;
  puVar1[3] = param_3;
  puVar1[5] = 0;
  puVar1[6] = 0;
  *puVar1 = 0;
  FUN_004d8000(&DAT_006182b4,puVar1);
  return puVar1;
}

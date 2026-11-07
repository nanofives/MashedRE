// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 0x00554010: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged except casts, a C++ `code(...)` typedef or callee prototypes for the VS2022 C++ check (DDK bytes identical, relocheck OK); globals moved to MGLOBAL. Score: see verify/matchs24_20261107/results.tsv. */
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
undefined4 FUN_0055deb0(int p0);
int FUN_005c4da0(int * p0);
// MATCH: 0x00554010 FUN_00554010
undefined4 * FUN_00554010(undefined4 *param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  if ((param_1 != (undefined4 *)0x0) && (param_1[1] == -1)) {
    uVar1 = FUN_0055deb0((int)*param_1);
    uVar2 = FUN_005c4da0((int *)*param_1);
    *param_1 = uVar2;
    param_1[1] = uVar1;
    FUN_00554010((undefined4 *)param_1[5]);
  }
  return param_1;
}

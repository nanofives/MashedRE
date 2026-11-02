// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428400: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs06_20261101/results.tsv.
   C++ check fixed (h12, 2026-11-01): FUN_004c5a60 takes void* (explicit casts of the handle globals), FUN_004ce2d0 takes const char* (0x5cc4de = ""). Prototypes only, codegen unchanged (100%, relocheck OK). */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0067d964, 0x0067d964);
MGLOBAL(undefined4, DAT_0067d970, 0x0067d970);
MGLOBAL(undefined4, DAT_0067d968, 0x0067d968);
MGLOBAL(undefined4, DAT_0067d96c, 0x0067d96c);
MGLOBAL(undefined4, DAT_00771964, 0x00771964);
undefined4 FUN_004c5a60(void *p0);
undefined4 *FUN_004ce2d0(const char *p0);
// MATCH: 0x00428400 FUN_00428400
void FUN_00428400(void)

{
  FUN_004ce2d0((const char *)0x5cc4de);
  FUN_004c5a60((void *)DAT_0067d964);
  FUN_004c5a60((void *)DAT_0067d970);
  FUN_004c5a60((void *)DAT_0067d968);
  FUN_004c5a60((void *)DAT_0067d96c);
  DAT_00771964 = 0;
  return;
}

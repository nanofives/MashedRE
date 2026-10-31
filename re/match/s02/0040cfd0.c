// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x0040cfd0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs02_20261031/results.tsv. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0080332c, 0x0080332c);
void FUN_00405400(void);
void FUN_004114e0(void);
void FUN_00426b40(undefined4 p0);
undefined4 FUN_00426c00(void);
void FUN_0045bed0(void);
void FUN_0045bf30(void);
void FUN_00471df0(void);
void FUN_004725f0(void);
void FUN_00484c90(void);
void FUN_00490000(void);
// MATCH: 0x0040cfd0 FUN_0040cfd0
void FUN_0040cfd0(void)



{

  undefined4 uVar1;

  

  FUN_00490000();

  FUN_0045bed0();

  FUN_0045bf30();

  uVar1 = FUN_00426c00();

  FUN_004725f0();

  FUN_00426b40(uVar1);

  FUN_00405400();

  DAT_0080332c = 0;

  FUN_00484c90();

  FUN_00471df0();

  FUN_004114e0();

  return;

}

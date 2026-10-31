// FLAGS: /O2 /GS /Ob1
/* MATCH-S02 0x00409a80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(undefined4, DAT_008a958c, 0x008a958c);
MGLOBAL(undefined4, DAT_008a9588, 0x008a9588);
MGLOBAL(undefined4, DAT_008a9590, 0x008a9590);
MGLOBAL(undefined4, DAT_008a9594, 0x008a9594);
MGLOBAL(undefined4, DAT_008a9598, 0x008a9598);
int FID_conflict__wprintf(char * p0, ...);
void FUN_0042c1a0(void);
void FUN_0045b350(void);
undefined4 FUN_005c9d00(void);
// MATCH: 0x00409a80 FUN_00409a80
void FUN_00409a80(void)



{

  int iVar1;

  

  iVar1 = FUN_005c9d00();

  if (iVar1 == 0) {

    iVar1 = FUN_005c9d00();

    if (iVar1 != 0) {

      FID_conflict__wprintf("status change\n");

      FUN_0042c1a0();

      DAT_008a958c = 2;

      DAT_008a9588 = 2;

      DAT_008a9590 = 2;

      DAT_008a9594 = 2;

      DAT_008a9598 = 2;

      FUN_0045b350();

      return;

    }

  }

  return;

}

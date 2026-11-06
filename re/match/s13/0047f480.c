// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047f480: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs13_20261102/results.tsv. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
MGLOBAL(int, DAT_006ce274, 0x006ce274);
MGLOBAL(undefined4, DAT_0086ecc0, 0x0086ecc0);
int FID_conflict__wprintf(char * p0, ...);
undefined4 FUN_0055dd60(undefined4 * p0);
undefined4 FUN_00562560(undefined4 p0);
// MATCH: 0x0047f480 FUN_0047f480
void FUN_0047f480(void)

{
  if (DAT_006ce274 != 0) {
    FID_conflict__wprintf("!!!!!!!!!!!!!!!!!!!! DESTROY CALLED !!!!!!!!!!!!!!!!!!!!!\n");
    FUN_00562560(DAT_0086ecc0);
    FUN_0055dd60(DAT_006ce274);
    DAT_006ce274 = 0;
  }
  return;
}

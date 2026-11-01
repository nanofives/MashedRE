// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042d560: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs07_20261101/results.tsv. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0067f18c, 0x0067f18c);
int FUN_0042d420(undefined4 p0, undefined4 p1, int p2);
void FUN_0042d4a0(void);
int FUN_004c1a40(int p0, undefined4 p1);
int FUN_004c1b10(int p0, undefined4 p1);
// MATCH: 0x0042d560 FUN_0042d560
undefined4 FUN_0042d560(void)

{
  DAT_0067f18c = FUN_0042d420(0,0,1);
  FUN_004c1a40(DAT_0067f18c,0x3dcccccd);
  FUN_004c1b10(DAT_0067f18c,0x43340000);
  FUN_0042d4a0();
  return 1;
}

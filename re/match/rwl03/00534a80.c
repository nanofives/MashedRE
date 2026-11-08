// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00534a80: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(int, DAT_007dc580, 0x007dc580);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
typedef int (*FNP)(void);
int FUN_00536630(void);
int FUN_00536660(void);
int FUN_00534ad0(void);
int FUN_00534af0(void);
int FUN_004c2d90(int p0, int p1, FNP p2, FNP p3);
int RpAtomicRegisterPlugin(int p0, int p1, FNP p2, FNP p3, int p4);
// MATCH: 0x00534a80 FUN_00534a80
int FUN_00534a80(void)

{
  DAT_007dc580 = FUN_004c2d90(4,0x12f,FUN_00536630,FUN_00536660);
  if (DAT_007dc580 >= 0) {
    DAT_007dc57c = RpAtomicRegisterPlugin(4,0x12f,FUN_00534ad0,FUN_00534af0,0);
    return DAT_007dc57c >= 0;
  }
  return 0;
}

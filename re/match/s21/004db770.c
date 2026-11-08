// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004db770: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs21_20261107/results.tsv. */
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
MGLOBAL(undefined4, DAT_007d70ac, 0x007d70ac);
MGLOBAL(undefined4, DAT_007d70a0, 0x007d70a0);
void FUN_004cb070(int p0);
void FUN_004cb0b0(int p0);
void FUN_004cb0f0(int p0);
void FUN_004cb130(int p0, int p1, int p2, int p3);
void FUN_004d53b0(void);
void FUN_004d5480(int p0, int p1);
void FUN_004d54f0(int p0, int p1, int p2);
undefined4 FUN_004d6e60(int p0);
// MATCH: 0x004db770 FUN_004db770
void FUN_004db770(void)

{
  int iVar1;
  
  FUN_004cb070(DAT_007d70ac);
  FUN_004cb0b0(0);
  FUN_004cb0f0(0);
  FUN_004d5480(0x88,0);
  FUN_004d5480(0x89,0);
  iVar1 = FUN_004d6e60(0);
  if (iVar1 == 0) {
    FUN_004d54f0(0,1,3);
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,3);
    FUN_004d54f0(0,6,0);
  }
  else {
    FUN_004d54f0(0,1,4);
    FUN_004d54f0(0,2,2);
    FUN_004d54f0(0,3,0);
    FUN_004d54f0(0,4,4);
    FUN_004d54f0(0,5,2);
    FUN_004d54f0(0,6,0);
  }
  FUN_004cb130(0,DAT_007d70a0,0,0x1c);
  FUN_004d53b0();
  return;
}

// FLAGS: /O1
/* MATCH-S17 0x004a9744: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees) under the CRT
   flags /O1 (67-75% under the game flags /O2 /GS /Ob1, verify/matchs17_20261107/flagsweep_raw.txt). Body unchanged except
   callee names -> FUN_<va8> (tools/funrename.py); globals moved to MGLOBAL. 100%, relocheck OK. */
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
int FUN_004a966f(int p0);
// MATCH: 0x004a9744 FUN_004a9744
int __cdecl FUN_004a9744(void)

{
  int iVar1;
  
  iVar1 = FUN_004a966f(1);
  return iVar1;
}

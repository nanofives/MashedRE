// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043df00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs08_20261101/results.tsv. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
MGLOBAL(undefined4, DAT_0067ed6c, 0x0067ed6c);
MGLOBAL(undefined4, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(undefined4, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(undefined4, DAT_0067ea64, 0x0067ea64);
MGLOBAL(undefined4, DAT_0067ecdc, 0x0067ecdc);
MGLOBAL(undefined4, DAT_0067ea74, 0x0067ea74);
MGLOBAL(undefined4, DAT_0067ea78, 0x0067ea78);
MGLOBAL(undefined4, DAT_0067ea70, 0x0067ea70);
MGLOBAL(undefined4, DAT_007f1a14, 0x007f1a14);
MGLOBAL(undefined4, DAT_007f1a24, 0x007f1a24);
MGLOBAL(undefined4, DAT_007f1a34, 0x007f1a34);
MGLOBAL(undefined4, DAT_007f1a44, 0x007f1a44);
MGLOBAL(undefined4, DAT_007f1a18, 0x007f1a18);
MGLOBAL(undefined4, DAT_007f1a28, 0x007f1a28);
MGLOBAL(undefined4, DAT_007f1a38, 0x007f1a38);
MGLOBAL(undefined4, DAT_007f1a48, 0x007f1a48);
MGLOBAL(undefined4, DAT_0067ea84, 0x0067ea84);
MGLOBAL(undefined1, DAT_0067ed34, 0x0067ed34);
MGLOBAL(undefined4, DAT_0067e914, 0x0067e914);
MGLOBAL(undefined4, DAT_0067ecb0, 0x0067ecb0);
void FUN_0040e170(undefined4 p0);
void FUN_0040e480(int p0, undefined4 p1);
void FUN_0043d2a0(int p0, int p1);
void FUN_0046dc00(int p0, undefined4 p1);
MGLOBAL(int, _DAT_0067ed34, 0x0067ed34);
// MATCH: 0x0043df00 FUN_0043df00
/* [C1 2026-05-06] No parameters; no return value. Initializes game-session state before entering
   the frontend menu loop. */

void FUN_0043df00(void)

{
  DAT_0067ed6c = 0;
  DAT_007f0fd0 = 6;
  DAT_0067e9fc = 0xb;
  DAT_0067ea64 = 0;
  DAT_0067ecdc = 0;
  DAT_0067ea74 = 0;
  DAT_0067ea78 = 0;
  DAT_0067ea70 = 0;
  DAT_007f1a14 = 0;
  DAT_007f1a24 = 1;
  DAT_007f1a34 = 2;
  DAT_007f1a44 = 0xffffffff;
  DAT_007f1a18 = 0xffffffff;
  DAT_007f1a28 = 0xffffffff;
  DAT_007f1a38 = 0xffffffff;
  DAT_007f1a48 = 0xffffffff;
  FUN_0040e480(0,2);
  FUN_0040e480(1,2);
  FUN_0040e480(2,2);
  FUN_0040e480(3,0);
  FUN_0046dc00(0,DAT_0067ea84);
  FUN_0040e170(0);
  FUN_0043d2a0(0,2);
  _DAT_0067ed34 = 0;
  DAT_0067e914 = 1;
  DAT_0067ecb0 = 1;
  return;
}

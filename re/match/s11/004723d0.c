// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004723d0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs11_20261102/results.tsv. */
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
MGLOBAL(undefined1, DAT_00691500, 0x00691500);
MGLOBAL(undefined1, DAT_00691504, 0x00691504);
MGLOBAL(undefined4, DAT_00691508, 0x00691508);
MGLOBAL(undefined4, DAT_0069150c, 0x0069150c);
MGLOBAL(undefined1, DAT_00691510, 0x00691510);
MGLOBAL(undefined1, DAT_00691514, 0x00691514);
MGLOBAL(undefined4, DAT_00691518, 0x00691518);
MGLOBAL(undefined4, DAT_0069151c, 0x0069151c);
MGLOBAL(undefined1, DAT_00691520, 0x00691520);
MGLOBAL(undefined1, DAT_00691524, 0x00691524);
MGLOBAL(undefined1, DAT_00691528, 0x00691528);
MGLOBAL(undefined1, DAT_0069152c, 0x0069152c);
MGLOBAL(undefined1, DAT_00691530, 0x00691530);
MGLOBAL(undefined1, DAT_00691534, 0x00691534);
MGLOBAL(undefined1, DAT_00691538, 0x00691538);
MGLOBAL(undefined1, DAT_0069153c, 0x0069153c);
MGLOBAL(undefined1, DAT_00691540, 0x00691540);
MGLOBAL(undefined1, DAT_00691544, 0x00691544);
MGLOBAL(undefined1, DAT_00691548, 0x00691548);
MGLOBAL(undefined1, DAT_0069154c, 0x0069154c);
MGLOBAL(undefined1, DAT_00691550, 0x00691550);
MGLOBAL(undefined1, DAT_00691554, 0x00691554);
MGLOBAL(undefined1, DAT_00691558, 0x00691558);
MGLOBAL(undefined1, DAT_0069155c, 0x0069155c);
MGLOBAL(undefined1, DAT_00691560, 0x00691560);
MGLOBAL(undefined1, DAT_00691564, 0x00691564);
MGLOBAL(undefined1, DAT_00691568, 0x00691568);
MGLOBAL(undefined1, DAT_0069156c, 0x0069156c);
MGLOBAL(undefined1, DAT_00691570, 0x00691570);
MGLOBAL(undefined1, DAT_00691574, 0x00691574);
MGLOBAL(undefined1, DAT_00691578, 0x00691578);
MGLOBAL(undefined1, DAT_0069157c, 0x0069157c);
MGLOBAL(undefined1, DAT_00691580, 0x00691580);
MGLOBAL(undefined1, DAT_00691584, 0x00691584);
MGLOBAL(undefined1, DAT_00691588, 0x00691588);
MGLOBAL(undefined1, DAT_0069158c, 0x0069158c);
MGLOBAL(undefined1, DAT_00691590, 0x00691590);
MGLOBAL(undefined1, DAT_00691594, 0x00691594);
MGLOBAL(undefined1, DAT_00691598, 0x00691598);
MGLOBAL(undefined1, DAT_0069159c, 0x0069159c);
MGLOBAL(int, _DAT_00691500, 0x00691500);
MGLOBAL(int, _DAT_00691504, 0x00691504);
MGLOBAL(int, _DAT_00691510, 0x00691510);
MGLOBAL(int, _DAT_00691514, 0x00691514);
MGLOBAL(int, _DAT_00691520, 0x00691520);
MGLOBAL(int, _DAT_00691524, 0x00691524);
MGLOBAL(int, _DAT_00691528, 0x00691528);
MGLOBAL(int, _DAT_0069152c, 0x0069152c);
MGLOBAL(int, _DAT_00691530, 0x00691530);
MGLOBAL(int, _DAT_00691534, 0x00691534);
MGLOBAL(int, _DAT_00691538, 0x00691538);
MGLOBAL(int, _DAT_0069153c, 0x0069153c);
MGLOBAL(int, _DAT_00691540, 0x00691540);
MGLOBAL(int, _DAT_00691544, 0x00691544);
MGLOBAL(int, _DAT_00691548, 0x00691548);
MGLOBAL(int, _DAT_0069154c, 0x0069154c);
MGLOBAL(int, _DAT_00691550, 0x00691550);
MGLOBAL(int, _DAT_00691554, 0x00691554);
MGLOBAL(int, _DAT_00691558, 0x00691558);
MGLOBAL(int, _DAT_0069155c, 0x0069155c);
MGLOBAL(int, _DAT_00691560, 0x00691560);
MGLOBAL(int, _DAT_00691564, 0x00691564);
MGLOBAL(int, _DAT_00691568, 0x00691568);
MGLOBAL(int, _DAT_0069156c, 0x0069156c);
MGLOBAL(int, _DAT_00691570, 0x00691570);
MGLOBAL(int, _DAT_00691574, 0x00691574);
MGLOBAL(int, _DAT_00691578, 0x00691578);
MGLOBAL(int, _DAT_0069157c, 0x0069157c);
MGLOBAL(int, _DAT_00691580, 0x00691580);
MGLOBAL(int, _DAT_00691584, 0x00691584);
MGLOBAL(int, _DAT_00691588, 0x00691588);
MGLOBAL(int, _DAT_0069158c, 0x0069158c);
MGLOBAL(int, _DAT_00691590, 0x00691590);
MGLOBAL(int, _DAT_00691594, 0x00691594);
MGLOBAL(int, _DAT_00691598, 0x00691598);
MGLOBAL(int, _DAT_0069159c, 0x0069159c);
// MATCH: 0x004723d0 FUN_004723d0
/* [C1 2026-05-23] Takes no parameters; no return value. */

void FUN_004723d0(void)

{
  _DAT_00691500 = 0xff01ffff;
  _DAT_00691504 = 0xff013cff;
  DAT_00691508 = 0;
  DAT_0069150c = 0;
  _DAT_00691510 = 0xff02ffff;
  _DAT_00691514 = 0xff023cff;
  DAT_00691518 = 0;
  DAT_0069151c = 0;
  _DAT_00691520 = 0xff03ffff;
  _DAT_00691524 = 0xff033cff;
  _DAT_00691528 = 0;
  _DAT_0069152c = 0;
  _DAT_00691530 = 0xff04ffff;
  _DAT_00691534 = 0xff043cff;
  _DAT_00691538 = 0;
  _DAT_0069153c = 0;
  _DAT_00691540 = 0xff05ffff;
  _DAT_00691544 = 0xff053cff;
  _DAT_00691548 = 0;
  _DAT_0069154c = 0;
  _DAT_00691550 = 0xff06ffff;
  _DAT_00691554 = 0xff063cff;
  _DAT_00691558 = 0;
  _DAT_0069155c = 0;
  _DAT_00691560 = 0xff07ffff;
  _DAT_00691564 = 0xff073cff;
  _DAT_00691568 = 0;
  _DAT_0069156c = 0;
  _DAT_00691570 = 0xff08ffff;
  _DAT_00691574 = 0xff083cff;
  _DAT_00691578 = 0;
  _DAT_0069157c = 0;
  _DAT_00691580 = 0xff09ffff;
  _DAT_00691584 = 0xff093cff;
  _DAT_00691588 = 0;
  _DAT_0069158c = 0;
  _DAT_00691590 = 0xff0affff;
  _DAT_00691594 = 0xff0a3cff;
  _DAT_00691598 = 0;
  _DAT_0069159c = 0;
  return;
}

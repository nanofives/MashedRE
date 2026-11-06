// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00466a50: matched, byte-identical whole symbol (match.py --bytes: 245 B, 0 differing unmasked
   bytes). match.py shows 98.7% only because Ghidra's size (242) stops before the tail call jmp FUN_005a6030 at
   0x00466b40. Raw g2s 94.9%: the counted loops written as for loops with an unsigned index (test/jbe guard
   instead of je), the 0x640-byte walk as i < 200 over the 8-byte entries; thunk_FUN_005abcf0 called by its
   address FUN_005c9220. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0069047c, 0x0069047c);
MGLOBAL(uint, DAT_0068fc84, 0x0068fc84);
MGLOBAL(uint, DAT_0068fc88, 0x0068fc88);
MGLOBAL(uint, DAT_006904a0, 0x006904a0);
MGLOBAL(int, DAT_0069049c, 0x0069049c);
MGLOBAL(int, DAT_0068f61c, 0x0068f61c);
MGLOBAL(int, DAT_0068f560, 0x0068f560);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
MGLOBAL(int, DAT_0068fcd0, 0x0068fcd0);
void FUN_00462ec0(void);
void FUN_005a6030(void);
int FUN_005a6150(void);
void FUN_005a6710(int p0, int p1);
void FUN_005a73b0(void);
void FUN_005a79a0(int p0);
void FUN_005aaa00(int p0, int p1, int p2);
int FUN_005bbb70(void);
void FUN_005c9220(int p0);
// MATCH: 0x00466a50 FUN_00466a50
void FUN_00466a50(void)
{
  uint i;

  if (DAT_0069047c != 0) {
    for (i = 0; i < DAT_0068fc84; i++) {
      FUN_005a6710((&DAT_0068f640)[i * 2],0);
    }
    for (i = 0; i < DAT_0068fc88; i++) {
      FUN_005c9220((&DAT_0068f560)[i]);
    }
    FUN_005a79a0(DAT_0069047c);
  }
  for (i = 0; i < 200; i++) {
    FUN_005aaa00((&DAT_0068f640)[i * 2],0,0);
  }
  FUN_005a73b0();
  for (i = 0; i < DAT_006904a0; i++) {
    FUN_005aaa00((&DAT_0068fcd0)[i],0,0);
  }
  FUN_00462ec0();
  FUN_005aaa00(DAT_0069049c,0,0);
  FUN_005aaa00(DAT_0068f61c,0,0);
  FUN_005bbb70();
  FUN_005a6150();
  FUN_005a6030();
}

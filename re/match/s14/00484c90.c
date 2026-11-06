// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00484c90 (h2): world-object registry reset. 300 records of 0x8c bytes at 0x006dccb8.
   100% (regs 100%). Raw 54.8%. Moved: record struct + indexed for-loop (pointer loop gave a down-counter),
   memset intrinsic for the 9-dword tail, and the store order f00[0], f00[1], idx (permuter; the order picks
   the loop pointer base 0x006dccbc). */
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
typedef struct WorldObj {
  int f00[3];
  int f0c;
  int f10;
  int f14;
  int f18;
  int f1c;
  short idx;
  short pad22;
  int f24[17];
  int f68[9];
} WorldObj;
MGLOBALA(WorldObj, DAT_006dccb8, 300, 0x006dccb8);
MGLOBAL(int, DAT_006e70d8, 0x006e70d8);
MGLOBAL(int, DAT_006e70cc, 0x006e70cc);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x00484c90 FUN_00484c90
void FUN_00484c90(void)
{
  int i;
  for (i = 0; i < 300; i++) {
    DAT_006dccb8[i].f00[0] = 0;
    DAT_006dccb8[i].f00[1] = 0;
    DAT_006dccb8[i].idx = (short)i;
    DAT_006dccb8[i].f00[2] = 0;
    DAT_006dccb8[i].f0c = 0;
    DAT_006dccb8[i].f10 = 0;
    DAT_006dccb8[i].f14 = 0;
    DAT_006dccb8[i].f1c = 0;
    memset(DAT_006dccb8[i].f68, 0, sizeof(DAT_006dccb8[i].f68));
  }
  DAT_006e70d8 = 0;
  DAT_006e70cc = 0;
}

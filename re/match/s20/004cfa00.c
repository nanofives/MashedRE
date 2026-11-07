// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cfa00 (h3): 100% (regs 100%). Registers a plugin (FUN_004c7690(0x24, 0x40c, FUN_004cfdd0, FUN_004cfdf0, 0)), stores its offset in 0x00911ae4 and returns 0 when negative; then clears the 128-entry table at 0x00911b00 (4-byte entries: byte +0, byte +1, ushort +2; memset intrinsic, rep stosd) and fills entries 20..86 field by field; returns 1. From 68.0%: one MGLOBALA struct array instead of 140 byte/short globals; the source is in entry order (a, depth, fmt per entry) for entries 20..61, and for entries 62.. too except that entry 81 comes after 83 (the compiler regroups the second half's stores by value, which is why Ghidra's listing looks grouped). Relocheck OK. */
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
void *memset(void *d, int c, unsigned int n);
#pragma intrinsic(memset)
typedef struct FmtEntry { unsigned char a; unsigned char depth; unsigned short fmt; } FmtEntry;
typedef void *(*RwPluginObjectConstructor)(void *object, int offset, int size);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBALA(FmtEntry, DAT_00911b00, 128, 0x00911b00);
void *FUN_004cfdd0(void *object, int offset, int size);
void *FUN_004cfdf0(void *object, int offset, int size);
int FUN_004c7690(int size, unsigned int pluginID, RwPluginObjectConstructor ctor, RwPluginObjectConstructor dtor, void *always);

// MATCH: 0x004cfa00 FUN_004cfa00
int FUN_004cfa00(void)
{
  DAT_00911ae4 = FUN_004c7690(0x24, 0x40c, FUN_004cfdd0, FUN_004cfdf0, 0);
  if (DAT_00911ae4 < 0) {
    return 0;
  }
  memset(DAT_00911b00, 0, sizeof(DAT_00911b00));
  DAT_00911b00[20].a = 0;
  DAT_00911b00[20].depth = 0x18;
  DAT_00911b00[20].fmt = 0;
  DAT_00911b00[21].a = 1;
  DAT_00911b00[21].depth = 0x20;
  DAT_00911b00[21].fmt = 0x500;
  DAT_00911b00[22].a = 0;
  DAT_00911b00[22].depth = 0x20;
  DAT_00911b00[22].fmt = 0x600;
  DAT_00911b00[23].a = 0;
  DAT_00911b00[23].depth = 0x10;
  DAT_00911b00[23].fmt = 0x200;
  DAT_00911b00[24].a = 0;
  DAT_00911b00[24].depth = 0x10;
  DAT_00911b00[24].fmt = 0xa00;
  DAT_00911b00[25].a = 1;
  DAT_00911b00[25].depth = 0x10;
  DAT_00911b00[25].fmt = 0x100;
  DAT_00911b00[26].a = 1;
  DAT_00911b00[26].depth = 0x10;
  DAT_00911b00[26].fmt = 0x300;
  DAT_00911b00[27].a = 0;
  DAT_00911b00[27].depth = 8;
  DAT_00911b00[27].fmt = 0;
  DAT_00911b00[28].a = 1;
  DAT_00911b00[28].depth = 8;
  DAT_00911b00[28].fmt = 0;
  DAT_00911b00[29].a = 1;
  DAT_00911b00[29].depth = 0x10;
  DAT_00911b00[29].fmt = 0;
  DAT_00911b00[30].a = 0;
  DAT_00911b00[30].depth = 0x10;
  DAT_00911b00[30].fmt = 0;
  DAT_00911b00[31].a = 1;
  DAT_00911b00[31].depth = 0x20;
  DAT_00911b00[31].fmt = 0;
  DAT_00911b00[32].a = 1;
  DAT_00911b00[32].depth = 0x20;
  DAT_00911b00[32].fmt = 0;
  DAT_00911b00[33].a = 0;
  DAT_00911b00[33].depth = 0x20;
  DAT_00911b00[33].fmt = 0;
  DAT_00911b00[34].a = 0;
  DAT_00911b00[34].depth = 0x20;
  DAT_00911b00[34].fmt = 0;
  DAT_00911b00[35].a = 1;
  DAT_00911b00[35].depth = 0x20;
  DAT_00911b00[35].fmt = 0;
  DAT_00911b00[36].a = 1;
  DAT_00911b00[36].depth = 0x40;
  DAT_00911b00[36].fmt = 0;
  DAT_00911b00[40].a = 1;
  DAT_00911b00[40].depth = 0x10;
  DAT_00911b00[40].fmt = 0;
  DAT_00911b00[41].a = 0;
  DAT_00911b00[41].depth = 8;
  DAT_00911b00[41].fmt = 0x2000;
  DAT_00911b00[50].a = 0;
  DAT_00911b00[50].depth = 8;
  DAT_00911b00[50].fmt = 0x400;
  DAT_00911b00[51].a = 1;
  DAT_00911b00[51].depth = 0x10;
  DAT_00911b00[51].fmt = 0;
  DAT_00911b00[52].a = 1;
  DAT_00911b00[52].depth = 8;
  DAT_00911b00[52].fmt = 0;
  DAT_00911b00[60].a = 0;
  DAT_00911b00[60].depth = 0x10;
  DAT_00911b00[60].fmt = 0;
  DAT_00911b00[61].a = 0;
  DAT_00911b00[61].depth = 0x10;
  DAT_00911b00[61].fmt = 0;
  DAT_00911b00[62].a = 0;
  DAT_00911b00[62].depth = 0x20;
  DAT_00911b00[62].fmt = 0;
  DAT_00911b00[63].a = 0;
  DAT_00911b00[63].depth = 0x20;
  DAT_00911b00[63].fmt = 0;
  DAT_00911b00[64].a = 0;
  DAT_00911b00[64].depth = 0x20;
  DAT_00911b00[64].fmt = 0;
  DAT_00911b00[67].a = 1;
  DAT_00911b00[67].depth = 0x20;
  DAT_00911b00[67].fmt = 0;
  DAT_00911b00[70].a = 0;
  DAT_00911b00[70].depth = 0x10;
  DAT_00911b00[70].fmt = 0x700;
  DAT_00911b00[71].a = 0;
  DAT_00911b00[71].depth = 0x20;
  DAT_00911b00[71].fmt = 0x900;
  DAT_00911b00[73].a = 0;
  DAT_00911b00[73].depth = 0x10;
  DAT_00911b00[73].fmt = 0x700;
  DAT_00911b00[75].a = 0;
  DAT_00911b00[75].depth = 0x20;
  DAT_00911b00[75].fmt = 0x900;
  DAT_00911b00[77].a = 0;
  DAT_00911b00[77].depth = 0x20;
  DAT_00911b00[77].fmt = 0x900;
  DAT_00911b00[79].a = 0;
  DAT_00911b00[79].depth = 0x20;
  DAT_00911b00[79].fmt = 0x900;
  DAT_00911b00[80].a = 0;
  DAT_00911b00[80].depth = 0x10;
  DAT_00911b00[80].fmt = 0x700;
  DAT_00911b00[82].a = 0;
  DAT_00911b00[82].depth = 0x20;
  DAT_00911b00[82].fmt = 0x900;
  DAT_00911b00[83].a = 0;
  DAT_00911b00[83].depth = 0x20;
  DAT_00911b00[83].fmt = 0x900;
  DAT_00911b00[81].a = 0;
  DAT_00911b00[81].depth = 0x10;
  DAT_00911b00[81].fmt = 0;
  DAT_00911b00[110].a = 0;
  DAT_00911b00[110].depth = 0x40;
  DAT_00911b00[110].fmt = 0;
  DAT_00911b00[111].a = 0;
  DAT_00911b00[111].depth = 0x10;
  DAT_00911b00[111].fmt = 0;
  DAT_00911b00[112].a = 0;
  DAT_00911b00[112].depth = 0x20;
  DAT_00911b00[112].fmt = 0;
  DAT_00911b00[113].a = 1;
  DAT_00911b00[113].depth = 0x40;
  DAT_00911b00[113].fmt = 0;
  DAT_00911b00[114].a = 0;
  DAT_00911b00[114].depth = 0x20;
  DAT_00911b00[114].fmt = 0;
  DAT_00911b00[115].a = 0;
  DAT_00911b00[115].depth = 0x40;
  DAT_00911b00[115].fmt = 0;
  DAT_00911b00[116].a = 1;
  DAT_00911b00[116].depth = 0x80;
  DAT_00911b00[116].fmt = 0;
  DAT_00911b00[117].a = 0;
  DAT_00911b00[117].depth = 0x10;
  DAT_00911b00[117].fmt = 0;
  return 1;
}

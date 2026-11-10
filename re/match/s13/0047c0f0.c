// FLAGS: /O2 /GS /Ob1
/* MATCH-NEARMISS-2-B (2026-11-09): BYTE-IDENTICAL, relocheck OK. The count at 0x006c2fe8 is a member of the object at
   0x006c1228 (the copied set, 0x840 bytes not touched here, the count at +0x1dc0); with it the count store lands before the
   rep movsd as in the original. The h3 notes below describe the earlier 97.3% form. */
/* MATCH-S13 0x0047c0f0 (h3): 97.3% (regs 97.3%). DAT_006c2fec and DAT_006c2fe8 are file-static (pilot finding 2; static fec moved its store past push ecx, 94.6 -> 97.3). Residual: the DAT_006c2fe8 = count store lands after the rep movsd instead of before it. Tried all 16 static/extern combos of the 4 globals, copy as struct assign / memcpy / loop, store before/after the copy, permuter (88 compiles). Class (b): the 0x6c1228 region ends at 0x6c2fe8 (0x6c1228+0x1dc0), so a containing struct layout not found yet. */
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
typedef struct SectRec { int d[0x56]; } SectRec;
typedef struct SectSet { SectRec r[16]; } SectSet;
/* 0x006c1228: the copied set, then 0x840 bytes not touched here [UNCERTAIN: content], then the count at +0x1dc0
   (0x006c2fe8). Typing the count as a member of this object moves its store before the rep movsd as in the original. */
typedef struct SectBuf { SectSet set; char pad[0x840]; int count; } SectBuf;
#ifndef __cplusplus
static int DAT_006c2fec;
#else
MGLOBAL(int, DAT_006c2fec, 0x006c2fec);
#endif
MGLOBAL(SectSet, DAT_006bfca8, 0x006bfca8);
MGLOBAL(SectBuf, DAT_006c1228, 0x006c1228);
int FUN_004b3fc0(int world, int *sectors);
void FUN_004c0e50(int p);
void FUN_0047bf70(SectRec *rec, int sector);
// MATCH: 0x0047c0f0 FUN_0047c0f0
void FUN_0047c0f0(int param_1)
{
  int sectors[16];
  int i;

  DAT_006c2fec = FUN_004b3fc0(param_1, sectors);
  FUN_004c0e50(*(int *)(param_1 + 4));
  for (i = 0; i < DAT_006c2fec; i++) {
    FUN_0047bf70(&DAT_006bfca8.r[i], sectors[i]);
  }
  DAT_006c1228.count = DAT_006c2fec;
  DAT_006c1228.set = DAT_006bfca8;
}

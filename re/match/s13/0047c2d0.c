// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047c2d0 (h3): matched; the file also holds its static callee 0x0047c1f0 (register args EAX = item array, EBX = item count, stack = query; pilot finding 1, the real caller is here so no stand-in). 0x0047c1f0: 100% (53 B). 0x0047c2d0: insn score 92.1% only because Ghidra's size (85 B) stops before the second return path; match.py --bytes: 91 B whole symbol, 0 differing unmasked bytes, BYTE-IDENTICAL. Raw 45.7% / 60.0%. */
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
typedef struct SectItem { int d[4]; } SectItem;
typedef struct SectBuf { SectItem it[8]; } SectBuf;
MGLOBAL(int, DAT_006c2fe8, 0x006c2fe8);
MGLOBALA(SectBuf, DAT_006c27a8, 16, 0x006c27a8);
MGLOBALA(int, DAT_006c2fa8, 16, 0x006c2fa8);
int FUN_00478030(SectItem *item, int q);
// MATCH: 0x0047c1f0 FUN_0047c1f0
static int FUN_0047c1f0(SectItem *items, int n, int q)
{
  int i;
  for (i = 0; i < n; i++) {
    if (FUN_00478030(&items[i], q) == 0) {
      return 0;
    }
  }
  return 1;
}

// MATCH: 0x0047c2d0 FUN_0047c2d0
int FUN_0047c2d0(int param_1)
{
  int i;
  for (i = 0; i < DAT_006c2fe8; i++) {
    if (DAT_006c2fa8[i] != 0 && FUN_0047c1f0(DAT_006c27a8[i].it, DAT_006c2fa8[i], param_1) != 0) {
      return 1;
    }
  }
  return 0;
}

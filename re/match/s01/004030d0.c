// FLAGS: /O2 /GS /Ob1
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
/* a2 0x004030d0 (137 B). Best: 100%.
   g2s 55.0%: floats typed as int, compare direction. Fixed: float globals, memset intrinsic over an
   8 x 0x4c slot array at 0x007f1038 (bytes +9/+0xa), int-index loop.
   MATCH-EXACT-DATA 2026-11-11: `DAT_00636ae8 = DAT_00636ae8 + DAT_007f1004` was byte-identical with the fld / fadd
   operands swapped (cl loads the addend first; relocheck did not check DAT_ names then). The original is
   fld [0x00636ae8] at +0x9, fadd [0x007f1004] at +0x10. Reading the addend into a local first gives that order. */
typedef unsigned int size_t;
void *memset(void *d, int c, size_t n);
#pragma intrinsic(memset)
typedef struct {
  char pad0[9];
  char m_09;        /* 0x7f1041 + i*0x4c */
  char m_0a;        /* 0x7f1042 + i*0x4c */
  char pad1[0x4c - 11];
} Slot4c;
MGLOBAL(float, DAT_00636ae8, 0x00636ae8);
MGLOBAL(float, DAT_007f1004, 0x007f1004);
MGLOBAL(float, DAT_00636aec, 0x00636aec);
MGLOBAL(unsigned int, DAT_00636af8, 0x00636af8);
MGLOBALA(Slot4c, DAT_007f1038, 8, 0x007f1038);
void FUN_0042b950(void);
int FUN_0042c1c0(void);
void FUN_0043df00(void);
void FUN_00472640(int p0);

// MATCH: 0x004030d0 FUN_004030d0
void FUN_004030d0(void)
{
  int i;
  float step;
  if (FUN_0042c1c0() == 0) {
    step = DAT_007f1004;
    DAT_00636ae8 = DAT_00636ae8 + step;
    FUN_00472640(0);
    FUN_0042b950();
    if (DAT_00636ae8 > DAT_00636aec) {
      if (DAT_00636af8 != 0) {
        memset(DAT_007f1038, 0, sizeof(DAT_007f1038));
        DAT_00636ae8 = 0.0f;
      } else {
        FUN_0043df00();
      }
      DAT_00636af8 = (DAT_00636af8 - 1) & 1;
    }
    for (i = 0; i < 8; i++) {
      if (DAT_007f1038[i].m_09 != 0)
        DAT_007f1038[i].m_0a = 1;
    }
  }
}

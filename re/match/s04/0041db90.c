// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041db90: 100% (regs 100%) by hand, written from the disassembly (raw g2s 72.0%).
   The record at 0x0063d5a0 (0x54 B, cleared by FUN_004b6520): 16 atomic pointers filled by FUN_004b51d0 from
   the "RacePanel.dff" clump, +0x40 clump, +0x44 its frame (clump+4), +0x48 flag = 1. Entry 2 gets
   FUN_004b52f0(atomic, 0x40, 1) and its first material's colour (geometry +0x18 -> materials +0x20 -> [0] +4)
   = {0x32, 0x32, 0x32, 0xff}, built as a 4-byte local and copied as a dword. Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct {
  char *m_p[16];     /* +0x00..+0x3c */
  char *m_40;        /* +0x40 (0x0063d5e0) */
  char *m_44;        /* +0x44 (0x0063d5e4) */
  int m_48;          /* +0x48 (0x0063d5e8) */
  float m_4c;        /* +0x4c (0x0063d5ec) */
  int m_50;          /* +0x50 (0x0063d5f0) */
} MatchRec0063d5a0;
typedef struct { unsigned char r, g, b, a; } MatchRGBA_0041db90;
MGLOBALA(char, s_RacePanel_dff_005cd23c, 14, 0x005cd23c);
MGLOBAL(MatchRec0063d5a0, DAT_0063d5a0, 0x0063d5a0);
char *FUN_0042a5d0(const char *name, int a1, int a2);
void FUN_004b6520(void *p, int n);
void FUN_004b51d0(char *clump, char **out, int n, int a3, int a4);
void FUN_004b52f0(char *atomic, int flags, int on);

// MATCH: 0x0041db90 FUN_0041db90
void FUN_0041db90(void)
{
  char *clump;
  int i;
  MatchRGBA_0041db90 c;

  clump = FUN_0042a5d0(s_RacePanel_dff_005cd23c, 0, 0);
  FUN_004b6520(&DAT_0063d5a0, 0x54);
  DAT_0063d5a0.m_40 = clump;
  DAT_0063d5a0.m_44 = *(char **)(clump + 4);
  DAT_0063d5a0.m_48 = 1;
  FUN_004b51d0(clump, DAT_0063d5a0.m_p, 0x10, 0, 0);
  for (i = 0; i < 16; i++) {
    char *a = DAT_0063d5a0.m_p[i];
    if (i == 2) {
      c.r = 0x32;
      c.g = 0x32;
      c.b = 0x32;
      c.a = 0xff;
      FUN_004b52f0(a, 0x40, 1);
      *(MatchRGBA_0041db90 *)(**(char ***)(*(char **)(a + 0x18) + 0x20) + 4) = c;
    }
  }
}

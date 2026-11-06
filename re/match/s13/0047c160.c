// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x0047c160 (h3): matched. Instruction score 86.3% only because Ghidra's size (133 B) stops before the epilogue (jl + pops + ret); match.py --bytes: 141 B whole symbol, 0 differing unmasked bytes, BYTE-IDENTICAL. Raw 79.2%. Plain loop over three parallel arrays (records 0x158 B at 0x6c1228, 0x80-B buffers at 0x6c27a8, flags at 0x6c2fa8). */
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
typedef struct SectBuf { unsigned char b[0x80]; } SectBuf;
MGLOBAL(int, DAT_006c2fe8, 0x006c2fe8);
MGLOBALA(SectRec, DAT_006c1228, 16, 0x006c1228);
MGLOBALA(SectBuf, DAT_006c27a8, 16, 0x006c27a8);
MGLOBALA(int, DAT_006c2fa8, 16, 0x006c2fa8);
int FUN_004c0ed0(int p);
int FUN_004c1b40(int q, void *box);
void FUN_0047bb10(SectRec *rec, int pos, SectBuf *buf, int *flag);
// MATCH: 0x0047c160 FUN_0047c160
void FUN_0047c160(int param_1)
{
  int pos;
  int i;

  pos = FUN_004c0ed0(*(int *)(param_1 + 4)) + 0x30;
  for (i = 0; i < DAT_006c2fe8; i++) {
    DAT_006c2fa8[i] = 0;
    if (FUN_004c1b40(param_1, &DAT_006c1228[i].d[0x4e]) != 0) {
      FUN_0047bb10(&DAT_006c1228[i], pos, &DAT_006c27a8[i], &DAT_006c2fa8[i]);
    }
  }
}

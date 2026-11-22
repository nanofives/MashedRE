// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c9310: RO-AUDIOINIT-45D460-REACH-4, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-4 (2026-11-22): new source from the listing of MASHED.exe.unpatched 0x005c9310..0x005c9377 (no
   Ghidra function, no hooks.csv row before this row). 0x005c9230 stores its address at record +0x14
   (re/match/s28/005c9230.c line 50). Copies param_4 bytes from param_3 to *(param_1+0x4c)+param_2 (rep movsd/movsb, 0x005c932c..0x005c9336). When the
   byte at +0x45 is 0 (0x005c9338): n = (*(uint*)(param_1+0x34) / (byte +0x38 >> 3)) / byte +0x39 (two div at
   0x005c9350 / 0x005c9357), then on byte +0x39 == 2 the word at buf+2 goes to buf+n*2+2 (0x005c9363) and on
   == 1 or 2 the word at buf goes to buf+n*2 (0x005c936c). Returns param_1 (0x005c9373). */

void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)

// MATCH: 0x005c9310 FUN_005c9310
int FUN_005c9310(int param_1, int param_2, void *param_3, uint param_4)
{
  short *buf;
  uint n;
  uint ch;

  buf = *(short **)(param_1 + 0x4c);
  memcpy((char *)buf + param_2, param_3, param_4);
  if (*(char *)(param_1 + 0x45) == '\0') {
    ch = *(byte *)(param_1 + 0x39);
    n = (*(uint *)(param_1 + 0x34) / (uint)(*(byte *)(param_1 + 0x38) >> 3)) / ch;
    switch (ch) {
    case 2:
      buf[n + 1] = buf[1];
    case 1:
      buf[n] = buf[0];
    }
  }
  return param_1;
}

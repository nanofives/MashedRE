// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x00430670: best 81.6% (regs 96.6%, ops 96.6%), 87/85 ins (the 2 extra are the epilogue past
   Ghidra's 226 B). Raw converter 43.5%. By hand: FUN_00413f90 returns an array of 48-byte records, 3 groups of
   {count, ids[3]} each (+0x10 / +0x20 for modes 4 / 5); r = -1 shared exit, l[4] filled with -1 first.
   Residual (b) register allocation in the else-branch loops only: the original walks the ids with the record
   pointer register (eax, `add eax,4`), keeps the ids base in edx, v in esi and j in ecx; ours gets esi / ecx /
   edx / eax for the same four. Tried: ids/p assignment order, g->ids vs a base pointer, indexing vs walking,
   declaration orders, do-while / for forms, permuter (6 rounds, 144 compiles, no gain). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct Grp00413f90 { int count; int ids[3]; } Grp00413f90;
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_007f1a1c, 0x007f1a1c);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBALA(int, DAT_0067ea94, 4, 0x0067ea94);
MGLOBALA(int, DAT_0067ea98, 4, 0x0067ea98);
Grp00413f90 *FUN_00413f90(void);

// MATCH: 0x00430670 FUN_00430670
int FUN_00430670(int param_1)
{
  int l[4];
  int r, i, j, n, v, cnt;
  int *ids, *p;
  Grp00413f90 *g;

  r = -1;
  l[0] = -1;
  l[1] = -1;
  l[2] = -1;
  l[3] = -1;
  if (DAT_0067e9fc == 10) {
    for (i = 0, n = 0; i < 4; i++) {
      if (n >= param_1)
        break;
      if (DAT_0067ea98[i] != 0)
        n++;
    }
    if (n == param_1)
      return DAT_0067ea94[i] - 1;
  } else {
    l[0] = DAT_007f1a1c;
    g = FUN_00413f90() + DAT_0067f17c * 3;
    if (DAT_0067e9fc == 4)
      g++;
    else if (DAT_0067e9fc == 5)
      g += 2;
    cnt = g->count;
    p = g->ids;
    ids = p;
    for (i = 0; i < cnt; i++) {
      v = *p++;
      if (v == DAT_007f1a1c) {
        for (j = 0; j < 6; j++) {
          if (j != DAT_007f1a1c && j != ids[0] && j != ids[1] && j != ids[2]) {
            v = j;
            break;
          }
        }
      }
      l[i + 1] = v;
    }
    r = l[param_1];
  }
  return r;
}

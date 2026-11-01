// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00431b80 (363 B) + 0x00431d00 (98 B): both 100%, by hand. Same file text as 00431d00.c
   (keep one copy in the port). 0x00431b80 takes EAX = slot index, ESI = step, one stack arg (flag): it is a
   `static` callee defined first, parameter order (idx, step, flag), called by its real caller 0x00431d00
   (pilot finding 1). 0x00431d00 pushes the leftover EAX (0) for the 2nd call because the compiler knows the
   static callee keeps EAX. Ghidra's size of 0x00431b80 stops at 0x00431ceb; the whole 370 B obj symbol is
   byte-identical (match.py --bytes). What moved 0x00431b80: the first bump loop as `while (a[idx] == top)
   { a[idx] += step; moved = 1; }`, the three other-slot loops written out with `a[k] == a[idx]` compared
   first, one flag variable toggled with `moved = (moved + 1) & 1` (the original's dec/and), and `moved = 0`
   before `top = DAT_007f1a1c + 1`. Raw g2s was 31.6%. Residual: none. */
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
MGLOBAL(int, DAT_007f1a1c, 0x007f1a1c);
MGLOBAL(int, DAT_0067ea74, 0x0067ea74);
MGLOBAL(int, DAT_0067ea90, 0x0067ea90);
MGLOBAL(int, DAT_0067ea94, 0x0067ea94);
MGLOBALA(int, DAT_0067ea98, 3, 0x0067ea98);
MGLOBAL(int, DAT_0067eaac, 0x0067eaac);

// MATCH: 0x00431b80 FUN_00431b80
static void FUN_00431b80(int idx, int step, int flag)
{
  int moved; int top;
  if (DAT_0067ea98[idx] != 0) {
    moved = 0;
    top = DAT_007f1a1c + 1;
    do {
      while (DAT_0067ea98[idx] == top) {
        DAT_0067ea98[idx] += step;
        moved = 1;
      }
      if (idx != 0) {
        while (DAT_0067ea98[idx] == DAT_0067ea98[0] && DAT_0067ea98[idx] != 0) {
          DAT_0067ea98[idx] += step;
          moved = 1;
        }
      }
      if (idx != 1) {
        while (DAT_0067ea98[idx] == DAT_0067ea98[1] && DAT_0067ea98[idx] != 0) {
          DAT_0067ea98[idx] += step;
          moved = 1;
        }
      }
      if (idx != 2) {
        while (DAT_0067ea98[idx] == DAT_0067ea98[2] && DAT_0067ea98[idx] != 0) {
          DAT_0067ea98[idx] += step;
          moved = 1;
        }
      }
      moved = (moved - 1) & 1;
      if (step == 1) {
        if (flag) {
          if (DAT_0067ea98[idx] == 7) DAT_0067ea98[idx] = 0;
        } else {
          if (DAT_0067ea98[idx] == 7) DAT_0067ea98[idx] = 1;
        }
      } else {
        if ((flag == 0 && DAT_0067ea98[idx] == 0) || DAT_0067ea98[idx] == -1)
          DAT_0067ea98[idx] = 6;
      }
    } while (!moved);
  }
}

// MATCH: 0x00431d00 FUN_00431d00
void FUN_00431d00(void)
{
  DAT_0067ea74 = 1;
  DAT_0067ea90 = 1;
  DAT_0067ea94 = 0;
  DAT_0067ea98[0] = 1;
  DAT_0067ea98[1] = 2;
  DAT_0067ea98[2] = 3;
  DAT_0067eaac = 0;
  FUN_00431b80(0, 1, 0);
  FUN_00431b80(1, 1, 0);
  FUN_00431b80(2, 1, 0);
}

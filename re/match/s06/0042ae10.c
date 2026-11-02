// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042ae10 (157 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 63.2%. Pads as in re/match/pilot/a7_0042b310.c (stride 0x4c), but here the two banks must be one
   array Pad[2][16] at 0x007f1042 (bank 1 = 0x007f1502): two separate arrays gave two bases and 57.9%. The two
   loops sit in if/else with one shared return 0 at the end (a return 0 inside the first branch gave 70.2%; the
   permuter found nothing from there). Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef struct { char down; char pad[0x4b]; } Pad;   /* stride 0x4c, as in re/match/pilot/a7_0042b310.c */
typedef struct { int id; int pad[3]; } Slot;          /* stride 0x10 at 0x007f1a14 */
MGLOBAL(int, DAT_0067eab0, 0x0067eab0);
MGLOBAL(int, DAT_0067e7c8, 0x0067e7c8);
MGLOBAL(int, DAT_00898ab0, 0x00898ab0);
MGLOBAL(int, DAT_007f1a0c, 0x007f1a0c);
/* two banks of 16 pads (current, previous): bank 1 starts at 0x007f1502 = 0x007f1042 + 0x4c0 */
typedef Pad PadBank[16];
MGLOBALA(PadBank, DAT_007f1042, 2, 0x007f1042);
MGLOBALA(Slot, DAT_007f1a14, 4, 0x007f1a14);
int FUN_0040e470(int i);
// MATCH: 0x0042ae10 FUN_0042ae10
int FUN_0042ae10(void)
{
  int i;
  if (DAT_0067eab0 != 2 && DAT_0067e7c8 == 0 && DAT_00898ab0 != 0)
    return 0;
  if (DAT_007f1a0c == 0x1000) {
    for (i = 0; i < 8; i++) {
      if (DAT_007f1042[0][i].down && !DAT_007f1042[1][i].down)
        return 1;
    }
  } else {
    for (i = 0; i < 4; i++) {
      if (DAT_007f1a14[i].id != -1 && FUN_0040e470(i) != 2 && DAT_007f1042[0][DAT_007f1a14[i].id].down
          && !DAT_007f1042[1][DAT_007f1a14[i].id].down)
        return 1;
    }
  }
  return 0;
}

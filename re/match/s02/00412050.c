// FLAGS: /O2 /GS /Ob1
/* 0x00412050 (165 B): 100.0% exact, regs 100.0%. Raw g2s 79.6%.
   Pilot finding 3 applies: FUN_004b3f90 takes 1 argument (its add esp,4 right after the call) and the
   other six pushes belong to FUN_0047fad0 (7 args, popped by the add esp,0x1c after it), so the source is
   FUN_0047fad0(FUN_004b3f90(DAT_0063bc40), DAT_005cd004, 0, 0.1f, 0.9999f, 3.0f, seed).
   0x005cd004 is a 3-float .rdata block (0.15, 0.13, 0.15), passed by address.
   Loop form that matches: an int pointer at 0x0063bb60 written p[-1] / p[0], stepped by 11 ints (stride
   0x2c), loop control on seed (0xcafe .. 0xcb0a step 2). A struct pointer at the 0x0063bb5c element
   gives esi = &a instead (91.8%); an index loop gives an offset counter (98.0%).
   [UNCERTAIN] where the 0x2c-byte element really starts: the code only touches +0x0/+0x4 of
   0x0063bb5c + k*0x2c; the Pair2c typedef places it at 0x0063bb5c. Residual: none. */
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
typedef struct Pair2c {
  int a;
  int b;
  undefined4 rest[9];
} Pair2c;

MGLOBALA(float, DAT_005cd004, 3, 0x005cd004);
MGLOBALA(Pair2c, DAT_0063bb5c, 6, 0x0063bb5c);
MGLOBAL(int, DAT_0063bc40, 0x0063bc40);

void FUN_0047cdc0(int p0, float p1);
int FUN_0047fad0(int p0, float *p1, int p2, float p3, float p4, float p5, int p6);
int FUN_004b3f90(int p0);

// MATCH: 0x00412050 FUN_00412050
void FUN_00412050(void)
{
  int *p;
  int seed;

  p = &DAT_0063bb5c[0].b;
  for (seed = 0xcafe; seed < 0xcb0a; seed += 2) {
    p[-1] = FUN_0047fad0(FUN_004b3f90(DAT_0063bc40), DAT_005cd004, 0, 0.1f, 0.9999f, 3.0f,
                                     seed);
    *p = FUN_0047fad0(FUN_004b3f90(DAT_0063bc40), DAT_005cd004, 0, 0.1f, 0.9999f, 3.0f,
                                     seed + 1);
    FUN_0047cdc0(p[-1], 6.0f);
    FUN_0047cdc0(*p, 6.0f);
    p += 11;
  }
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00463590 (h7, by hand): 100%. DAT_0068f640 is an 8-byte record array (object pointer + dword).
   DAT_006904e0 is file-static under the DDK (pilot finding 2): only then may the flag-byte load at 0x004635d6 be
   scheduled ahead of the counter store at 0x004635dd. `<= 1` (not `< 2`) gives cmp 1 / jg. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct Obj00463590 {
  char pad0[0x30];
  int h30;
  unsigned char f34;
  char pad35[3];
  char pad38[4];
  float f3c;
} Obj00463590;
typedef struct { Obj00463590 *p; int q; } Rec0068f640;
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(int, DAT_006053e4, 0x006053e4);
#ifndef __cplusplus
static int DAT_006904e0;
#else
MGLOBAL(int, DAT_006904e0, 0x006904e0);
#endif
MGLOBALA(Rec0068f640, DAT_0068f640, 1, 0x0068f640);
int FUN_004115c0(int p0, float *p1, float *p2, float *p3);
void FUN_0045e0f0(int p0, float p1);
void FUN_005a66d0(Obj00463590 *p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00463590 FUN_00463590
void FUN_00463590(void)
{
  int i;
  float a;
  float b;
  float c;

  if (DAT_008aa254 <= 1) {
    i = DAT_006053e4;
    if (FUN_004115c0(DAT_006904e0, &a, &b, &c) != 0) {
      DAT_006904e0 = DAT_006904e0 + 1;
      if ((DAT_0068f640[i].p->f34 & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[i].p, 1);
        DAT_0068f640[i].p->f3c = 39050.0f;
        if (DAT_0068f640[i].p->h30 != 0) {
          FUN_005a6dc0(DAT_0068f640[i].p->h30, 4, 1, 39050.0f);
        }
        FUN_0045e0f0(i, 0.5f);
      }
    }
  }
}

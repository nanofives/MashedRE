// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 h8 0x00475c10: 100% (regs 100%), 86/86 instructions. Raw converter 34.9%.
   Per group i < DAT_0069160c: elements DAT_00691604[i] (0x50 B records), count DAT_00691610[i], live count
   DAT_00691614[i], pool record &DAT_00691608[i] (0x40 B). Fixes: typed globals and record structs; count in a local n;
   elements indexed e[j] (the compiler's +0x48-biased pointer); kill branch first (v >= s4c); pool pointer in a local.
   The last 23% was pilot finding 2: DAT_00691614 is a file-static global (static under C, MGLOBAL under C++). Only
   then does the compiler keep it in ebx across the inner loop and reload it after the three calls. */
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
typedef struct {
  undefined4 m[16];
  undefined1 b40[3];
  char b43;
  float age;
  int active;
  short s4c;
  short s4e;
} Elem;
typedef struct { undefined4 a[16]; } Pool;
MGLOBAL(Elem **, DAT_00691604, 0x00691604);
MGLOBAL(Pool *, DAT_00691608, 0x00691608);
MGLOBAL(int, DAT_0069160c, 0x0069160c);
MGLOBAL(int *, DAT_00691610, 0x00691610);
#ifndef __cplusplus
static int *DAT_00691614;
#else
MGLOBAL(int *, DAT_00691614, 0x00691614);
#endif
MGLOBAL(float, DAT_007f100c, 0x007f100c);
void FUN_004769f0(undefined1 *p0);
void FUN_00476a10(Elem *p0);
void FUN_00476d00(Pool *p0);
void FUN_00476df0(Pool *p0);
// MATCH: 0x00475c10 FUN_00475c10
void FUN_00475c10(void)
{
  int i;
  int j;
  int v;
  int n;
  float f;
  Elem *e;
  Pool *pool;

  for (i = 0; i < DAT_0069160c; i++) {
    e = DAT_00691604[i];
    n = DAT_00691610[i];
    f = (float)DAT_00691614[i] / n;
    pool = &DAT_00691608[i];
    for (j = 0; j < n; j++) {
      if (e[j].active != 0) {
        e[j].age = DAT_007f100c + e[j].age;
        v = (int)(e[j].age * e[j].age * f * f);
        if (v >= e[j].s4c) {
          e[j].active = 0;
          DAT_00691614[i]--;
        } else {
          if (v < e[j].s4e) v = e[j].s4e;
          else e[j].s4e = (short)v;
          e[j].b43 = (char)e[j].s4c - (char)v;
          FUN_004769f0(e[j].b40);
          FUN_00476a10(&e[j]);
          FUN_00476d00(pool);
        }
      }
    }
    FUN_00476df0(pool);
  }
}

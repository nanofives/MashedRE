// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cfe40 (h4): 100% (regs 100%). Teardown: drains the block list at 0x007d46e0 back into the free list
   0x007d46e4 through RwGlobals+0x11c, destroys that free list (FUN_004cc9f0), zeroes 0x007d46cc..d4, frees
   0x007d46d8 through RwGlobals+0x10c and destroys the free list 0x007d46dc. What moved it: no store of 0 to
   0x007d46e0 after the drain loop (the loop leaves it 0; Ghidra shows a store the bytes do not have). */
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
typedef void (*FreeToListFn)(void *list, void *block);
typedef void (*FreeFn)(void *p);
typedef struct Link { void *a; struct Link *next; } Link;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(Link *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(void *, DAT_007d46e4, 0x007d46e4);
MGLOBAL(int, DAT_007d46cc, 0x007d46cc);
MGLOBAL(int, DAT_007d46d0, 0x007d46d0);
MGLOBAL(int, DAT_007d46d4, 0x007d46d4);
MGLOBAL(void *, DAT_007d46d8, 0x007d46d8);
MGLOBAL(void *, DAT_007d46dc, 0x007d46dc);
int FUN_004cc9f0(void *list);

// MATCH: 0x004cfe40 FUN_004cfe40
void FUN_004cfe40(void)
{
  while (DAT_007d46e0) {
    Link *next = DAT_007d46e0->next;
    (*(FreeToListFn *)(DAT_007d3ff8 + 0x11c))(DAT_007d46e4, DAT_007d46e0);
    DAT_007d46e0 = next;
  }
  if (DAT_007d46e4) {
    FUN_004cc9f0(DAT_007d46e4);
    DAT_007d46e4 = 0;
  }
  DAT_007d46cc = 0;
  DAT_007d46d0 = 0;
  DAT_007d46d4 = 0;
  if (DAT_007d46d8) {
    (*(FreeFn *)(DAT_007d3ff8 + 0x10c))(DAT_007d46d8);
    DAT_007d46d8 = 0;
  }
  if (DAT_007d46dc) {
    FUN_004cc9f0(DAT_007d46dc);
    DAT_007d46dc = 0;
  }
}

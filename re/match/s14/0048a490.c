// FLAGS: /O2 /GS /Ob1
/* 0x0048a490: 100% (regs 100%, whole obj symbol byte-identical 77 B). Register-arg function: the record
   pointer is in ESI (Ghidra unaff_ESI, no ECX param: Ghidra's fastcall guess was wrong; the `push ecx` is
   a 4-byte local, an RGBA of 0xff bytes copied into obj+4). Matched as a `static` callee defined before its
   real caller 0x0048a5d0, which this file also holds (byte-identical too; same text as 0048a5d0.c).
   FUN_004c1480 takes 3 args (Ghidra's p0..p2), not the 4 the raw converter passed. Raw: 60.0%. */
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
typedef struct { uchar r, g, b, a; } Rgba;
typedef struct {
  int obj;              /* +0x00 */
  int f4, f8, fc;       /* +0x04 */
  char at10[0x40];      /* +0x10 */
} Rec50;                /* 0x50 */
typedef struct {
  int f0;               /* +0x000 */
  float f4;             /* +0x004 */
  float f8;             /* +0x008 */
  Rec50 recs[10];       /* +0x00c */
  int count;            /* +0x32c */
} Block330;             /* 0x330 */
MGLOBALA(Block330, DAT_00709238, 50, 0x00709238);
int FUN_004b40f0(int p0);
void FUN_004b5240(int p0, int p1);
int FUN_004c1480(int p0, void *p1, int p2);
void FUN_00474d80(int p0, int p1);
// MATCH: 0x0048a490 FUN_0048a490
static void FUN_0048a490(Rec50 *r)
{
  int x;
  if (r->obj != 0) {
    FUN_004c1480(*(int *)(r->obj + 4), r->at10, 0);
    x = FUN_004b40f0(r->obj);
    if (x != 0) {
      Rgba c;
      c.r = 0xff; c.g = 0xff; c.b = 0xff; c.a = 0xff;
      *(Rgba *)(x + 4) = c;
    }
    FUN_004b5240(r->obj, 0);
  }
}
// MATCH: 0x0048a5d0 FUN_0048a5d0
void FUN_0048a5d0(void)
{
  Block330 *b;
  int count;
  b = DAT_00709238;
  do {
    count = b->count;
    if (count != 0) {
      Rec50 *r = &b->recs[count];
      int n = count;
      do {
        r--;
        FUN_0048a490(r);
        FUN_00474d80(r->obj, 1);
      } while (--n != 0);
    }
    b->f0 = 0;
    *(int *)&b->f4 = 0;
    b++;
  } while ((int)b < 0x713198);
}

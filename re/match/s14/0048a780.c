// FLAGS: /O2 /GS /Ob1
/* 0x0048a780: 49.1% (regs 58.2%, ops 89.1%), 52/55 ins. Raw converter 30.9%. This file also holds the static
   callee 0x0048a490 (ESI register arg, 100% byte-identical; same body as 0048a490.c / 0048a5d0.c).
   FUN_0048a6f0 takes (record, float) (Ghidra had one arg). Semantics are complete; the residual is
   register allocation, class (b): the original keeps the block pointer in EDI, &b->f8 in ESI (loaded before
   the count test, used for the push and the store), record pointer in EBX and the counter in EBP; cl 9176
   here biases EDI to &b->f4 and folds the &b->f8 pointer into [edi+4]. Tried: explicit float *pf (at two
   places), temp t for the f4 store, index loops, raw int* form (40%). Permuter: 63.2% only with a
   non-semantic reorder (reads t before set), rejected. */
/* MATCH-S14 0x0048a780: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs14_20261102/results.tsv. */
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
MGLOBAL(float, DAT_007f100c, 0x007f100c);
void FUN_0048a6f0(Rec50 *p0, float p1);
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
// MATCH: 0x0048a780 FUN_0048a780
void FUN_0048a780(void)
{
  Block330 *b;
  int count;
  float t;
  b = DAT_00709238;
  do {
    if (b->f0 != 0) {
      float *pf = &b->f8;
      count = b->count;
      if (count != 0) {
        Rec50 *r = &b->recs[count];
        int n = count;
        do {
          r--;
          FUN_0048a6f0(r, *pf);
        } while (--n != 0);
      }
      *pf = 1.0f - b->f4;
      t = b->f4 - DAT_007f100c;
      b->f4 = t;
      if (t < 0.0f) {
        b->f0 = 0;
        count = b->count;
        if (count != 0) {
          Rec50 *r = &b->recs[count];
          int n = count;
          do {
            r--;
            FUN_0048a490(r);
          } while (--n != 0);
        }
      }
    }
    b++;
  } while ((int)b < 0x713198);
}

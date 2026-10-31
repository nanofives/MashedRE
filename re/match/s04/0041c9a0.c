// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041c9a0: 100% by hand (regs 100%), whole 348 B symbol byte-identical including the switch tables
   at 0x0041cadc/0x0041cae4 past Ghidra's 316 B. Raw converter 82.1%. What moved it: record in EAX (pilot finding
   1), so FUN_0041c9a0 is `static` and its real caller 0x0041ccc0 (4 records of 0x114 bytes at 0x0063ce20, flag at
   +0x110) is in this file after it, also 100%; the 0xff323232 value is a 4-byte colour struct built byte by byte
   and copied by value to obj->+0x18->+0x20->[0]->+4; the loop is `for (i < 32)` with `switch (i)` (the compiler
   rebases it to i - 2). This file holds 0x0041ccc0 (the caller). Note: the other helper's 0041ccc0.c has a
   stand-in FUN_0041c9a0 (no MATCH line); this file has the real one. Residual: none. */
/* MATCH-S04 0x0041c9a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs04_20261031/results.tsv. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct Rgba0041c9a0 { uchar red, green, blue, alpha; } Rgba0041c9a0;
typedef struct Mat0041c9a0 { void *m00; Rgba0041c9a0 color; } Mat0041c9a0;
typedef struct Geo0041c9a0 { char pad0[0x20]; Mat0041c9a0 **materials; } Geo0041c9a0;
typedef struct Obj0041c9a0 Obj0041c9a0;
struct Obj0041c9a0 { char pad0[0x18]; Geo0041c9a0 *geo; char pad1c[0x48 - 0x1c]; void (*m48)(Obj0041c9a0 *); };
typedef struct Rec0041c9a0 {
  int on[32];              /* +0x00 */
  Obj0041c9a0 *obj[32];    /* +0x80 */
  char pad100[0x110 - 0x100];
  int m110;
} Rec0041c9a0;
MGLOBALA(Rec0041c9a0, DAT_0063ce20, 4, 0x0063ce20);
// MATCH: 0x0041c9a0 FUN_0041c9a0
static void FUN_0041c9a0(Rec0041c9a0 *r)
{
  Rgba0041c9a0 c;
  int i;
  c.red = 0x32;
  c.green = 0x32;
  c.blue = 0x32;
  c.alpha = 0xff;
  if (r->on[25] != 0)
    r->obj[25]->m48(r->obj[25]);
  if (r->on[2] != 0) {
    r->obj[2]->geo->materials[0]->color = c;
    r->obj[2]->m48(r->obj[2]);
  }
  if (r->on[5] != 0) {
    r->obj[5]->geo->materials[0]->color = c;
    r->obj[5]->m48(r->obj[5]);
  }
  for (i = 0; i < 32; i++) {
    if (r->obj[i] != 0) {
      switch (i) {
      case 2: case 5: case 7: case 8: case 9: case 10: case 11: case 12: case 25:
        break;
      default:
        if (r->on[i] != 0)
          r->obj[i]->m48(r->obj[i]);
      }
    }
  }
  if (r->on[7] != 0) r->obj[7]->m48(r->obj[7]);
  if (r->on[8] != 0) r->obj[8]->m48(r->obj[8]);
  if (r->on[9] != 0) r->obj[9]->m48(r->obj[9]);
  if (r->on[10] != 0) r->obj[10]->m48(r->obj[10]);
  if (r->on[11] != 0) r->obj[11]->m48(r->obj[11]);
  if (r->on[12] != 0) r->obj[12]->m48(r->obj[12]);
}
// MATCH: 0x0041ccc0 FUN_0041ccc0
void FUN_0041ccc0(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    if (DAT_0063ce20[i].m110 != 0)
      FUN_0041c9a0(&DAT_0063ce20[i]);
  }
}

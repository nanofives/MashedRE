// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041b630: 100% by hand (regs 100%). Raw converter 83.3%: the callee 0x0041b340 takes the record
   in EAX (pilot finding 1). This file holds 0x0041b340 as a `static` callee defined first (also 100%, 99 ins).
   The loop is an index loop over the 4 records of 0x74 bytes at 0x0063c8d0 (the signed `jl` against the end
   address is the strength-reduced `i < 4`). Residual: none. */
/* MATCH-S04 0x0041b630: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef struct Obj0041b630 Obj0041b630;
struct Obj0041b630 { char pad0[0x48]; void (*m48)(Obj0041b630 *); };
typedef struct Rec0041b630 {
  Obj0041b630 *m00, *m04, *m08, *m0c, *m10, *m14, *m18, *m1c, *m20, *m24, *m28, *m2c, *m30, *m34, *m38, *m3c, *m40;
  char pad44[0x68 - 0x44]; uint m68; int m6c; char pad70[0x74 - 0x70];
} Rec0041b630;
MGLOBALA(Rec0041b630, DAT_0063c8d0, 4, 0x0063c8d0);
// MATCH: 0x0041b340 FUN_0041b340
static void FUN_0041b340(Rec0041b630 *r)
{
  r->m40->m48(r->m40);
  r->m00->m48(r->m00);
  if (r->m68 & 0x800) {
    r->m14->m48(r->m14);
    r->m10->m48(r->m10);
    if (r->m68 & 0x40)
      r->m18->m48(r->m18);
  } else if (r->m68 & 0x1000) {
    r->m08->m48(r->m08);
    r->m04->m48(r->m04);
    if (r->m68 & 0x40)
      r->m0c->m48(r->m0c);
  }
  if (r->m68 & 1) r->m1c->m48(r->m1c);
  if (r->m68 & 2) r->m20->m48(r->m20);
  if (r->m68 & 4) r->m24->m48(r->m24);
  if (r->m68 & 8) r->m28->m48(r->m28);
  if (r->m68 & 0x10) r->m2c->m48(r->m2c);
  if (r->m68 & 0x20) r->m30->m48(r->m30);
  if (r->m68 & 0x2000) r->m34->m48(r->m34);
  if (r->m68 & 0x4000) r->m38->m48(r->m38);
  if (r->m68 & 0x8000) r->m3c->m48(r->m3c);
}
// MATCH: 0x0041b630 FUN_0041b630
void FUN_0041b630(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    if (DAT_0063c8d0[i].m6c != 0)
      FUN_0041b340(&DAT_0063c8d0[i]);
  }
}

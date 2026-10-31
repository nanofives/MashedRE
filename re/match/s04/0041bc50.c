// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041bc50: 100% by hand (regs 100%), whole 603 B symbol byte-identical. Raw converter 84.7%:
   the record arrives in EAX (in_EAX, pilot finding 1), so FUN_0041bc50 is `static` and its real caller 0x0041c0c0
   (index loop over the 2 records of 0x16c bytes at 0x0063cab8) is in this file after it, also 100%. The record is
   an int array at +0x10 and an object-pointer array at +0xb0 (40 apart); each object is called through +0x48 when
   its int is set, in the order the binary shows. This file holds 0x0041c0c0 (the caller). Note: the other
   helper's 0041c0c0.c also holds 0x0041bc50 (same bytes); keep one copy in the port. Residual: none. */
/* MATCH-S04 0x0041bc50: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef struct Obj0041bc50 Obj0041bc50;
struct Obj0041bc50 { char pad0[0x48]; void (*m48)(Obj0041bc50 *); };
typedef struct Rec0041bc50 {
  char pad0[0x10];
  int on[40];             /* +0x10 */
  Obj0041bc50 *obj[40];   /* +0xb0 */
  char pad150[0x16c - 0x150];
} Rec0041bc50;
MGLOBALA(Rec0041bc50, DAT_0063cab8, 2, 0x0063cab8);
// MATCH: 0x0041bc50 FUN_0041bc50
static void FUN_0041bc50(Rec0041bc50 *r)
{
  if (r->on[3] != 0)
    r->obj[3]->m48(r->obj[3]);
  if (r->on[2] != 0)
    r->obj[2]->m48(r->obj[2]);
  if (r->on[0] != 0)
    r->obj[0]->m48(r->obj[0]);
  if (r->on[1] != 0)
    r->obj[1]->m48(r->obj[1]);
  if (r->on[4] != 0)
    r->obj[4]->m48(r->obj[4]);
  if (r->on[28] != 0)
    r->obj[28]->m48(r->obj[28]);
  if (r->on[29] != 0)
    r->obj[29]->m48(r->obj[29]);
  if (r->on[30] != 0)
    r->obj[30]->m48(r->obj[30]);
  if (r->on[31] != 0)
    r->obj[31]->m48(r->obj[31]);
  if (r->on[32] != 0)
    r->obj[32]->m48(r->obj[32]);
  if (r->on[33] != 0)
    r->obj[33]->m48(r->obj[33]);
  if (r->on[22] != 0)
    r->obj[22]->m48(r->obj[22]);
  if (r->on[23] != 0)
    r->obj[23]->m48(r->obj[23]);
  if (r->on[24] != 0)
    r->obj[24]->m48(r->obj[24]);
  if (r->on[25] != 0)
    r->obj[25]->m48(r->obj[25]);
  if (r->on[26] != 0)
    r->obj[26]->m48(r->obj[26]);
  if (r->on[27] != 0)
    r->obj[27]->m48(r->obj[27]);
  if (r->on[11] != 0)
    r->obj[11]->m48(r->obj[11]);
  if (r->on[12] != 0)
    r->obj[12]->m48(r->obj[12]);
  if (r->on[13] != 0)
    r->obj[13]->m48(r->obj[13]);
  if (r->on[14] != 0)
    r->obj[14]->m48(r->obj[14]);
  if (r->on[15] != 0)
    r->obj[15]->m48(r->obj[15]);
  if (r->on[16] != 0)
    r->obj[16]->m48(r->obj[16]);
  if (r->on[5] != 0)
    r->obj[5]->m48(r->obj[5]);
  if (r->on[6] != 0)
    r->obj[6]->m48(r->obj[6]);
  if (r->on[7] != 0)
    r->obj[7]->m48(r->obj[7]);
  if (r->on[8] != 0)
    r->obj[8]->m48(r->obj[8]);
  if (r->on[9] != 0)
    r->obj[9]->m48(r->obj[9]);
  if (r->on[10] != 0)
    r->obj[10]->m48(r->obj[10]);
}
// MATCH: 0x0041c0c0 FUN_0041c0c0
void FUN_0041c0c0(void)
{
  int i;
  for (i = 0; i < 2; i++)
    FUN_0041bc50(&DAT_0063cab8[i]);
}

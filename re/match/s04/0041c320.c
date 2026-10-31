// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041c320 (91 B): 100.0% exact (regs 100, ops 100), by hand. Raw converter: 51.6%.
   Two register args: clump in EAX, record in EBX. `static (clump, rec)`, defined before its only caller
   FUN_0041cb10 (call site 0x0041cb61); EBX is the record because the caller needs it after the call
   (rec->idx = i). FUN_004b3fc0 takes 2 args (clump, list), FUN_004b6520 2, FUN_004b5190 3 (returns the slot).
   This file also holds the caller FUN_0041cb10 (100%). Its clump argument is `i == 0 ? d : FUN_004e6ab0(d)`;
   the `i ? FUN_004e6ab0(d) : d` spelling inverts the branch (91.7%).
   Record (stride 0x114 at 0x0063ce20): +0x80 atom[32], +0x100 clump, +0x104 clump->+4, +0x108 idx.
   Residual: none. */
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
typedef struct { int pad0; int frame; } Clump_0041c320;
typedef struct { char pad[0x80]; int atom[32]; Clump_0041c320 *clump; int frame; int idx; char pad2[0x114 - 0x10c]; } Emit0041c320;   /* stride 0x114 */
MGLOBALA(Emit0041c320, DAT_0063ce20, 4, 0x0063ce20);
MGLOBAL(int, DAT_0063cdd0, 0x0063cdd0);
MGLOBALA(char, s_endpointpanel_txd_005cd1f4, 18, 0x005cd1f4);
MGLOBALA(char, s_endpointpanel_dff_005cd1e0, 18, 0x005cd1e0);
void FUN_004b3fc0(Clump_0041c320 *c, int *out);
void FUN_004b6520(void *p, int n);
int FUN_004b5190(int a, int b, int c);
int FUN_0042a6b0(char *name, int a, int b);
void FUN_004c5c80(int a);
Clump_0041c320 *FUN_0042a5d0(char *name, int a, int b);
Clump_0041c320 *FUN_004e6ab0(Clump_0041c320 *c);

// MATCH: 0x0041c320 FUN_0041c320
static void FUN_0041c320(Clump_0041c320 *c, Emit0041c320 *e)
{
  int i;
  int a;
  int list[24];
  FUN_004b3fc0(c, list);
  e->clump = c;
  e->frame = c->frame;
  FUN_004b6520(e->atom, 0x80);
  for (i = 0; i < 24; i++) {
    a = list[i];
    e->atom[FUN_004b5190(a, 0, 0)] = a;
  }
}

// MATCH: 0x0041cb10 FUN_0041cb10
void FUN_0041cb10(void)
{
  int i;
  Clump_0041c320 *d;
  Emit0041c320 *e;
  DAT_0063cdd0 = FUN_0042a6b0(s_endpointpanel_txd_005cd1f4, 0, 0);
  FUN_004c5c80(0);
  d = FUN_0042a5d0(s_endpointpanel_dff_005cd1e0, 0, 0);
  for (i = 0; i < 4; i++) {
    e = &DAT_0063ce20[i];
    FUN_0041c320(i == 0 ? d : FUN_004e6ab0(d), e);
    e->idx = i;
  }
}

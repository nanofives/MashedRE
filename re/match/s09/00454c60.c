// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00454c60 (h3): 100% (regs 100%, whole symbol byte-identical). Raw form 64.4% (in_EAX). The 0x14-byte
   entry arrives in EAX and is moved to ESI for FUN_00454bd0 (ESI): both are file-static callees (pilot finding 1),
   0x00454bd0 with its own MATCH line (100%). Stand-in caller shaped like the first loop of the original's caller
   at 0x00454e00 (not in pc_funcs.csv, not a MATCH target). The state chain is an if/else-if chain inside
   `!= 0 && != 1` (a switch gives a jump table). Field names are offsets only. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3 { float x; float y; float z; } V3;
typedef struct Clump20 { int f0; int *f4; } Clump20;
typedef struct Mat40 { char pad0[0x30]; V3 f30; } Mat40;
typedef struct E14 { int *f0[2]; Mat40 *f8; int fc; Clump20 *f10; } E14;
typedef struct E18 { V3 f0; int fc; int f10; int f14; } E18;
MGLOBALA(E14, DAT_00688358, 4, 0x00688358);
MGLOBALA(E18, DAT_006882f8, 4, 0x006882f8);
int FUN_004e4800(Clump20 *p0);
int FUN_004e45b0(int p0, Clump20 *p1);
int FUN_004c1480(int *p0, void *p1, int p2);
// MATCH: 0x00454bd0 FUN_00454bd0
static void FUN_00454bd0(E14 *e)
{
  int iVar1;
  e->f8 = 0;
  e->fc = 0;
  iVar1 = FUN_004e4800(e->f10);
  if (iVar1 != 0) {
    FUN_004e45b0(iVar1, e->f10);
  }
}
// MATCH: 0x00454c60 FUN_00454c60
static void FUN_00454c60(E14 *e)
{
  if (e->fc > 0 && e->fc <= 3) {
    FUN_004c1480(e->f10->f4, e->f8, 0);
  }
  if (e->fc != 0 && e->fc != 1) {
    if (e->fc == 2) {
      e->fc = 3;
    } else if (e->fc == 3) {
      e->fc = 4;
    } else if (e->fc == 4) {
      e->fc = 5;
    } else if (e->fc == 5) {
      e->fc = 0;
      FUN_00454bd0(e);
    }
  }
}
/* stand-in caller (not a MATCH target): first loop of the original's caller at 0x00454e00 */
void standin_00454e00(void)
{
  E14 *e;
  for (e = DAT_00688358; (int)e < (int)(DAT_00688358 + 4); e++) FUN_00454c60(e);
}

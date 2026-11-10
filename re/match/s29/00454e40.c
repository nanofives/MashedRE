// FLAGS: /O2 /GS /Ob1
/* MATCH-S29 (POWERUPINIT-B) 0x00454e40: the tail target of the 0x00454e70 init and of 0x00454f70. Static callees
   0x00454bd0 (ESI) and 0x00454c00 (EAX) copied from re/match/s09/00454bd0.c, where they are C4. */
/* ---- MATCH-S29 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
/* 0x00454bd0 FUN_00454bd0: matched in its own s09 source; here only so the caller's register passing is the original's */
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
/* 0x00454c00 FUN_00454c00: matched in its own s09 source; here only so the caller's register passing is the original's */
static void FUN_00454c00(E18 *p)
{
  p->fc = 0;
  p->f10 = 0;
  p->f14 = 0;
}
// MATCH: 0x00454e40 FUN_00454e40
void FUN_00454e40(void)
{
  E14 *e;
  E18 *p;
  for (e = DAT_00688358; (int)e < (int)(DAT_00688358 + 4); e++) FUN_00454bd0(e);
  for (p = DAT_006882f8; (int)p < (int)(DAT_006882f8 + 4); p++) FUN_00454c00(p);
}

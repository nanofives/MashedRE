// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004547c0 (h3): 100% (regs 100%, whole symbol byte-identical). Raw form 27% (unaff_EDI). The entry
   pointer arrives in EDI and FUN_00454170's record in ESI: both are file-static callees defined before the real
   caller FUN_004548a0 (pilot finding 1). The file holds 0x00454170, 0x004547c0 and 0x004548a0, all 100% with their
   own MATCH lines. FUN_004b6520 takes (ptr, n) (its body pushes both into 0x004b64e0 with 0), not () as Ghidra
   typed it. Entry = 0x2c-byte entries of the table at 0x00688240 (4 of them). Field names are offsets only. */
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
typedef struct Mat40 { V3 f0; int fc; V3 f10; int f1c; V3 f20; int f2c; V3 f30; int f3c; } Mat40;
typedef struct Frame34 { int f0; int f4; int f8; int fc; Mat40 f10; } Frame34;
typedef struct Clump20 { int f0; int *f4; } Clump20;
typedef struct Rec44 { V3 f0; float fc; V3 f10; V3 f1c; int f28; int f2c; Clump20 *f30; Frame34 *f34;
  int *f38; float f3c; float f40; } Rec44;
typedef struct Obj54 { int f0; int *f4; char pad8[0x40]; void (*f48)(struct Obj54 *); } Obj54;
typedef struct Obj0c { char pad0[0x10]; V3 f10; char pad1c[0x74]; V3 f90; } Obj0c;
typedef struct Ent2c { Rec44 *f0[2]; int f8; Obj0c *fc; int f10; Obj54 *f14[3]; Clump20 *f20; int f24;
  int f28; } Ent2c;
MGLOBALA(Rec44, DAT_00688020, 8, 0x00688020);
MGLOBALA(Ent2c, DAT_00688240, 4, 0x00688240);
int FUN_004c1210(Frame34 *p0);
void FUN_004c15c0(void *p0);
int FUN_004e43b0(int p0, Clump20 *p1);
int FUN_004e4440(Clump20 *p0);
void FUN_004b6520(void *p0, int p1);
int FUN_004e4800(Clump20 *p0);
int FUN_004e45b0(int p0, Clump20 *p1);
// MATCH: 0x00454170 FUN_00454170
static void FUN_00454170(Rec44 *p)
{
  int iVar1;
  p->f2c = 0;
  p->f28 = 0;
  p->f3c = 0;
  p->f0.x = 0;
  p->f0.y = 0;
  p->f0.z = 0;
  p->fc = 0;
  p->f40 = 0;
  FUN_004c15c0(p->f34);
  FUN_004c15c0(p->f38);
  if (p->f34->f4 != 0) {
    FUN_004c1210(p->f34);
  }
  iVar1 = FUN_004e4440(p->f30);
  if (iVar1 != 0) {
    FUN_004e43b0(iVar1, p->f30);
  }
}
// MATCH: 0x004547c0 FUN_004547c0
static void FUN_004547c0(Ent2c *e)
{
  int i;
  int iVar1;
  Rec44 *r;
  for (i = 0; i < 2; i++) {
    r = e->f0[i];
    if (r != 0) {
      e->f0[i] = 0;
      FUN_00454170(r);
    }
  }
  e->f8 = 0;
  e->fc = 0;
  e->f10 = 0;
  FUN_004b6520(e, 8);
  e->f24 = 0;
  e->f28 = 0;
  iVar1 = FUN_004e4800(e->f20);
  if (iVar1 != 0) {
    FUN_004e45b0(iVar1, e->f20);
  }
}
// MATCH: 0x004548a0 FUN_004548a0
void FUN_004548a0(void)
{
  Ent2c *e;
  Rec44 *p;
  for (e = DAT_00688240; (int)e < (int)(DAT_00688240 + 4); e++) FUN_004547c0(e);
  for (p = DAT_00688020; (int)p < (int)(DAT_00688020 + 8); p++) FUN_00454170(p);
}

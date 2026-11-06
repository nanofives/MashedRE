// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x004548e0 (h3): 100% (whole obj symbol byte-identical, 318 B). Ghidra's size (311 B) stops before
   the epilogue, so the instruction score reads 95.2%; match.py --bytes reports BYTE-IDENTICAL. Raw form 75.7%.
   What moved it: the k test as a switch (case 0 -> 1, case 1/2 -> 0; the if/else-if spellings gave 89-92%), the
   Rec44 loop as an int index (9176 biases the IV to +0x30, cmp 0x688270), the Ent2c loop as a pointer with an
   int-cast bound, the two .dff names as string literals (0x005ce454 / 0x005ce43c), FUN_004b3f90 with 1 arg and
   FUN_004c39b0's float result discarded (fstp st(0)). +0x14 of the 0x2c entry is a 3-slot array indexed by k.
   Field names are offsets only. */
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
MGLOBAL(int, DAT_006882f4, 0x006882f4);
MGLOBAL(int, DAT_006882f0, 0x006882f0);
MGLOBAL(V3, DAT_005f9808, 0x005f9808);
int FUN_0042a5d0(const char *p0, int p1, int p2);
void FUN_004548a0(void);
int FUN_004b3f90(int p0);
int FUN_004b3fc0(Clump20 *p0, Obj54 **p1);
int FUN_004b5190(Obj54 *p0, int p1, int p2);
void FUN_004b5240(void *p0, int p1);
void FUN_004b6520(void *p0, int p1);
void *FUN_004c0b30(void);
int FUN_004c1040(void *p0, void *p1);
float FUN_004c39b0(V3 *p0, V3 *p1);
void FUN_004c5c80(int p0);
Clump20 *FUN_004e69a0(int p0);
Clump20 *FUN_004e6ab0(int p0);
int FUN_004e7e30(Clump20 *p0, void *p1);
// MATCH: 0x004548e0 FUN_004548e0
void FUN_004548e0(void)
{
  Ent2c *e;
  Obj54 *x;
  Obj54 *l[3];
  int i;
  int k;
  int m;

  FUN_004b6520(DAT_00688240, 0xb0);
  FUN_004b6520(DAT_00688020, 0x220);
  FUN_004c5c80(0);
  DAT_006882f4 = FUN_0042a5d0("DepthCharge.dff", 0, 0);
  for (e = DAT_00688240; (int)e < (int)(DAT_00688240 + 4); e++) {
    e->f20 = FUN_004e6ab0(DAT_006882f4);
    FUN_004b3fc0(e->f20, l);
    for (i = 0; i < 3; i++) {
      x = l[i];
      k = FUN_004b5190(x, 0, 0);
      switch (k) {
      case 0:
        FUN_004b5240(x, 1);
        break;
      case 1:
      case 2:
        FUN_004b5240(x, 0);
        break;
      }
      e->f14[k] = x;
    }
  }
  DAT_006882f0 = FUN_0042a5d0("DepthChargeRound.dff", 0, 0);
  m = FUN_004b3f90(DAT_006882f0);
  for (i = 0; i < 8; i++) {
    DAT_00688020[i].f30 = FUN_004e69a0(m);
    FUN_004e7e30(DAT_00688020[i].f30, FUN_004c0b30());
    DAT_00688020[i].f38 = DAT_00688020[i].f30->f4;
    DAT_00688020[i].f34 = (Frame34 *)FUN_004c0b30();
    FUN_004c1040(DAT_00688020[i].f34, DAT_00688020[i].f38);
    FUN_004b5240(DAT_00688020[i].f30, 1);
  }
  FUN_004548a0();
  FUN_004c39b0(&DAT_005f9808, &DAT_005f9808);
}

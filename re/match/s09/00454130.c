// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00454130 (h3): 100% (regs 100%, whole symbol byte-identical). Raw form 56.5%. The 4-entry
   0x2c-byte table at 0x00688240 (same table as 0x004547c0/0x004548a0) as a struct array walked by an int index:
   9176 then emits the biased pointer IV (cmp esi,0x688308). A pointer loop gives a down-counter (dec edi), and a
   pointer loop with an int-cast bound an unbiased lea/cmp. The object at +0x18/+0x1c is called through its own
   +0x48 slot with itself as the argument. Field names are offsets only. */
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
MGLOBALA(Ent2c, DAT_00688240, 4, 0x00688240);
// MATCH: 0x00454130 FUN_00454130
void FUN_00454130(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    if (DAT_00688240[i].f10 == 1) {
      if (DAT_00688240[i].f28 != 0) DAT_00688240[i].f14[1]->f48(DAT_00688240[i].f14[1]);
      if (DAT_00688240[i].f24 != 0) DAT_00688240[i].f14[2]->f48(DAT_00688240[i].f14[2]);
    }
  }
}

// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b7110: 88.1% (regs 94.0%). Hand: Obj/Row/Out/Blk structs (rows stride 0x28, out entries 0x14), 1<<param_2 unmasked, blk!=0 then-arm first so the error arm lands at the end. Residual (b): original keeps stride in edx and copies the incremented index (mov ecx,ebx; cmp ecx,edx; jb) - 83 vs 84 instr; tail compare forms, row address form, permuter 158 compiles did not add the copy. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef struct Blk { int f0; int f4; int f8; int fc; } Blk;
typedef struct Row { char pad[0x20]; int f20; int f24; } Row;
typedef struct Out { int f0; int f4; int f8; int fc; int f10; } Out;
typedef struct Obj {
  char pad0[0x38]; char sub[0x18];
  int state; char pad50[0x24];
  Row *rows; char pad7c[0x10];
  uint stride; int err; uint *mask; Blk **blk; uint *idx; char pada0[4]; Out *out;
} Obj;
undefined4 FUN_005b1080(void *p0);
// MATCH: 0x005b7110 FUN_005b7110
Out *FUN_005b7110(Obj *o, int param_2)
{
  uint *puVar1;
  Out *puVar2;
  uint uVar3;

  if (o->state != 3) {
    puVar1 = o->idx + param_2;
    if (o->blk[*puVar1] == 0) {
      o->blk[*puVar1] = (Blk *)FUN_005b1080(o->sub);
      if (o->blk[*puVar1] != 0) {
        o->mask[*puVar1] = 0xffffffff;
        o->err = 0;
      } else {
        o->err = 1;
        return (Out *)0;
      }
    }
    uVar3 = *puVar1;
    if ((o->mask[uVar3] & (1 << param_2)) != 0) {
      puVar2 = o->out + (o->stride * param_2 + uVar3);
      puVar2->f0 = o->blk[uVar3]->f0;
      puVar2->fc = o->blk[*puVar1]->fc;
      puVar2->f8 = o->rows[param_2].f20;
      puVar2->f4 = o->blk[*puVar1]->f4 + o->rows[param_2].f24;
      uVar3 = *puVar1;
      *puVar1 = uVar3 + 1;
      if (uVar3 + 1 >= o->stride) {
        *puVar1 = 0;
        return puVar2;
      }
      return puVar2;
    }
  }
  return (Out *)0;
}

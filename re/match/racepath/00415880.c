// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH4 0x00415880 (first hand source; raw g2s 28.7%): 40.3% (regs 46.4, ops 89.3),
   345/345 ins, 889 of 1143 B differ. Kept as the next slice's start, not promoted.
   Shape done: mode switch with the int[][5] table at 0x005f2fc0 indexed [(int)level][band]; 4-car scan with
   `d > 0.0f || d < 5 - lvl` (fall-through = the behind branch); one float local shared by the progress value,
   the first dot product and the FUN_0046d6d0 output (the original stores all three to [esp+0x14]); dot products
   as `pf[2]*n.z + (pf[1]*n.y + n.x*pf[0])`; `!(x < 0.2f)` for the `test ah,5 / jnp` compares; literals 0.75f
   0x005cc950, 0.2f 0x005cc9c0, 1500.0f 0x005cd0ac, 500.0f 0x005ccd04, 0.6f 0x005cc318, 3.0f 0x005cc31c, 0.5f
   0x005cc32c, 0.0f 0x005d757c.
   Residual (register allocation): the original keeps car*0x74 in EBP for the whole function (absolute
   displacements [ebp+0x89a508] / [ebp+0x89a4dc] / [ebp+0x89a4e0]), car in ESI only before the loop, and
   compares the loop index against the stack copy of car (0x00415965). cl 9176 here keeps car in EBP and the
   offset in ESI, spilled during the loop, which shifts every register and the RMW `sub [mem],eax` at
   0x00415893. Tried with no change: the record as a struct array, as three field arrays, as the AiBlock at
   0x0089a360, an explicit `off = car * 0x74` byte offset, a pointer to the timer, `-=` folded into the test,
   `i == car` continue form, car unsigned, a copy `me = car` for the pre-loop calls, dy written into car's
   stack slot. Permuter: all kinds 2332 compiles (52.5%, but every gain moved a statement after a goto or a
   return, or reordered two calls: discarded); semantics-safe kinds 198 compiles: `timer <= -1` for
   `timer < 0` (49.0% insn but 896 B differ, and the original's jns matches `< 0`: not kept). */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { float x, y, z; } RwV3d;
typedef struct { float x, y, z; } V3;
typedef struct {
  int v;
  char pad[0x70];
} CarInt;
MGLOBAL(int, DAT_007f1008, 0x007f1008);
MGLOBAL(int, DAT_0089a368, 0x0089a368);
MGLOBAL(float, DAT_0089a360, 0x0089a360);
MGLOBAL(int, DAT_0089a374, 0x0089a374);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
typedef int Row5[5];
MGLOBALA(Row5, DAT_005f2fc0, 11, 0x005f2fc0);
MGLOBALA(CarInt, DAT_0089a4dc, 4, 0x0089a4dc);
MGLOBALA(CarInt, DAT_0089a4e0, 4, 0x0089a4e0);
MGLOBALA(CarInt, DAT_0089a508, 4, 0x0089a508);
float FUN_00408a50(int car);
float *FUN_00408af0(int car);
int FUN_0040e470(int car);
int FUN_00426bb0(void);
int FUN_0046c7b0(int car);
void FUN_0046cbb0(int car, int *a, int *b);
int FUN_0046cc10(int *out, int car);
int FUN_0046d4a0(int *out, int car);
int FUN_0046d510(V3 *out, int car);
int FUN_0046d6d0(float *out, int car);
float FUN_004c39b0(V3 *out, V3 *in);
float FUN_004c3ac0(V3 *v);

// MATCH: 0x00415880 FUN_00415880
int FUN_00415880(int unused1, int unused2, int *out, int car)
{
  int rp;
  float px;
  float pz;
  float myf;
  float f;
  V3 h;
  int lvl;
  float myprog;
  int best;
  int behind;
  float bestd;
  int i;
  int a;
  int b;
  float d;
  V3 v;
  float len;
  V3 n;
  float align;
  float dy;
  int st;
  float *pf;
  int n5;

  DAT_0089a508[car].v -= DAT_007f1008;
  if (DAT_0089a508[car].v < 0) {
    DAT_0089a508[car].v = 0;
  }
  if (DAT_0089a368 == 0) {
    n5 = DAT_005f2fc0[(int)DAT_0089a360][DAT_0089a374];
  }
  else if (DAT_0089a368 == 1) {
    n5 = 0;
  }
  else {
    n5 = 2;
    if (DAT_0089a368 != 2) {
      n5 = car;
    }
  }
  if ((DAT_007f0fd0 != 8 && n5 == 0) || DAT_0089a508[car].v == 0) {
    return 0;
  }
  FUN_0046d4a0(&rp, car);
  px = *(float *)(rp + 0x30);
  pz = *(float *)(rp + 0x38);
  FUN_0046d6d0(&myf, car);
  FUN_0046d510(&h, car);
  h.y = 0.0f;
  lvl = FUN_00426bb0();
  myprog = FUN_00408a50(car);
  best = -1;
  behind = 0;
  bestd = 2.0f;
  for (i = 0; i < 4; i++) {
    if (i != car && FUN_0046c7b0(i) != 0 && FUN_0040e470(i) == 1) {
      FUN_0046cbb0(i, &a, &b);
      if (a == 0) {
        f = FUN_00408a50(i);
        d = f - myprog;
        if (d > 0.0f || d < 5 - lvl) {
          if (d < bestd) {
            bestd = d;
            behind = 0;
            best = i;
          }
        }
        else if (-d < bestd) {
          bestd = -d;
          behind = 1;
          best = i;
        }
      }
    }
  }
  if (DAT_0089a4dc[car].v != 0) {
    if (best == DAT_0089a4e0[car].v) {
      if (behind == 0) {
        DAT_0089a4dc[car].v = 0;
      }
      FUN_0046d4a0(&rp, best);
      v.x = *(float *)(rp + 0x30) - px;
      v.y = 0.0f;
      v.z = *(float *)(rp + 0x38) - pz;
      len = FUN_004c3ac0(&v);
      FUN_004c39b0(&n, &v);
      pf = FUN_00408af0(best);
      if (-((pf[2] * n.z + (pf[1] * n.y + n.x * pf[0])) * len) > 0.75f) {
        DAT_0089a4dc[car].v = 0;
      }
      FUN_0046d510(&v, DAT_0089a4e0[car].v);
      pf = FUN_00408af0(DAT_0089a4e0[car].v);
      f = pf[2] * v.z + (pf[1] * v.y + v.x * pf[0]);
      FUN_0046d510(&v, car);
      pf = FUN_00408af0(car);
      if (!(f < 0.2f) && !(pf[2] * v.z + (pf[1] * v.y + v.x * pf[0]) < 0.2f)) {
        best = DAT_0089a4e0[car].v;
        goto hit;
      }
    }
    DAT_0089a4dc[car].v = 0;
    return 0;
  }
  if (best == -1) {
    return 0;
  }
  FUN_0046d4a0(&rp, best);
  v.x = *(float *)(rp + 0x30) - px;
  v.y = 0.0f;
  v.z = *(float *)(rp + 0x38) - pz;
  len = FUN_004c3ac0(&v);
  if (behind != 1) {
    return 0;
  }
  FUN_004c39b0(&n, &v);
  pf = FUN_00408af0(best);
  align = -((pf[2] * n.z + (pf[1] * n.y + n.x * pf[0])) * len);
  FUN_0046d6d0(&f, best);
  dy = f - myf;
  if (dy < 0.0f) {
    dy = -dy;
  }
  FUN_0046cc10(&st, best);
  if (st != 4 || !(dy < 1500.0f) || !(f > 500.0f) || !(align < 0.6f) || !(len < 3.0f) || !(len > 0.5f)) {
    return 0;
  }
  DAT_0089a4dc[car].v = 1;
  DAT_0089a4e0[car].v = best;
hit:
  FUN_0046d4a0(&rp, best);
  out[0] = *(int *)(rp + 0x30);
  out[1] = *(int *)(rp + 0x38);
  return 1;
}

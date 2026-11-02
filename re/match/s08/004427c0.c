// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x004427c0: best 52.7% (regs ~70%), by hand (raw did not compile: Ghidra's __thiscall is wrong, the
   leading push ecx only reserves the local the FUN_0046cbb0 out-pointer writes). cdecl, one stack arg. Shape:
   early-out chain to `done` with idx = 0 (mode 5; mode 10 with FUN_0046cbb0/FUN_0046c7b0; mode 8 with
   FUN_00417740(0) + 1 == 0, which is what gives the original's inc eax / jne; FUN_0046c7b0(car)), then the table
   DAT_005f9550[t >> 8]: 0 -> 0, 2 -> 0x0089899c or 0x008964a4 by bit 8 of the counter, 3 -> 0x00898998 when its
   +0xc8 is above 80.0f, else the index of the smallest +0xc8 between 0x008964a0 and 0x0089649c. Then the 16/60
   hysteresis on 0x006831cc and the copy of the record's matrix into the frame (rep movsd).
   Residual (b): 9176 here unrolls the min loop by 8, the original by 4 (loop forms tried: <=, < hi+1, hi >= i
   (no unroll), local bound, pointer walk, ++i, idx-indexed compare, while: none gives 4); the original keeps
   param_1 in ebp and the table value in eax, 9176 here ebx / esi; a switch on the table value is worse (49.8%).
   Permuter: no improvement (174 compiles in 300 s). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_004427c0;
typedef struct {
  V3_004427c0 right; unsigned int flags;
  V3_004427c0 up; unsigned int pad1;
  V3_004427c0 at; unsigned int pad2;
  V3_004427c0 pos; unsigned int pad3;
} Mtx_004427c0;
typedef struct {
  char pad0[0x4c];
  Mtx_004427c0 mtx;
  char pad8c[0xc0 - 0x8c];
  int fc0;
  int pad_c4;
  float fc8;
  int fcc;
  char padd0[0xd8 - 0xd0];
} Rec_004427c0;
typedef struct {
  char pad0[0x20];
  int car;
  char pad24[0x58 - 0x24];
  int f58;
  char pad5c[0x84 - 0x5c];
  int *frame;
} Cam_004427c0;
MGLOBAL(unsigned int, DAT_00897fc0, 0x00897fc0);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_006831d4, 0x006831d4);
MGLOBAL(int, DAT_006831cc, 0x006831cc);
MGLOBAL(int, DAT_006831d0, 0x006831d0);
MGLOBAL(int, DAT_00896498, 0x00896498);
MGLOBAL(int, DAT_0089899c, 0x0089899c);
MGLOBAL(int, DAT_008964a4, 0x008964a4);
MGLOBAL(int, DAT_00898998, 0x00898998);
MGLOBAL(int, DAT_008964a0, 0x008964a0);
MGLOBAL(int, DAT_0089649c, 0x0089649c);
MGLOBALA(int, DAT_005f9550, 16, 0x005f9550);
MGLOBALA(Rec_004427c0, DAT_008964c0, 32, 0x008964c0);
int FUN_00417740(int p0);
void FUN_00441700(Cam_004427c0 *p0);
int FUN_0046c7b0(int p0);
void FUN_0046cbb0(int p0, int *p1, int *p2);
// MATCH: 0x004427c0 FUN_004427c0
void FUN_004427c0(Cam_004427c0 *param_1)
{
  int a;
  int b;
  int mode;
  int idx;
  int i;
  unsigned int t;
  float m;

  DAT_00897fc0++;
  t = DAT_00897fc0 & 0xfff;
  if (DAT_007f0fd0 == 5) {
    idx = 0;
    goto done;
  }
  if (DAT_007f0fd0 == 10) {
    FUN_0046cbb0(0, &b, &a);
    if (b != 0 || FUN_0046c7b0(0) == 0) {
      idx = 0;
      goto done;
    }
  }
  if (DAT_007f0fd0 == 8 && FUN_00417740(0) + 1 == 0) {
    idx = 0;
    goto done;
  }
  if (FUN_0046c7b0(param_1->car) == 0) {
    idx = 0;
    goto done;
  }
  mode = DAT_005f9550[(int)t >> 8];
  if (mode == 0) {
    idx = 0;
    goto done;
  }
  if (mode == 2) {
    idx = DAT_0089899c;
    if (DAT_00897fc0 & 0x100) {
      idx = DAT_008964a4;
    }
    goto done;
  }
  if (mode == 3) {
    idx = DAT_00898998;
    if (DAT_008964c0[idx].fc8 > 80.0f) {
      goto done;
    }
  }
  idx = DAT_008964a0;
  m = DAT_008964c0[idx].fc8;
  for (i = idx; i <= DAT_0089649c; i++) {
    if (DAT_008964c0[i].fc8 < m) {
      m = DAT_008964c0[i].fc8;
      idx = i;
    }
  }
done:
  DAT_006831d4++;
  if (idx != DAT_006831cc) {
    DAT_006831d0++;
    if (DAT_006831d0 < 16 || DAT_006831d4 <= 60) {
      idx = DAT_006831cc;
    } else {
      DAT_006831cc = idx;
      DAT_006831d0 = 0;
      DAT_006831d4 = 0;
    }
  }
  DAT_00896498 = DAT_008964c0[idx].fcc;
  param_1->f58 = DAT_008964c0[idx].fc0;
  FUN_00441700(param_1);
  *(Mtx_004427c0 *)(param_1->frame[1] + 0x10) = DAT_008964c0[idx].mtx;
}

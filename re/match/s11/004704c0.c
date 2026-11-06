// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x004704c0 (h5): best 91.0% (regs 91.8%). Raw 28.6%.
   0x0046f6c0 is a static callee with a register convention (ctx in EAX, car record in EDI, flag on the stack; its
   prologue is push ebx / mov ebx, eax and it never writes EDI). Its real body (3581 B) is in 0046f6c0.c; this file
   holds a small STAND-IN with no MATCH line, shaped only so cl 9176 gives it that same convention (car used in a
   loop across calls -> EDI, ctx used once after two calls -> EAX). The stand-in is not the real TU: the port must
   drop it and call the real 0x0046f6c0. pos is a 12-byte struct by value (the caller copies it with mov ecx, esp).
   Moved the score: static stand-in convention (55.6 -> 87.3), `if (i >= 16)` (cmp 0x10 / jb), the m0 flags store
   after `m1 = ...` (permuter, 91.0). The two matrix inits are RenderWare's RwMatrixSetIdentityMacro shape
   (0x20003); written as the macro they give the same stores (90.3%), kept as plain stores here.
   Residual (b): the original subtracts pos.y - FUN_0046d400() right after the call (fsubr before add esp) and keeps
   it in ST0 until just before the first FUN_004c51a0 call (fstp v.y there); ours stores it to v.y at once. Tried:
   y as local float/double, `pos.y -=` then v = pos / field copy, aggregate init in a nested block, h kept and
   subtracted late (85.8%). The trailing pops past 0x00470664 are beyond Ghidra's size. */
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
typedef struct V3_004704c0 { float x, y, z; } V3_004704c0;
MGLOBALA(char, DAT_008815a0, 0xd040, 0x008815a0);
MGLOBALA(float, DAT_006146fc, 3, 0x006146fc);
float FUN_0046d400(V3_004704c0 pos, int ctx);
void FUN_004c51a0(float *m, V3_004704c0 *v, int combine);
void FUN_004c4d20(float *m, float *axis, float angle, int combine);
void FUN_0047ea60(unsigned int car, float *m);
void FUN_0041eda0(unsigned int car, int on);
int FUN_0046baa0(int id);

#ifndef MATCH_PORT
/* stand-in for the static callee 0x0046f6c0 (ctx in EAX, car record in EDI, flag on the stack); the real body is
   in 0046f6c0.c */
extern int g_stub_0046f6c0[4];
extern void stub_call_0046f6c0(int a, int b);
static void FUN_0046f6c0(int flag, int *ctx, char *car)
{
  int k;
  float *q = (float *)(car + *(int *)(car + 0x9ac) * 0x40 + 0x928);
  g_stub_0046f6c0[0] = 0;
  stub_call_0046f6c0((int)q, 1);
  stub_call_0046f6c0((int)q, 2);
  g_stub_0046f6c0[1] = (int)ctx;
  stub_call_0046f6c0(ctx[2], 0);
  for (k = 0; k < 4; k++) {
    stub_call_0046f6c0((int)q, k);
    stub_call_0046f6c0(k, *(int *)(car + 0x198));
    stub_call_0046f6c0(k, *(int *)(car + 0x19c));
    stub_call_0046f6c0(k, *(int *)(car + 0x1a0));
    stub_call_0046f6c0(k, *(int *)(car + 0x1a4));
    stub_call_0046f6c0(0, *(int *)(car + 0x1a8));
  }
  if (flag)
    stub_call_0046f6c0(*(int *)(car + 0x9e0), 2);
}
#else
void FUN_0046f6c0(int flag, int *ctx, char *car);
#endif

// MATCH: 0x004704c0 FUN_004704c0
int FUN_004704c0(unsigned int i, V3_004704c0 pos, float angle, int ctx)
{
  char *car;
  int *p;
  int n;
  float *m0;
  float *m1;
  V3_004704c0 v;
  float y;

  if (i >= 16)
    return 0;
  car = DAT_008815a0 + i * 0xd04;
  *(int *)(car + 0x9f0) = 0;
  *(int *)(car + 0x9f4) = 0;
  y = pos.y - FUN_0046d400(pos, ctx);
  p = (int *)(car + 0x4ac);
  n = 0x12;
  do {
    *p = -1;
    p += 0x10;
  } while (--n);
  p = (int *)(car + 0x1ec);
  n = 4;
  do {
    *p = -1;
    p += 0x31;
  } while (--n);
  *(int *)(car + 0x9a8) = 0;
  *(int *)(car + 0x9ac) = 1;
  *(int *)(car + 0xad0) = 0;
  m0 = (float *)(car + 0x928);
  m0[4] = 0; m0[2] = 0; m0[1] = 0;
  m0[9] = 0; m0[8] = 0; m0[6] = 0;
  m0[14] = 0; m0[13] = 0; m0[12] = 0;
  m0[10] = 1.0f; m0[5] = 1.0f; m0[0] = 1.0f;
  m1 = (float *)(car + 0x968);
  ((unsigned int *)m0)[3] |= 0x20003;
  ((unsigned int *)m1)[3] |= 0x20003;
  m1[10] = 1.0f; m1[5] = 1.0f; m1[0] = 1.0f;
  m1[4] = 0; m1[2] = 0; m1[1] = 0;
  m1[9] = 0; m1[8] = 0; m1[6] = 0;
  m1[14] = 0; m1[13] = 0; m1[12] = 0;
  v.x = pos.x;
  v.y = y;
  v.z = pos.z;
  FUN_004c51a0(m0, &v, 0);
  FUN_004c51a0(m1, &v, 0);
  FUN_004c4d20(m0, DAT_006146fc, angle, 1);
  FUN_004c4d20(m1, DAT_006146fc, angle, 1);
  FUN_0046f6c0(0, (int *)ctx, car);
  FUN_0047ea60(i, m0);
  FUN_0041eda0(i, 1);
  FUN_0046baa0(*(int *)car);
  return 1;
}

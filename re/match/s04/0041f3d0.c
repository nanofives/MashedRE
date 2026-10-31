// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041f3d0 by hand (h9): 100% (regs 100%). Raw g2s 42.4%. Moved it: record struct (wheel clumps +0x00/+0x84 by mode +0x298, spin angles +0x220, +0x290 key), indexing r->spin[i] in a for loop (cl strength-reduces all three arrays onto one pointer), the parenthesised ((float)dt * k) * rate (fimul), the compound += (a temp float t kept the value in st0 instead of the stack slot), and float LITERALS 360.0f / -360.0f / 0.00033333333f (= 0x39aec33e; the original pools them at 0x005ccac4 / 0x005cd270 / 0x005cc948): with a global operand cl emits fld k; fadd [x] for the += 360 step, with the literal fld [x]; fadd k as the original. dt is unsigned (2^32 fix-up at 0x005cc94c is the compiler's). The caller 0x00420870 pushes 3 arguments; the third is unused here. Permuter on the 84.8% form found nothing. No residual. */
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
typedef struct { float x, y, z; } V3;
typedef struct {
  char *wheelA[4];               /* +0x000 */
  char u010[0x84 - 0x10];
  char *wheelB[4];               /* +0x084 */
  char u094[0x220 - 0x94];
  float spin[4];                 /* +0x220 */
  char u230[0x290 - 0x230];
  unsigned int x290;             /* +0x290 */
  int u294;
  int mode;                      /* +0x298 */
} CarRec;
MGLOBAL(V3, DAT_006146f0, 0x006146f0);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
void FUN_0046bd60(int *out, unsigned int v, int i);
void FUN_0046bda0(float *out, unsigned int v, int i);
char *FUN_004c1340(char *frame, V3 *v, int op);
char *FUN_004c1520(char *frame, V3 *axis, float angle, int op);
// MATCH: 0x0041f3d0 FUN_0041f3d0
void FUN_0041f3d0(CarRec *r, unsigned int dt, int unused)
{
  int i;
  char *frame;
  int rate;
  float steer;
  V3 pos;
  for (i = 0; i < 4; i++) {
    if (r->mode == 0)
      frame = r->wheelA[i];
    else if (r->mode == 1)
      frame = r->wheelB[i];
    else
      return;
    frame = *(char **)(frame + 4);
    FUN_0046bd60(&rate, r->x290, i);
    r->spin[i] += ((float)dt * 0.00033333333f) * rate;
    if (r->spin[i] > 360.0f)
      r->spin[i] -= 360.0f;
    if (r->spin[i] < -360.0f)
      r->spin[i] += 360.0f;
    FUN_004c1520(frame, &DAT_006146f0, r->spin[i], 0);
    FUN_0046bda0(&steer, r->x290, i);
    frame = *(char **)(frame + 4);
    pos = *(V3 *)(frame + 0x40);
    FUN_004c1520(frame, &DAT_006146fc, steer, 0);
    FUN_004c1340(frame, &pos, 2);
  }
}

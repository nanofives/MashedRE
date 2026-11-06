// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00459480: 100% by hand (179 B whole symbol BYTE-IDENTICAL). Raw was 57.9% (unaff_EDI).
   The original takes its slot pointer in EDI: its only caller FUN_0045a190 (call at 0x0045a1bd) walks the
   0x58-byte slot array at 0x0068b9f8 with EDI. That is the static-function convention (pilot finding 1):
   FUN_00459480 is static, defined before a caller in the same file. caller_0045a190 is a stand-in for that
   caller (not matched). The .rdata floats 0x005cc35c/0x005cd0e8/0x005cc31c/0x005cc32c are the literals
   4.0f/0.003f/3.0f/0.5f. What moved the last step: `d = d * 0.5f` as its own statement. Residual: none. */
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
typedef struct {
  int idx;          /* +0x00 */
  int f04;
  int f08;
  int f0c;
  int f10;          /* +0x10 */
  int f14;
  int f18;
  float f1c;        /* +0x1c */
  int f20[4];
  float f30;        /* +0x30 */
  int f34;
  int mat;          /* +0x38 */
  float f3c[3];     /* +0x3c */
  float f48[3];     /* +0x48 */
  int clump;        /* +0x54 */
} Slot459480;
int FUN_004671a0(int a);
float FUN_00474e70(float *a, float *b, float *c);
void FUN_004c0e50(int frame);
void FUN_004c45f0(int m);
void FUN_004c5010(int m, float *v, int op);
// MATCH: 0x00459480 FUN_00459480
static void FUN_00459480(Slot459480 *p)
{
  float v[3];
  float d;
  int m;

  v[2] = p->f1c * 4.0f;
  v[0] = 4.0f;
  v[1] = 4.0f;
  d = FUN_00474e70(p->f48, p->f3c, (float *)(*(int *)(FUN_004671a0(0) + 4) + 0x40));
  if (d < 0.003f) {
    d = 0.003f;
  } else if (d > 3.0f) {
    d = 3.0f;
  }
  d = d * 0.5f; v[0] = v[0] * d; v[1] = d * v[1];
  m = *(int *)(p->clump + 4) + 0x10;
  FUN_004c5010(m, v, 1);
  FUN_004c45f0(m);
  FUN_004c0e50(*(int *)(p->clump + 4));
}

MGLOBALA(Slot459480, DAT_0068b9f8, 4, 0x0068b9f8);
/* stand-in for the real caller FUN_0045a190 (loop over the slots with the slot pointer in EDI); not matched */
void caller_0045a190(void)
{
  Slot459480 *p;

  p = &DAT_0068b9f8[4];
  do {
    p--;
    if (p->f10 != 0) {
      p->f30 = p->f30 + 1.0f;
      FUN_00459480(p);
      FUN_004c45f0(p->f34);
    }
  } while (p != DAT_0068b9f8);
}

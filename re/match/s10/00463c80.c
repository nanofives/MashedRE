// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00463c80 (h7, by hand): 100%, relocheck OK. DAT_0068f640 is an 8-byte record array (object
   pointer + dword); DAT_0088e670 points at 0x90-byte records (dword at +0x80). Float constants are source literals
   (100.0, 0.01, 200.0, 0.005, 1.57, 4.71, double 1.0 at 0x005cd0c8, 1.0, 0.0, 22050.0, 2205.0).
   What moved it: the sin() expression sits in both branches (the compiler tail-merges it at 0x00463dc1, which
   gives the store-and-reload of s), written `1.57f * (v * 0.01f)`: `v * 0.01f * 1.57f` folds the two constants
   (relocheck BAD on the masked operands). */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct Obj00463c80 {
  char pad0[0x30];
  int h30;
  unsigned char f34;
  char pad35[7];
  float f3c;
} Obj00463c80;
typedef struct { Obj00463c80 *p; int q; } Rec0068f640;
typedef struct { char pad0[0x80]; int base; char pad84[0xc]; } Rec0088e670;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(Rec0088e670 *, DAT_0088e670, 0x0088e670);
MGLOBAL(int, DAT_0068fc90, 0x0068fc90);
MGLOBAL(int, DAT_0068f4c0, 0x0068f4c0);
MGLOBALA(Rec0068f640, DAT_0068f640, 1, 0x0068f640);
double sin(double);
#pragma intrinsic(sin)
int FUN_0040e350(void);
void FUN_0045daf0(int p0, int p1);
void FUN_0045e0f0(int p0, float p1);
int FUN_00461e90(int p0, int p1, int *p2, int *p3);
int FUN_0046cc10(int *p0, int p1);
int FUN_0046d2e0(int p0, int p1, int *p2);
int FUN_0046d6a0(float *p0, int p1);
float FUN_00472650(float lo, float hi);
void FUN_005a66d0(Obj00463c80 *p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00463c80 FUN_00463c80
void FUN_00463c80(int param_1, int param_2)
{
  int a;
  int b;
  int st;
  int k;
  int r;
  float v;
  float s;
  float pitch;

  if (DAT_0068f4c8 == 0 || DAT_008aa254 <= 1) {
    FUN_0045e0f0(DAT_0088e670[0].base + param_1, 0.0f);
    FUN_0045e0f0(DAT_0088e670[1].base + param_1, 0.0f);
    FUN_0045e0f0(DAT_0088e670[2].base + param_1, 0.0f);
    FUN_0045e0f0(DAT_0088e670[3].base + param_1, 0.0f);
    FUN_0046cc10(&st, param_1);
    if (st == 4) {
      FUN_0046d2e0(param_1, 2, &k);
      r = FUN_00461e90(param_2, k, &DAT_0068fc90, &DAT_0068f4c0);
      a = DAT_0088e670[DAT_0068fc90].base + param_1;
      b = DAT_0088e670[DAT_0068f4c0].base + param_1;
      FUN_0046d6a0(&v, param_1);
      if (param_2 == 3) {
        if (v > 100.0f) {
          v = 100.0f;
        }
        s = (float)(sin(1.57f * (v * 0.01f) + 4.71f) + 1.0);
      }
      else {
        if (v > 200.0f) {
          v = 200.0f;
        }
        s = (float)(sin(1.57f * (v * 0.005f) + 4.71f) + 1.0);
      }
      if (s > 1.0f) {
        s = 1.0f;
      }
      else if (s < 0.0f) {
        s = 0.0f;
      }
      FUN_0045daf0(a, param_1);
      FUN_0045daf0(b, param_1);
      if ((DAT_0068f640[a].p->f34 & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[a].p, 1);
      }
      if ((DAT_0068f640[b].p->f34 & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[b].p, 1);
      }
      if (FUN_0040e350() != 6) {
        FUN_0045e0f0(a, 0.0f);
        FUN_0045e0f0(b, 0.0f);
        return;
      }
      DAT_0068f640[a].p->f3c = 22050.0f - FUN_00472650(0.0f, 2205.0f);
      if (DAT_0068f640[a].p->h30 != 0) {
        FUN_005a6dc0(DAT_0068f640[a].p->h30, 4, 1, 22050.0f - FUN_00472650(0.0f, 2205.0f));
      }
      FUN_0045e0f0(a, s);
      if (r != 0) {
        pitch = 22050.0f - s * 2205.0f;
        DAT_0068f640[b].p->f3c = pitch;
        if (DAT_0068f640[b].p->h30 != 0) {
          FUN_005a6dc0(DAT_0068f640[b].p->h30, 4, 1, pitch);
        }
        FUN_0045e0f0(b, s);
      }
    }
  }
}

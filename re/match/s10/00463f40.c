// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00463f40 (h7, by hand): matched. match.py scores 98.4% only because Ghidra's size (932 B) stops
   before the last 4 epilogue instructions; `match.py --bytes` gives 938 B whole symbol, 0 differing unmasked
   bytes (BYTE-IDENTICAL), relocheck OK. DAT_0068f640 is an 8-byte record array (object pointer + dword);
   DAT_0088e670 points at 0x90-byte records (dword at +0x80). The colour switch takes signed case values
   (the compiler's binary search uses jg). Float constants are source literals (1/4500, 0.001, 1.0, 0.0, 5000.0,
   22050.0, 441.0). What moved it: if/else calls for FUN_0045e0f0(x, 0.0f) / FUN_0045e0f0(x, s) (cross-jumped
   to one call), not a ternary argument (which costs two stack temporaries). `b` is set only in the 0xffff8040
   case and read only under the same test, as in the original (ebx starts from the `mode` slot). */
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
typedef struct Obj00463f40 {
  char pad0[0x30];
  int h30;
  unsigned char f34;
  char pad35[7];
  float f3c;
} Obj00463f40;
typedef struct { Obj00463f40 *p; int q; } Rec0068f640;
typedef struct { char pad0[0x80]; int base; char pad84[0xc]; } Rec0088e670;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(Rec0088e670 *, DAT_0088e670, 0x0088e670);
MGLOBALA(Rec0068f640, DAT_0068f640, 1, 0x0068f640);
int FUN_0040e350(void);
int FUN_00426c00(void);
void FUN_0045daf0(int p0, int p1);
void FUN_0045e0f0(int p0, float p1);
int FUN_0046cc10(int *p0, int p1);
int FUN_0046d2e0(int p0, int p1, int *p2);
int FUN_0046d6d0(float *p0, int p1);
float FUN_00472650(float lo, float hi);
void FUN_005a66d0(Obj00463f40 *p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00463f40 FUN_00463f40
void FUN_00463f40(int param_1)
{
  int a;
  int b;
  int w;
  int mode;
  int st;
  int col;
  float v;
  float s;
  float add;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1) {
    return;
  }
  FUN_0045e0f0(DAT_0088e670[4].base + param_1, 0.0f);
  FUN_0045e0f0(DAT_0088e670[5].base + param_1, 0.0f);
  FUN_0045e0f0(DAT_0088e670[6].base + param_1, 0.0f);
  mode = FUN_00426c00();
  if (mode == 0x22) {
    FUN_0045e0f0(DAT_0088e670[12].base + param_1, 0.0f);
    FUN_0045e0f0(DAT_0088e670[13].base + param_1, 0.0f);
  }
  FUN_0046cc10(&st, param_1);
  if (st != 4) {
    return;
  }
  for (w = 0; w < 2; w++) {
    FUN_0046d2e0(param_1, w + 2, &col);
    FUN_0046d6d0(&v, param_1);
    switch (col) {
    case -0x7fc000:
    case -0x9b9b9c:
    case -0x7fbfc0:
    case -0x37e1a6:
    case -0x69e1a6:
    case -0x697f80:
    case -0x377f80:
      a = DAT_0088e670[4].base + param_1;
      s = v * (1.0f / 4500.0f);
      break;
    case -0x7f3800:
    case -0xebaf74:
    case -0xff7f38:
    case -0xffa54c:
    case -0xff9b4c:
    case -0xe17f4c:
    case -0x5f7f80:
    case -0x4b7f80:
    case -0x4b4b4c:
    case -0x237f80:
      a = DAT_0088e670[5].base + param_1;
      s = v * (1.0f / 4500.0f);
      break;
    case -0xff3701:
    case -0xebb956:
    case -0xcd01:
      a = DAT_0088e670[6].base + param_1;
      s = v * 0.001f;
      break;
    case -0x7fc0:
      a = DAT_0088e670[12].base + param_1;
      b = DAT_0088e670[13].base + param_1;
      s = v * 0.001f;
      break;
    default:
      return;
    }
    if (s > 1.0f) {
      s = 1.0f;
    }
    else if (s < 0.0f) {
      s = 0.0f;
    }
    FUN_0045daf0(a, param_1);
    if ((DAT_0068f640[a].p->f34 & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[a].p, 1);
    }
    add = s * 5000.0f;
    DAT_0068f640[a].p->f3c = 22050.0f - FUN_00472650(0.0f, 441.0f) + add;
    if (DAT_0068f640[a].p->h30 != 0) {
      FUN_005a6dc0(DAT_0068f640[a].p->h30, 4, 1, 22050.0f - FUN_00472650(0.0f, 441.0f) + add);
    }
    if (FUN_0040e350() != 6) {
      FUN_0045e0f0(a, 0.0f);
    }
    else {
      FUN_0045e0f0(a, s);
    }
    if (mode == 0x22 && col == -0x7fc0) {
      FUN_0045daf0(b, param_1);
      if ((DAT_0068f640[b].p->f34 & 2) == 0) {
        FUN_005a66d0(DAT_0068f640[b].p, 1);
      }
      DAT_0068f640[b].p->f3c = 22050.0f;
      if (DAT_0068f640[b].p->h30 != 0) {
        FUN_005a6dc0(DAT_0068f640[b].p->h30, 4, 1, 22050.0f);
      }
      if (FUN_0040e350() != 6) {
      FUN_0045e0f0(b, 0.0f);
    }
    else {
      FUN_0045e0f0(b, s);
    }
    }
  }
}

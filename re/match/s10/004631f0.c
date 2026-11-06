// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x004631f0 (h6): 100.0% (regs 100%), whole symbol byte-identical (476 B). Raw did not compile.
   431b20/431b60 return float, voice table read at every use, 22050.0f/19845.0f as float literals (the
   compiler keeps 22050's bits in edi itself). Residual: none. */
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
typedef struct { int h; int pad; } S10Voice;
MGLOBAL(int, DAT_00604eb0, 0x00604eb0);
MGLOBAL(int, DAT_00604e1c, 0x00604e1c);
MGLOBAL(int, DAT_00605918, 0x00605918);
MGLOBAL(int, DAT_00605d24, 0x00605d24);
MGLOBALA(S10Voice, DAT_0068f640, 1, 0x0068f640);
int FUN_0042f760(void);
int FUN_0042f770(void);
int FUN_0042f780(void);
float FUN_00431b20(void);
float FUN_00431b60(void);
int FUN_00432230(void);
int FUN_00432260(void);
void FUN_0045e0f0(int p0, float p1);
void FUN_005a66d0(int p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);
// MATCH: 0x004631f0 FUN_004631f0
void FUN_004631f0(void)
{
  float vol;
  float vol2;
  int ch;

  vol = FUN_00431b20();
  vol2 = FUN_00431b60();
  ch = DAT_00604eb0;
  if (FUN_0042f760() != 0) {
    FUN_005a66d0(DAT_0068f640[ch].h, 1);
    *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
    if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
    }
    FUN_0045e0f0(ch, vol);
  }
  ch = DAT_00604e1c;
  if (FUN_0042f770() != 0) {
    FUN_005a66d0(DAT_0068f640[ch].h, 1);
    *(float *)(DAT_0068f640[ch].h + 0x3c) = 19845.0f;
    if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 19845.0f);
    }
    FUN_0045e0f0(ch, vol);
  }
  if (FUN_0042f780() != 0) {
    FUN_005a66d0(DAT_0068f640[ch].h, 1);
    *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
    if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
    }
    FUN_0045e0f0(ch, vol);
  }
  ch = DAT_00605918;
  if (FUN_00432230() != 0 && (*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
    FUN_005a66d0(DAT_0068f640[ch].h, 1);
    *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
    if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
    }
    FUN_0045e0f0(ch, vol);
  }
  ch = DAT_00605d24;
  if (FUN_00432260() != 0 && (*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) == 0) {
    FUN_005a66d0(DAT_0068f640[ch].h, 1);
    *(float *)(DAT_0068f640[ch].h + 0x3c) = 22050.0f;
    if (*(int *)(DAT_0068f640[ch].h + 0x30) != 0) {
      FUN_005a6dc0(*(int *)(DAT_0068f640[ch].h + 0x30), 4, 1, 22050.0f);
    }
  }
  if ((*(unsigned char *)(DAT_0068f640[ch].h + 0x34) & 2) != 0) {
    FUN_0045e0f0(ch, vol2);
  }
}

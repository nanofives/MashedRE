// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00464670 (h7, by hand): 100%. DAT_0068f640 is an 8-byte record array (object pointer + dword);
   DAT_00604aa4 is the first dword of a 0x94-byte record array. param_2 is an int index (Ghidra typed it float
   because its dead slot holds the float local `a`). FUN_00472650 is cdecl float(float lo, float hi).
   Float constants are source literals (0x005d757c 0.0, 0x005ccd04 500.0, 0x005ce9bc 1/5500, 0x005ce9b8 0.00025,
   0x005cc320 1.0, 0x005ce6b8 22050.0). What moved it: the record base load hoisted above the if (`base`),
   if/else stores for the scale (not a ternary or a selected factor). */
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
typedef struct Obj00464670 {
  char pad0[0x30];
  int h30;
  unsigned char f34;
  char pad35[7];
  float f3c;
} Obj00464670;
typedef struct { Obj00464670 *p; int q; } Rec0068f640;
typedef struct { int base; char pad[0x90]; } Rec00604aa4;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(float, DAT_006130dc, 0x006130dc);
MGLOBALA(Rec00604aa4, DAT_00604aa4, 1, 0x00604aa4);
MGLOBALA(Rec0068f640, DAT_0068f640, 1, 0x0068f640);
void FUN_0045daf0(int p0, int p1);
void FUN_0045e0f0(int p0, float p1);
int FUN_0046d6d0(float *p0, int p1);
float FUN_00472650(float lo, float hi);
void FUN_005a66d0(Obj00464670 *p0, int p1);
void FUN_005a6dc0(int p0, int p1, int p2, float p3);

// MATCH: 0x00464670 FUN_00464670
void FUN_00464670(int param_1, int param_2, int param_3, float param_4)
{
  int i;
  int base;
  float a;
  float b;

  base = DAT_00604aa4[param_2].base;
  if (DAT_0068f4c8 == 0 || DAT_008aa254 <= 1) {
    i = base + param_1;
    if ((DAT_0068f640[i].p->f34 & 2) == 0 &&
        (DAT_0068f640[base + param_3].p->f34 & 2) == 0) {
      FUN_0045daf0(i, param_1);
      FUN_0046d6d0(&a, param_1);
      FUN_0046d6d0(&b, param_3);
      if (param_4 == 0.0f) {
        if (a < b) {
          a = b;
        }
      }
      else {
        a = param_4 + 500.0f;
      }
      if (param_2 == 1) {
        DAT_006130dc = a * (1.0f / 5500.0f);
      }
      else {
        DAT_006130dc = a * 0.00025f;
      }
      if (DAT_006130dc > 1.0f) {
        DAT_006130dc = 1.0f;
      }
      FUN_005a66d0(DAT_0068f640[i].p, 1);
      DAT_0068f640[i].p->f3c = FUN_00472650(0.0f, 2205.0f) + 22050.0f;
      if (DAT_0068f640[i].p->h30 != 0) {
        FUN_005a6dc0(DAT_0068f640[i].p->h30, 4, 1, FUN_00472650(0.0f, 2205.0f) + 22050.0f);
      }
      FUN_0045e0f0(i, DAT_006130dc);
    }
  }
}

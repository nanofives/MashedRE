// FLAGS: /O2 /GS /Ob1
/* 0x0041fe10 FUN_0041fe10 (384 B Ghidra size, 401 B obj symbol with the switch tables at 0x0041ff90):
   100.0% exact, regs 100.0%, whole symbol BYTE-IDENTICAL including the two-level jump table.
   Raw g2s was 50.7% (in_EAX). The record arrives in EAX: static-function convention (pilot finding 1);
   Caller_0041fe10 is a stand-in for the real caller FUN_00420420 (not matched).
   Shape: switch on FUN_004b5190(a,0,0) with cases 4,6,8,9,10,12 breaking and the default removing the atomic;
   the float test is a literal C chain of == (r->f230[3] == [4] == [5] == [0] == [1] == [2]): each step
   compares the int 0/1 result as float with the next field (fild + fcomp), so the source is written that way;
   two 16-byte struct copies (Sphere) from atomic + 0x1c into +0x248 and +0x258.
   Residual: none. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { V3 c; float r; } Sphere;
typedef struct {
  char u000[0xec];
  char *atomic;                  /* +0x0ec */
  char u0f0[0x230 - 0xf0];
  float f230[6];                 /* +0x230 */
  Sphere s248;                   /* +0x248 */
  Sphere s258;                   /* +0x258 */
  char u268[0x26c - 0x268];
  char *clump;                   /* +0x26c */
} CarRec;

char *FUN_004e6ab0(char *clump);
int FUN_004b3fc0(char *clump, char **out);
int FUN_004b5190(char *a, int b, int c);
void FUN_004e6fb0(char *clump, char *a);
void FUN_004e6920(char *a);
void FUN_004223b0(float *s, char *clump);
void FUN_004e6e00(char *clump);
void FUN_004e5fc0(char *a);

// MATCH: 0x0041fe10 FUN_0041fe10
static void FUN_0041fe10(CarRec *r)
{
  char *c;
  char *a;
  int n;
  int i;
  char *list[128];

  c = FUN_004e6ab0(r->clump);
  n = FUN_004b3fc0(c, list);
  for (i = 0; i < n; i++) {
    a = list[i];
    switch (FUN_004b5190(a, 0, 0)) {
    case 4:
    case 6:
    case 8:
    case 9:
    case 10:
    case 12:
      break;
    default:
      FUN_004e6fb0(c, a);
      FUN_004e6920(a);
      list[i] = 0;
      break;
    }
  }
  if (r->f230[3] == r->f230[4] == r->f230[5] == r->f230[0] == r->f230[1] == r->f230[2])
    FUN_004223b0(r->f230, c);
  FUN_004e6e00(c);
  if (*(unsigned char *)(r->atomic + 0x4c) & 2)
    FUN_004e5fc0(r->atomic);
  r->s248 = *(Sphere *)(r->atomic + 0x1c);
  r->s258 = *(Sphere *)(r->atomic + 0x1c);
}

/* stand-in for the real caller FUN_00420420 (record in EAX); not matched */
void Caller_0041fe10(CarRec *r)
{
  FUN_0041fe10(r);
}

// FLAGS: /O2 /GS /Ob1
/* 0x0040da50 FUN_0040da50 (265 B Ghidra size; 321 B real symbol): BYTE-IDENTICAL (match.py --bytes: 321 B whole
   symbol, 0 differing unmasked bytes). The per-instruction score reads 91.8% only because Ghidra's size stops at
   0x0040db59, before the loop tail, epilogue and the case 0/0x1a/0x21/0x24 jump table. Raw g2s was 52.9%.
   Hand form: i-indexed loop over 4 cars, PTR_PTR_005f2770[i + 0xd] gate, mat[4] locals, switch with one
   shared case, flag = byte +1 of the 0x4c-stride record indexed by DAT_007f1a14[i].v. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
typedef struct Rec10 { int v; int pad[3]; } Rec10;
typedef struct Rec4c { char f0; char f1; char pad[0x4a]; } Rec4c;
MGLOBAL(int *, PTR_PTR_005f2770, 0x005f2770);
MGLOBALA(int, DAT_0063bc44, 4, 0x0063bc44);
MGLOBALA(Rec4c, DAT_007f103c, 1, 0x007f103c);
MGLOBALA(Rec10, DAT_007f1a14, 4, 0x007f1a14);

void FUN_0045b350(void);
int FUN_0046c7b0(int p0);
int FUN_0046c770(int p0);
undefined4 FUN_0046d4a0(void **p0, int p1);
void FUN_00420870(int p0, undefined4 p1, void *p2);
void FUN_0040d8f0(int p0);
int FUN_00426c00(void);
undefined4 FUN_00426080(int p0);
void FUN_00408610(void *p0, undefined4 p1);
void FUN_0041eeb0(int p0, int p1, int p2, int p3);

// MATCH: 0x0040da50 FUN_0040da50
void FUN_0040da50(undefined4 param_1)
{
  int i;
  int flag;
  void *mat[4];

  flag = 0;
  FUN_0045b350();
  for (i = 0; i < 4; i++) {
    if (PTR_PTR_005f2770[i + 0xd] != 0) {
      if (FUN_0046c7b0(i) == 1 || FUN_0046c770(i) == 1) {
        FUN_0046d4a0(&mat[i], i);
        FUN_00420870(i, param_1, mat[i]);
        FUN_0040d8f0(i);
        flag = DAT_007f103c[DAT_007f1a14[i].v].f1 != 0;
        switch (FUN_00426c00()) {
        case 0:
        case 0x1a:
        case 0x21:
        case 0x24:
          DAT_0063bc44[i] = 1;
          break;
        default:
          DAT_0063bc44[i] = 0;
          break;
        }
        FUN_00408610(mat[i], FUN_00426080(i));
      }
      else {
        DAT_0063bc44[i] = 0;
      }
      FUN_0041eeb0(i, DAT_0063bc44[i], DAT_0063bc44[i], flag);
    }
  }
}

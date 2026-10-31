// FLAGS: /O2 /GS /Ob1
/* 0x00410d10 FUN_00410d10 (1075 B per Ghidra; the obj symbol runs on to the jump table at 0x0041114c).
   Best: 94.9% exact (regs 94.9, ops 96.3), 356/353 ins. Raw Ghidra 38.4%.
   Path: hand rewrite 70.2% (case order 5,4,9,8,7,10 from the code layout; table at 0x0041114c maps
   4->d59 5->d3f 6->default 7->e32 8->dea 9->d81 10->e9a) -> 85.0% (cases 4 and 10 written
   `if (!(f >= c)) break; return 1;`: test ah,1 and the compiler cross-jumps case 10's tail into case
   4's at 0x00410d66 as the original does; `<` gives test ah,5) -> 90.2% (the two FUN_0040e180 outputs
   copied to register locals a/b one at a time, right before each FUN_00408ad0 call: esi/edi) -> 94.1%
   (first output `ta` in its own block scope: its slot then packs with fb at +0x14, frame 0x28) ->
   94.4% (case 7 `i = 0; n = 0;` order) -> 94.9% (tail written `if (n != 1) {...} if (alive == 0)
   return 1; return 0;`).
   Residual (b) source shape: the n==1 block. The original places `test ebx,ebx; jne ret0; ret 1` at
   0x004110f5, between loop 2's store/jmp and its fstp st(0) compensation block, and never folds it to
   setcc. Every form tried (if/else either way, nested returns, shared tail return, ?:, else-if chains,
   10 variants) either inlines it after the je or puts it at the end as sete. Permuter: no improvement
   (176 compiles). Constants: 0x005cc31c 3.0f, 0x005cc574 2.0f, 0x005d757c 0.0f, 0x005cc55c 10.0f,
   0x005cc730 80.0f, 0x005ccd6c 20.0f, 0x005cc568 100.0f; DAT_005f2770 is a pointer read as int[13 + i]. */
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
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0fd4, 0x007f0fd4);
MGLOBAL(float, DAT_007f0fe4, 0x007f0fe4);
MGLOBAL(int *, DAT_005f2770, 0x005f2770);
MGLOBAL(int, DAT_00803320, 0x00803320);

int FUN_00405890(void);
float FUN_00408a50(int i);
void FUN_00408a70(int i, float f);
float FUN_00408ad0(int i);
void FUN_0040e180(int *a, int *b);
int FUN_0040e340(void);
int FUN_0040eee0(int i, int b);
float FUN_00417730(int i);
int FUN_00422fd0(int i);
int FUN_00423b20(int i);
float FUN_00442df0(void);
int FUN_00443080(void);
int FUN_0046c7b0(int i);
void FUN_0046cbb0(int i, int *a, int *b);
void FUN_004922e0(int i, int a, int b, int c);

// MATCH: 0x00410d10 FUN_00410d10
int FUN_00410d10(void)
{
  int i, n, alive, ok, dummy;
  int tb, a, b, ra, rb, da, db, sel;
  float fa, fb, mx, f;

  if (FUN_00443080() == 1)
    return 0;
  switch (DAT_007f0fd0) {
  case 5:
    if (FUN_0046c7b0(0) == 0)
      return 1;
    return FUN_00405890();
  case 4:
    if (!(FUN_00417730(0) >= 3.0f))
      break;
    return 1;
  case 9:
    if (FUN_00417730(0) >= 3.0f)
      return 1;
    if (FUN_00417730(1) >= 3.0f)
      return 1;
    FUN_0046cbb0(0, &ok, &dummy);
    if (ok != 0)
      return 1;
    if (FUN_00423b20(1) != 0)
      return 1;
    break;
  case 8:
    if (FUN_00417730(0) >= 3.0f)
      return 1;
    FUN_0046cbb0(0, &ok, &dummy);
    if (ok != 0)
      return 1;
    break;
  case 7:
    if (FUN_0046c7b0(0) == 0)
      return 1;
    i = 0;
    n = 0;
    for (; i < FUN_0040e340(); i++) {
      if (FUN_0046c7b0(i) == 0)
        n++;
      if (FUN_00417730(i) > 3.0f)
        return 1;
    }
    return n >= FUN_0040e340() - 1;
  case 10:
    FUN_0046cbb0(0, &ok, &dummy);
    if (ok != 0)
      return 1;
    if (DAT_007f0fe4 <= 0.0f)
      return 1;
    if (!(FUN_00417730(0) >= 2.0f))
      break;
    return 1;
  }
  mx = 0.0f;
  if (FUN_00442df0() == 10.0f) {
    {
      int ta;
      FUN_0040e180(&ta, &tb);
      a = ta;
    }
    fa = FUN_00408ad0(a);
    b = tb;
    fb = FUN_00408ad0(b);
    if (fa > 80.0f && fb < 20.0f)
      fa -= 100.0f;
    else if (fb > 80.0f && fa < 20.0f)
      fb -= 100.0f;
    if (DAT_007f0fd0 == 1) {
      if (a == DAT_007f0fd4)
        fa = 100.0f;
      if (b == DAT_007f0fd4)
        fb = 100.0f;
    }
    FUN_0046cbb0(a, &ra, &da);
    FUN_0046cbb0(b, &rb, &db);
    if (fa > fb) {
      if (ra == 0)
        sel = b;
      else if (rb == 0)
        sel = a;
      else
        return 0;
    } else {
      if (rb == 0)
        sel = a;
      else if (ra == 0)
        sel = b;
      else
        return 0;
    }
    FUN_004922e0(sel, 3, 10, 0x80);
    FUN_00422fd0(sel);
    if (FUN_0040eee0(sel, 1) == 1)
      return 1;
  }
  n = 0;
  alive = 0;
  for (i = 0; i < 4; i++) {
    if (DAT_005f2770[i + 13] != 0) {
      n++;
      if (FUN_0046c7b0(i) == 1)
        alive++;
    }
  }
  DAT_00803320 = alive;
  if (n != 1) {
    if (alive == 1)
      return 1;
    if (alive == 0) {
      for (i = 0; i < 4; i++) {
    if (DAT_005f2770[i + 13] != 0)
      mx = FUN_00408a50(i);
  }
  for (i = 0; i < 4; i++) {
    if (DAT_005f2770[i + 13] != 0) {
      f = FUN_00408a50(i);
      if (f > mx)
        mx = f;
    }
  }
  FUN_00408a70(0, mx);
  FUN_00408a70(1, mx);
  FUN_00408a70(2, mx);
  FUN_00408a70(3, mx);
      return 1;
    }
    return 0;
  }
  if (alive == 0)
    return 1;
  return 0;
}

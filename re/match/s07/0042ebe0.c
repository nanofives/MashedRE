// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x0042ebe0 (h7): 100%. Raw g2s 10.7% (Ghidra saw the compiler unroll). Source: a 10-record loop over
   Rec30 DAT_007f0a40[] (0x30-byte records; param 10/11/12 picks field +4/+8/+0x14), `if (locked) break;` at the
   loop end; the compiler unrolls it x10 and jump-threads the mode tests. A `break` inside the if-chain, or a
   shared value variable, stops the unroll (11.9%). Return as `if (!rec.f04) return 0; return locked == 0;`
   (the AND-form return gives mov eax,1 / xor eax,eax, 92.5%). Residual: none. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct Rec30 {
  int f00;
  int f04;
  int f08;
  int f0c;
  int f10;
  int f14;
  int f18[6];
} Rec30;
MGLOBALA(Rec30, DAT_007f0a40, 13, 0x007f0a40);
// MATCH: 0x0042ebe0 FUN_0042ebe0
int FUN_0042ebe0(int param_1)
{
  int locked;
  int i;
  int v;

  locked = 0;
  for (i = 0; i < 10; i++) {
    if (param_1 == 10) {
      if (DAT_007f0a40[i].f04 == 0) locked = 1;
    } else if (param_1 == 0xb) {
      if (DAT_007f0a40[i].f08 == 0) locked = 1;
    } else if (param_1 == 0xc) {
      if (DAT_007f0a40[i].f14 == 0) locked = 1;
    }
    if (locked) break;
  }
  if (DAT_007f0a40[param_1].f04 == 0) {
    return 0;
  }
  return locked == 0;
}

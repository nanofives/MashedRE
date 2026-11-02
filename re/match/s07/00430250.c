// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 h4 0x00430250: best 71.4% (regs 71.4%, ops 85.7%), 21/21 ins, every instruction in place except the
   block order. Raw converter 71.4% with an untyped table walk; this form types it (int **, 13 x 20-byte records).
   Residual (b) block layout: the original emits the shared `xor eax,eax; ret` right after the param_2 test (jne
   over it, the not-found exit jumps back to it); this compiler places it at the end for every form tried:
   nested if / early return / do-while / for with break / return in the loop / ternary / result variable / an
   __inline lookup helper, plus the permuter (88 compiles, no gain). */
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
typedef struct Rec007f0cb0 { int f[5]; } Rec007f0cb0;
MGLOBALA(Rec007f0cb0, DAT_007f0cb0, 13, 0x007f0cb0);
int **FUN_0040d430(void);

// MATCH: 0x00430250 FUN_00430250
Rec007f0cb0 *FUN_00430250(int param_1, int param_2)
{
  int **tbl;
  int i;


  if (param_2 != 0) {
    tbl = FUN_0040d430();
    for (i = 0; i < 0xd; i++) {
      if (*tbl[i] == param_1)
        break;
    }
    if (i != 0xd)
      return DAT_007f0cb0 + i;
  }
  return 0;
}

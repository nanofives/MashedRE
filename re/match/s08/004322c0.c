// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 h3 0x004322c0 (244 B): 100% (regs 100%, ops 100%), byte-identical. Raw g2s 46.2%.
   What moved it: the original keeps DAT_0067f17c / DAT_0067f184 / DAT_0067e9fc in EBP/EBX/EDI across the calls
   to FUN_00430830 and FUN_00430910, and reloads only DAT_0067e9fc after FUN_0042f6b0. cl 13.00.9176 does that
   without /GL when those three leaf callees are defined EARLIER IN THE SAME FILE (intra-TU side-effect knowledge:
   0x00430830/0x00430910 write no global, 0x0042f6b0 writes only DAT_0067e9fc). With local-variable caching instead
   the best was 83.6% (EBX/EDI swap + store of the copy). So this file holds the real bodies of 0x0042f6b0 (100%),
   0x00430830 (100%) and 0x00430910 (61.7%, only the `return 0` block placement differs; tried negated/local forms).
   They sit behind a guard because 0x004323c0's file holds the same three (the port must keep them in the same TU,
   ahead of these callers). Other forms: `(DAT_0067ea68 != 0) * 2 + 1` gives an extra ECX copy (ternary `? 3 : 1`
   is the source form), `idx > 12` gives cmp 0xc/jle (`>= 13` is the form), final stores are f1a4 then f17c.
   Residual class: none. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
#ifndef MATCH_S08_TU_0042F6B0_00430830_00430910
#define MATCH_S08_TU_0042F6B0_00430830_00430910
MGLOBAL(int, DAT_0067e9fc, 0x0067e9fc);
MGLOBAL(int, DAT_0067f184, 0x0067f184);
MGLOBAL(int, DAT_0067f17c, 0x0067f17c);
MGLOBALA(int, DAT_007f0a40, 156, 0x007f0a40);
// MATCH: 0x0042f6b0 FUN_0042f6b0
void FUN_0042f6b0(void)
{
  switch (DAT_0067f184) {
  case 0:
    DAT_0067e9fc = 2;
    return;
  case 1:
    DAT_0067e9fc = 3;
    return;
  case 2:
    DAT_0067e9fc = 4;
    return;
  case 3:
    DAT_0067e9fc = 6;
    return;
  case 5:
    DAT_0067e9fc = 5;
    return;
  case 8:
    DAT_0067e9fc = 7;
    return;
  case 9:
    DAT_0067e9fc = 8;
    return;
  case 10:
    DAT_0067e9fc = 9;
    return;
  case 11:
    DAT_0067e9fc = 10;
    return;
  }
}
// MATCH: 0x00430830 FUN_00430830
int FUN_00430830(int idx)
{
  switch (DAT_0067e9fc) {
  case 2:
    return DAT_007f0a40[idx * 12 + 1];
  case 3:
    return DAT_007f0a40[idx * 12 + 1];
  case 4:
    return DAT_007f0a40[idx * 12 + 2];
  case 5:
    return DAT_007f0a40[idx * 12 + 5];
  case 6:
    return DAT_007f0a40[idx * 12 + 3];
  case 7:
    return DAT_007f0a40[idx * 12 + 8];
  case 8:
    return DAT_007f0a40[idx * 12 + 9];
  case 9:
    return DAT_007f0a40[idx * 12 + 10];
  case 10:
    return DAT_007f0a40[idx * 12 + 1];
  }
  return 0;
}
// MATCH: 0x00430910 FUN_00430910
int FUN_00430910(void)
{
  switch (DAT_0067e9fc) {
  case 3:
  case 4:
  case 5:
    if (DAT_0067f184 == 0 || DAT_0067f184 == 3 || DAT_0067f184 == 4 || DAT_0067f184 == 6 ||
        DAT_0067f184 == 7 || DAT_0067f184 == 8 || DAT_0067f184 == 9 || DAT_0067f184 == 10 ||
        DAT_0067f184 == 11) {
      return 0;
    }
    return DAT_007f0a40[DAT_0067f17c * 12 + DAT_0067f184];
  case 6:
  case 7:
  case 8:
  case 9:
    if (DAT_0067f184 == 0 || DAT_0067f184 == 1 || DAT_0067f184 == 2 || DAT_0067f184 == 5 ||
        DAT_0067f184 == 6 || DAT_0067f184 == 7 || DAT_0067f184 == 4 || DAT_0067f184 == 11) {
      return 0;
    }
    return DAT_007f0a40[DAT_0067f17c * 12 + DAT_0067f184];
  }
}
MGLOBAL(int, DAT_0067e9f8, 0x0067e9f8);
#endif
MGLOBAL(int, DAT_0067f1a4, 0x0067f1a4);
MGLOBAL(int, DAT_0067ea68, 0x0067ea68);
MGLOBALA(int, DAT_0067ed3c, 64, 0x0067ed3c);
void FUN_0042aa00(int p0);
// MATCH: 0x004322c0 FUN_004322c0
void FUN_004322c0(void)
{
  int lo;

  FUN_0042aa00(1);
  if (DAT_0067ed3c[DAT_0067e9f8 * 0x10] >= 6 && DAT_0067ed3c[DAT_0067e9f8 * 0x10] <= 7) {
    for (;;) {
      DAT_0067f17c++;
      if (DAT_0067f17c >= 13) {
        DAT_0067f17c = 12;
        DAT_0067f1a4 = 0;
      }
      if (FUN_00430830(DAT_0067f17c) != 0) {
        return;
      }
      lo = DAT_0067ea68 ? 3 : 1;
      if (DAT_0067e9fc == 10) {
        lo = 11;
      }
      else if (DAT_0067e9fc == 2) {
        lo = 0;
      }
      if (DAT_0067f184 == lo) {
        break;
      }
      do {
        DAT_0067f184--;
        if (DAT_0067f184 < lo) {
          DAT_0067f184 = lo;
          break;
        }
      } while (FUN_00430910() == 0);
      if (DAT_0067e9fc == 3 || DAT_0067e9fc == 4 || DAT_0067e9fc == 5) {
        FUN_0042f6b0();
      }
      DAT_0067f17c--;
    }
    DAT_0067f1a4 = 0;
    DAT_0067f17c--;
  }
}

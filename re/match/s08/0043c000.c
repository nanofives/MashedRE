// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0043c000: 100% (regs 100%), raw g2s 46.7%. By hand: 18 (int state, uchar alpha) fade pairs, alpha = (uchar)((1 - t) * 255) on state 1, (uchar)(t * 255) on state 2, else 0; 1.0 / 255.0 are compiler literals, 0x005d757c stays a global. The state-0x0067e7d8 else branch stores a byte into the state itself (mov byte [0x67e7d8], bl): written as *(uchar *)&DAT_0067e7d8 = 0. relocheck OK (unchecked = __ftol CRT calls only). Residual: none. */
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
MGLOBAL(float, DAT_0067e840, 0x0067e840);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(int, DAT_0067e7d8, 0x0067e7d8);
MGLOBAL(int, DAT_0067e7b0, 0x0067e7b0);
MGLOBAL(int, DAT_0067e7f0, 0x0067e7f0);
MGLOBAL(int, DAT_0067e7d0, 0x0067e7d0);
MGLOBAL(int, DAT_0067e7a8, 0x0067e7a8);
MGLOBAL(int, DAT_0067e7c0, 0x0067e7c0);
MGLOBAL(int, DAT_0067e7c8, 0x0067e7c8);
MGLOBAL(int, DAT_0067e7f8, 0x0067e7f8);
MGLOBAL(int, DAT_0067e838, 0x0067e838);
MGLOBAL(int, DAT_0067e828, 0x0067e828);
MGLOBAL(int, DAT_0067e830, 0x0067e830);
MGLOBAL(int, DAT_0067e820, 0x0067e820);
MGLOBAL(int, DAT_0067e7e0, 0x0067e7e0);
MGLOBAL(int, DAT_0067e7e8, 0x0067e7e8);
MGLOBAL(int, DAT_0067e808, 0x0067e808);
MGLOBAL(int, DAT_0067e810, 0x0067e810);
MGLOBAL(int, DAT_0067e818, 0x0067e818);
MGLOBAL(int, DAT_0067e7b8, 0x0067e7b8);
MGLOBAL(int, DAT_0067e800, 0x0067e800);
MGLOBAL(uchar, DAT_0067e7ac, 0x0067e7ac);
MGLOBAL(uchar, DAT_0067e7dc, 0x0067e7dc);
MGLOBAL(uchar, DAT_0067e7b4, 0x0067e7b4);
MGLOBAL(uchar, DAT_0067e7f4, 0x0067e7f4);
MGLOBAL(uchar, DAT_0067e7d4, 0x0067e7d4);
MGLOBAL(uchar, DAT_0067e7c4, 0x0067e7c4);
MGLOBAL(uchar, DAT_0067e7cc, 0x0067e7cc);
MGLOBAL(uchar, DAT_0067e7fc, 0x0067e7fc);
MGLOBAL(uchar, DAT_0067e83c, 0x0067e83c);
MGLOBAL(uchar, DAT_0067e82c, 0x0067e82c);
MGLOBAL(uchar, DAT_0067e834, 0x0067e834);
MGLOBAL(uchar, DAT_0067e824, 0x0067e824);
MGLOBAL(uchar, DAT_0067e7e4, 0x0067e7e4);
MGLOBAL(uchar, DAT_0067e7ec, 0x0067e7ec);
MGLOBAL(uchar, DAT_0067e80c, 0x0067e80c);
MGLOBAL(uchar, DAT_0067e814, 0x0067e814);
MGLOBAL(uchar, DAT_0067e81c, 0x0067e81c);
MGLOBAL(uchar, DAT_0067e7bc, 0x0067e7bc);
void FUN_004332a0(void);
void FUN_0042f7b0(void);
void FUN_0042c960(void);
void FUN_0042fa00(void);

// MATCH: 0x0043c000 FUN_0043c000
void FUN_0043c000(void)
{
  if (DAT_0067e840 == DAT_005d757c) {
    if (DAT_0067e7d8 != 1) {
      DAT_0067e7d8 = 0;
    }
    if (DAT_0067e7b0 != 1) {
      DAT_0067e7b0 = 0;
    }
    if (DAT_0067e7f0 != 1) {
      DAT_0067e7f0 = 0;
    }
    if (DAT_0067e7d0 != 1) {
      DAT_0067e7d0 = 0;
    }
    if (DAT_0067e7a8 != 1) {
      DAT_0067e7a8 = 0;
    }
    if (DAT_0067e7c0 != 1) {
      DAT_0067e7c0 = 0;
    }
    if (DAT_0067e7c8 != 1) {
      DAT_0067e7c8 = 0;
    }
    if (DAT_0067e7f8 != 1) {
      DAT_0067e7f8 = 0;
    }
    if (DAT_0067e838 != 1) {
      DAT_0067e838 = 0;
    }
    if (DAT_0067e828 != 1) {
      DAT_0067e828 = 0;
    }
    if (DAT_0067e830 != 1) {
      DAT_0067e830 = 0;
    }
    if (DAT_0067e820 != 1) {
      DAT_0067e820 = 0;
    }
    if (DAT_0067e7e0 != 1) {
      DAT_0067e7e0 = 0;
    }
    if (DAT_0067e7e8 != 1) {
      DAT_0067e7e8 = 0;
    }
    if (DAT_0067e808 != 1) {
      DAT_0067e808 = 0;
    }
    if (DAT_0067e810 != 1) {
      DAT_0067e810 = 0;
    }
    if (DAT_0067e818 != 1) {
      DAT_0067e818 = 0;
    }
    if (DAT_0067e7b8 != 1) {
      DAT_0067e7b8 = 0;
    }
    if (DAT_0067e800 != 1) {
      DAT_0067e800 = 0;
    }
  }
  if (DAT_0067e7a8 == 1) {
    DAT_0067e7ac = (uchar)((1.0f - DAT_0067e840) * 255.0f);
    FUN_004332a0();
  } else if (DAT_0067e7a8 == 2) {
    DAT_0067e7ac = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7ac = 0;
  }
  if (DAT_0067e7d8 == 1) {
    DAT_0067e7dc = (uchar)((1.0f - DAT_0067e840) * 255.0f);
    FUN_0042f7b0();
  } else if (DAT_0067e7d8 == 2) {
    DAT_0067e7dc = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    *(uchar *)&DAT_0067e7d8 = 0;
  }
  if (DAT_0067e7b0 == 1) {
    DAT_0067e7b4 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e7b0 == 2) {
    DAT_0067e7b4 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7b4 = 0;
  }
  if (DAT_0067e7f0 == 1) {
    DAT_0067e7f4 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e7f0 == 2) {
    DAT_0067e7f4 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7f4 = 0;
  }
  if (DAT_0067e7d0 == 1) {
    DAT_0067e7d4 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e7d0 == 2) {
    DAT_0067e7d4 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7d4 = 0;
  }
  if (DAT_0067e7c0 == 1) {
    DAT_0067e7c4 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e7c0 == 2) {
    DAT_0067e7c4 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7c4 = 0;
  }
  if (DAT_0067e7c8 == 1) {
    DAT_0067e7cc = (uchar)((1.0f - DAT_0067e840) * 255.0f);
    FUN_0042c960();
  } else if (DAT_0067e7c8 == 2) {
    DAT_0067e7cc = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7cc = 0;
  }
  if (DAT_0067e7f8 == 1) {
    DAT_0067e7fc = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e7f8 == 2) {
    DAT_0067e7fc = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7fc = 0;
  }
  if (DAT_0067e838 == 1) {
    DAT_0067e83c = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e838 == 2) {
    DAT_0067e83c = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e83c = 0;
  }
  if (DAT_0067e828 == 1) {
    DAT_0067e82c = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e828 == 2) {
    DAT_0067e82c = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e82c = 0;
  }
  if (DAT_0067e830 == 1) {
    DAT_0067e834 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e830 == 2) {
    DAT_0067e834 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e834 = 0;
  }
  if (DAT_0067e820 == 1) {
    DAT_0067e824 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e820 == 2) {
    DAT_0067e824 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e824 = 0;
  }
  if (DAT_0067e7e0 == 1) {
    DAT_0067e7e4 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
    FUN_0042fa00();
  } else if (DAT_0067e7e0 == 2) {
    DAT_0067e7e4 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7e4 = 0;
  }
  if (DAT_0067e7e8 == 1) {
    DAT_0067e7ec = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e7e8 == 2) {
    DAT_0067e7ec = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7ec = 0;
  }
  if (DAT_0067e808 == 1) {
    DAT_0067e80c = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e808 == 2) {
    DAT_0067e80c = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e80c = 0;
  }
  if (DAT_0067e810 == 1) {
    DAT_0067e814 = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e810 == 2) {
    DAT_0067e814 = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e814 = 0;
  }
  if (DAT_0067e818 == 1) {
    DAT_0067e81c = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e818 == 2) {
    DAT_0067e81c = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e81c = 0;
  }
  if (DAT_0067e7b8 == 1) {
    DAT_0067e7bc = (uchar)((1.0f - DAT_0067e840) * 255.0f);
  } else if (DAT_0067e7b8 == 2) {
    DAT_0067e7bc = (uchar)(DAT_0067e840 * 255.0f);
  } else {
    DAT_0067e7bc = 0;
  }
}

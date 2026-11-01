// FLAGS: /O2 /GS /Ob1
/* 0x00423040 (556 B). Best 100.0% exact (regs 100, ops 100). Pilot head start a6_00423040.c brought into s05 form.
   Pilot: Ghidra shape (81.6%: else-first order put the pressed branch first), then each key block
   written as if (!key) { clear } else { repeat logic } -> 100%. Float literals -4.0f / 0.25f are
   the constants at 0x005cca00 / 0x005cc564. Residual: none. */
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
MGLOBAL(float, DAT_007f1a5c, 0x007f1a5c);
MGLOBAL(float, DAT_007f1a58, 0x007f1a58);
MGLOBAL(char, DAT_007f1042, 0x007f1042);
MGLOBAL(char, DAT_007f1076, 0x007f1076);
MGLOBAL(int, DAT_007f1a54, 0x007f1a54);
MGLOBAL(int, DAT_007f1a64, 0x007f1a64);
MGLOBAL(unsigned char, DAT_007f1a90, 0x007f1a90);
MGLOBAL(char, DAT_007f1044, 0x007f1044);
MGLOBAL(char, DAT_007f1045, 0x007f1045);
MGLOBAL(char, DAT_007f1046, 0x007f1046);
MGLOBAL(char, DAT_007f1047, 0x007f1047);
MGLOBAL(int, DAT_006440fc, 0x006440fc);
MGLOBAL(int, DAT_00644108, 0x00644108);
MGLOBAL(int, DAT_0064410c, 0x0064410c);
MGLOBAL(int, DAT_006440f4, 0x006440f4);
MGLOBAL(int, DAT_006440ec, 0x006440ec);
MGLOBAL(int, DAT_00644104, 0x00644104);
MGLOBAL(int, DAT_006440f0, 0x006440f0);
MGLOBAL(int, DAT_006440f8, 0x006440f8);
void FUN_00417450(int x, int y, unsigned char b);
void FUN_00417530(int x, int y);

// MATCH: 0x00423040 FUN_00423040
void FUN_00423040(void)
{
    int x = -0x14 - (int)(DAT_007f1a5c * -4.0f);
    int y = -0xc - (int)(DAT_007f1a58 * -4.0f);
    if (DAT_007f1042 && DAT_007f1a54 == 0 && DAT_007f1a64 == 4)
        FUN_00417450(x, y, DAT_007f1a90 & 3);
    if (DAT_007f1076 && DAT_007f1a54 == 0 && DAT_007f1a64 == 4)
        FUN_00417530(x, y);

    if (!DAT_007f1044) { DAT_006440fc = 0; DAT_00644108 = 0; }
    else {
        if (DAT_00644108 == 0) DAT_007f1a5c -= 0.25f;
        if (DAT_006440fc == 0) { DAT_006440fc = 1; DAT_00644108 = 10; }
        else { DAT_006440fc = 2; if (DAT_00644108 == 0) DAT_00644108 = 1; else if (--DAT_00644108 < 0) DAT_00644108 = 0; }
    }

    if (!DAT_007f1045) { DAT_0064410c = 0; DAT_006440f4 = 0; }
    else {
        if (DAT_006440f4 == 0) DAT_007f1a5c += 0.25f;
        if (DAT_0064410c == 0) { DAT_0064410c = 1; DAT_006440f4 = 10; }
        else { DAT_0064410c = 2; if (DAT_006440f4 == 0) DAT_006440f4 = 1; else if (--DAT_006440f4 < 0) DAT_006440f4 = 0; }
    }

    if (!DAT_007f1046) { DAT_006440ec = 0; DAT_00644104 = 0; }
    else {
        if (DAT_00644104 == 0) DAT_007f1a58 += 0.25f;
        if (DAT_006440ec == 0) { DAT_006440ec = 1; DAT_00644104 = 10; }
        else { DAT_006440ec = 2; if (DAT_00644104 == 0) DAT_00644104 = 1; else if (--DAT_00644104 < 0) DAT_00644104 = 0; }
    }

    if (!DAT_007f1047) { DAT_006440f0 = 0; DAT_006440f8 = 0; }
    else {
        if (DAT_006440f8 == 0) DAT_007f1a58 -= 0.25f;
        if (DAT_006440f0 == 0) { DAT_006440f0 = 1; DAT_006440f8 = 10; }
        else { DAT_006440f0 = 2; if (DAT_006440f8 == 0) DAT_006440f8 = 1; else if (--DAT_006440f8 < 0) DAT_006440f8 = 0; }
    }
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 (h1) 0x0045c110: 100% (regs 100%), 155/155 ins, 537 B bytes identical. Packed RGBA word
   (a<<24 | b<<16 | g<<8 | r from bytes +4..+7), two colours return 1 before the switch, then a switch whose
   cases return 0x00472550(i) (tail jump) or 0x00472550(i) == 0. Residual: none. */
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
typedef struct Col0045c110 {
    int f00;
    unsigned char r, g, b, a;
} Col0045c110;
int FUN_00472550(int i);

// MATCH: 0x0045c110 FUN_0045c110
int FUN_0045c110(Col0045c110 *c)
{
    unsigned int rgba;
    rgba = ((unsigned int)c->a << 24) | ((unsigned int)c->b << 16) | ((unsigned int)c->g << 8) | c->r;
    if (rgba == 0xff010101 || rgba == 0xffff0080)
        return 1;
    switch (rgba) {
    case 0xff013cff:
        return FUN_00472550(0);
    case 0xff01ffff:
        return FUN_00472550(0) == 0;
    case 0xff023cff:
        return FUN_00472550(1);
    case 0xff02ffff:
        return FUN_00472550(1) == 0;
    case 0xff033cff:
        return FUN_00472550(2);
    case 0xff03ffff:
        return FUN_00472550(2) == 0;
    case 0xff043cff:
        return FUN_00472550(3);
    case 0xff04ffff:
        return FUN_00472550(3) == 0;
    case 0xff053cff:
        return FUN_00472550(4);
    case 0xff05ffff:
        return FUN_00472550(4) == 0;
    case 0xff063cff:
        return FUN_00472550(5);
    case 0xff06ffff:
        return FUN_00472550(5) == 0;
    case 0xff073cff:
        return FUN_00472550(6);
    case 0xff07ffff:
        return FUN_00472550(6) == 0;
    case 0xff083cff:
        return FUN_00472550(7);
    case 0xff08ffff:
        return FUN_00472550(7) == 0;
    case 0xff093cff:
        return FUN_00472550(8);
    case 0xff09ffff:
        return FUN_00472550(8) == 0;
    case 0xff0a3cff:
        return FUN_00472550(9);
    case 0xff0affff:
        return FUN_00472550(9) == 0;
    }
    return 0;
}

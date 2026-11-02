// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00487d00 (h4): 100%. Local 4-byte colour {0,0,0,a} as a byte struct (the converter's int << 24 scored 54.2%). All three callees cdecl, one arg. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct RGBA { unsigned char r, g, b, a; } RGBA;
MGLOBAL(int, DAT_00703058, 0x00703058);
MGLOBAL(int, DAT_00703070, 0x00703070);
MGLOBALA(char, DAT_00703068, 64, 0x00703068);
void FUN_00476a10(int p);
void FUN_004769f0(RGBA *c);
void FUN_00476d00(void *blk);

// MATCH: 0x00487d00 FUN_00487d00
void FUN_00487d00(int param_1, unsigned char param_2)
{
    RGBA c;
    c.r = 0;
    c.g = 0;
    c.b = 0;
    c.a = param_2;
    if (DAT_00703058 < DAT_00703070) {
        FUN_00476a10(param_1);
        FUN_004769f0(&c);
        FUN_00476d00(DAT_00703068);
        DAT_00703058++;
    }
}

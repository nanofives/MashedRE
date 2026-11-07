// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052d980: 100% (regs 100%). Image-format handler registrar (no library source): table of 16
   0x30-byte entries at 0x00912160, count at 0x007dbcd8; a full table or a duplicate id raises an RW error
   (pluginID 0x1b7, code from FUN_004d7ff0, set via FUN_004d8480). What moved it: `if (count < 16) {...}` with the
   full-table error after it (the early-return form put that error first and shrink-wrapped esi/edi, 64.3%).
   Residual: none. */
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
typedef struct { int pluginID; int errorCode; } RwError;
typedef struct { int id; int rest[11]; } fmt_handler_t;     /* 0x30 bytes */

MGLOBAL(int, DAT_007dbcd8, 0x007dbcd8);
MGLOBALA(fmt_handler_t, DAT_00912160, 16, 0x00912160);

int FUN_004d7ff0(int code);
RwError *FUN_004d8480(RwError *err);

// MATCH: 0x0052d980 FUN_0052d980
int FUN_0052d980(fmt_handler_t *h)
{
    int i;
    RwError err;

    if (DAT_007dbcd8 < 16) {
        for (i = 0; i < DAT_007dbcd8; i++) {
            if (DAT_00912160[i].id == h->id) {
                err.pluginID = 0x1b7;
                err.errorCode = FUN_004d7ff0(0);
                FUN_004d8480(&err);
                return 0;
            }
        }
        DAT_00912160[DAT_007dbcd8] = *h;
        DAT_007dbcd8++;
        return 1;
    }
    err.pluginID = 0x1b7;
    err.errorCode = FUN_004d7ff0(1);
    FUN_004d8480(&err);
    return 0;
}

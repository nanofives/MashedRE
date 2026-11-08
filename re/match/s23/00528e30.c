// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x00528e30: 100% (regs 100%). zlib 1.1.3 trees.c _tr_init with init_block written inline
   (tr_static_init is empty). The two chained zeroings (opt_len = static_len = 0, last_lit = matches = 0) had to be
   split into four stores in the original's order (0x16a4, 0x16a0, 0x16a8, 0x1698). Residual: none. */
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
MGLOBALA(int, DAT_006228d8, 5, 0x006228d8);
MGLOBALA(int, DAT_006228ec, 5, 0x006228ec);
MGLOBALA(int, DAT_00622900, 5, 0x00622900);

typedef struct { unsigned short fc; unsigned short dl; } ct_data;

// MATCH: 0x00528e30 FUN_00528e30
void FUN_00528e30(char *s)
{
    int n;

    *(ct_data **)(s + 0xb10) = (ct_data *)(s + 0x8c);
    *(int **)(s + 0xb18) = DAT_006228d8;
    *(ct_data **)(s + 0xb1c) = (ct_data *)(s + 0x980);
    *(int **)(s + 0xb24) = DAT_006228ec;
    *(ct_data **)(s + 0xb28) = (ct_data *)(s + 0xa74);
    *(int **)(s + 0xb30) = DAT_00622900;

    *(unsigned short *)(s + 0x16b0) = 0;
    *(int *)(s + 0x16b4) = 0;
    *(int *)(s + 0x16ac) = 8;

    for (n = 0; n < 286; n++) ((ct_data *)(s + 0x8c))[n].fc = 0;
    for (n = 0; n < 30; n++) ((ct_data *)(s + 0x980))[n].fc = 0;
    for (n = 0; n < 19; n++) ((ct_data *)(s + 0xa74))[n].fc = 0;

    ((ct_data *)(s + 0x8c))[256].fc = 1;
    *(int *)(s + 0x16a4) = 0;
    *(int *)(s + 0x16a0) = 0;
    *(int *)(s + 0x16a8) = 0;
    *(int *)(s + 0x1698) = 0;
}

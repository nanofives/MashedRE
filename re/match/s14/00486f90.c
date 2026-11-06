// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00486f90 (h4): 100% by bytes (match.py 95.1% instruction-level: Ghidra size 134 B stops before the trailing store + ret; --bytes: 140 B whole symbol BYTE-IDENTICAL). Two arrays: 150 x 0x7c records at 0x006fe790 (count 0x00703038) and 100 x 0x90 records at 0x006fa5d0 (count 0x0070303c), the 0x90 record = 0x14 header + a 0x7c record. One __inline clear helper for the 0x7c record (field order = store order). The first loop counts down (for (i = 150; i != 0;) { i--; ... }); a pointer do/while or for(i=149;i>=0;i--) gave 72.7-87.8%. Raw converter 17.4%. */
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
typedef struct S7c {
    int f00, f04, f08, f0c, f10, f14, f18, f1c, f20, f24, f28;
    float f2c;
    int f30[16];
    int f70, f74, f78;
} S7c;
typedef struct S90 {
    int f00, f04, f08, f0c, f10;
    S7c in;
} S90;
MGLOBALA(S7c, DAT_006fe790, 150, 0x006fe790);
MGLOBAL(int, DAT_00703038, 0x00703038);
MGLOBALA(S90, DAT_006fa5d0, 100, 0x006fa5d0);
MGLOBAL(int, DAT_0070303c, 0x0070303c);

static __inline void ClearInner(S7c *p)
{
    p->f78 = 0;
    p->f70 = 0;
    p->f20 = 0;
    p->f08 = 0;
    p->f04 = 0;
    p->f00 = 0;
    p->f14 = 0;
    p->f0c = 0;
    p->f18 = 0;
    p->f1c = 0;
    p->f74 = 0;
    p->f2c = 50.0f;
}

// MATCH: 0x00486f90 FUN_00486f90
void FUN_00486f90(void)
{
    int i;
    for (i = 150; i != 0; ) { i--; ClearInner(&DAT_006fe790[i]); }
    DAT_00703038 = 0;
    for (i = 0; i < 100; i++) {
        DAT_006fa5d0[i].f00 = 0;
        ClearInner(&DAT_006fa5d0[i].in);
    }
    DAT_0070303c = 0;
}

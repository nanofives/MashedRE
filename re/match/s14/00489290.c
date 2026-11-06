// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00489290 (h5): by hand. 100% (regs 100%), whole obj symbol byte-identical (Ghidra size 29 stops before the loop tail).
   Pool records: 100 x 0x68 at 0x00703118 (layout from FUN_004892c0). Loop form `for (i = 100; i-- != 0;)`;
   `for (i = 99; i >= 0; i--)` rotates the loop (60%), a pointer do-while becomes a counter (30%). */
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
typedef struct ParticlePool {
    int *m_items;
    int m_count;
    int m_08;
    int m_0c;
    unsigned char m_10[0x40];
    int m_50;
    int m_54;
    int m_58;
    float m_5c;
    int m_60;
    int m_64;
} ParticlePool;
MGLOBALA(ParticlePool, DAT_00703118, 100, 0x00703118);
// MATCH: 0x00489290 FUN_00489290
void FUN_00489290(void)
{ int i; for (i = 100; i-- != 0; ) { DAT_00703118[i].m_54 = 0; DAT_00703118[i].m_58 = 1; } }
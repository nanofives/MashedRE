// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004898d0 (h5): by hand. 100% (regs 100%), byte-identical. Tail call through RwGlobals
   (DAT_007d3ff8) slot +0x10c with the pool's item array. Raw converter lost the argument (23%). */
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
typedef struct PartV2 { float x, y; } PartV2;
typedef struct PartV3 { float x, y, z; } PartV3;
typedef struct PartItem {
    unsigned char m_rgba[4];
    PartV3 m_pos;
    PartV2 m_10;
    float m_18;
    int m_1c;
    PartV2 m_20;
    PartV2 m_28;
} PartItem;
typedef struct PartPool {
    PartItem *m_items;
    int m_max;
    int m_count;
    int m_0c;
    unsigned char m_10[0x40];
    int m_50;
    int m_54;
    int m_58;
    float m_5c;
    int m_60;
    int m_64;
} PartPool;
typedef void (*RwFreeFn)(void *p);
MGLOBAL(void **, DAT_007d3ff8, 0x007d3ff8);
void FUN_004768c0(void *p0);
// MATCH: 0x004898d0 FUN_004898d0
void FUN_004898d0(PartPool *param_1)
{
    param_1->m_54 = 0;
    param_1->m_58 = 1;
    FUN_004768c0(param_1->m_10);
    ((RwFreeFn)DAT_007d3ff8[0x10c / 4])(param_1->m_items);
}

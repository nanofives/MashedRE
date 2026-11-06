// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004892c0 (h5): by hand. 100% (regs 100%); whole obj symbol byte-identical (Ghidra size 215 stops
   3 instructions before the end). Typed PartPool (0x68 B x 100 at 0x00703118) and PartItem (0x30 B); search loop
   with pool = NULL + break, then `if (pool == 0) return 0` (the original keeps that dead NULL test). Raw 48%. */
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
MGLOBALA(PartPool, DAT_00703118, 100, 0x00703118);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
void *FUN_00474db0(int p0, int p1);
void FUN_00476cb0(void *p0, int p1, int p2);
void FUN_004770c0(void *p0, int p1, int p2, int p3);
// MATCH: 0x004892c0 FUN_004892c0
PartPool *FUN_004892c0(int param_1, int param_2, int param_3, int param_4)
{
    PartPool *pool;
    PartItem *it;
    int i;

    pool = 0;
    for (i = 0; i < 100; i++) {
        if (DAT_00703118[i].m_58 != 0) {
            pool = &DAT_00703118[i];
            break;
        }
    }
    if (pool == 0)
        return 0;
    if (param_3 != 0 && param_2 != 0) {
        pool->m_max = param_2;
        pool->m_5c = DAT_005cc320 / (float)param_3;
        if (param_4 == 0)
            param_4 = 0x817;
        pool->m_50 = param_4;
        FUN_004770c0(pool->m_10, param_4, param_2 * param_3, param_1);
        FUN_00476cb0(pool->m_10, 5, 6);
        pool->m_54 = 1;
        pool->m_58 = 0;
        pool->m_items = (PartItem *)FUN_00474db0(param_2, 0x30);
        for (i = param_2; i-- != 0; ) {
            it = &pool->m_items[i];
            it->m_pos.z = 0;
            it->m_pos.y = 0;
            it->m_pos.x = 0;
            it->m_rgba[0] = 0xff;
            it->m_rgba[1] = 0xff;
            it->m_rgba[2] = 0xff;
            it->m_rgba[3] = 0xff;
        }
        return pool;
    }
    return 0;
}

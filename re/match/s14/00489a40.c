// FLAGS: /O2 /GS /Ob1
/* h5 */
/* MATCH-S14-FU (2026-11-11): 78.0% -> 78.9%: the two multipliers as float literals 0.3f / 0.6f (.rdata 0x005cc99c / 0x005cc318). Residual: the (float)i and m_max fild temps sit one slot higher than the original's (param_1 / param_2 homes at 0x00489b02..0x00489b0d), and the pos copy / sz are scheduled after the uv stores. match_permute (933 compiles) reached 85.6% only by moving c = m_col[idx] below its c >> 24 use, which changes the value, so it was rejected. */
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
typedef struct PartTrail {
    PartV3 *m_pos;
    PartV3 *m_vel;
    unsigned int *m_col;
    int m_max;
    int m_len;
    int m_14;
    float m_18;
    float m_1c;
    float m_20;
    int m_head;
    PartPool *m_pool;
} PartTrail;
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc99c, 0x005cc99c);
MGLOBAL(float, DAT_005cc318, 0x005cc318);
void FUN_004893d0(PartPool *p0, PartV3 *p1, unsigned int *p2);
void FUN_00489450(PartPool *p0, PartV2 *p1);
void FUN_00489480(PartPool *p0, float p1);
void FUN_00489890(PartPool *p0);
// MATCH: 0x00489a40 FUN_00489a40
void FUN_00489a40(PartTrail *param_1, PartV3 *param_2, PartV3 *param_3, unsigned int *param_4)
{
    int i;

    param_1->m_pos[param_1->m_head] = *param_2;
    if (param_3 != 0) {
        param_1->m_vel[param_1->m_head] = *param_3;
    } else {
        param_1->m_vel[param_1->m_head].x = 0;
        param_1->m_vel[param_1->m_head].y = 0;
        param_1->m_vel[param_1->m_head].z = 0;
    }
    if (param_4 != 0) {
        param_1->m_col[param_1->m_head] = *param_4;
    } else {
        ((unsigned char *)&param_1->m_col[param_1->m_head])[0] = 0xff;
        ((unsigned char *)&param_1->m_col[param_1->m_head])[1] = 0xff;
        ((unsigned char *)&param_1->m_col[param_1->m_head])[2] = 0xff;
        ((unsigned char *)&param_1->m_col[param_1->m_head])[3] = 0xff;
    }
    param_1->m_head++;
    if (param_1->m_head >= param_1->m_max)
        param_1->m_head = 0;
    for (i = param_1->m_max - param_1->m_len; i < param_1->m_max; i++) {
        int idx;
        float frac;
        float span;
        float w;
        float sz;
        unsigned int c;
        int a;
        PartV3 p;
        PartV2 uv;
        frac = DAT_005cc320 - (float)i / param_1->m_max;
        span = param_1->m_1c - param_1->m_18;
        idx = param_1->m_head + i;
        if (idx >= param_1->m_max)
            idx = param_1->m_head - param_1->m_max + i;
        c = param_1->m_col[idx];
        a = (int)(c >> 24);
        ((unsigned char *)&c)[3] = (unsigned char)(int)(a - (a - 10) * frac);
        w = (span * frac + param_1->m_18) * 0.3f;
        p = param_1->m_pos[idx];
        uv.y = w;
        uv.x = w * 0.6f;
        sz = frac * param_1->m_20;
        FUN_004893d0(param_1->m_pool, &p, &c);
        FUN_00489450(param_1->m_pool, &uv);
        FUN_00489480(param_1->m_pool, sz);
        param_1->m_pos[idx].x = param_1->m_vel[idx].x + param_1->m_pos[idx].x;
        param_1->m_pos[idx].y = param_1->m_vel[idx].y + param_1->m_pos[idx].y;
        param_1->m_pos[idx].z = param_1->m_vel[idx].z + param_1->m_pos[idx].z;
    }
    FUN_00489890(param_1->m_pool);

}

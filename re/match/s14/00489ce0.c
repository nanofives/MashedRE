// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00489ce0 (h5): by hand. 100% (regs 100%). Emitter table 4 x 16 records of 0x38 B at 0x007059b8,
   counts at 0x007067b8. What moved it: field-wise V3 copies (struct copies: 17%), param_3 / +0x0c / +0x34 as float
   (int keeps it in edx: 88.7 -> 98.1%), and indexing with DAT_007067b8[param_1] each time instead of a cached
   count local (100%). Raw 84.9%. */
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
typedef struct PartV3 { float x, y, z; } PartV3;
typedef struct PartEmit {
    PartV3 m_00;
    float m_0c;
    PartV3 m_10;
    PartV3 m_1c;
    PartV3 m_28;
    float m_34;
} PartEmit;
typedef struct PartEmitRow { PartEmit m_e[16]; } PartEmitRow;
MGLOBALA(PartEmitRow, DAT_007059b8, 4, 0x007059b8);
MGLOBALA(int, DAT_007067b8, 4, 0x007067b8);
// MATCH: 0x00489ce0 FUN_00489ce0
int FUN_00489ce0(int param_1, PartV3 *param_2, float param_3, PartV3 *param_4, PartV3 *param_5)
{
    if (DAT_007067b8[param_1] >= 16)
        return 0;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_00.x = param_2->x;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_00.y = param_2->y;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_00.z = param_2->z;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_10.x = param_4->x;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_10.y = param_4->y;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_10.z = param_4->z;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_1c.x = param_5->x;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_1c.y = param_5->y;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_1c.z = param_5->z;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_0c = param_3;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_28.x = param_2->x;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_28.y = param_2->y;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_28.z = param_2->z;
    DAT_007059b8[param_1].m_e[DAT_007067b8[param_1]].m_34 = param_3;
    DAT_007067b8[param_1]++;
    return 1;
}

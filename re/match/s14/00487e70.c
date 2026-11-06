// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00487e70 (h4): 100%. 128 x 0x38 records at 0x0086ae20 (the same array as 0x00487e40: active flag at +0x14). Moves: search loop as for (i..128) { p = &a[i]; if (!p->active) goto found; } return -1; (pointer-compare forms gave jb and a second test, 68%); switch cases written in binary order 0,5,1,2,3,4,6,11,12,7 (source order 0..12 gave 72%, the compiler tail-merges 1/5 and 4/6/7 itself); type stored before f2c. The float tables at 0x006142a0..0x0061448c and 0x007030b8/0x007030dc are declared float[4] (the decomp's s_33s / s_fff string names are float tables). .rdata 0x005cc328 = 0.01f literal. */
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
typedef struct Rec38 {
    int type;
    float pos[3];
    int n;
    int active;
    float f18;
    float *t1c, *t20, *t24, *t28;
    int f2c;
    float f30;
    int f34;
} Rec38;
MGLOBALA(Rec38, DAT_0086ae20, 128, 0x0086ae20);
MGLOBAL(int, DAT_007030b4, 0x007030b4);
MGLOBALA(float, DAT_006142a0, 4, 0x006142a0);
MGLOBALA(float, DAT_006142b0, 4, 0x006142b0);
MGLOBALA(float, DAT_006142c0, 4, 0x006142c0);
MGLOBALA(float, DAT_006142e4, 4, 0x006142e4);
MGLOBALA(float, DAT_00614304, 4, 0x00614304);
MGLOBALA(float, DAT_00614314, 4, 0x00614314);
MGLOBALA(float, DAT_00614324, 4, 0x00614324);
MGLOBALA(float, DAT_00614348, 4, 0x00614348);
MGLOBALA(float, DAT_00614368, 4, 0x00614368);
MGLOBALA(float, DAT_00614378, 4, 0x00614378);
MGLOBALA(float, DAT_00614388, 4, 0x00614388);
MGLOBALA(float, DAT_006143a8, 4, 0x006143a8);
MGLOBALA(float, DAT_006143c8, 4, 0x006143c8);
MGLOBALA(float, DAT_006143d8, 4, 0x006143d8);
MGLOBALA(float, DAT_006143e8, 4, 0x006143e8);
MGLOBALA(float, DAT_0061440c, 4, 0x0061440c);
MGLOBALA(float, DAT_0061442c, 4, 0x0061442c);
MGLOBALA(float, DAT_0061443c, 4, 0x0061443c);
MGLOBALA(float, DAT_0061444c, 4, 0x0061444c);
MGLOBALA(float, DAT_0061446c, 4, 0x0061446c);
MGLOBALA(float, DAT_0061447c, 4, 0x0061447c);
MGLOBALA(float, DAT_0061448c, 4, 0x0061448c);
MGLOBALA(float, DAT_006144b0, 4, 0x006144b0);
MGLOBALA(float, DAT_007030b8, 4, 0x007030b8);
MGLOBALA(float, DAT_007030dc, 4, 0x007030dc);

// MATCH: 0x00487e70 FUN_00487e70
int FUN_00487e70(int param_1, float *param_2, int param_3, int param_4)
{
    int i;
    Rec38 *p;
    for (i = 0; i < 128; i++) {
        p = &DAT_0086ae20[i];
        if (p->active == 0) {
            goto found;
        }
    }
    return -1;
found:
    p->pos[0] = param_2[0];
    p->pos[1] = param_2[1];
    p->pos[2] = param_2[2];
    p->type = param_1;
    p->f2c = param_3;
    p->f30 = 0.0f;
    switch (param_1) {
    case 0:
        p->n = 5;
        p->t1c = DAT_006142b0;
        p->t20 = DAT_006142a0;
        p->t28 = DAT_006142e4;
        p->t24 = DAT_006142c0;
        p->f18 = -0.5f;
        break;
    case 5:
        p->n = 5;
        p->t1c = DAT_00614314;
        p->t20 = DAT_00614304;
        p->t28 = DAT_00614348;
        p->t24 = DAT_00614324;
        p->f18 = 0.5f;
        break;
    case 1:
        p->n = 2;
        p->t1c = DAT_00614314;
        p->t20 = DAT_00614304;
        p->t28 = DAT_00614348;
        p->t24 = DAT_00614324;
        p->f18 = 0.5f;
        break;
    case 2:
        p->n = 1;
        p->t1c = DAT_00614378;
        p->t20 = DAT_00614368;
        p->t28 = DAT_00614388;
        p->t24 = DAT_007030b8;
        p->f18 = 0.5f;
        break;
    case 3:
        p->n = 2;
        p->t1c = DAT_00614378;
        p->t20 = DAT_00614368;
        p->t28 = DAT_006143a8;
        p->t24 = DAT_007030b8;
        p->f18 = 0.5f;
        break;
    case 4:
        p->n = 3;
        p->t1c = DAT_006143d8;
        p->t20 = DAT_006143c8;
        p->t28 = DAT_0061440c;
        p->t24 = DAT_006143e8;
        p->f30 = (float)param_4 * 0.01f;
        p->f18 = 0.5f;
        break;
    case 6:
        p->n = 3;
        p->t1c = DAT_006143d8;
        p->t20 = DAT_006143c8;
        p->t28 = DAT_00614388;
        p->t24 = DAT_006143e8;
        p->f30 = (float)param_4 * 0.01f;
        p->f18 = 0.5f;
        break;
    case 11:
        p->n = 3;
        p->t1c = DAT_0061447c;
        p->t20 = DAT_0061446c;
        p->t28 = DAT_006144b0;
        p->t24 = DAT_0061448c;
        p->f18 = -1.25f;
        break;
    case 12:
        p->n = 2;
        p->t1c = DAT_0061443c;
        p->t20 = DAT_0061442c;
        p->t28 = DAT_0061444c;
        p->t24 = DAT_007030dc;
        p->f18 = 0.5f;
        break;
    case 7:
        p->n = 3;
        p->t1c = DAT_0061443c;
        p->t20 = DAT_0061442c;
        p->t28 = DAT_0061440c;
        p->t24 = DAT_007030dc;
        p->f18 = 0.5f;
        break;
    }
    DAT_0086ae20[i].active = 1;
    DAT_007030b4 = 0;
    return i;
}

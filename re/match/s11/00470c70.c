// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00470c70 (h6): best 69.8% (regs 73.7%, ops 93.4%), 530/543 instructions, class (c)+(b).
   (c): two callees take register arguments in the original (static in its TU, pilot finding 1), see below;
   their call sites cannot match while those callees are outside this file. (b): stack frame is 8 bytes
   smaller than the original's (0x1c vs 0x24) and the first car loop keeps the player pointer in a register
   (original: stack slot + separate car pointer); register allocation of the 4-float sort differs slightly.
   Moves: rdata floats are compiler literals (the 1/16 is `/ 16.0f`, fixed the average stores), the
   0..3 index loop as do-while (a for loop was unrolled), car loops driven by the car pointer (dec counters),
   the 0x00470670 call written in both if/else arms (the original cross-jumps two call sites), sum
   accumulators initialised z, y, x. Permuter 1625 compiles on an earlier base: 65.0%; on the final base 690 compiles, no gain.
   16 car records of 0xd04 bytes at 0x008815a0. Register-passed arguments the
   C below does not reproduce: the call to 0x00470670 at 0x00471079 also passes the car pointer in EAX
   (mov eax, ebp at 0x00471074), the call to 0x0046f6c0 at 0x00471389 passes param_2 in EAX and the car
   (+0x18 base - 0x18) in EDI (0x00471382 / 0x00471385): both callees are static in the original TU
   (pilot finding 1) and are not held in this file. 0x004a2c48 is the CRT float-to-int helper. */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct M16 { int m[16]; } M16;
typedef struct Car {
    int index;                 /* +0x000 */
    int f4;                    /* +0x004 */
    int f8;                    /* +0x008 */
    int fc;                    /* +0x00c */
    int f10;                   /* +0x010 */
    int f14;                   /* +0x014 */
    int timer;                 /* +0x018 */
    unsigned char pad1c[0x928 - 0x1c];
    M16 cur;                   /* +0x928 */
    M16 prev;                  /* +0x968 */
    int ring;                  /* +0x9a8 */
    int ring_prev;             /* +0x9ac */
    V3 v9b0;                   /* +0x9b0 */
    V3 v9bc;                   /* +0x9bc */
    unsigned char pad9c8[0x9e0 - 0x9c8];
    float f9e0;                /* +0x9e0 */
    float len9e4;              /* +0x9e4 */
    float len9e8;              /* +0x9e8 */
    int f9ec;                  /* +0x9ec */
    int f9f0;                  /* +0x9f0 */
    unsigned char pad9f4[0xaec - 0x9f4];
    int faec;                  /* +0xaec */
    float faf0;                /* +0xaf0 */
    V3 avg;                    /* +0xaf4 */
    unsigned char padb00[0xb2c - 0xb00];
    V3 pts[16];                /* +0xb2c */
    unsigned char padbec[0xd00 - 0xbec];
    int fd00;                  /* +0xd00 */
} Car;
typedef struct Pl10 { int sel; int flag; int pad[2]; } Pl10;
typedef struct Tab4c { int m[0x13]; } Tab4c;
MGLOBALA(Car, DAT_008815a0, 16, 0x008815a0);
MGLOBAL(float, DAT_007f0f64, 0x007f0f64);
MGLOBAL(float, DAT_007f0f68, 0x007f0f68);
MGLOBAL(float, DAT_007f0f6c, 0x007f0f6c);
MGLOBAL(float, DAT_007f0f90, 0x007f0f90);
MGLOBAL(int, DAT_007f0fd0, 0x007f0fd0);
MGLOBAL(int, DAT_007f0fd4, 0x007f0fd4);
MGLOBAL(int, DAT_007f0fd8, 0x007f0fd8);
MGLOBAL(int, DAT_007f0ff4, 0x007f0ff4);
MGLOBAL(int, DAT_007f1018, 0x007f1018);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBALA(Tab4c, DAT_007f1038, 16, 0x007f1038);
MGLOBAL(Tab4c, DAT_007f19b8, 0x007f19b8);
MGLOBALA(Pl10, DAT_007f1a14, 4, 0x007f1a14);
MGLOBAL(unsigned int, DAT_007e9de0, 0x007e9de0);
MGLOBALA(int, DAT_007e9de4, 256, 0x007e9de4);
MGLOBALA(int, DAT_007ea1e4, 256, 0x007ea1e4);
MGLOBALA(int, DAT_007ea5e4, 256, 0x007ea5e4);
MGLOBALA(unsigned int, DAT_0088e660, 4, 0x0088e660);
MGLOBAL(float, DAT_0088e610, 0x0088e610);
MGLOBAL(float, DAT_0088e5f0, 0x0088e5f0);
MGLOBAL(int, DAT_00803334, 0x00803334);
MGLOBAL(int, DAT_00803338, 0x00803338);
MGLOBAL(int, DAT_0080333c, 0x0080333c);
MGLOBAL(int, DAT_00803340, 0x00803340);
int FUN_0046c7b0(int i);
int FUN_0040e370(unsigned int i);
int FUN_00422fd0(unsigned int i);
int FUN_0040e350(void);
int FUN_0040e340(void);
void FUN_00413c70(int a, int b, int c);
int FUN_0042fe70(void);
void FUN_00467350(unsigned int a);
int FUN_0047eb30(void);
float FUN_00408a50(unsigned int i);
int FUN_0040e470(int i);
int FUN_00443080(void);
void FUN_00470670(int i, float f, Tab4c *tab, float *p);
float FUN_004c3ac0(V3 *v);
void FUN_0046dc20(int i);
void FUN_004709a0(float f, float *p);
int FUN_0046da80(int i, float *p);
void FUN_0046f6c0(int a);
void FUN_00480b70(int i);
int FUN_00480720(int i);
int FUN_004809e0(int i);
// MATCH: 0x00470c70 FUN_00470c70
int FUN_00470c70(unsigned int param_1, float *param_2)
{
    unsigned int u, n, rem, step, s;
    int i, j, t, flag, mode;
    float v[4];
    float tf;
    unsigned int ti;
    int swapped;
    float sx, sy, sz;
    Car *c;
    Tab4c *tab;
    Pl10 *pl;

    DAT_007f0f64 = (float)(DAT_008815a0[0].f4 + 1000);
    DAT_007f0f68 = (float)(DAT_008815a0[0].f14 + 1000);
    DAT_007f0f6c += 1.0f;
    if (DAT_007f0fd0 == 1) {
        if (DAT_007f0fd4 < 0 || DAT_007f0fd4 >= 4)
            goto skip;
        if (FUN_0046c7b0(DAT_007f0fd4) == 0) {
            for (u = 0; u < 4; u++) {
                if (FUN_0040e370(u))
                    FUN_00422fd0(u);
            }
        }
        if (FUN_0040e350() == 6)
            FUN_00413c70(DAT_007f0fd4, 3, 1);
        else
            FUN_00413c70(DAT_007f0fd4, 3, 0);
    }
    if (DAT_007f0fd0 == 2) {
        if (DAT_007f0fd8 < 0 || DAT_007f0fd8 >= FUN_0040e340())
            DAT_007f0fd8 = 0;
        for (u = 0; u < 4; u++)
            DAT_007f1a14[u].flag = 1;
        DAT_007f1a14[DAT_007f0fd8].flag = 0;
        if (FUN_0040e350() == 6)
            FUN_00413c70(DAT_007f0fd8, 3, 1);
        else
            FUN_00413c70(DAT_007f0fd8, 3, 0);
    }
skip:
    DAT_007f1018 = 900000;
    t = FUN_0042fe70();
    if (t == 1)
        DAT_007f1018 = 1800000;
    else if (t == 2)
        DAT_007f1018 = 3600000;
    if (DAT_007f0fd0 == 1 || DAT_007f0fd0 == 2) {
        DAT_007f0f90 += 1.0f;
        DAT_007f1018 = (int)((float)DAT_007f0ff4 + 30000.0f);
    }
    FUN_00467350(param_1);
    FUN_0047eb30();
    v[0] = 1000.0f;
    v[1] = 1000.0f;
    v[2] = 1000.0f;
    u = 0;
    v[3] = 1000.0f;
    do {
        DAT_0088e660[u] = u;
        u++;
    } while (u < 4);
    n = FUN_0040e340();
    for (u = 0; u < n; u++)
        v[u] = FUN_00408a50(u);
    do {
        swapped = 0;
        if (v[0] < v[1]) {
            tf = v[0]; v[0] = v[1]; v[1] = tf;
            ti = DAT_0088e660[0]; DAT_0088e660[0] = DAT_0088e660[1]; DAT_0088e660[1] = ti;
            swapped = 1;
        }
        if (v[1] < v[2]) {
            tf = v[1]; v[1] = v[2]; v[2] = tf;
            ti = DAT_0088e660[1]; DAT_0088e660[1] = DAT_0088e660[2]; DAT_0088e660[2] = ti;
            swapped = 1;
        }
        if (v[2] < v[3]) {
            tf = v[2]; v[2] = v[3]; v[3] = tf;
            ti = DAT_0088e660[2]; DAT_0088e660[2] = DAT_0088e660[3]; DAT_0088e660[3] = ti;
            swapped = 1;
        }
    } while (swapped == 1);
    DAT_0088e610 = *param_2 * 0.0027778f;
    DAT_0088e5f0 = 3000.0f / DAT_0088e610;
    for (rem = param_1; rem != 0; rem -= step) {
        step = 50;
        if (rem < 50)
            step = rem;
        pl = DAT_007f1a14;
        for (i = 0, c = DAT_008815a0; c < DAT_008815a0 + 16; i++, pl++, c++) {
            c->index = i;
            flag = 0;
            t = FUN_0040e470(i);
            if ((t != 0 && t != 1) || FUN_00443080() == 1)
                flag = 1;
            c->fd00 = flag;
            tab = &DAT_007f1038[pl->sel];
            if (c->f9f0 != 0)
                tab = &DAT_007f19b8;
            if (c->f10 != 0)
                tab = &DAT_007f19b8;
            if (FUN_0040e350() != 6 && FUN_0040e350() != 0xb && FUN_0040e350() != 10)
                tab = &DAT_007f19b8;
            if (c->f14 == 0 && (c->f4 != 0 || c->f10 == 1)) {
                c->fc = 0;
                if (c->faec != 0) {
                    c->faec -= step;
                    if (c->faec < -1200)
                        c->faec = 0;
                    if (c->faec < 0)
                        FUN_00470670(i, (float)step, tab, param_2);
                } else {
                    FUN_00470670(i, (float)step, tab, param_2);
                }
            }
        }
        for (i = 0, c = DAT_008815a0; c < DAT_008815a0 + 16; i++, c++) {
            if (c->f4 != 0 || c->f10 == 1) {
                c->len9e4 = FUN_004c3ac0(&c->v9b0);
                c->len9e8 = FUN_004c3ac0(&c->v9bc);
            }
            if ((float)c->fc > 12000.0f)
                FUN_0046dc20(i);
        }
        for (u = step; u != 0; u -= s) {
            s = u;
            if (s > 25)
                s = 25;
            FUN_004709a0((float)s, param_2);
        }
    }
    for (i = 0, c = DAT_008815a0; c < DAT_008815a0 + 16; i++, c++) {
        if (c->f14 == 0 && (c->f4 != 0 || c->f10 == 1)) {
            if (c->ring != 0) {
                c->ring_prev = c->ring;
                c->ring = (c->ring - 1) & 1;
                c->prev = c->cur;
            }
            sz = 0.0f;
            sy = 0.0f;
            sx = 0.0f;
            for (j = 0; j < 16; j++) {
                sx += c->pts[j].x;
                sy += c->pts[j].y;
                sz += c->pts[j].z;
            }
            c->avg.x = sx / 16.0f;
            c->avg.y = sy / 16.0f;
            c->avg.z = sz / 16.0f;
            if (c->f9e0 == 0.0f) {
                c->faf0 += (float)param_1;
                if (c->faf0 > 15000.0f) {
                    mode = FUN_0040e350();
                    if (mode != 1 && mode != 2 && mode != 3 && mode != 4 && mode != 5) {
                        DAT_007e9de4[DAT_007e9de0] = 1;
                        DAT_007ea1e4[DAT_007e9de0] = DAT_007f1030;
                        DAT_007ea5e4[DAT_007e9de0] = i + 0xff00;
                        if (++DAT_007e9de0 >= 256)
                            DAT_007e9de0 = 0;
                    }
                }
            } else {
                c->faf0 = 0;
            }
        }
    }
    for (i = 0, c = DAT_008815a0; c < DAT_008815a0 + 16; i++, c++) {
        if (c->f4 != 0 || c->f10 == 1) {
            if (c->timer != 0) {
                c->timer -= param_1;
                if (c->timer < 0)
                    c->timer = 0;
            }
            if (c->f14 != 0) {
                c->f9e0 = (float)FUN_0046da80(i, param_2);
                if (c->f9e0 == 4.0f)
                    FUN_0046f6c0(0);
                FUN_00480b70(i);
            }
            if (c->timer == 0) {
                if (c->f9e0 < 4.0f)
                    FUN_00480720(i);
                if (c->f9e0 == 4.0f && c->f9f0 == 0)
                    FUN_004809e0(i);
            }
        }
    }
    DAT_0080333c = 0;
    DAT_00803338 = 0;
    DAT_00803334 = 0;
    DAT_00803340 = 0;
    return 1;
}

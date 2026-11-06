// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046b540 (h2): 59.2% (regs 64.4%, ops 90.8%), 348/348 ins. Raw converter did not compile.
   param_1 is the uint car index (Ghidra typed it float). Shape: tuning defaults into the 0x00613108 block,
   key = FUN_0040ce80(FUN_00430790() or FUN_00431d70() when FUN_0042f6a0()==10), the {id,a,b,c,d} table at
   0x00613140 scanned to id -1, then the 0xd04 record of 0x008815a0 (wheel[4] of 0xc4 at +0x168, points
   p[18] at +0x60, slots of 0x40 at +0x4ac) filled in the original store order, five loops (wheel speed max
   with the zero-guarded sqrt, wheel radius, three slot-distance loops of 4/8/6 with the max in the middle
   one), and the load split through the q0..q3 = s/len temporaries. What moved it: 0.27777779f / 1.05f
   literals (52.3 -> 54.0%), permuter store moves (-> 56.2%, its one unsafe move that read `s` before
   assigning it was rejected), q temporaries (-> 59.2%). FUN_004c3b30 is sqrt (cdecl float).
   Residual (b): the car pointer stays in ESI as in the original, but the store block caches constants in
   eax/edx where the original runs everything through ecx, and the first wheel loop and the slot-distance
   loops pick other induction bases (pts[i].z vs .y) and x87 operand orders. */
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
typedef struct { float x, y, z; } V3_0046b540;
typedef struct { int v[6]; } Six_0046b540;
typedef struct {
    int f00;           /* +0x00 */
    V3_0046b540 pos;   /* +0x04 */
    float len;         /* +0x10 */
    float f14;         /* +0x14 */
    undefined1 pad18[0xc];
    float f24;         /* +0x24 */
    float f28;         /* +0x28 */
    undefined1 pad2c[0xc4 - 0x2c];
} Wheel_0046b540;      /* 0xc4 */
typedef struct {
    undefined1 pad00[0x10];
    float f10;         /* +0x10 */
    undefined1 pad14[0x2c];
} Slot_0046b540;       /* 0x40 */
typedef struct {
    int f000;
    int f004;
    undefined1 pad008[0x8];
    int f010;
    undefined1 pad014[0x3c];
    float f050;
    float f054;
    float f058;
    float f05c;
    V3_0046b540 pts[18];            /* +0x060 */
    undefined1 pad138[0xc];
    float f144;
    float f148;
    float f14c;
    float f150;
    float f154;
    float f158;
    V3_0046b540 center;             /* +0x15c */
    Wheel_0046b540 w[4];            /* +0x168 */
    Six_0046b540 f478;              /* +0x478 */
    undefined1 pad490[0x8];
    float f498;
    int f49c;
    float f4a0;
    float f4a4;
    undefined1 pad4a8[4];
    Slot_0046b540 slot[18];         /* +0x4ac; the last one runs into +0x928 */
    undefined1 pad92c[0x9a8 - 0x92c];
    int f9a8;
    int f9ac;
    undefined1 pad9b0[0xad0 - 0x9b0];
    int fad0;
    undefined1 padad4[0xd04 - 0xad4];
} Car_0046b540;                     /* 0xd04 */
typedef struct { int id; int a; int b; int c; int d; } Tune_0046b540;
MGLOBALA(Car_0046b540, DAT_008815a0, 16, 0x008815a0);
MGLOBAL(float, DAT_00613108, 0x00613108);
MGLOBAL(int, DAT_00613110, 0x00613110);
MGLOBAL(float, DAT_00613114, 0x00613114);
MGLOBAL(Six_0046b540, DAT_00613118, 0x00613118);
MGLOBAL(float, DAT_00613130, 0x00613130);
MGLOBAL(float, DAT_0061313c, 0x0061313c);
MGLOBALA(Tune_0046b540, DAT_00613140, 1, 0x00613140);
MGLOBAL(float, DAT_005cc558, 0x005cc558);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
int FUN_00430790(void);
int FUN_0042f6a0(void);
int FUN_00431d70(void);
int FUN_0040ce80(int p);
float FUN_004c3b30(float x);

// MATCH: 0x0046b540 FUN_0046b540
undefined4 FUN_0046b540(uint param_1)
{
    Car_0046b540 *c;
    Tune_0046b540 *t;
    Wheel_0046b540 *w;
    V3_0046b540 v;
    float l, mx, s, k, q0, q1, q2, q3;
    int key, id, i;

    DAT_00613108 = 100.0f;
    DAT_00613114 = 40000.0f;
    DAT_00613130 = 1.0f;
    DAT_0061313c = 1.5f;
    id = FUN_00430790();
    if (FUN_0042f6a0() == 10) {
        id = FUN_00431d70();
    }
    key = FUN_0040ce80(id);
    for (t = DAT_00613140; t->id != -1; t++) {
        if (t->id == key) {
            DAT_00613108 = (float)t->a;
            DAT_00613114 = (float)t->b;
            DAT_00613130 = (float)t->c * DAT_005cc558;
            DAT_0061313c = (float)t->d * DAT_005cc558;
        }
    }
    if (param_1 >= 0x10) return 0;
    c = &DAT_008815a0[param_1];
    c->f154 = DAT_0061313c;
    c->w[0].pos.z = 1.08288f;
    c->w[1].pos.z = 1.08288f;
    c->w[2].pos.z = -1.14624f;
    c->w[0].f00 = 2;
    c->w[1].f00 = 2;
    c->w[0].f24 = DAT_00613130;
    c->w[3].pos.z = -1.14624f;
    c->w[1].f24 = DAT_00613130;
    c->w[2].f24 = DAT_00613130;
    c->w[3].f24 = DAT_00613130;
    c->w[0].f28 = 34.0f;
    c->w[1].f28 = 34.0f;
    c->w[2].f28 = 34.0f;
    c->w[3].f28 = 34.0f;
    c->f49c = DAT_00613110;
    c->f498 = DAT_00613114;
    c->f478.v[0] = DAT_00613118.v[0];
    c->f478.v[1] = DAT_00613118.v[1];
    c->f478.v[2] = DAT_00613118.v[2];
    c->f478.v[3] = DAT_00613118.v[3];
    c->f478.v[4] = DAT_00613118.v[4];
    c->f478.v[5] = DAT_00613118.v[5];
    c->pts[0].z = 0.3008f;
    c->pts[1].z = 0.3008f;
    c->f004 = 1;
    c->f010 = 0;
    c->f150 = 0.15f;
    c->f158 = 1.29f;
    c->f050 = 1000.0f;
    c->center.x = 0.0f;
    c->center.y = 0.0f;
    c->center.z = 0.0f;
    c->w[0].pos.x = 0.6012f;
    c->w[0].pos.y = 0.397094f;
    c->w[1].pos.x = -0.6012f;
    c->w[1].pos.y = 0.397094f;
    c->w[2].pos.x = 0.91224f;
    c->w[2].pos.y = 0.529524f;
    c->w[3].pos.x = -0.91224f;
    c->w[3].pos.y = 0.529524f;
    c->w[2].f00 = 1;
    c->w[3].f00 = 1;
    c->pts[0].x = 0.167f;
    c->pts[1].x = -0.167f;
    c->pts[1].y = 0.0f;
    c->pts[2].x = 0.2534f;
    c->pts[2].y = 0.0f;
    c->pts[0].y = 0.0f;
    c->f9ac = 1;
    c->pts[2].z = -0.3184f;
    c->pts[3].x = -0.2534f;
    c->pts[3].y = 0.0f;
    c->f9a8 = 0;
    c->fad0 = 0;
    c->f14c = 0.0f;
    c->pts[3].z = -0.3184f;
    c->f148 = 0.0f;
    c->f144 = 0.0f;
    c->f054 = 0.001f;
    mx = 0.0f;
    for (i = 0; i < 4; i++) {
        w = &c->w[i];
        w->f14 = w->pos.y;
        v.x = w->pos.x * 0.27777779f;
        v.y = w->pos.y * 0.27777779f;
        v.z = w->pos.z * 0.27777779f;
        l = v.x * v.x + v.y * v.y + v.z * v.z;
        if (l != DAT_005d757c) {
            l = FUN_004c3b30(l);
        }
        if (l > mx) {
            mx = l;
        }
    }
    c->f4a0 = mx * 1.05f;
    for (i = 0; i < 4; i++) {
        w = &c->w[i];
        w->len = FUN_004c3b30(w->pos.x * w->pos.x + w->pos.y * w->pos.y + w->pos.z * w->pos.z);
    }
    for (i = 0; i < 4; i++) {
        c->slot[i].f10 = FUN_004c3b30((c->pts[i].x - c->center.x) * (c->pts[i].x - c->center.x) +
                                      (c->pts[i].y - c->center.y) * (c->pts[i].y - c->center.y) +
                                      (c->pts[i].z - c->center.z) * (c->pts[i].z - c->center.z)) * 3.6f;
    }
    for (i = 4; i < 12; i++) {
        l = FUN_004c3b30((c->pts[i].x - c->center.x) * (c->pts[i].x - c->center.x) +
                         (c->pts[i].y - c->center.y) * (c->pts[i].y - c->center.y) +
                         (c->pts[i].z - c->center.z) * (c->pts[i].z - c->center.z));
        c->slot[i].f10 = l * 3.6f;
        if (l > mx) {
            mx = l;
        }
    }
    for (i = 12; i < 18; i++) {
        c->slot[i].f10 = FUN_004c3b30((c->pts[i].x - c->center.x) * (c->pts[i].x - c->center.x) +
                                      (c->pts[i].y - c->center.y) * (c->pts[i].y - c->center.y) +
                                      (c->pts[i].z - c->center.z) * (c->pts[i].z - c->center.z)) * 3.6f;
    }
    c->f4a4 = mx * 1.05f;
    s = c->w[2].len + c->w[3].len + c->w[1].len + c->w[0].len;
    q0 = s / c->w[0].len;
    q1 = s / c->w[1].len;
    q2 = s / c->w[2].len;
    q3 = s / c->w[3].len;
    k = c->f050 / (q3 + q2 + q1 + q0);
    l = q3 * k * c->w[3].len + q2 * k * c->w[2].len + q1 * k * c->w[1].len + q0 * k * c->w[0].len;
    c->f058 = l;
    c->f05c = DAT_005cc320 / l;
    return 1;
}

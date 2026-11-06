// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048a130 (h5): by hand + permuter. This file also holds its register-arg callee FUN_0048a060
   (static, defined before its caller, pilot finding 1: EAX = spark array, ESI = emitter record), 100% (regs 100%).
   0x0048a060: typed PartSpark (0x20 B, 512 per row) / PartEmit (0x38 B); `for (...; param_1++, i++)` (pointer step
   before the counter, 98.6% the other way); the z-before-y random draw order is the original's.
   0x0048a130: 93.0% (regs 93.0%). Typed globals; `s->m_age++` after the three position updates (84.1 -> 87.1%);
   the permuter's swap of the d.x / d.y lines gives 93.0% (natural x, y, z order 87.1%). Residual (b): the camera
   delta loads the frame position first (fld [frame+0x40]; fsubr [e]) for x and z in the original, ours loads the
   emitter first; the y position update orders gravity first; plugin-offset address operands [geo+off] vs ours
   [off+geo] (unchanged by swapping the source operands or using char* for the geometry). */
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
typedef struct PartSpark {
    int m_active;
    int m_age;
    PartV3 m_pos;
    PartV3 m_vel;
} PartSpark;
float FUN_00472650(float lo, float hi);
// MATCH: 0x0048a060 FUN_0048a060
static void FUN_0048a060(PartSpark *param_1, PartEmit *param_2)
{
    int i;
    float vx, vz;
    for (i = 0; i < 0x200; param_1++, i++) {
        if (param_1->m_active == 0) {
            param_1->m_active = 1;
            param_1->m_age = 0;
            vx = FUN_00472650(param_2->m_10.x, param_2->m_1c.x);
            vz = FUN_00472650(param_2->m_10.z, param_2->m_1c.z);
            param_1->m_vel.x = vx;
            param_1->m_vel.y = FUN_00472650(param_2->m_10.y, param_2->m_1c.y);
            param_1->m_vel.z = vz;
            param_1->m_pos.x = FUN_00472650(-param_2->m_0c, param_2->m_0c) + param_2->m_00.x;
            param_1->m_pos.y = FUN_00472650(-param_2->m_0c, param_2->m_0c) + param_2->m_00.y;
            param_1->m_pos.z = FUN_00472650(-param_2->m_0c, param_2->m_0c) + param_2->m_00.z;
            return;
        }
    }
}
typedef struct PartLockF { float *m_p; int m_stride; } PartLockF;
typedef struct PartLockB { unsigned char *m_p; int m_stride; } PartLockB;
typedef struct PartGeoExt { int m_00; int m_count; char m_08[0x38]; unsigned int m_flags; } PartGeoExt;
MGLOBALA(PartEmitRow, DAT_007059b8, 4, 0x007059b8);
MGLOBALA(int, DAT_007067b8, 4, 0x007067b8);
MGLOBAL(int, DAT_007067c8, 0x007067c8);
MGLOBALA(int, DAT_007067cc, 4, 0x007067cc);
MGLOBALA(void *, DAT_007067dc, 4, 0x007067dc);
MGLOBAL(float, DAT_007067f0, 0x007067f0);
MGLOBAL(float, DAT_007067f4, 0x007067f4);
MGLOBAL(float, DAT_007067f8, 0x007067f8);
MGLOBALA(PartSpark *, DAT_007067fc, 4, 0x007067fc);
MGLOBAL(float, DAT_005cc9cc, 0x005cc9cc);
MGLOBAL(int, DAT_007dc57c, 0x007dc57c);
int FUN_004c1b40(void *p0, void *p1);
float FUN_004c3ac0(PartV3 *p0);
int FUN_00487e20(void *p0, void *p1, int p2, int p3);
void FUN_00487e30(void *p0);
// MATCH: 0x0048a130 FUN_0048a130
void FUN_0048a130(int param_1, char *param_2, unsigned char *param_3, float param_4, float param_5)
{
    PartSpark *sparks;
    PartSpark *s;
    PartEmit *e;
    float *frame;
    PartV3 d;
    PartLockF pos;
    PartLockB col;
    PartLockF size;
    int i;
    int a;
    float f;

    sparks = DAT_007067fc[param_1];
    if (DAT_007067c8 != 0) {
        for (i = 0; i < DAT_007067b8[param_1]; i++) {
            e = &DAT_007059b8[param_1].m_e[i];
            frame = *(float **)(param_2 + 4);
            d.y = e->m_00.y - frame[0x11];
            d.x = e->m_00.x - frame[0x10];
            d.z = e->m_00.z - frame[0x12];
            if (FUN_004c3ac0(&d) < DAT_005cc9cc && FUN_004c1b40(param_2, &e->m_28) == 2)
                FUN_0048a060(sparks, e);
        }
    }
    DAT_007067c8++;
    DAT_007067cc[param_1] = 0;
    FUN_00487e20(DAT_007067dc[param_1], &pos, 1, 0x40000000);
    FUN_00487e20(DAT_007067dc[param_1], &col, 2, 0x40000000);
    FUN_00487e20(DAT_007067dc[param_1], &size, 4, 0x40000000);
    if (pos.m_p != 0 && col.m_p != 0 && size.m_p != 0) {
        s = sparks;
        for (i = 0x200; i != 0; i--, s++) {
            if (s->m_active == 1) {
                DAT_007067cc[param_1]++;
                s->m_pos.x = s->m_vel.x + s->m_pos.x + DAT_007067f0;
                s->m_pos.y = s->m_vel.y + s->m_pos.y + DAT_007067f4;
                s->m_pos.z = s->m_vel.z + s->m_pos.z + DAT_007067f8;
                s->m_age++;
                if (s->m_age > 0x80) {
                    s->m_active = 0;
                    s->m_age = 0;
                    DAT_007067cc[param_1]--;
                }
                pos.m_p[0] = s->m_pos.x;
                pos.m_p[1] = s->m_pos.y;
                pos.m_p[2] = s->m_pos.z;
                pos.m_p = (float *)((char *)pos.m_p + pos.m_stride);
                col.m_p[0] = param_3[0];
                col.m_p[1] = param_3[1];
                col.m_p[2] = param_3[2];
                if (s->m_age < 0x20)
                    a = s->m_age / 2;
                else
                    a = 0x40 - s->m_age / 2;
                if (a < 0)
                    a = 0;
                else if (a > 0xff)
                    a = 0xff;
                col.m_p[3] = (unsigned char)a;
                col.m_p += col.m_stride;
                f = (float)s->m_age * param_5 + param_4;
                size.m_p[0] = f;
                size.m_p[1] = f;
                size.m_p = (float *)((char *)size.m_p + size.m_stride);
            }
        }
        FUN_00487e30(DAT_007067dc[param_1]);
    }
    (*(PartGeoExt **)((char *)DAT_007067dc[param_1] + DAT_007dc57c))->m_flags |= 0x800000;
    (*(PartGeoExt **)((char *)DAT_007067dc[param_1] + DAT_007dc57c))->m_count = DAT_007067cc[param_1];
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH2: three neighbours in the particle code (0x0048a830..0x0048b960), one declaration block.
   0x0048aff0 (raw g2s 43.8%), 0x0048b230 (23.4%), 0x0048b440 (33.3%): BYTE-IDENTICAL. Literals read from the
   anchored image: 0.14f 0x005cf4a0, 1.6f 0x005cf49c, 0.7f 0x005cc340, 36.0f 0x005cda5c, 3.1415927f 0x005ce2f4,
   1.0f 0x005cc320, 0.75f 0x005cc950, 255.0f 0x005cd04c, 0.0f 0x005d757c.
   0x0048aff0: the free-slot walk is 0x0048ab70's scan with the byte inverted after the 0xff test (`not cl` at
   0x0048b03c) and the 128 exit going to the fallback (0x0048b008). After a hit the `jg` at 0x0048b0ab returns to
   the scan at 0x0048b003, so the walk sets the bit of every clear slot and keeps the last one: kept as in the
   binary (`while (i > 0)`). Fallback = max of +0x28 over the 128 slots written as one plain loop (cl unrolls it
   by 8, 0x0048b0c5..0x0048b17b). Moved it: the +0x2c store written before +0x30 (the binary stores +0x30 first)
   and the position as a struct copy: 13 B to 0.
   0x0048b230: byte-identical at the first compile. The 15-step loop counts down in edi, the compiler reuses the
   dead param slots 0x2c/0x30 for two temporaries and for the second loop's counter (fild at 0x0048b3c9).
   0x0048b440: same scan as 0x0048ab70, `sz` reset to 0.5f at the loop head (0x0048b450), sin as the fsin
   intrinsic (0x0048b506). Moved it: the product `t` declared double, so it stays on the x87 stack and the vel
   loads become `fld vel; fmul st(1)` (0x0048b52b, 0x0048b538): the only residue, then 0 B. A named float local
   for vel.x / vel.z gives the same bytes. The 0x0048b440 insn score is 96.8% only because the Ghidra body
   (483 B) stops before the epilogue; the 524 B whole symbol (with the switch table at 0x0048b62c) is identical. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3;
typedef struct { float x, y; } V2;
typedef struct { uchar r, g, b, a; } RGBA;
/* puff pool: 128 x 0x34 B at 0x007206f0, in-use bitmap 0x007200c0 (4 words) */
typedef struct {
    V3 pos;        /* +0x00 */
    V3 vel;        /* +0x0c */
    RGBA col;      /* +0x18 */
    float ang;     /* +0x1c */
    float f20;     /* +0x20 */
    float f24;     /* +0x24 */
    float age;     /* +0x28 */
    float f2c;     /* +0x2c */
    float f30;     /* +0x30 */
} Puff;
/* blast records: 8 x 0xa4 B at 0x007201d0, cursor 0x00722110, flags 0x007220f0 */
typedef struct {
    V3 pos;        /* +0x00 */
    float f0c;     /* +0x0c */
    float f10;     /* +0x10 */
    float f14;     /* +0x14 */
    int f18;       /* +0x18 */
    float f1c;     /* +0x1c */
    int f20;       /* +0x20 */
    int f24;       /* +0x24 */
    int f28;       /* +0x28 */
    int obj[10];   /* +0x2c */
    float t[10];   /* +0x54 */
    int n[10];     /* +0x7c */
} Blast;
MGLOBALA(uint, DAT_007200c0, 4, 0x007200c0);
MGLOBALA(Puff, DAT_007206f0, 128, 0x007206f0);
MGLOBAL(int, DAT_00722110, 0x00722110);
MGLOBALA(int, DAT_007220f0, 8, 0x007220f0);
MGLOBALA(Blast, DAT_007201d0, 8, 0x007201d0);
MGLOBAL(RGBA, DAT_0061450c, 0x0061450c);
MGLOBAL(RGBA, DAT_00614510, 0x00614510);
MGLOBAL(V3, DAT_006146fc, 0x006146fc);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBAL(int, DAT_00720080, 0x00720080);
double sin(double);
#pragma intrinsic(sin)
float FUN_00472650(float lo, float hi);
void FUN_00472740(RGBA *out, RGBA *a, RGBA *b, float t);
void FUN_00474de0(V3 *out, V3 *in, float s);
void FUN_00476320(V3 *out, V3 *in, float s);
void FUN_004769a0(V3 *pos);
void FUN_004769d0(V2 *size);
void FUN_004769f0(RGBA *col);
void FUN_00476a30(float rot);
void FUN_00476d00(int *batch);
void FUN_00476df0(int *batch);
int FUN_004c1520(int frame, V3 *axis, float angle, int op);
int FUN_004c1340(int frame, V3 *v, int op);
int FUN_00544bf0(int obj, int f);
void FUN_00465ca0(int id, V3 *pos);
void FUN_0048aff0(V3 *pos, RGBA *col, float a, float b, float c);

// MATCH: 0x0048aff0 FUN_0048aff0
void FUN_0048aff0(V3 *pos, RGBA *col, float a, float b, float c)
{
    float best = 0.0f;
    int i = 0;
    int idx = 0;
    Puff *p = 0;
    int j;
    do {
        uchar m;
        int k, w;
        for (;;) {
            for (;;) {
                if (i >= 128) goto full;
                w = i >> 5;
                if (DAT_007200c0[w] != 0xffffffff) break;
                i = (w + 1) * 32;
            }
            k = i >> 3;
            m = ((uchar *)DAT_007200c0)[k];
            if (m != 0xff) break;
        next:
            i = k * 8 + 8;
        }
        m = ~m;
        switch (i & 7) {
        case 0: i++; if (m & 1) break;
        case 1: i++; if (m & 2) break;
        case 2: i++; if (m & 4) break;
        case 3: i++; if (m & 8) break;
        case 4: i++; if (m & 0x10) break;
        case 5: i++; if (m & 0x20) break;
        case 6: i++; if (m & 0x40) break;
        case 7: i++; if (m & 0x80) break;
        default: goto next;
        }
        if (i < 0) break;
        p = &DAT_007206f0[i - 1];
        ((uchar *)DAT_007200c0)[(i - 1) >> 3] |= 1 << ((i - 1) & 7);
    } while (i > 0);
full:
    if (p == 0) {
        for (j = 0; j < 128; j++) {
            if (best < DAT_007206f0[j].age) {
                idx = j;
                best = DAT_007206f0[j].age;
            }
        }
        p = &DAT_007206f0[idx];
        ((uchar *)DAT_007200c0)[idx >> 3] |= 1 << (idx & 7);
    }
    p->age = 0;
    p->f24 = 0;
    p->ang = FUN_00472650(-6.2831855f, 6.2831855f);
    p->f20 = a;
    p->f2c = b;
    p->f30 = c;
    p->col = *col;
    FUN_00476320(&p->vel, &DAT_006146fc, 60.0f);
    p->pos = *pos;
}

// MATCH: 0x0048b230 FUN_0048b230
void FUN_0048b230(V3 *pos, int a, int b)
{
    int idx;
    Blast *r;
    int i;
    V3 p;
    RGBA col;
    float s0, s1, s2;

    idx = DAT_00722110;
    r = &DAT_007201d0[idx];
    DAT_007220f0[idx] = 1;
    DAT_00722110++;
    if (DAT_00722110 >= 8)
        DAT_00722110 = 0;
    r->pos = *pos;
    r->f0c = 0;
    r->f10 = 0;
    r->f20 = a;
    r->f14 = 0;
    r->f18 = b;
    r->f1c = 0;
    for (i = 0; i < 15; i++) {
        p = *pos;
        p.x = FUN_00472650(-0.05f, 0.05f) + p.x;
        p.z = FUN_00472650(-0.05f, 0.05f) + p.z;
        s0 = FUN_00472650(-0.1f, 0.2f) + 0.14f;
        s1 = FUN_00472650(-0.1f, 0.1f) + 1.6f;
        s2 = FUN_00472650(-0.1f, 0.3f) + 0.7f;
        FUN_00472740(&col, &DAT_0061450c, &DAT_00614510, FUN_00472650(0.0f, 0.2f));
        FUN_0048aff0(&p, &col, s0, s1, s2);
    }
    r->f1c = 0.3f;
    for (i = 0; i < 10; i++) {
        int frame;
        p.x = 0.0f;
        p.y = 0.0f;
        p.z = -0.2f;
        frame = *(int *)(r->obj[i] + 4);
        p.z = FUN_00472650(0.7f, 1.3f) * p.z;
        FUN_004c1520(frame, &DAT_006146fc, (float)i * 36.0f, 0);
        FUN_004c1340(frame, &p, 1);
        FUN_00544bf0(r->obj[i], 0);
        r->t[i] = FUN_00472650(0.7f, 1.7f);
        r->n[i] = 0;
    }
    FUN_00465ca0(0x1d, pos);
}

// MATCH: 0x0048b440 FUN_0048b440
void FUN_0048b440(void)
{
    int i = 0;
    do {
        Puff *p;
        uchar b;
        int k, w;
        int n;
        RGBA col;
        V2 sz;
        V3 q;
        float s, age; double t;
        sz.x = 0.5f;
        sz.y = 0.5f;
        for (;;) {
            for (;;) {
                if (i >= 128) goto done;
                w = i >> 5;
                if (DAT_007200c0[w] != 0) break;
                i = (w + 1) * 32;
            }
            k = i >> 3;
            b = ((uchar *)DAT_007200c0)[k];
            if (b != 0) break;
        next:
            i = k * 8 + 8;
        }
        switch (i & 7) {
        case 0: i++; if (b & 1) break;
        case 1: i++; if (b & 2) break;
        case 2: i++; if (b & 4) break;
        case 3: i++; if (b & 8) break;
        case 4: i++; if (b & 0x10) break;
        case 5: i++; if (b & 0x20) break;
        case 6: i++; if (b & 0x40) break;
        case 7: i++; if (b & 0x80) break;
        default: goto next;
        }
        if (i < 0) break;
        n = i - 1;
        p = &DAT_007206f0[n];
        col = p->col;
        s = (float)sin(p->f24 * 3.1415927f);
        q = p->pos;
        age = DAT_007f100c + p->age;
        p->age = age;
        p->f24 = age;
        t = age * p->f2c;
        q.x = p->vel.x * t + q.x;
        q.z = p->vel.z * t + q.z;
        q.y = s * p->f30 + p->pos.y;
        if (age > 1.0f) {
            ((uchar *)DAT_007200c0)[n >> 3] &= ~(1 << (n & 7));
            i++;
        } else {
            float r;
            { float h = s * p->f20; sz.y = h + h + p->f20; }
            sz.x = sz.y * 0.75f;
            col.a = (uchar)(255 - (int)(age * age * age * 255.0f));
            FUN_00474de0(&q, &q, 0.05f);
            r = p->ang * p->f24;
            FUN_004769a0(&q);
            FUN_004769f0(&col);
            FUN_004769d0(&sz);
            FUN_00476a30(r);
            FUN_00476d00(&DAT_00720080);
        }
    } while (i > 0);
done:
    FUN_00476df0(&DAT_00720080);
}

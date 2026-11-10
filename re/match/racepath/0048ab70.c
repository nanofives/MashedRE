// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH 0x0048ab70 (first hand source; raw g2s 31.3%): NOT CLOSED (FRESH 93.2% insn, 41 B differ; FRESH2 91.2% insn, 34 B differ)
   (468 B whole symbol). The bitmap walk matches with Ghidra's own shape: two nested loops (word skip,
   byte load) with the not-found step `next: i = k*8+8` at the bottom of the inner loop and the 8-way
   switch after it, `break` = found, `default: goto next` (0x0048ab83..0x0048abf6). The template form of
   re/match/s15/0048d360.c put the not-found block after the epilogue here. The kill path also advances i
   (0x0048ac4a inc edi): kept as in the binary. The vertex buffer is indexed (n2) so the pointer is
   strength-reduced; the second field written becomes the base, so a.z is written second to get
   EBP = 0x0071ba38. Residual: d.y operand order (fld scale; fmul vel.y at 0x0048ac5c), the a.y store
   placed after a.z, and the pos.x / pos.y fadd operand order. Tried: swaps, comma rvalues, dblgreedy
   (220 variants), 30 store orders. 
   MATCH-RACEPATH-FRESH2 (34 B): a.x, a.y, a.z written in natural order and d.y as a double product
   `(float)((double)scale * vel.y)`: the d.y load order (fld scale; fmul vel.y at 0x0048ac5c) and the a.y slot now
   match. Residual: the pointer base is EBP = &a.y, the original's is &a.z (0x0071ba38), so every vertex store
   displacement is off by 4, plus the col.g / col.b byte stores scheduled before the a.x fstp (the original puts
   them after it) and the pos.x fadd order (fld vel.x; fadd pos.x at 0x0048accd). Tried: 9 orders of the 10 vertex
   stores with a.x/a.y/a.z in order (none gives base &a.z), d.y as W/V/F/comma, pos.x as E/F/comma/`+=`, d as a
   double struct. */
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
typedef struct { uchar r, g, b, a; } RGBA;
typedef struct {
    V3 pos;        /* +0x00 */
    V3 vel;        /* +0x0c */
    RGBA col;      /* +0x18 */
    float scale;   /* +0x1c */
    float age;     /* +0x20 */
    float rate;    /* +0x24 */
    V3 acc;        /* +0x28 */
} Spark;           /* 0x34 B, pool at 0x00715230 (512) */
typedef struct {
    V3 a;          /* +0x00 */
    V3 b;          /* +0x0c */
    int pad18;     /* +0x18 */
    RGBA col;      /* +0x1c */
} SparkVert;       /* 0x20 B, array at 0x0071ba30 */
MGLOBAL(int, DAT_0071fa30, 0x0071fa30);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(uint, DAT_007151f0, 16, 0x007151f0);
MGLOBALA(Spark, DAT_00715230, 512, 0x00715230);
MGLOBALA(SparkVert, DAT_0071ba30, 512, 0x0071ba30);

// MATCH: 0x0048ab70 FUN_0048ab70
void FUN_0048ab70(void)
{
    int i = 0;
    int n2 = 0;
    DAT_0071fa30 = 0;
    do {
        Spark *p;
        uchar b;
        int k, w;
        float t;
        V3 d;
        int a;
        int n;
        for (;;) {
            for (;;) {
                if (i >= 512) return;
                w = i >> 5;
                if (DAT_007151f0[w] != 0) break;
                i = (w + 1) * 32;
            }
            k = i >> 3;
            b = ((uchar *)DAT_007151f0)[k];
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
found:
        if (i < 0) return;
        n = i - 1;
        p = &DAT_00715230[n];
        p->age = DAT_007f100c + p->age;
        t = p->age * p->rate;
        if (t > 1.0f) {
            ((uchar *)DAT_007151f0)[n >> 3] &= ~(1 << (n & 7));
            i++;
        } else {
            d.x = p->vel.x * p->scale;
            d.y = (float)((double)p->scale * p->vel.y);
            d.z = p->vel.z * p->scale;
            a = p->col.a;
            DAT_0071ba30[n2].a.x = d.x + p->pos.x;
            DAT_0071ba30[n2].a.y = d.y + p->pos.y;
            DAT_0071ba30[n2].a.z = d.z + p->pos.z;
            DAT_0071ba30[n2].col.r = p->col.r;
            DAT_0071ba30[n2].col.g = p->col.g;
            DAT_0071ba30[n2].col.b = p->col.b;
            DAT_0071ba30[n2].b.x = d.x * -1.0f;
            DAT_0071ba30[n2].b.y = d.y * -1.0f;
            DAT_0071ba30[n2].b.z = d.z * -1.0f;
            DAT_0071ba30[n2].col.a = (uchar)((1.0f - t) * a);
            DAT_0071fa30++;
            n2++;
            p->pos.x = p->vel.x + p->pos.x;
            p->pos.y = (0, p->pos.y) + p->vel.y;
            p->pos.z = p->vel.z + p->pos.z;
            p->vel.x = p->acc.x + p->vel.x;
            p->vel.y = p->acc.y + p->vel.y;
            p->vel.z = p->acc.z + p->vel.z;
        }
    } while (i >= 0);
}

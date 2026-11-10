// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH3 0x00489500 (first hand source; raw g2s 7.1%): 48.8%, 734 B differ, NOT CLOSED.
   Literals read from the anchored image (1.0/3.0/4.0/5.0/0.5). The Catmull-Rom component expressions
   compile to the original x87 sequence (0x00489664..0x00489784; cl reorders the tree, so the source order
   of the terms does not matter). The vertex is one 0x18 B local {id, a, b, pos} (the original's
   [esp+0x38..0x4f] are contiguous). Residual: the original has an EBP frame with `and esp, -8`
   (0x00489500..0x00489506) and keeps p3 in [esp+0x1c] / ECX; this source compiles without the aligned
   frame, so EBP is a fifth pointer register and every stack operand differs. What makes cl 13 align
   this frame is not found (no double local in the body). Worked last, after the yield rule's sixth row. */
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
typedef struct {
    int id;         /* +0x00 */
    V3 p;           /* +0x04 */
    float a;        /* +0x10 */
    float b;        /* +0x14 */
    float col;      /* +0x18 */
    int brk;        /* +0x1c */
    float uv[4];    /* +0x20 */
} Node;                                       /* 0x30 B */
typedef struct {
    Node *nodes;    /* +0x00 */
    int pad4;
    int count;      /* +0x08 */
    int passes;     /* +0x0c */
    char tex[0x40]; /* +0x10 */
    unsigned char flags; /* +0x50 */
    char pad51[0x5c - 0x51];
    float step;     /* +0x5c */
    float t;        /* +0x60 */
    float speed;    /* +0x64 */
} Strip;
typedef struct { int id; float a; float b; V3 pos; } CVert;   /* 0x18 B */
void FUN_004769a0(void *p);
void FUN_004769d0(void *p);
void FUN_004769f0(void *p);
void FUN_00476a30(float f);
void FUN_00476a40(float *uv);
void FUN_00476d00(void *p);

// MATCH: 0x00489500 _FUN_00489500
void FUN_00489500(Strip *s)
{
    float step;
    float t0;
    float u;
    float u2;
    float u3;
    int cur;
    int nxt;
    int pass;
    int last;
    int segs;
    int k;
    int j;
    int idx;
    V3 *p0;
    V3 *p1;
    V3 *p2;
    V3 *p3;
    Node *n;
    CVert cv;

    step = s->step;
    t0 = s->t;
    s->t = s->speed + s->t;
    cur = 0;
    pass = 0;
    nxt = 0;
    if (s->speed + t0 > step)
        s->t = 0;
    for (pass = 0; pass < s->passes; pass++) {
        last = s->count - 1;
        segs = s->count - 2;
        k = 0;
        if (s->count >= 2 && segs < 1)
            segs = 1;
        else if (segs <= 0)
            continue;
        do {
            for (j = cur; j < s->count; j++) {
                if (s->nodes[j].brk != 0) {
                    last = j;
                    break;
                }
            }
            idx = k + cur;
            if (s->nodes[idx].brk != 0) {
                nxt = idx + 1;
                k = 0;
            } else {
                segs = last - cur;
                p0 = &s->nodes[idx - 1].p;
                p1 = &s->nodes[idx].p;
                p2 = &s->nodes[idx + 1].p;
                p3 = &s->nodes[idx + 2].p;
                if (k == 0)
                    p0 = p1 = &s->nodes[nxt].p;
                if (k == segs - 1)
                    p3 = p2 = &s->nodes[last].p;
                if (idx == last - 1)
                    nxt = last + 1;
                u = t0;
                if (t0 < 1.0f) {
                    do {
                        u2 = u * u;
                        u3 = u * u2;
                        cv.pos.x = ((((p0->x + p0->x) - p1->x * 5.0f) + p2->x * 4.0f - p3->x) * u2
                                 + (((p1->x * 3.0f - p0->x) - p2->x * 3.0f) + p3->x) * u3
                                 + (p2->x - p0->x) * u + (p1->x + p1->x)) * 0.5f;
                        cv.pos.y = ((((p0->y + p0->y) - p1->y * 5.0f) + p2->y * 4.0f - p3->y) * u2
                                 + (((p1->y * 3.0f - p0->y) - p2->y * 3.0f) + p3->y) * u3
                                 + (p2->y - p0->y) * u + (p1->y + p1->y)) * 0.5f;
                        n = &s->nodes[idx];
                        cv.pos.z = ((((p0->z + p0->z) - p1->z * 5.0f) + p2->z * 4.0f - p3->z) * u2
                                 + (((p1->z * 3.0f - p0->z) - p2->z * 3.0f) + p3->z) * u3
                                 + (p2->z - p0->z) * u + (p1->z + p1->z)) * 0.5f;
                        cv.id = n->id;
                        cv.a = n->a - (n->a - n[1].a) * u;
                        cv.b = n->b - (n->b - n[1].b) * u;
                        FUN_004769a0(&cv.pos);
                        if (s->flags & 2)
                            FUN_004769d0(&cv.a);
                        if (s->flags & 0x20)
                            FUN_00476a40(s->nodes[idx].uv);
                        if (s->flags & 0x10)
                            FUN_00476a30(s->nodes[idx].col);
                        if (s->flags & 4)
                            FUN_004769f0(&cv);
                        FUN_00476d00(s->tex);
                        u = step + u;
                    } while (u < 1.0f);
                }
                k++;
            }
            cur = nxt;
        } while (k < segs);
    }
}

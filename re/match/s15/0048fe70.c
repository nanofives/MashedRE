// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048fe70 (h3): 100% (regs 100%), relocheck OK. Raw 22.7%. What moved it: one local
   struct {result; intersection} with the 3-dword position copied as a struct, the intersection record
   (center, radius 0.1f at +0x0c, type 3 at +0x18) passed as &q.is and &q as the callback data.
   FUN_0048fe30 is the 5-arg cdecl callback (extern). No residual. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct V3_48fe { float x, y, z; } V3_48fe;
typedef struct Isect_48fe {
    V3_48fe center;    /* +0x00 */
    float radius;      /* +0x0c */
    float pad[2];      /* +0x10 (union with a 24-byte line) */
    int type;          /* +0x18 */
} Isect_48fe;
typedef struct Query_48fe {
    int result;        /* +0x00 */
    Isect_48fe is;     /* +0x04 */
} Query_48fe;
MGLOBAL(int, DAT_0080332c, 0x0080332c);
void *FUN_0048fe30(void *a, void *b, void *c, float d, void *e);
int FUN_00538c80(int world, Isect_48fe *is, void *(*cb)(void *, void *, void *, float, void *), void *data);
// MATCH: 0x0048fe70 FUN_0048fe70
int FUN_0048fe70(V3_48fe *param_1)
{
    Query_48fe q;
    q.is.center = *param_1;
    q.result = 0;
    q.is.type = 3;
    q.is.radius = 0.1f;
    FUN_00538c80(DAT_0080332c, &q.is, FUN_0048fe30, &q);
    return q.result;
}

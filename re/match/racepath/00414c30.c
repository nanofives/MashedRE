// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH3 0x00414c30 (first hand source; raw g2s 3.4%): 96.5%, 50 B differ, NOT CLOSED.
   Moved it: the world-object array indexed o[i] (a pointer walk o++ keeps EBX = base, the original biases
   it to +0xc); the sub pointer read once into a local in the type-10 test (with three o[i].sub reads cl
   picks +0x14 as the IV base, with one it picks +0xc = the original's); `!(s->f10 < 0.4f)` for the
   lifetime gate (fld field; fcomp 0.4; jnp at 0x00414d9f); `type == 7` tested first (jne 0x00414ead);
   `h > -0.75f` (fcom; test ah,0x41 at 0x00414e8b); own[] / obj[] declared at loop-body scope (function
   scope keeps the stores in source order, loop scope lets cl schedule them as the original does).
   Residual: two store-scheduling spots, the own[0] = c->x store and the pos.z load placed one slot early
   at 0x00414d05..0x00414d39 and 0x00414e0c..0x00414e28. Tried: all 24 store orders in each block, V2
   struct / int typed copies, block scope per branch (frame grows 0x10), comma rvalues, double temps,
   dblgreedy (153 variants, no gain). */
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
typedef struct { float v[4]; } V4;
typedef struct { float v[6]; } Box6;
typedef struct { char pad[0x10]; float f10; } Sub2;
typedef struct { char pad[4]; Sub2 *p4; char pad2[0x60]; Box6 box; } Sub;   /* box at +0x68 */
typedef struct { V3 pos; float radius; int type; Sub *sub; char pad[0x8c - 0x18]; } WObj; /* 0x8c B */
typedef struct { char pad[0x30]; float x, y, z; } CarPos;
int FUN_0046d4a0(CarPos **out, int car);
WObj *FUN_00484c70(int *count);
void FUN_0041f030(int car, V4 *out);
int FUN_0048a630(Box6 *box, V4 *v);
float FUN_004c3ac0(V3 *v);
int FUN_00414300(int car, float *own, float *tgt, float *obj, float r, float *out);
int FUN_00414490(int car, float *own, float *tgt, float *obj, float r, float *out);
float FUN_00442cc0(int car);

// MATCH: 0x00414c30 _FUN_00414c30
int FUN_00414c30(int p1, float *tgt, float *out, int car)
{
    CarPos *c;
    WObj *o;
    int count;
    int i;
    float r;
    V3 me;
    V3 d;
    V4 v;
    Box6 b;
    float t;
    float len;
    float h;
    Sub2 *s;
    Sub *sb2;

    FUN_0046d4a0(&c, car);
    me.x = c->x;
    me.y = c->y;
    me.z = c->z;
    o = FUN_00484c70(&count);
    for (i = 0; i < count; i++) {
        float own[2];
        float obj[2];
        if (o[i].type >= 0xff00)
            continue;
        if (o[i].type == 0x15) {
            Sub *sb = o[i].sub;
            FUN_0041f030(car, &v);
            b = sb->box;
            t = (b.v[2] - b.v[5]) * 0.875f;
            b.v[2] = b.v[2] + t;
            b.v[5] = b.v[5] - t;
            if (FUN_0048a630(&b, &v) == 1) {
                own[0] = c->x;
                own[1] = c->z;
                obj[0] = o[i].pos.x;
                obj[1] = o[i].pos.z;
                if (FUN_00414300(car, own, tgt, obj, o[i].radius * 0.33333334f + 0.5f, out) != 0)
                    return 1;
            }
        } else if (o[i].type != 10 || (sb2 = o[i].sub) == 0 || (s = sb2->p4) == 0 || !(s->f10 < 0.4f)) {
            r = o[i].radius;
            if (o[i].type == 0xd)
                r = r + r;
            d.x = me.x - o[i].pos.x;
            d.y = me.y - o[i].pos.y;
            d.z = me.z - o[i].pos.z;
            len = FUN_004c3ac0(&d);
            if (d.y < r && len < 8.0f) {
                own[0] = c->x;
                own[1] = c->z;
                obj[0] = o[i].pos.x;
                obj[1] = o[i].pos.z;
                if (o[i].type == 7) {
                    if (FUN_00442cc0(car) == 0.0f &&
                        FUN_00414490(car, own, tgt, obj, r + 0.5f, out) != 0) {
                        h = c->y - o[i].pos.y;
                        if (h > -0.75f && h < 0.75f)
                            return 2;
                    }
                } else if (FUN_00414300(car, own, tgt, obj, r + 0.5f, out) != 0)
                    return 1;
            }
        }
    }
    return 0;
}

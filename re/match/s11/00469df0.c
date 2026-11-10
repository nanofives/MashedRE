// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00469df0 (h2): 30.1% (regs 33.3%, ops 70.7%), first structural pass of a 5060 B function.
   Raw converter did not compile (C4234 __thiscall). Two car records (the 0xd04 record of 0x008815a0) come in
   EAX (a) and ECX (b), plus two stack args (cdecl, the caller pops 8): static callee plus a stand-in caller
   (built only outside MATCH_PORT; real caller 0x004709a0, call at 0x00470bcd). Shape: 2-pass do/while over
   both cars testing the other car's 4 hull points against 4 edge planes, mean contact point, two
   normalized offsets, the impulse pair, velocity/spin updates, two inverse-matrix transforms, then the
   event-queue record (count at 0x007e9de0 + type/time/who/hdr/ev arrays as one struct). The 64-byte
   inverse-matrix local is a float struct (a byte array adds the /GS cookie). Hull corners copied as V3
   structs (int moves, as in the original): 28.4 -> 30.1%.
   Residual (b): ran out of budget. The args land in EDI/ESI (the original takes EAX/ECX and moves them
   there; parameter order does not change it), the frame is 0x15c not 0x164, and the x87 code of the plane
   tests and the impulse block is scheduled differently. Constants are MGLOBALs; the literal-vs-global
   pass (worth several % on other functions in this slice) and the permuter were not run. */
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
typedef struct { float x, y, z; } V3_00469df0;
typedef struct { float m[12]; V3_00469df0 pos; float pw; } Mat_00469df0;
typedef struct {
    int f000;
    undefined1 pad004[0xc];
    int f010;
    undefined1 pad014[0x130];
    V3_00469df0 spin;               /* +0x144 */
    undefined1 pad150[0x928 - 0x150];
    Mat_00469df0 mat[2];            /* +0x928 */
    int f9a8;                       /* +0x9a8 */
    int f9ac;                       /* +0x9ac */
    V3_00469df0 vel;                /* +0x9b0 */
    float f9bc;
    float f9c0;
    float f9c4;
    V3_00469df0 n;                  /* +0x9c8 */
    undefined1 pad9d4[0xa28 - 0x9d4];
    V3_00469df0 q[4];               /* +0xa28 */
    undefined1 pada58[0xaec - 0xa58];
    int faec;                       /* +0xaec */
    undefined1 padaf0[0xd04 - 0xaf0];
} Car_00469df0;
typedef struct { int f00; int f04; int f08; float f0c; float f10; float f14; float f18; float f1c; } EvHdr_00469df0;
typedef struct { float v[16]; } Ev_00469df0;
typedef struct {
    uint count;
    int type[256];
    int time[256];
    int who[256];
    EvHdr_00469df0 hdr[256];
    Ev_00469df0 ev[256];
} EvQ_00469df0;
MGLOBAL(EvQ_00469df0, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBAL(int, DAT_00803320, 0x00803320);
MGLOBAL(float, DAT_005cc568, 0x005cc568);
MGLOBAL(float, DAT_005ce194, 0x005ce194);
int FUN_0040e350(void);
float FUN_004c39b0(V3_00469df0 *out, const V3_00469df0 *in);
float FUN_004c3ac0(const V3_00469df0 *v);
void *FUN_004c3df0(V3_00469df0 *out, const V3_00469df0 *in, int n, const void *m);
void *FUN_004c4dc0(void *out, const Mat_00469df0 *m);

static __inline V3_00469df0 Sub_00469df0(const V3_00469df0 *a, const V3_00469df0 *p)
{
    V3_00469df0 r;
    r.x = a->x - p->x;
    r.y = a->y - p->y;
    r.z = a->z - p->z;
    return r;
}
static __inline float Dot_00469df0(const V3_00469df0 *n, V3_00469df0 d)
{
    return n->z * d.z + d.y * n->y + d.x * n->x;
}

// MATCH: 0x00469df0 FUN_00469df0
static int FUN_00469df0(Car_00469df0 *b, int p2, int p3, Car_00469df0 *a)
{
    V3_00469df0 *pa, *pb, *va, *vb;
    Car_00469df0 *c, *o;
    uint k, cnt;
    int side[2];
    float sx, sy, sz, inv;
    V3_00469df0 P[4];
    V3_00469df0 n3;
    float nx, ny, nz, ex, ey, ez;
    V3_00469df0 n0, n1, n2, nn;
    V3_00469df0 da, db, ra, ia, ib, ta, tb;
    struct { V3_00469df0 v; float e[2]; float ev[16]; } rb;
    float la, lb, dot, f, sum, imp, h;
    int wa, wb, st, diff;
    Mat_00469df0 inv_m;

    pa = &a->mat[a->f9a8].pos;
    k = 0;
    pb = &b->mat[b->f9a8].pos;
    cnt = 0;
    side[1] = 0;
    side[0] = 0;
    sz = 0.0f;
    sy = 0.0f;
    sx = 0.0f;
    do {
        c = a;
        if (k != 0) c = b;
        nx = c->n.x;
        ny = c->n.y;
        nz = c->n.z;
        P[0].x = c->q[0].x;
        P[0].y = c->q[0].y;
        P[0].z = c->q[0].z;
        P[1].x = c->q[1].x;
        P[1].y = c->q[1].y;
        P[1].z = c->q[1].z;
        P[3].x = c->q[3].x;
        P[3].y = c->q[3].y;
        P[3].z = c->q[3].z;
        P[2].x = c->q[2].x;
        P[2].y = c->q[2].y;
        P[2].z = c->q[2].z;
        ex = P[1].x - P[0].x;
        ey = P[1].y - P[0].y;
        ez = P[1].z - P[0].z;
        n0.x = ez * ny - ey * nz;
        n0.y = ex * nz - ez * nx;
        n0.z = ey * nx - ex * ny;
        ex = P[3].x - P[1].x;
        ey = P[3].y - P[1].y;
        ez = P[3].z - P[1].z;
        n1.x = ez * ny - ey * nz;
        n1.y = ex * nz - ez * nx;
        n1.z = ey * nx - ex * ny;
        ex = P[2].x - P[3].x;
        ey = P[2].y - P[3].y;
        ez = P[2].z - P[3].z;
        n2.x = ez * ny - ey * nz;
        n2.y = ex * nz - ez * nx;
        n2.z = ey * nx - ex * ny;
        ex = P[0].x - P[2].x;
        ey = P[0].y - P[2].y;
        ez = P[0].z - P[2].z;
        n3.x = ez * ny - ey * nz;
        n3.y = ex * nz - ez * nx;
        n3.z = ey * nx - ex * ny;
        o = b;
        if (k != 0) o = a;
        da.x = o->q[0].x;
        da.y = o->q[0].y;
        da.z = o->q[0].z;
        if (Dot_00469df0(&n0, Sub_00469df0(&da, &P[0])) < 0.0f &&
            Dot_00469df0(&n1, Sub_00469df0(&da, &P[1])) < 0.0f &&
            Dot_00469df0(&n2, Sub_00469df0(&da, &P[3])) < 0.0f &&
            Dot_00469df0(&n3, Sub_00469df0(&da, &P[2])) < 0.0f) {
            sx = sx + da.x;
            cnt++;
            sy = da.y + sy;
            side[k] = side[k] + 1;
            sz = da.z + sz;
        }
        da.x = o->q[1].x;
        da.y = o->q[1].y;
        da.z = o->q[1].z;
        if (Dot_00469df0(&n0, Sub_00469df0(&da, &P[0])) < 0.0f &&
            Dot_00469df0(&n1, Sub_00469df0(&da, &P[1])) < 0.0f &&
            Dot_00469df0(&n2, Sub_00469df0(&da, &P[3])) < 0.0f &&
            Dot_00469df0(&n3, Sub_00469df0(&da, &P[2])) < 0.0f) {
            sx = sx + da.x;
            cnt++;
            sy = da.y + sy;
            side[k] = side[k] + 1;
            sz = da.z + sz;
        }
        da.x = o->q[2].x;
        da.y = o->q[2].y;
        da.z = o->q[2].z;
        if (Dot_00469df0(&n0, Sub_00469df0(&da, &P[0])) < 0.0f &&
            Dot_00469df0(&n1, Sub_00469df0(&da, &P[1])) < 0.0f &&
            Dot_00469df0(&n2, Sub_00469df0(&da, &P[3])) < 0.0f &&
            Dot_00469df0(&n3, Sub_00469df0(&da, &P[2])) < 0.0f) {
            sx = sx + da.x;
            cnt++;
            sy = da.y + sy;
            sz = da.z + sz;
        }
        da.x = o->q[3].x;
        da.y = o->q[3].y;
        da.z = o->q[3].z;
        if (Dot_00469df0(&n0, Sub_00469df0(&da, &P[0])) < 0.0f &&
            Dot_00469df0(&n1, Sub_00469df0(&da, &P[1])) < 0.0f &&
            Dot_00469df0(&n2, Sub_00469df0(&da, &P[3])) < 0.0f &&
            Dot_00469df0(&n3, Sub_00469df0(&da, &P[2])) < 0.0f) {
            sx = sx + da.x;
            cnt++;
            sy = da.y + sy;
            sz = da.z + sz;
        }
        k++;
    } while (k < 2);
    if (cnt != 0) {
    inv = 1.0f / (float)cnt;
    n3.x = inv * sx;
    n3.y = inv * sy;
    n3.z = inv * sz;
    da.x = n3.x - pb->x;
    da.y = n3.y - pb->y;
    da.z = n3.z - pb->z;
    ra = da;
    FUN_004c39b0(&da, &da);
    db.x = n3.x - pa->x;
    db.y = n3.y - pa->y;
    db.z = n3.z - pa->z;
    rb.v = db;
    FUN_004c39b0(&db, &db);
    vb = &b->vel;
    va = &a->vel;
    ta.x = b->spin.x * 600.0f + b->vel.x;
    ib.z = 0.0f;
    ib.y = 0.0f;
    ib.x = 0.0f;
    ia.z = 0.0f;
    ia.y = 0.0f;
    ia.x = 0.0f;
    ta.y = b->spin.y * 600.0f + b->vel.y;
    ta.z = b->spin.z * 600.0f + b->vel.z;
    tb.x = a->spin.x * 600.0f + a->vel.x;
    tb.y = a->spin.y * 600.0f + a->vel.y;
    tb.z = a->spin.z * 600.0f + a->vel.z;
    la = FUN_004c3ac0(&ta);
    lb = FUN_004c3ac0(&tb);
    dot = tb.z * ta.z + tb.x * ta.x + tb.y * ta.y;
    if (la != 0.0f) {
        f = dot * (1.0f / la);
        if (lb != 0.0f) f = f / lb;
        f = -((1.0f / la) * lb * f);
        if (f > 0.0f) f = 0.0f;
        else if (f < -1.0f) f = -1.0f;
        f = f + 1.0f;
        n2.y = f * ta.y;
        n2.z = f * ta.z;
        ib.x = ta.x * f;
        ib.y = f * ta.y;
        ib.z = f * ta.z;
        ia.x = -(ta.x * f);
        ia.y = -n2.y;
        ia.z = -n2.z;
    }
    if (lb != 0.0f) {
        f = dot * (1.0f / lb);
        if (la != 0.0f) f = f / la;
        f = -((1.0f / lb) * la * f);
        if (f > 0.0f) f = 0.0f;
        else if (f < -1.0f) f = -1.0f;
        f = f + 1.0f;
        n2.y = f * tb.y;
        n2.z = f * tb.z;
        ia.x = ia.x + tb.x * f;
        ia.y = ia.y + n2.y;
        ia.z = ia.z + n2.z;
        ib.x = ib.x - tb.x * f;
        ib.y = ib.y - n2.y;
        ib.z = ib.z - n2.z;
    }
    vb->x = ia.x * 0.5f + vb->x;
    b->vel.y = ia.y * 0.5f + b->vel.y;
    b->vel.z = ia.z * 0.5f + b->vel.z;
    va->x = ib.x * 0.5f + va->x;
    a->vel.y = ib.y * 0.5f + a->vel.y;
    a->vel.z = ib.z * 0.5f + a->vel.z;
    h = FUN_004c3ac0(&ia);
    sum = FUN_004c3ac0(&ib) + h;
    if (DAT_00803320 < 3) h = DAT_005cc568;
    if (DAT_00803320 == 3 || DAT_00803320 == 4) h = DAT_005ce194;
    imp = sum * 2e-07f;
    if (1.0f < sum * 2e-07f) imp = 1.0f;
    imp = imp * h;
    if (imp < DAT_005ce194) imp = 200.0f;
    FUN_004c39b0(&n0, vb);
    FUN_004c39b0(&n1, va);
    n0.x = n0.x - da.x;
    n0.y = n0.y - da.y;
    n0.z = n0.z - da.z;
    n1.x = n1.x - db.x;
    n1.y = n1.y - db.y;
    n1.z = n1.z - db.z;
    FUN_004c39b0(&n0, &n0);
    FUN_004c39b0(&n1, &n1);
    f = imp * -0.5f;
    vb->x = vb->x - n0.x * f;
    b->vel.y = b->vel.y - n0.y * f;
    b->vel.z = b->vel.z - n0.z * f;
    va->x = va->x - n1.x * f;
    a->vel.y = a->vel.y - n1.y * f;
    a->vel.z = a->vel.z - f * n1.z;
    vb->x = vb->x - da.x * imp;
    b->vel.y = b->vel.y - da.y * imp;
    b->vel.z = b->vel.z - da.z * imp;
    va->x = va->x - db.x * imp;
    a->vel.y = a->vel.y - db.y * imp;
    a->vel.z = a->vel.z - db.z * imp;
    if (b->faec == 0) b->faec = 0x1e0;
    if (a->faec == 0) a->faec = 0x1e0;
    FUN_004c4dc0(&inv_m, &b->mat[b->f9ac]);
    FUN_004c3df0(&da, &ra, 1, &inv_m);
    FUN_004c4dc0(&inv_m, &a->mat[a->f9ac]);
    FUN_004c3df0(&db, &rb.v, 1, &inv_m);
    f = imp;
    if (da.x < 0.0f) {
        if (da.z < 0.0f) f = -imp;
    } else {
        if (da.z > 0.0f) f = -imp;
    }
    if (b->f010 == 0) {
        b->spin.y = f * 0.0005f + b->spin.y;
    } else {
        f = f * 0.01f;
        b->f9bc = 0.0f;
        b->f9c4 = 0.0f;
        b->f9c0 = f;
    }
    if (db.x < 0.0f) {
        if (db.z < 0.0f) imp = -imp;
    } else {
        if (db.z > 0.0f) imp = -imp;
    }
    if (a->f010 == 0) {
        a->spin.y = imp * 0.0005f + a->spin.y;
    } else {
        imp = imp * 0.01f;
        a->f9bc = 0.0f;
        a->f9c4 = 0.0f;
        a->f9c0 = imp;
    }
    rb.ev[0] = n3.x;
    rb.ev[1] = n3.y;
    rb.ev[2] = n3.z;
    diff = side[0] != side[1];
    if (side[0] >= side[1]) {
        wa = b->f000;
        wb = a->f000;
        rb.ev[3] = da.x; rb.ev[4] = da.y; rb.ev[5] = da.z;
        rb.ev[9] = ia.x; rb.ev[10] = ia.y; rb.ev[11] = ia.z;
        rb.ev[6] = db.x; rb.ev[7] = db.y; rb.ev[8] = db.z;
        rb.ev[12] = ib.x; rb.ev[13] = ib.y; rb.ev[14] = ib.z;
        st = FUN_0040e350();
        if (st == 1 || st == 2 || st == 3 || st == 4 || st == 5) return p3 == 0;
        DAT_007e9de0.type[DAT_007e9de0.count] = 4;
        DAT_007e9de0.time[DAT_007e9de0.count] = DAT_007f1030;
        DAT_007e9de0.who[DAT_007e9de0.count] = b->f000 + 0xff00;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f00 = wa;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f04 = wb;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f08 = diff;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f0c = rb.v.x;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f10 = rb.v.y;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f14 = rb.v.z;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f18 = rb.e[0];
        DAT_007e9de0.hdr[DAT_007e9de0.count].f1c = rb.e[1];
        for (k = 0; k < 16; k++) {
            DAT_007e9de0.ev[DAT_007e9de0.count].v[k] = rb.ev[k];
        }
    } else {
        wa = a->f000;
        wb = b->f000;
        rb.ev[3] = db.x; rb.ev[4] = db.y; rb.ev[5] = db.z;
        rb.ev[9] = ib.x; rb.ev[10] = ib.y; rb.ev[11] = ib.z;
        rb.ev[6] = da.x; rb.ev[7] = da.y; rb.ev[8] = da.z;
        rb.ev[12] = ia.x; rb.ev[13] = ia.y; rb.ev[14] = ia.z;
        st = FUN_0040e350();
        if (st == 1 || st == 2 || st == 3 || st == 4 || st == 5) return p3 == 0;
        DAT_007e9de0.type[DAT_007e9de0.count] = 4;
        DAT_007e9de0.time[DAT_007e9de0.count] = DAT_007f1030;
        DAT_007e9de0.who[DAT_007e9de0.count] = a->f000 + 0xff00;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f00 = wa;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f04 = wb;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f08 = diff;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f0c = rb.v.x;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f10 = rb.v.y;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f14 = rb.v.z;
        DAT_007e9de0.hdr[DAT_007e9de0.count].f18 = rb.e[0];
        DAT_007e9de0.hdr[DAT_007e9de0.count].f1c = rb.e[1];
        for (k = 0; k < 16; k++) {
            DAT_007e9de0.ev[DAT_007e9de0.count].v[k] = rb.ev[k];
        }
    }
    DAT_007e9de0.count++;
    if (DAT_007e9de0.count >= 0x100) DAT_007e9de0.count = 0;
    return p3 == 0;
    }
    return 0;
}

#ifndef MATCH_PORT
MGLOBALA(Car_00469df0, DAT_008815a0, 16, 0x008815a0);
/* stand-in caller (not a MATCH target): real caller 0x004709a0, call at 0x00470bcd
   (EAX = &DAT_008815a0[j], ECX = the current car, two stack args) */
int standin_004709a0(Car_00469df0 *self, int i, int j)
{
    int n = 0;
    for (; j < 16; j++) {
        if (FUN_00469df0(self, i, n, &DAT_008815a0[j]))
            n++;
    }
    return n;
}
#endif

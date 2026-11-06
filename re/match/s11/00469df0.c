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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
MGLOBAL(float, DAT_005cea50, 0x005cea50);
MGLOBAL(float, DAT_005cc568, 0x005cc568);
MGLOBAL(float, DAT_005ce194, 0x005ce194);
MGLOBAL(float, DAT_005cea4c, 0x005cea4c);
MGLOBAL(float, DAT_005cd50c, 0x005cd50c);
MGLOBAL(float, DAT_005ce268, 0x005ce268);
MGLOBAL(float, DAT_005cc328, 0x005cc328);
int FUN_0040e350(void);
void *FUN_004c39b0(V3_00469df0 *out, const V3_00469df0 *in);
float FUN_004c3ac0(const V3_00469df0 *v);
void *FUN_004c3df0(V3_00469df0 *out, const V3_00469df0 *in, int n, const void *m);
void *FUN_004c4dc0(void *out, const Mat_00469df0 *m);

// MATCH: 0x00469df0 FUN_00469df0
static int FUN_00469df0(Car_00469df0 *b, int p2, int p3, Car_00469df0 *a)
{
    V3_00469df0 *pa, *pb, *va, *vb;
    Car_00469df0 *c, *o;
    uint k, cnt;
    int side[2];
    float sx, sy, sz, inv;
    V3_00469df0 P0, P1, P2, P3;
    float n3x, n3y, n3z;
    V3_00469df0 n0, n1, n2, nn;
    float x, y, z;
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
    sz = DAT_005d757c;
    sy = DAT_005d757c;
    sx = DAT_005d757c;
    do {
        c = a;
        if (k != 0) c = b;
        P0 = c->q[0];
        P1 = c->q[1];
        P3 = c->q[3];
        P2 = c->q[2];
        n0.x = (P1.z - P0.z) * c->n.y - (P1.y - P0.y) * c->n.z;
        n0.y = (P1.x - P0.x) * c->n.z - (P1.z - P0.z) * c->n.x;
        n0.z = (P1.y - P0.y) * c->n.x - (P1.x - P0.x) * c->n.y;
        n1.x = (P3.z - P1.z) * c->n.y - (P3.y - P1.y) * c->n.z;
        n1.y = (P3.x - P1.x) * c->n.z - (P3.z - P1.z) * c->n.x;
        n1.z = (P3.y - P1.y) * c->n.x - (P3.x - P1.x) * c->n.y;
        n2.x = (P2.z - P3.z) * c->n.y - (P2.y - P3.y) * c->n.z;
        n2.y = (P2.x - P3.x) * c->n.z - (P2.z - P3.z) * c->n.x;
        n2.z = (P2.y - P3.y) * c->n.x - (P2.x - P3.x) * c->n.y;
        n3x = (P0.z - P2.z) * c->n.y - (P0.y - P2.y) * c->n.z;
        n3y = (P0.x - P2.x) * c->n.z - (P0.z - P2.z) * c->n.x;
        n3z = (P0.y - P2.y) * c->n.x - (P0.x - P2.x) * c->n.y;
        o = b;
        if (k != 0) o = a;
        x = o->q[0].x;
        y = o->q[0].y;
        z = o->q[0].z;
        if ((x - P0.x) * n0.x + (y - P0.y) * n0.y + n0.z * (z - P0.z) < DAT_005d757c &&
            (x - P1.x) * n1.x + (y - P1.y) * n1.y + n1.z * (z - P1.z) < DAT_005d757c &&
            (x - P3.x) * n2.x + (y - P3.y) * n2.y + n2.z * (z - P3.z) < DAT_005d757c &&
            (x - P2.x) * n3x + (y - P2.y) * n3y + n3z * (z - P2.z) < DAT_005d757c) {
            sx = sx + x;
            cnt++;
            sy = y + sy;
            side[k] = side[k] + 1;
            sz = z + sz;
        }
        x = o->q[1].x;
        y = o->q[1].y;
        z = o->q[1].z;
        if ((x - P0.x) * n0.x + (y - P0.y) * n0.y + n0.z * (z - P0.z) < DAT_005d757c &&
            (x - P1.x) * n1.x + (y - P1.y) * n1.y + n1.z * (z - P1.z) < DAT_005d757c &&
            (x - P3.x) * n2.x + (y - P3.y) * n2.y + n2.z * (z - P3.z) < DAT_005d757c &&
            (x - P2.x) * n3x + (y - P2.y) * n3y + n3z * (z - P2.z) < DAT_005d757c) {
            sx = sx + x;
            cnt++;
            sy = y + sy;
            side[k] = side[k] + 1;
            sz = z + sz;
        }
        x = o->q[2].x;
        y = o->q[2].y;
        z = o->q[2].z;
        if ((x - P0.x) * n0.x + (y - P0.y) * n0.y + n0.z * (z - P0.z) < DAT_005d757c &&
            (x - P1.x) * n1.x + (y - P1.y) * n1.y + n1.z * (z - P1.z) < DAT_005d757c &&
            (x - P3.x) * n2.x + (y - P3.y) * n2.y + n2.z * (z - P3.z) < DAT_005d757c &&
            (x - P2.x) * n3x + (y - P2.y) * n3y + n3z * (z - P2.z) < DAT_005d757c) {
            sx = sx + x;
            cnt++;
            sy = y + sy;
            sz = z + sz;
        }
        x = o->q[3].x;
        y = o->q[3].y;
        z = o->q[3].z;
        if ((x - P0.x) * n0.x + (y - P0.y) * n0.y + n0.z * (z - P0.z) < DAT_005d757c &&
            (x - P1.x) * n1.x + (y - P1.y) * n1.y + n1.z * (z - P1.z) < DAT_005d757c &&
            (x - P3.x) * n2.x + (y - P3.y) * n2.y + n2.z * (z - P3.z) < DAT_005d757c &&
            (x - P2.x) * n3x + (y - P2.y) * n3y + n3z * (z - P2.z) < DAT_005d757c) {
            sx = sx + x;
            cnt++;
            sy = y + sy;
            sz = z + sz;
        }
        k++;
    } while (k < 2);
    if (cnt == 0) return 0;
    inv = DAT_005cc320 / (float)cnt;
    sx = inv * sx;
    sy = inv * sy;
    sz = inv * sz;
    da.x = sx - pb->x;
    da.y = sy - pb->y;
    da.z = sz - pb->z;
    ra = da;
    FUN_004c39b0(&da, &da);
    db.x = sx - pa->x;
    db.y = sy - pa->y;
    db.z = sz - pa->z;
    rb.v = db;
    FUN_004c39b0(&db, &db);
    vb = &b->vel;
    va = &a->vel;
    ta.x = b->spin.x * DAT_005cea50 + b->vel.x;
    ib.z = 0.0f;
    ib.y = 0.0f;
    ib.x = 0.0f;
    ia.z = 0.0f;
    ia.y = 0.0f;
    ia.x = 0.0f;
    ta.y = b->spin.y * DAT_005cea50 + b->vel.y;
    ta.z = b->spin.z * DAT_005cea50 + b->vel.z;
    tb.x = a->spin.x * DAT_005cea50 + a->vel.x;
    tb.y = a->spin.y * DAT_005cea50 + a->vel.y;
    tb.z = a->spin.z * DAT_005cea50 + a->vel.z;
    la = FUN_004c3ac0(&ta);
    lb = FUN_004c3ac0(&tb);
    dot = tb.z * ta.z + tb.x * ta.x + tb.y * ta.y;
    if (la != DAT_005d757c) {
        f = dot * (DAT_005cc320 / la);
        if (lb != DAT_005d757c) f = f / lb;
        f = -((DAT_005cc320 / la) * lb * f);
        if (f > DAT_005d757c) f = DAT_005d757c;
        else if (f < DAT_005cc33c) f = DAT_005cc33c;
        f = f + DAT_005cc320;
        n2.y = f * ta.y;
        n2.z = f * ta.z;
        ib.x = ta.x * f;
        ib.y = f * ta.y;
        ib.z = f * ta.z;
        ia.x = -(ta.x * f);
        ia.y = -n2.y;
        ia.z = -n2.z;
    }
    if (lb != DAT_005d757c) {
        f = dot * (DAT_005cc320 / lb);
        if (la != DAT_005d757c) f = f / la;
        f = -((DAT_005cc320 / lb) * la * f);
        if (f > DAT_005d757c) f = DAT_005d757c;
        else if (f < DAT_005cc33c) f = DAT_005cc33c;
        f = f + DAT_005cc320;
        n2.y = f * tb.y;
        n2.z = f * tb.z;
        ia.x = ia.x + tb.x * f;
        ia.y = ia.y + n2.y;
        ia.z = ia.z + n2.z;
        ib.x = ib.x - tb.x * f;
        ib.y = ib.y - n2.y;
        ib.z = ib.z - n2.z;
    }
    vb->x = ia.x * DAT_005cc32c + vb->x;
    b->vel.y = ia.y * DAT_005cc32c + b->vel.y;
    b->vel.z = ia.z * DAT_005cc32c + b->vel.z;
    va->x = ib.x * DAT_005cc32c + va->x;
    a->vel.y = ib.y * DAT_005cc32c + a->vel.y;
    a->vel.z = ib.z * DAT_005cc32c + a->vel.z;
    h = FUN_004c3ac0(&ia);
    sum = FUN_004c3ac0(&ib) + h;
    if (DAT_00803320 < 3) h = DAT_005cc568;
    if (DAT_00803320 == 3 || DAT_00803320 == 4) h = DAT_005ce194;
    imp = sum * DAT_005cea4c;
    if (DAT_005cc320 < sum * DAT_005cea4c) imp = DAT_005cc320;
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
    f = imp * DAT_005cd50c;
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
    if (da.x < DAT_005d757c) {
        if (da.z < DAT_005d757c) f = -imp;
    } else {
        if (da.z > DAT_005d757c) f = -imp;
    }
    if (b->f010 == 0) {
        b->spin.y = f * DAT_005ce268 + b->spin.y;
    } else {
        f = f * DAT_005cc328;
        b->f9bc = 0.0f;
        b->f9c4 = 0.0f;
        b->f9c0 = f;
    }
    if (db.x < DAT_005d757c) {
        if (db.z < DAT_005d757c) imp = -imp;
    } else {
        if (db.z > DAT_005d757c) imp = -imp;
    }
    if (a->f010 == 0) {
        a->spin.y = imp * DAT_005ce268 + a->spin.y;
    } else {
        imp = imp * DAT_005cc328;
        a->f9bc = 0.0f;
        a->f9c4 = 0.0f;
        a->f9c0 = imp;
    }
    rb.ev[0] = sx;
    rb.ev[1] = sy;
    rb.ev[2] = sz;
    diff = side[0] != side[1];
    if (side[0] < side[1]) {
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
    } else {
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
    }
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
    DAT_007e9de0.count++;
    if (DAT_007e9de0.count >= 0x100) DAT_007e9de0.count = 0;
    return p3 == 0;
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

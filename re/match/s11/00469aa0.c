// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x00469aa0 (h2): 60.0% (regs 70.7%, ops 88.3%). Raw 15.5% (unaff_ESI). The car record (the 0xd04
   record of 0x008815a0) is register-passed: static callee plus a stand-in caller (built only outside
   MATCH_PORT; real caller 0x004709a0, call at 0x00470ae8). 9176 passes it in a register only once the event
   queue is one struct (count at 0x007e9de0 followed by the type/time/who/hdr/ev arrays, so each store reloads
   the count as in the original); before that the arg stayed on the stack. Slot loop through `s = &car->slot[i]`
   gives the original's 9x unrolled single-base loop; tpts[i]/pts[i] indexed share one induction pointer.
   Slots are 0x40 at +0x4ac and the last one runs into the matrices at +0x928, so the matrix is
   `(Mat *)((char *)car + 0x928) + car->matidx`. Statement order inside the slot loop/event block is the
   permuter's (1304 compiles, 38.1 -> 60.0%).
   Residual (b): the arg lands in EDI not ESI (all registers rotate from there), the frame is 0x5c not 0x54
   (one extra spill), and the scaled-force/point copies schedule differently. */
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
typedef struct { float x, y, z; } V3_00469aa0;
typedef struct {
    int id;            /* +0x00 */
    undefined1 pad04[0x10];
    V3_00469aa0 dir;   /* +0x14 */
    float cur;         /* +0x20 */
    float prev;        /* +0x24 */
    undefined1 pad28[0xc];
    float scale;       /* +0x34 */
    undefined1 pad38[0x8];
} Slot_00469aa0;       /* 0x40 */
typedef struct { float m[16]; } Mat_00469aa0;
typedef struct {
    int f000;                       /* +0x000 */
    undefined1 pad004[0x5c];
    V3_00469aa0 pts[18];            /* +0x060 */
    undefined1 pad138[0x4ac - 0x138];
    Slot_00469aa0 slot[18];         /* +0x4ac; the last one runs into the matrices at +0x928 */
    undefined1 pad92c[0x9ac - 0x92c];
    int matidx;                     /* +0x9ac */
    undefined1 pad9b0[0x9e4 - 0x9b0];
    int f9e4;                       /* +0x9e4 */
    int f9e8;                       /* +0x9e8 */
    int ncontact;                   /* +0x9ec */
    undefined1 pad9f0[0x9f8 - 0x9f0];
    V3_00469aa0 tpts[18];           /* +0x9f8 */
    undefined1 padac8[0xd04 - 0xac8];
} Car_00469aa0;                     /* 0xd04 */
typedef struct {
    int f00;
    V3_00469aa0 tp;
    V3_00469aa0 force;
    V3_00469aa0 p;
    int pad[6];
} Ev_00469aa0;                      /* 0x40 */
typedef struct { int f00; int f04; int f08; int f0c; float f10; float f14; float f18; int f1c; } EvHdr_00469aa0;
typedef struct {
    uint count;
    int type[256];
    int time[256];
    int who[256];
    EvHdr_00469aa0 hdr[256];
    Ev_00469aa0 ev[256];
} EvQ_00469aa0;
MGLOBAL(EvQ_00469aa0, DAT_007e9de0, 0x007e9de0);
MGLOBAL(int, DAT_007f1030, 0x007f1030);
MGLOBAL(V3_00469aa0, DAT_0088e600, 0x0088e600);
MGLOBAL(int, DAT_00828320, 0x00828320);
int FUN_0040e350(void);
void FUN_00468d80(Car_00469aa0 *car, int *p);
void FUN_004694e0(Car_00469aa0 *car);
void *FUN_004c3d90(V3_00469aa0 *out, const V3_00469aa0 *in, int n, const Mat_00469aa0 *m);

// MATCH: 0x00469aa0 FUN_00469aa0
static int FUN_00469aa0(Car_00469aa0 *car)
{
    Mat_00469aa0 *m;
    Ev_00469aa0 ev;
    V3_00469aa0 up;
    Slot_00469aa0 *s;
    int i, j, found, count, id, st;

    up.y = 0.1f;
    m = (Mat_00469aa0 *)((char *)car + 0x928) + car->matidx;
    up.z = 0.0f;
    up.x = 0.0f;
    for (i = 0; i < 18; i++) {
        s = &car->slot[i];
        s->id = -1;
        s->prev = s->cur;
        s->cur = 0.0f;
    }
    FUN_004c3d90(car->tpts, car->pts, 0x12, m);
    FUN_004c3d90(&DAT_0088e600, &up, 1, m);
    FUN_00468d80(car, &DAT_00828320);
    FUN_004694e0(car);
    s = car->slot;
    found = 0;
    count = 0;
    for (i = 0; i < 18; i++, s++) {
        id = s->id;
        if (id != -1) {
            count++;
            if (!found && s->cur >= s->prev) {
                ev.f00 = car->f9e4;
                ev.tp = car->tpts[i];
                ev.force.y = s->dir.y * s->scale;
                ev.force.z = s->dir.z * s->scale;
                ev.force.x = s->dir.x * s->scale;
                st = FUN_0040e350();
                ev.p = car->pts[i];
                if (st != 1 && st != 2 && st != 3 && st != 4 && st != 5) {
                    DAT_007e9de0.type[DAT_007e9de0.count] = 3;
                    DAT_007e9de0.who[DAT_007e9de0.count] = car->f000 + 0xff00;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f0c = *(int *)&ev.tp.z;
                    DAT_007e9de0.time[DAT_007e9de0.count] = DAT_007f1030;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f00 = i;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f04 = id;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f10 = ev.force.x;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f18 = ev.force.z;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f1c = *(int *)&ev.p.x;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f14 = ev.force.y;
                    DAT_007e9de0.hdr[DAT_007e9de0.count].f08 = *(int *)&ev.tp.y;
                    for (j = 0; j < 16; j++) {
                        ((int *)&DAT_007e9de0.ev[DAT_007e9de0.count])[j] = ((int *)&ev)[j];
                    }
                    DAT_007e9de0.count = DAT_007e9de0.count + 1;
                    if (DAT_007e9de0.count >= 0x100) {
                        DAT_007e9de0.count = 0;
                    }
                }
                found = 1;
            }
        }
    }
    car->ncontact = count;
    return found;
}

#ifndef MATCH_PORT
MGLOBALA(Car_00469aa0, DAT_008815a0, 16, 0x008815a0);
void standin_004709a0(void)
{
    Car_00469aa0 *c;
    for (c = DAT_008815a0; c < DAT_008815a0 + 16; c++)
        FUN_00469aa0(c);
}
#endif

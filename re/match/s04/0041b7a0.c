// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041b7a0 (h2): BYTE-IDENTICAL by hand. match.py 98.7% (regs 98.7%) only because Ghidra's size (1037)
   stops before the last case block; `match.py --bytes` gives the whole obj symbol (1188 B with the jump table)
   0 differing unmasked bytes. Raw g2s was 28.6%.
   What moved it: (1) the record arrives in ESI: the function is `static`, defined before a caller in the same file
   (pilot finding 1). This file holds a stand-in caller, MatchCaller0041b7a0, shaped like the real caller 0x0041c090
   (loop over the 2 records at 0x0063cab8, stride 0x16c); it is not a byte match of 0x0041c090 (jb vs jl).
   (2) switch case order in source = the original block layout: 0-3, 4, 28-33, 5-16, 22-27, default; with numeric
   order the cross-jumped tails land in other cases (37%). (3) chained assignments for the identity matrix and for
   scale.y/z (store order). (4) the self copy of `scale` before the first FUN_004c13e0 is `scale.x *= k` with a
   local `float k = 1.0f` (a literal 1.0f folds away; 0x0041ce00 has the same shape with 1.125f).
   Residual: none. */
/* ---- MATCH-S04 preamble (same text in every re/match/s04 source; not part of any function body) ----
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
typedef struct RwV3d0041b7a0 { float x, y, z; } RwV3d0041b7a0;
typedef struct RwMatrix0041b7a0 {
    RwV3d0041b7a0 right; unsigned int flags; RwV3d0041b7a0 up; unsigned int pad1;
    RwV3d0041b7a0 at; unsigned int pad2; RwV3d0041b7a0 pos; unsigned int pad3;
} RwMatrix0041b7a0;
typedef struct Obj0041b7a0 {
    int ids[3];
    int count;
    int vis[40];
    void *parts[40];
    RwV3d0041b7a0 pos;
    int unk15c;
    void *frame;
    int unk164;
    unsigned int flags;
} Obj0041b7a0;

MGLOBALA(int, DAT_0063cd90, 5, 0x0063cd90);
MGLOBALA(Obj0041b7a0, DAT_0063cab8, 2, 0x0063cab8);

int FUN_0040b6d0(int id);
int FUN_0040b890(void);
void FUN_0040b930(int *p);
void *FUN_004c13e0(void *frame, RwV3d0041b7a0 *v, int combine);
void *FUN_004c1480(void *frame, RwMatrix0041b7a0 *m, int combine);
void *FUN_004c51a0(RwMatrix0041b7a0 *m, RwV3d0041b7a0 *v, int combine);

// MATCH: 0x0041b7a0 FUN_0041b7a0
static void FUN_0041b7a0(Obj0041b7a0 *p)
{
    RwV3d0041b7a0 scale;
    RwMatrix0041b7a0 m;
    int i;
    int on;
    float k;

    k = 1.0f;
    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;
    if (DAT_0063cd90[p->ids[0]])
        p->flags |= 0x40000;
    else
        p->flags &= ~0x40000;
    for (i = 0; i < 40; i++) {
        if (p->parts[i]) {
            switch (i) {
            case 0: case 1: case 2: case 3: on = 1; break;
            case 4: on = p->flags & 0x40000; break;
            case 28: on = (p->flags & 0x800000) && (p->flags & 0x1); break;
            case 29: on = (p->flags & 0x800000) && (p->flags & 0x2); break;
            case 30: on = (p->flags & 0x800000) && (p->flags & 0x4); break;
            case 31: on = (p->flags & 0x800000) && (p->flags & 0x8); break;
            case 32: on = (p->flags & 0x800000) && (p->flags & 0x10); break;
            case 33: on = (p->flags & 0x800000) && (p->flags & 0x20); break;
            case 5: on = (p->flags & 0x3000000) && (p->flags & 0x1); break;
            case 6: on = (p->flags & 0x3000000) && (p->flags & 0x2); break;
            case 7: on = (p->flags & 0x3000000) && (p->flags & 0x4); break;
            case 8: on = (p->flags & 0x3000000) && (p->flags & 0x8); break;
            case 9: on = (p->flags & 0x3000000) && (p->flags & 0x10); break;
            case 10: on = (p->flags & 0x3000000) && (p->flags & 0x20); break;
            case 11: on = (p->flags & 0x3000000) && (p->flags & 0x40); break;
            case 12: on = (p->flags & 0x3000000) && (p->flags & 0x80); break;
            case 13: on = (p->flags & 0x3000000) && (p->flags & 0x100); break;
            case 14: on = (p->flags & 0x3000000) && (p->flags & 0x200); break;
            case 15: on = (p->flags & 0x3000000) && (p->flags & 0x400); break;
            case 16: on = (p->flags & 0x3000000) && (p->flags & 0x800); break;
            case 22: on = (p->flags & 0x2000000) && (p->flags & 0x1000); break;
            case 23: on = (p->flags & 0x2000000) && (p->flags & 0x2000); break;
            case 24: on = (p->flags & 0x2000000) && (p->flags & 0x4000); break;
            case 25: on = (p->flags & 0x2000000) && (p->flags & 0x8000); break;
            case 26: on = (p->flags & 0x2000000) && (p->flags & 0x10000); break;
            case 27: on = (p->flags & 0x2000000) && (p->flags & 0x20000); break;
            default: on = 0; break;
            }
            p->vis[i] = on;
        }
    }
    m.flags |= 0x20003;
    m.right.x = m.up.y = m.at.z = 1.0f;
    m.right.y = m.right.z = m.up.x = 0.0f;
    m.up.z = m.at.x = m.at.y = 0.0f;
    m.pos.x = m.pos.y = m.pos.z = 0.0f;
    FUN_004c51a0(&m, &p->pos, 0);
    FUN_004c1480(p->frame, &m, 0);
    scale.x *= k; scale.y *= k; scale.z *= k;
    FUN_004c13e0(p->frame, &scale, 1);
    scale.x = (float)FUN_0040b6d0(p->ids[0]) / (float)FUN_0040b890();
    scale.y = scale.z = 1.0f;
    FUN_004c13e0(((void **)p->parts[2])[1], &scale, 0);
}

/* stand-in caller (0x0041c090 shape) so the compiler gives FUN_0041b7a0 its ESI convention */
void MatchCaller0041b7a0(void)
{
    Obj0041b7a0 *p;
    FUN_0040b930(DAT_0063cd90);
    for (p = DAT_0063cab8; p < (Obj0041b7a0 *)DAT_0063cd90; p++)
        FUN_0041b7a0(p);
    DAT_0063cd90[4]++;
}

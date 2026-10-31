// FLAGS: /O2 /GS /Ob1
/* MATCH-S04 0x0041ce00 (h2): 98.7% (regs 98.7%) by hand, raw g2s was 24.4%. Same family as 0x0041b7a0 (see
   re/match/s04/0041b7a0.c, BYTE-IDENTICAL): record in ESI via a `static` callee defined before a stand-in caller
   (MatchCaller0041ce00, shaped like the real caller 0x0041d830: 2 records at 0x0063d298, stride 0x160; not a byte
   match of it), switch cases in the original block order (0-3, 4, 28-33, 5-16, 22-27, 17-21, default), chained
   identity-matrix stores, `m.flags |= 0x20003` written after the identity (it lands last, from ebx), scale.y/z chain.
   2 of the 4 "missing" instructions are Ghidra's size (1388) stopping before `add esp, 0x60; ret`.
   Residual (a): in the scale-by-1.125 block before the first FUN_004c13e0 the original issues `fld scale.y` before
   `push edx`, ours after it (one x87 load one slot later; same bytes otherwise). Survived: k as local float / init /
   static const / the real constant global, literal 1.125f and 1.125 (double), `x *= k` / `x = k * x`, statement order
   of the 3 components, an inline V3dScale(out, in, s) helper, float[3] instead of the struct, the multiply inside the
   call's argument expression, frame/combine in locals, and match_permute (318 variants, no gain). */
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
typedef struct RwV3d0041ce00 { float x, y, z; } RwV3d0041ce00;
typedef struct RwMatrix0041ce00 {
    RwV3d0041ce00 right; unsigned int flags; RwV3d0041ce00 up; unsigned int pad1;
    RwV3d0041ce00 at; unsigned int pad2; RwV3d0041ce00 pos; unsigned int pad3;
} RwMatrix0041ce00;
typedef struct Obj0041ce00 {
    int ids[3];
    int count;
    int vis[40];
    void *parts[40];
    int unk150;
    void *frame;
    int unk158;
    unsigned int flags;
} Obj0041ce00;

MGLOBALA(int, DAT_0063d278, 4, 0x0063d278);
MGLOBAL(int, DAT_0063d288, 0x0063d288);
MGLOBALA(Obj0041ce00, DAT_0063d298, 2, 0x0063d298);
MGLOBAL(unsigned int, DAT_0063d558, 0x0063d558);
MGLOBALA(RwV3d0041ce00, DAT_005f33c4, 2, 0x005f33c4);

double sin(double);
#pragma intrinsic(sin)
int FUN_0040b420(int id);
int FUN_0040b6d0(int id);
int FUN_0040b890(void);
void FUN_0040b930(int *p);
void FUN_0040b540(int *p);
void *FUN_004c13e0(void *frame, RwV3d0041ce00 *v, int combine);
void *FUN_004c1480(void *frame, RwMatrix0041ce00 *m, int combine);
void *FUN_004c51a0(RwMatrix0041ce00 *m, RwV3d0041ce00 *v, int combine);

// MATCH: 0x0041ce00 FUN_0041ce00
static void FUN_0041ce00(Obj0041ce00 *p)
{
    RwV3d0041ce00 scale;
    RwMatrix0041ce00 m;
    int i;
    int on;
    int idx;
    float k;

    k = 1.125f;
    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;
    if (DAT_0063d278[p->ids[0]])
        p->flags |= 0x40000;
    else
        p->flags &= ~0x40000;
    i = FUN_0040b420(p->ids[0]);
    p->flags &= 0x83ffffff;
    switch (i) {
    case 2: p->flags |= 0x4000000; break;
    case 1: p->flags |= 0x8000000; break;
    case 0: p->flags |= 0x10000000; break;
    case -1: p->flags |= 0x20000000; break;
    case -2: p->flags |= 0x40000000; break;
    }
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
            case 17: on = p->flags & 0x4000000; break;
            case 18: on = p->flags & 0x8000000; break;
            case 19: on = p->flags & 0x10000000; break;
            case 20: on = p->flags & 0x20000000; break;
            case 21: on = p->flags & 0x40000000; break;
            default: on = 0; break;
            }
            p->vis[i] = on;
        }
    }
    idx = 1;
    for (i = 0; i < p->count; i++) {
        if (p->ids[i] == DAT_0063d288) {
            idx = 0;
            break;
        }
    }
    m.right.x = m.up.y = m.at.z = 1.0f;
    m.right.y = m.right.z = m.up.x = 0.0f;
    m.up.z = m.at.x = m.at.y = 0.0f;
    m.pos.x = m.pos.y = m.pos.z = 0.0f;
    m.flags |= 0x20003;
    FUN_004c51a0(&m, &DAT_005f33c4[idx], 0);
    FUN_004c1480(p->frame, &m, 0);
    scale.x *= k; scale.y *= k; scale.z *= k;
    FUN_004c13e0(p->frame, &scale, 1);
    scale.x = (float)FUN_0040b6d0(p->ids[0]) / (float)FUN_0040b890();
    scale.y = scale.z = 1.0f;
    FUN_004c13e0(((void **)p->parts[2])[1], &scale, 0);
    if (p->flags & 0x40000)
        scale.x = scale.y = (float)sin((float)DAT_0063d558 * 0.2f) * 0.15f + 1.0f;
    else
        scale.x = scale.y = 1.0f;
    scale.z = 1.0f;
    FUN_004c13e0(((void **)p->parts[4])[1], &scale, 0);
}

/* stand-in caller (0x0041d830 shape) so the compiler gives FUN_0041ce00 its ESI convention */
void MatchCaller0041ce00(void)
{
    Obj0041ce00 *p;
    FUN_0040b930(DAT_0063d278);
    FUN_0040b540(&DAT_0063d288);
    for (p = DAT_0063d298; p < (Obj0041ce00 *)&DAT_0063d558; p++)
        FUN_0041ce00(p);
    DAT_0063d558++;
}

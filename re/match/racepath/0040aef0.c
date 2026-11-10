// FLAGS: /O2 /GS /Ob1
/* MATCH-RACEPATH-FRESH 0x0040aef0 (first hand source; raw g2s 34.7%): NOT CLOSED, 97.6% insn, 12 B differ
   (409 B whole symbol). The constants are literals (-1.0f 0x005cc33c, 1.0f 0x005cc320, 0.99f 0x005cc9b4,
   0.0f 0x005d757c). The second branch zeroes a0, not a1 (0x0040af6d stores [esp+0x20], the a0 slot): kept
   as in the binary. Moved it: literals (21 B), dblgreedy W on v.z (16), F on m->pos.x (12). Residual: the
   fld of m->pos.y and of v.y one slot after the next push (0x0040afbe, 0x0040b020). Tried W/D/E/F/C on
   both y lines, x forms, comma rvalues: no gain. */
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
typedef struct { V3 right; int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } RwMatrix;
typedef struct { float dur0, dur1, t0, amp0, t1, amp1, frac0, frac1; int on0, on1; } Wobble;
MGLOBALA(Wobble, DAT_0063b838, 4, 0x0063b838);
void FUN_0046d4a0(RwMatrix **out, int car);
RwMatrix *FUN_004c4d20(RwMatrix *m, V3 *axis, float angle, int op);
RwMatrix *FUN_004c51a0(RwMatrix *m, V3 *v, int op);

// MATCH: 0x0040aef0 _FUN_0040aef0
void FUN_0040aef0(int car)
{
    Wobble *w;
    RwMatrix *m;
    float a0;
    float a1;
    V3 v;

    w = &DAT_0063b838[car];
    a1 = 0.0f;
    if (w->on0 == 0 && w->on1 == 0)
        return;
    if (w->dur0 == 0.0f) {
        a0 = 0.0f;
    } else {
        w->frac0 = w->t0 / w->dur0;
        a0 = (1.0f - w->t0 / w->dur0) * w->amp0;
        w->t0 = a0 + w->t0;
    }
    if (w->dur1 == 0.0f) {
        a0 = 0.0f;
    } else {
        w->frac1 = w->t1 / w->dur1;
        a1 = (1.0f - w->t1 / w->dur1) * w->amp1;
        w->t1 = a1 + w->t1;
    }
    FUN_0046d4a0(&m, car);
    { float tF_ = m->pos.x; v.x = tF_ * -1.0f; }
    v.y = m->pos.y * -1.0f;
    v.z = m->pos.z * -1.0f;
    FUN_004c51a0(m, &v, 2);
    FUN_004c4d20(m, &m->up, a0, 2);
    FUN_004c4d20(m, &m->at, a1, 2);
    v.x = v.x * -1.0f;
    v.y = v.y * -1.0f;
    { double tW_ = v.z * -1.0f; v.z = (float)tW_; }
    FUN_004c51a0(m, &v, 2);
    if (w->frac0 >= 0.99f) {
        w->dur0 = 0; w->amp0 = 0; w->t0 = 0; w->frac0 = 0; w->on0 = 0;
    }
    if (w->frac1 >= 0.99f) {
        w->dur1 = 0; w->amp1 = 0; w->t1 = 0; w->frac1 = 0; w->on1 = 0;
    }
}

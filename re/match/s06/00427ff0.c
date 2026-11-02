// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00427ff0 (helper h6): 100% (regs 100%), relocheck OK. By hand.
   Holds static copies of its register-arg callees, each with its own // MATCH line at 100%:
   0x00427680 (text EDI, out ESI; same body as in 00427e00.c) and 0x004277a0 (src EAX, dst EBX).
   What moved the score: scale * 0.025f written twice (off[1] += ..., then -(...)) so the compiler CSEs it into
   the fst / fadd spill; a named local for it gives fstp / fld / fadd (95.3%) in every assignment form tried.
   0x00552df0 takes two floats (the second call passes 0.0f, -(scale * 0.025f)). Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(float, DAT_0067d830, 0x0067d830);
MGLOBAL(float, DAT_0067d834, 0x0067d834);
MGLOBAL(int, DAT_0067d838, 0x0067d838);
MGLOBAL(int, DAT_0067d83c, 0x0067d83c);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
float FUN_005554d0(int font, short *text, float size);
short *FUN_00427780(int id);
int FUN_00552d10(void);
int FUN_00552750(void);
void FUN_00552d70(void);
int FUN_00556e90(int font, void *c0, void *c1, void *c2, void *c3);
void FUN_00556ca0(int font, short *text, float size, float *pos, int font2);
typedef void (*RenderStateFn)(int, int);

#ifndef __cplusplus
// MATCH-HELD: 0x00427680 (static copy for the DDK register convention; port body: re/match/s06/00427680.c)
static void FUN_00427680(short *text, float *out, float x, float y, int align, float scale)
{
    float w = (float)FUN_0042b8b0();
    float h = (float)FUN_0042b8c0();
    float v;
    float r;
    switch (align) {
    case 1:
        r = FUN_005554d0(DAT_0067d838, text, scale * 0.0708f);
        v = x * (w * 0.0015625f) - r / DAT_0067d830 * w;
        out[0] = DAT_0067d830 * (v / w);
        break;
    case 2:
        r = FUN_005554d0(DAT_0067d838, text, scale * 0.0708f);
        v = x * (w * 0.0015625f) - r / DAT_0067d830 * w * 0.5f;
        out[0] = DAT_0067d830 * (v / w);
        break;
    default:
        v = x * (w * 0.0015625f);
        out[0] = DAT_0067d830 * (v / w);
        break;
    }
    out[1] = (1.0f - (y * (h * 0.0020833334f)) / h * DAT_0067d834) - scale * 0.025f;
}
#else
void FUN_00427680(short *text, float *out, float x, float y, int align, float scale);   /* port: one body, bound by the wrapper's #define */
#endif
#ifndef __cplusplus
// MATCH-HELD: 0x004277a0 (static copy for the DDK register convention; port body: re/match/s06/004277a0.c)
static void FUN_004277a0(short *src, short *dst)
{
    short n = *src++;
    short i;
    for (i = 0; i < n; i++) {
        short c = src[i];
        if (c == 8) c = 0x81;
        else if (c == 9) c = 0x7f;
        else if (c == 10) c = 0x81;
        else if (c == 11) c = 0x8d;
        else if (c == 12) c = 0x80;
        else if (c == 13) c = 0x87;
        else if (c == 14) c = 0x8f;
        dst[i] = c;
    }
    dst[n] = 0;
}
#else
void FUN_004277a0(short *src, short *dst);   /* port: one body, bound by the wrapper's #define */
#endif
void FUN_00552df0(float x, float y);
void FUN_00552840(int p);

// MATCH: 0x00427ff0 FUN_00427ff0
void FUN_00427ff0(int id, float x, float y, unsigned int color, float scale, int align, int p7)
{
    float off[2];
    float pos[2];
    short text[128];
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    FUN_004277a0(FUN_00427780(id), text);
    (*(RenderStateFn *)(DAT_007d3ff8 + 0x20))(9, 2);
    FUN_00552d10();
    FUN_00552750();
    FUN_00556e90(DAT_0067d83c, &color, &color, &color, &color);
    FUN_00427680(text, off, x, y, align, scale);
    off[1] += scale * 0.025f;
    FUN_00552df0(off[0], off[1]);
    FUN_00552840(p7);
    FUN_00552df0(0.0f, -(scale * 0.025f));
    FUN_00556ca0(DAT_0067d838, text, scale * 0.0708f, pos, DAT_0067d83c);
    FUN_00552d70();
}

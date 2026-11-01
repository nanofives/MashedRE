// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h5 0x00427680 (248 B). Best 100.0% (regs 100.0%). Raw converter 16.4%.
   Static callee, mixed convention: text in EDI, out (float[2]) in ESI, plus 4 stack args (x, y, align,
   size). The real callers (0x00427e00, 0x00427f00, 0x00427ff0, 0x00428140) are not in this slice:
   FUN_00427680_caller below is a STAND-IN (not matched, not shipped).
   What moved it: (1) each switch case stores out[0] itself (`((...) / sw) * DAT_0067d830`, tail-merged by
   the compiler); one px local stored once gave text = ESI / out = EDI (92.5%), and no param order or type
   change moved that. (2) Float reassociation (no /Op): `(0.0015625f * sw) * x` and
   `((0.0020833334f * sh) * y) / sh`; Ghidra's `sw * c * x` order gives the constant-first loads (81%).
   (3) The width call result in a local (tw) per case. Constants: 0.0015625f @0x005cd5a8,
   0.0020833334f @0x005cc560, 1.0f @0x005cc320, 0.0708f @0x005cd5fc, 0.5f @0x005cc32c, 0.025f @0x005cc9a4.
   Residual: none. */
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
MGLOBAL(int, DAT_0067d838, 0x0067d838);
MGLOBAL(float, DAT_0067d830, 0x0067d830);
MGLOBAL(float, DAT_0067d834, 0x0067d834);

short FUN_0042b8b0(void);
short FUN_0042b8c0(void);
float FUN_005554d0(int font, short *text, float size);
void FUN_005555b0(int a, short *text, float size, float *pos, int align, int f);

// MATCH: 0x00427680 FUN_00427680
static void FUN_00427680(short *text, float *out, float x, float y, int align, float size)
{
    float sw = (float)FUN_0042b8b0();
    float sh = (float)FUN_0042b8c0();
    float tw;
    switch (align) {
    case 1:
        tw = FUN_005554d0(DAT_0067d838, text, size * 0.0708f);
        out[0] = ((0.0015625f * sw) * x - tw / DAT_0067d830 * sw) / sw * DAT_0067d830;
        break;
    case 2:
        tw = FUN_005554d0(DAT_0067d838, text, size * 0.0708f);
        out[0] = ((0.0015625f * sw) * x - tw / DAT_0067d830 * sw * 0.5f) / sw * DAT_0067d830;
        break;
    default:
        out[0] = ((0.0015625f * sw) * x) / sw * DAT_0067d830;
        break;
    }
    out[1] = 1.0f - ((0.0020833334f * sh) * y) / sh * DAT_0067d834 - size * 0.025f;
}

/* stand-in caller (not matched, not shipped): out in ESI, text in EDI, as 0x00427e94 does */
void FUN_00427680_caller(float x, float y, int align, float size)
{
    float pos[2];
    short text[64];
    text[0] = 0;
    FUN_00427680(text, pos, x, y, align, size);
    FUN_005555b0(DAT_0067d838, text, size, pos, align, 0);
}

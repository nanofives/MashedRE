// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00497310 (294 B Ghidra size; 312 B obj symbol with the 4-entry jump table at 0x00497438), h4.
   100.0% per instruction and match.py --bytes BYTE-IDENTICAL over the whole symbol. Controller binding lookup:
   per-player record 0x200 B at 0x007e95c0 (+0x108 bindings, +0x13c type 1/2, +0x140 joystick index).
   Only fix over the first pass: the binding is unsigned (shr, not sar). Float literals 0.0f / -255.0f / 255.0f
   are the .rdata constants at 0x005d757c / 0x005d0108 / 0x005cd04c; float->uchar is the __ftol tail jump. */
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
typedef struct ContCfg_h4 {
    unsigned char pad0[0x108];
    unsigned int bind[0x0d];          /* +0x108 */
    int type;                /* +0x13c: 1 joystick, 2 keyboard */
    int joy;                 /* +0x140 */
    unsigned char pad144[0x200 - 0x144];
} ContCfg_h4;
typedef struct JoyAxes_h4 { float x, y; } JoyAxes_h4;
MGLOBALA(ContCfg_h4, DAT_007e95c0, 4, 0x007e95c0);
MGLOBALA(JoyAxes_h4, DAT_0077311c, 4, 0x0077311c);
MGLOBALA(unsigned char, DAT_007730d4, 0x48, 0x007730d4);
MGLOBALA(unsigned char, DAT_0077313c, 0x20, 0x0077313c);
// MATCH: 0x00497310 FUN_00497310
unsigned char FUN_00497310(int player, int action)
{
    ContCfg_h4 *c = &DAT_007e95c0[player];
    if (c->type == 1) {
        unsigned int b = c->bind[action];
        int j = c->joy;
        float x = DAT_0077311c[j].x;
        float y = DAT_0077311c[j].y;
        switch (action) {
        case 9:
            if (x < 0.0f)
                return (unsigned char)(x * -255.0f);
            break;
        case 10:
            if (x > 0.0f)
                return (unsigned char)(x * 255.0f);
            break;
        case 11:
            if (y > 0.0f)
                return 0xff;
            break;
        case 12:
            if (y < 0.0f)
                return 0xff;
            break;
        default:
            return (DAT_007730d4[j * 4 + (b >> 3)] & (1 << (b & 7))) ? 0xff : 0;
        }
    } else if (c->type == 2) {
        unsigned int b = c->bind[action];
        return (DAT_0077313c[b >> 3] & (1 << (b & 7))) ? 0xff : 0;
    }
    return 0;
}

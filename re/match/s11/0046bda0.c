// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046bda0 (h2): 100% (regs 100%). Raw 38.2%. Record pointer r = &DAT_008815a0[p2] as its own
   statement, then a 16-iteration sum of r->w[p3].s[i] (indexing through r keeps p2*0xd04 unfolded; a wheel
   pointer or direct indexing folds it into (p2*17+p3)*0xc4), and `/ 16.0f` (the compiler's 0.0625 constant
   at 0x005cd058; `* DAT_005cd058` adds an fld/fmul st(1)). Two early returns place return-0 mid-function. */
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
typedef struct {
    float f0;
    undefined1 pad4[4];
    float s[16];
    undefined1 pad[0xc4 - 8 - 64];
} Wheel_0046bda0;
typedef struct {
    undefined1 pad0[0x1a4];
    Wheel_0046bda0 w[4];
    undefined1 pad1[0xd04 - 0x1a4 - 4 * 0xc4];
} Rec_008815a0;
MGLOBALA(Rec_008815a0, DAT_008815a0, 16, 0x008815a0);
MGLOBAL(float, DAT_005d757c, 0x005d757c);

// MATCH: 0x0046bda0 FUN_0046bda0
undefined4 FUN_0046bda0(float *param_1, uint param_2, uint param_3)
{
    Rec_008815a0 *r;
    float sum;
    int i;
    if (param_2 >= 0x10) return 0;
    if (param_3 >= 4) return 0;
    r = &DAT_008815a0[param_2];
    sum = DAT_005d757c;
    for (i = 0; i < 16; i++) {
        sum += r->w[param_3].s[i];
    }
    *param_1 = sum / 16.0f;
    return 1;
}

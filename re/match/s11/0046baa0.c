// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046baa0 (h2): BYTE-IDENTICAL whole symbol (572 B, match.py --bytes); the instruction score
   reads 97.5% only because Ghidra's size (569) stops 3 B early, before `mov [ecx+0x38],edx; pop esi; ret`.
   Raw converter did not compile. Typed 0xd04 record at 0x008815a0 (wheel[4] of 0xc4 at +0x194, slot[18] of
   0x40 at +0x4ac, vec3[16] at +0xb2c). Wheel fields go through a wheel pointer `w = &r->w[k]` re-taken before
   each wheel's run: with `r->w[k].f` the compiler hoists the 1.0f/1 stores and the push esi to the top
   (89.8%). Descending zero runs are chained assignments. Statement order = the original store order. */
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
    int f_000;
    int f_004;
    undefined1 pad_008[0x4];
    int f_00c;
    int f_010;
    int f_014;
    undefined1 pad_018[0x54];
    int f_06c;
    float f_070;
    int f_074;
    undefined1 pad_078[0x38];
    int f_0b0;
    int f_0b4;
    undefined1 pad_0b8[0xc];
} Wheel_0046baa0;
typedef struct { int id; undefined1 pad[0x3c]; } Slot_0046baa0;
typedef struct { float x, y, z; } Vec3_0046baa0;
typedef struct {
    int f_000;
    undefined1 pad_004[0x10];
    int f_014;
    int f_018;
    int f_01c;
    int f_020;
    int f_024;
    int f_028;
    int f_02c;
    int f_030;
    int f_034;
    int f_038;
    int f_03c;
    int f_040;
    int f_044;
    int f_048;
    int f_04c;
    undefined1 pad_050[0x144];
    Wheel_0046baa0 w[4];
    undefined1 pad_4a4[0x8];
    Slot_0046baa0 slot[18];
    undefined1 pad_92c[0x84];
    int f_9b0;
    int f_9b4;
    int f_9b8;
    int f_9bc;
    int f_9c0;
    int f_9c4;
    undefined1 pad_9c8[0x18];
    float f_9e0;
    int f_9e4;
    int f_9e8;
    int f_9ec;
    int f_9f0;
    int f_9f4;
    undefined1 pad_9f8[0xd8];
    int f_ad0;
    int f_ad4;
    int f_ad8;
    int f_adc;
    int f_ae0;
    int f_ae4;
    int f_ae8;
    int f_aec;
    int f_af0;
    int f_af4;
    float f_af8;
    int f_afc;
    int f_b00;
    int f_b04;
    int f_b08;
    int f_b0c;
    int f_b10;
    int f_b14;
    int f_b18;
    int f_b1c;
    undefined1 pad_b20[0x4];
    int f_b24;
    int f_b28;
    Vec3_0046baa0 dir[16];
    int f_bec;
    undefined1 pad_bf0[0x114];
} Rec_008815a0;
MGLOBALA(Rec_008815a0, DAT_008815a0, 16, 0x008815a0);

// MATCH: 0x0046baa0 FUN_0046baa0
undefined4 FUN_0046baa0(uint param_1)
{
    Rec_008815a0 *r;
    int i;
    Wheel_0046baa0 *w;
    if (param_1 >= 0x10) return 0;
    r = &DAT_008815a0[param_1];
    r->f_000 = param_1;
    w = &r->w[0];
    w->f_00c = 0;
    w->f_010 = 0;
    w->f_014 = 0;
    w->f_000 = 0;
    w->f_06c = 0;
    w->f_074 = 0;
    w = &r->w[1];
    w->f_00c = 0;
    w->f_010 = 0;
    w->f_014 = 0;
    w->f_000 = 0;
    w->f_06c = 0;
    w->f_074 = 0;
    w = &r->w[2];
    w->f_00c = 0;
    w->f_010 = 0;
    w->f_014 = 0;
    w->f_000 = 0;
    w->f_06c = 0;
    w->f_074 = 0;
    w = &r->w[0];
    w->f_070 = 1.0f;
    w = &r->w[1];
    w->f_070 = 1.0f;
    w = &r->w[2];
    w->f_070 = 1.0f;
    w = &r->w[0];
    w->f_004 = 1;
    w = &r->w[1];
    w->f_004 = 1;
    w = &r->w[2];
    w->f_004 = 1;
    w = &r->w[3];
    w->f_070 = 1.0f;
    w->f_00c = 0;
    w->f_010 = 0;
    w->f_014 = 0;
    w->f_004 = 1;
    w->f_000 = 0;
    w->f_06c = 0;
    w->f_074 = 0;
    r->f_9e0 = 4.0f;
    r->f_b14 = r->f_b18 = r->f_b1c = 0;
    r->f_af0 = 0;
    r->f_af4 = 0;
    r->f_af8 = -1.0f;
    r->f_afc = 0;
    for (i = 0; i < 16; i++) {
        r->dir[i].x = 0;
        r->dir[i].y = -1.0f;
        r->dir[i].z = 0;
    }
    r->f_bec = 0;
    r->f_9b0 = r->f_9b4 = r->f_9b8 = r->f_9bc = r->f_9c0 = r->f_9c4 = 0;
    r->f_ad4 = r->f_ad8 = r->f_adc = r->f_ae0 = r->f_ae4 = r->f_ae8 = 0;
    r->f_9e4 = r->f_9e8 = 0;
    r->f_b00 = r->f_b04 = r->f_b08 = 0;
    r->f_9ec = 0;
    r->f_9f0 = 0;
    r->f_9f4 = 0;
    r->f_ad0 = 0;
    w->f_0b0 = 0;
    w->f_0b4 = 0;
    r->f_b24 = 0;
    r->f_b28 = 0;
    r->f_aec = 0;
    r->f_b0c = 0;
    r->f_b10 = 0;
    for (i = 0; i < 18; i++) {
        r->slot[i].id = -1;
    }
    r->f_014 = 0;
    r->f_018 = 0;
    r->f_01c = 0;
    r->f_020 = r->f_024 = r->f_028 = r->f_02c = r->f_030 = r->f_034 = 0;
    r->f_038 = r->f_03c = r->f_040 = r->f_044 = r->f_048 = r->f_04c = 0;
    return 1;
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046b1c0 (h2): 97.7% (regs 97.7%). Raw 24%. Typed 0xd04 record at 0x008815a0 with vec3 p[18]
   at +0x60 (this function writes p[4..17]). Each derived point goes through one Vec3 local `t` (a struct
   temp gives the original's 12-byte frame and its [esp]/[esp+8] spills; three float locals give 61%).
   The 0.5 and 0.333333 multipliers are literals (the compiler's constants land at 0x005cc32c/0x005ce034);
   through a MGLOBAL extern each multiply becomes fld + fmul st(N) (74%). Tried: direct expressions,
   swapped operands, scaled temp + struct copy, __inline VecAdd/VecScale helpers (88%), distinct temps
   per group, permuter (589 compiles, no gain).
   Residual (b): x87 scheduling of the first midpoint only - the original issues `fadd [eax+0x9c]` one store
   earlier and the `fstp [esp+8]; fxch; fmul` triple 20 stores later than ours. */
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
typedef struct { float x, y, z; } Vec3_0046b1c0;
typedef struct {
    undefined1 pad0[0x60];
    Vec3_0046b1c0 p[18];
    undefined1 pad1[0xd04 - 0x60 - 18 * 12];
} Rec_008815a0;
MGLOBALA(Rec_008815a0, DAT_008815a0, 16, 0x008815a0);

// MATCH: 0x0046b1c0 FUN_0046b1c0
undefined4 FUN_0046b1c0(uint param_1, float *param_2)
{
    Rec_008815a0 *r;
    Vec3_0046b1c0 t;
    if (param_1 >= 0x10) return 0;
    r = &DAT_008815a0[param_1];
    r->p[4].x = param_2[0]; r->p[4].y = param_2[4]; r->p[4].z = param_2[2];
    r->p[5].x = param_2[3]; r->p[5].y = param_2[4]; r->p[5].z = param_2[2];
    r->p[6].x = param_2[0]; r->p[6].y = param_2[4]; r->p[6].z = param_2[5];
    r->p[7].x = param_2[3]; r->p[7].y = param_2[4]; r->p[7].z = param_2[5];
    r->p[8].x = param_2[0]; r->p[8].y = param_2[1]; r->p[8].z = param_2[2];
    r->p[9].x = param_2[3]; r->p[9].y = param_2[1]; r->p[9].z = param_2[2];
    r->p[10].x = param_2[0]; r->p[10].y = param_2[1]; r->p[10].z = param_2[5];
    r->p[11].x = param_2[3]; r->p[11].y = param_2[1]; r->p[11].z = param_2[5];
    t.x = r->p[4].x + r->p[5].x;
    t.y = r->p[4].y + r->p[5].y;
    t.z = r->p[4].z + r->p[5].z;
    r->p[12].x = t.x * 0.5f;
    r->p[12].y = t.y * 0.5f;
    r->p[12].z = t.z * 0.5f;
    t.x = r->p[5].x + r->p[5].x;
    t.y = r->p[5].y + r->p[5].y;
    t.z = r->p[5].z + r->p[5].z;
    t.x = t.x + r->p[7].x;
    t.y = t.y + r->p[7].y;
    t.z = t.z + r->p[7].z;
    r->p[13].x = t.x * 0.333333f;
    r->p[13].y = t.y * 0.333333f;
    r->p[13].z = t.z * 0.333333f;
    t.x = r->p[7].x + r->p[7].x;
    t.y = r->p[7].y + r->p[7].y;
    t.z = r->p[7].z + r->p[7].z;
    t.x = t.x + r->p[5].x;
    t.y = t.y + r->p[5].y;
    t.z = t.z + r->p[5].z;
    r->p[14].x = t.x * 0.333333f;
    r->p[14].y = t.y * 0.333333f;
    r->p[14].z = t.z * 0.333333f;
    t.x = r->p[7].x + r->p[6].x;
    t.y = r->p[7].y + r->p[6].y;
    t.z = r->p[7].z + r->p[6].z;
    r->p[15].x = t.x * 0.5f;
    r->p[15].y = t.y * 0.5f;
    r->p[15].z = t.z * 0.5f;
    t.x = r->p[6].x + r->p[6].x;
    t.y = r->p[6].y + r->p[6].y;
    t.z = r->p[6].z + r->p[6].z;
    t.x = t.x + r->p[4].x;
    t.y = t.y + r->p[4].y;
    t.z = t.z + r->p[4].z;
    r->p[16].x = t.x * 0.333333f;
    r->p[16].y = t.y * 0.333333f;
    r->p[16].z = t.z * 0.333333f;
    t.x = r->p[4].x + r->p[4].x;
    t.y = r->p[4].y + r->p[4].y;
    t.z = r->p[4].z + r->p[4].z;
    t.x = t.x + r->p[6].x;
    t.y = t.y + r->p[6].y;
    t.z = t.z + r->p[6].z;
    r->p[17].x = t.x * 0.333333f;
    r->p[17].y = t.y * 0.333333f;
    r->p[17].z = t.z * 0.333333f;
    return 1;
}

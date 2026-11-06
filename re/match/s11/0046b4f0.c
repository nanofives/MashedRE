// FLAGS: /O2 /GS /Ob1
/* MATCH-S11 0x0046b4f0 (h2): 100% (regs 100%). Raw converter did not compile. Typed the 0xd04-byte record
   array at 0x008815a0 (vec3[18] at +0x60), took a record pointer, and wrote two separate early returns:
   the combined `a && b` / `a || b` forms place the return-0 block at the end (83.3%). */
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
typedef struct { float x, y, z; } Vec3_0046b4f0;
typedef struct {
    undefined1 pad0[0x60];
    Vec3_0046b4f0 v[18];
    undefined1 pad1[0xd04 - 0x60 - 18 * 12];
} Rec_008815a0;
MGLOBALA(Rec_008815a0, DAT_008815a0, 16, 0x008815a0);

// MATCH: 0x0046b4f0 FUN_0046b4f0
undefined4 FUN_0046b4f0(float *param_1, uint param_2, int param_3)
{
    Rec_008815a0 *r;
    if (param_2 >= 0x10) return 0;
    if (param_3 >= 0x12) return 0;
    r = &DAT_008815a0[param_2];
    param_1[0] = r->v[param_3].x;
    param_1[1] = r->v[param_3].y;
    param_1[2] = r->v[param_3].z;
    return 1;
}

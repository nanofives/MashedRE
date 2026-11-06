// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048ebf0 (h3): 71.8% (regs 92.9%). Raw did not compile. Smoke/ring emitter over a 4-entry
   0x488-byte struct array at 0x0076d994 (0x0048ebc0 reads its +0 field). Shape: pop a queued position
   from stack[--stackn] into *param_1 and divide param_2 by rand(1,1.2); s = min(param_2*2.5,1) (the
   compiler keeps s in param_1's stack home, as the original); first-call origin/r0/r1; one ring entry;
   then puffs [count*n, count*n+n) with the i<n / else split. Literals for every constant (the globals
   reverse fld/fmul), exact-bit literals 0.0022500001f / 0.0056249998f, field-wise Vec3 copies.
   Residual (b): register assignment (original esi=param_3, ebp=param_3*0x488, edi=param_1, ebx=k; ours
   rotates ebx/ebp/edi/esi) and the first-call block, where the original recomputes param_3*0x488 twice
   (eax for r0/r1, edx for rt) and stores origin.z after r0/r1: a source form not found (tried an
   __inline helper for r0/r1/rt, tail store order). Permuter 600 s reached 79.6% only by moving
   `k = nring` below the first rpos stores (changes behaviour, rejected). */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct Vec3_48eb { float x, y, z; } Vec3_48eb;
typedef struct Smoke_48eb {
    int per;               /* +0x000 */
    int count;             /* +0x004 */
    int f8;                /* +0x008 */
    int fc;                /* +0x00c */
    int f10;               /* +0x010 */
    int stackn;            /* +0x014 */
    int nring;             /* +0x018 */
    Vec3_48eb pos[8];      /* +0x01c */
    Vec3_48eb vel[8];      /* +0x07c */
    float size[8][2];      /* +0x0dc */
    float rot[8];          /* +0x11c */
    float spin[8];         /* +0x13c */
    Vec3_48eb origin;      /* +0x15c */
    float r0;              /* +0x168 */
    float r1;              /* +0x16c */
    int rt;                /* +0x170 */
    Vec3_48eb rpos[8];     /* +0x174 */
    Vec3_48eb rvel[8];     /* +0x1d4 */
    float rsize[8][2];     /* +0x234 */
    float ralpha[8];       /* +0x274 */
    float rx[8];           /* +0x294 */
    float ry[8];           /* +0x2b4 */
    int rz[8];             /* +0x2d4 */
    Vec3_48eb stack[26];   /* +0x2f4 */
    int pad42c[2];         /* +0x42c */
    int stackflag[21];     /* +0x434 */
} Smoke_48eb;
MGLOBALA(Smoke_48eb, DAT_0076d994, 4, 0x0076d994);
float FUN_00472650(float lo, float hi);
int FUN_0048ebc0(int idx);
// MATCH: 0x0048ebf0 FUN_0048ebf0
void FUN_0048ebf0(Vec3_48eb *param_1, float param_2, int param_3)
{
    int k;
    int i;
    int n;
    int end;
    float s;
    float v;
    float lo;
    float hi;
    if (DAT_0076d994[param_3].count != 0) {
        k = --DAT_0076d994[param_3].stackn;
        param_1->x = DAT_0076d994[param_3].stack[k].x;
        param_1->y = DAT_0076d994[param_3].stack[k].y;
        param_1->z = DAT_0076d994[param_3].stack[k].z;
        param_2 = param_2 / FUN_00472650(1.0f, 1.2f);
        DAT_0076d994[param_3].stackflag[k] = 0;
    }
    s = param_2 * 2.5f;
    if (s > 1.0f) {
        s = 1.0f;
    }
    if (DAT_0076d994[param_3].count == 0) {
        v = param_2 * 1.2f;
        DAT_0076d994[param_3].origin.x = param_1->x;
        DAT_0076d994[param_3].origin.y = param_1->y;
        DAT_0076d994[param_3].origin.z = param_1->z;
        DAT_0076d994[param_3].r0 = v;
        DAT_0076d994[param_3].r1 = v;
        DAT_0076d994[param_3].rt = 0;
    }
    k = DAT_0076d994[param_3].nring;
    DAT_0076d994[param_3].rpos[k].x = param_1->x;
    DAT_0076d994[param_3].rpos[k].y = param_1->y;
    DAT_0076d994[param_3].rpos[k].z = param_1->z;
    DAT_0076d994[param_3].rsize[k][0] = param_2;
    DAT_0076d994[param_3].rsize[k][1] = param_2;
    DAT_0076d994[param_3].rz[k] = 0;
    DAT_0076d994[param_3].ralpha[k] = 255.0f;
    DAT_0076d994[param_3].rx[k] = FUN_00472650(0.0f, 0.5f) + 0.5f;
    DAT_0076d994[param_3].rvel[k].x = FUN_00472650(0.0f, 0.0005f) * s;
    DAT_0076d994[param_3].rvel[k].y = FUN_00472650(0.0022500001f, 0.005f) * s;
    DAT_0076d994[param_3].rvel[k].z = FUN_00472650(0.0f, 0.0005f) * s;
    DAT_0076d994[param_3].nring++;
    n = FUN_0048ebc0(param_3);
    i = DAT_0076d994[param_3].count * n;
    end = n + i;
    for (; i < end; i++) {
        DAT_0076d994[param_3].vel[i].x = FUN_00472650(0.0f, 0.001f) * s;
        if (i & 1) {
            DAT_0076d994[param_3].vel[i].y = FUN_00472650(0.0056249998f, 0.0075f) * s;
        } else {
            DAT_0076d994[param_3].vel[i].y = FUN_00472650(0.001875f, 0.0025f) * s;
        }
        DAT_0076d994[param_3].vel[i].z = FUN_00472650(0.0f, 0.001f) * s;
        if (i < n) {
            lo = param_2 * -0.2f;
            hi = param_2 * 0.2f;
            DAT_0076d994[param_3].pos[i].x = FUN_00472650(lo, hi) + param_1->x;
            DAT_0076d994[param_3].pos[i].z = FUN_00472650(lo, hi) + param_1->z;
            DAT_0076d994[param_3].pos[i].y = FUN_00472650(param_2 * 0.05f, param_2 * 0.15f) + param_1->y;
            s = FUN_00472650(0.05f, 0.25f) + param_2 * 0.33333334f;
            v = s;
        } else {
            lo = param_2 * -0.1f;
            hi = param_2 * 0.1f;
            DAT_0076d994[param_3].pos[i].x = FUN_00472650(lo, hi) + param_1->x;
            DAT_0076d994[param_3].pos[i].y = param_1->y;
            DAT_0076d994[param_3].pos[i].z = FUN_00472650(lo, hi) + param_1->z;
            DAT_0076d994[param_3].vel[i].y = DAT_0076d994[param_3].vel[i].y * 0.5f;
            v = param_2 * 0.25f;
        }
        DAT_0076d994[param_3].size[i][0] = v;
        DAT_0076d994[param_3].size[i][1] = v;
        DAT_0076d994[param_3].rot[i] = FUN_00472650(-20.0f, 20.0f) + 140.0f;
        DAT_0076d994[param_3].spin[i] = FUN_00472650(0.0f, 0.4f) + 0.4f;
    }
    DAT_0076d994[param_3].f8 = 0;
    DAT_0076d994[param_3].count++;
}

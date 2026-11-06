// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x004864f0 (h4): best 90.1% (regs 90.1%; Ghidra size 273 B stops before the loop tail, so 6 extra ours-only lines are the real dec esi/jne/epilogue; whole symbol 282 B differs only in the fld scheduling below). This file also holds its register-arg static callee 0x00486370 (flag in EAX, pilot finding 1) at 100%. Moves: .rdata constants 0x005cc9a0 (0.05f) and 0x005cc950 (0.75f) written as float literals (as globals the compiler put the constant first, 74%); distance test as !(dz*dz + (dx*dx + dy*dy) < 0.05f) (other groupings 75%); in 0x00486370 sc.x = sc.y = s * 0.75f (store order). Residual: in the loop the fld of v.y is scheduled one instruction before lea edx instead of after; survived *=, x = x * k, an __inline scale helper, float[3], statement reorders and 162 permuter variants. Class (b). */
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
typedef struct V3 { float x, y, z; } V3;
typedef struct Mat { V3 right; unsigned int flags; V3 up; unsigned int pad1; V3 at; unsigned int pad2; V3 pos; unsigned int pad3; } Mat;
typedef struct S7c {
    V3 pos;
    int f0c, f10;
    float f14;
    float f18;
    int f1c, f20, f24, f28;
    float f2c;
    Mat m;
    int f70, f74, f78;
} S7c;
MGLOBALA(S7c, DAT_006fe790, 150, 0x006fe790);
MGLOBAL(int, DAT_00703038, 0x00703038);
MGLOBAL(float, DAT_006fe780, 0x006fe780);
MGLOBAL(float, DAT_006fe784, 0x006fe784);
MGLOBAL(float, DAT_006fe788, 0x006fe788);
float FUN_00472650(float lo, float hi);
Mat *FUN_004c5010(Mat *m, V3 *v, int op);
Mat *FUN_004c51a0(Mat *m, V3 *v, int op);

// MATCH: 0x00486370 FUN_00486370
static void FUN_00486370(int flag, Mat *m, float s)
{
    V3 sc;
    S7c *p;
    sc.x = 1.0f;
    sc.y = 1.0f;
    sc.z = 1.0f;
    if (DAT_00703038 >= 150) {
        DAT_00703038 = 0;
    }
    p = &DAT_006fe790[DAT_00703038];
    if (flag) {
        sc.x = sc.y = s * 0.75f;
        p->m = *FUN_004c5010(m, &sc, 1);
        p->f78 = 1;
    } else {
        p->f78 = 0;
    }
    p->f70 = 1;
    p->f20 = 0;
    p->pos = m->pos;
    p->f14 = s;
    p->f0c = 0;
    p->f18 = FUN_00472650(0.6f, 1.3f);
    p->f1c = 0;
    p->f74 = 0;
    p->f2c = 50.0f;
    DAT_00703038++;
}

// MATCH: 0x004864f0 FUN_004864f0
void FUN_004864f0(Mat *param_1, float param_2, int param_3)
{
    Mat m;
    V3 v;
    float dx, dy, dz;
    m = *param_1;
    dx = m.pos.x - DAT_006fe780;
    dy = m.pos.y - DAT_006fe784;
    dz = m.pos.z - DAT_006fe788;
    if (!(dz * dz + (dx * dx + dy * dy) < 0.05f)) {
        DAT_006fe780 = m.pos.x;
        DAT_006fe784 = m.pos.y;
        DAT_006fe788 = m.pos.z;
        FUN_00486370(1, &m, param_2);
        for (; param_3 != 0; param_3--) {
            v.x = FUN_00472650(-1.0f, 1.0f);
            v.y = 0.0f;
            v.z = FUN_00472650(-1.0f, 1.0f);
            v.x *= 0.05f;
            v.y *= 0.05f;
            v.z *= 0.05f;
            FUN_004c51a0(&m, &v, 2);
            FUN_00486370(0, &m, param_2);
        }
    }
}

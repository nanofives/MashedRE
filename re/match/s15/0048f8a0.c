// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048f8a0 (h3): 100% (regs 100%), relocheck OK (unchecked: __ftol).
   256-entry 0x38-byte particle array at 0x0076a100 (loop pointer = &arr[i].handle). What moved it (raw 21%):
   indexed for-loop (cmp/jl end pointer), Vec3 struct copies for the position (integer moves),
   kill branch first (`x < 16.0` gives test ah,5/jp), and the 0.006f scale as a literal
   (the global DAT_005cf4f4 reversed the fld/fmul operand order). No residual. */
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
typedef struct Vec3_48f8 { float x, y, z; } Vec3_48f8;
typedef struct Part48f8 {
    int active;        /* +0x00 */
    Vec3_48f8 pos;     /* +0x04 */
    float vx, vy, vz;  /* +0x10 */
    float a;           /* +0x1c */
    float b;           /* +0x20 */
    float rate;        /* +0x24 */
    unsigned char col[4]; /* +0x28 */
    float fade;        /* +0x2c */
    int follow;        /* +0x30 */
    int handle;        /* +0x34 */
} Part48f8;
MGLOBALA(Part48f8, DAT_0076a100, 256, 0x0076a100);
MGLOBAL(float, DAT_005cc750, 0x005cc750);
MGLOBAL(char, DAT_00766938, 0x00766938);
int FUN_0041f290(int a, int b);
float FUN_00472650(float lo, float hi);
void FUN_004769a0(float *p);
void FUN_004769d0(float *p);
void FUN_004769f0(unsigned char *p);
void FUN_00476d00(char *t);
void FUN_00476df0(char *t);
// MATCH: 0x0048f8a0 FUN_0048f8a0
void FUN_0048f8a0(void)
{
    int i;
    float d;
    Vec3_48f8 pos;
    for (i = 0; i < 256; i++) {
        Part48f8 *p = &DAT_0076a100[i];
        if (p->active) {
            if (p->follow) {
                pos = *(Vec3_48f8 *)(FUN_0041f290(p->handle, 0x33) + 0x30);
                pos.x = FUN_00472650(-0.05f, 0.05f) + pos.x;
                pos.y = FUN_00472650(0.15f, 0.25f) + pos.y;
                pos.z = FUN_00472650(-0.05f, 0.05f) + pos.z;
            } else {
                p->pos.x = p->vx + p->pos.x;
                p->pos.y = p->vy + p->pos.y;
                p->pos.z = p->vz + p->pos.z;
                pos = p->pos;
            }
            d = p->rate * 0.006f;
            p->a = d + p->a;
            p->b = d + p->b;
            if ((float)p->col[3] - p->fade < DAT_005cc750) {
                p->active = 0;
            } else {
                p->col[3] = (unsigned char)((float)p->col[3] - p->fade);
                FUN_004769f0(p->col);
                FUN_004769a0(&pos.x);
                FUN_004769d0(&p->a);
                FUN_00476d00(&DAT_00766938);
            }
        }
    }
    FUN_00476df0(&DAT_00766938);
}

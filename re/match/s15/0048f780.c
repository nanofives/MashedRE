// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x0048f780 (h3): 98.6% (regs 98.6%). Raw 43.5%. Spawn into the 256-entry particle array
   at 0x0076a100 (same struct as 0x0048f8a0). What moved it: struct array + indexed search loop with the
   body inside (return on first free slot), field-wise copy of the position (a struct copy built a base
   pointer), 0.001f literal (the global reversed fld/fmul), permuter store order.
   Residual (b): the fade fstp lands one store later than the original (after the b store instead of
   before the rate store). Tried all orders of the a/fade/rate/b/follow/col stores (inline product and
   `param_4 = param_4 * 0.001f` first), `0.001f * param_4`, chained a=b, int copy of the colour. */
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
typedef struct Vec3_48f7 { float x, y, z; } Vec3_48f7;
typedef struct Col_48f7 { unsigned char r, g, b, a; } Col_48f7;
typedef struct Part48f7 {
    int active;        /* +0x00 */
    Vec3_48f7 pos;     /* +0x04 */
    Vec3_48f7 vel;     /* +0x10 */
    float a;           /* +0x1c */
    float b;           /* +0x20 */
    float rate;        /* +0x24 */
    Col_48f7 col;      /* +0x28 */
    float fade;        /* +0x2c */
    int follow;        /* +0x30 */
    int handle;        /* +0x34 */
} Part48f7;
MGLOBALA(Part48f7, DAT_0076a100, 256, 0x0076a100);
float FUN_00472650(float lo, float hi);
// MATCH: 0x0048f780 FUN_0048f780
void FUN_0048f780(Vec3_48f7 *param_1, float param_2, Col_48f7 *param_3, float param_4,
                  float param_5, int param_6, int param_7)
{
    int i;
    for (i = 0; i < 256; i++) {
        if (DAT_0076a100[i].active == 0) {
            if (param_7) {
                DAT_0076a100[i].pos.x = FUN_00472650(-0.05f, 0.05f);
                DAT_0076a100[i].pos.y = FUN_00472650(0.15f, 0.25f);
                DAT_0076a100[i].pos.z = FUN_00472650(-0.05f, 0.05f);
            } else {
                DAT_0076a100[i].pos.x = param_1->x;
                DAT_0076a100[i].pos.y = param_1->y;
                DAT_0076a100[i].pos.z = param_1->z;
            }
            DAT_0076a100[i].vel.x = FUN_00472650(0.0025f, 0.005f);
            DAT_0076a100[i].vel.y = FUN_00472650(0.01f, 0.02f);
            DAT_0076a100[i].vel.z = FUN_00472650(0.0025f, 0.005f);
            param_4 = param_4 * 0.001f;
            DAT_0076a100[i].a = param_2;
            DAT_0076a100[i].fade = param_4;
            DAT_0076a100[i].b = param_2;
            DAT_0076a100[i].rate = param_5;
            DAT_0076a100[i].col = *param_3;
            DAT_0076a100[i].follow = param_7;
            DAT_0076a100[i].active = 1;
            DAT_0076a100[i].handle = param_6;
            return;
        }
    }
}

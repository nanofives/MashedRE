// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00455100 (h4): 100% (regs 100%), 25/25 ins on the first hand pass. Raw converter 45.9%.
   What moved it: the record as a struct, the frame pointer read into a local first, and the .rdata
   constants as float literals (1.0f 0x005cc320, 0.0f 0x005d757c, 25.0f pushed as 0x41c80000).
   Residual: none. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct Obj00455100 { int f00; void *frame; } Obj00455100;
typedef struct Rec00455100 {
    int pad00[7];
    int f1c;            /* +0x1c */
    Obj00455100 *obj;   /* +0x20 */
    float f24;          /* +0x24 */
    float f28;          /* +0x28 */
} Rec00455100;
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(float, DAT_00614708, 3, 0x00614708);
int FUN_004c1520(void *p0, float *p1, float p2, int p3);
// MATCH: 0x00455100 FUN_00455100
void FUN_00455100(Rec00455100 *r)
{
    void *fr = r->obj->frame;
    r->f24 = r->f24 + 1.0f;
    FUN_004c1520(fr, DAT_00614708, 25.0f, 1);
    r->f28 = r->f28 - DAT_007f100c;
    if (r->f28 <= 0.0f) {
        r->f28 = 0;
        r->f1c = 0;
    }
}

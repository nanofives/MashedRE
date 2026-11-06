// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00455060 (h4): 100% (regs 100%), 46/46 ins. Raw converter 87.0% (regs 100).
   What moved it: every field written as DAT_006885d0[i].f and the result as `return &DAT_006885d0[i]`
   (the compiler then keeps that address in EBX and stores the owner through it). A local `r` gave 93.5%
   (owner stored as [esi+disp]), the permuter found nothing from there. 0x0045bfd0 takes no argument
   (its second call pushes the frame for 0x004e4450, not for itself). Residual: none. */
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
typedef struct Rec00455060 {
    void *owner;        /* +0x00 */
    int f04, f08;       /* +0x04 */
    int pad0c[3];
    void *frame;        /* +0x18 */
    int f1c;            /* +0x1c */
    int pad20[2];
    int f28;            /* +0x28 */
} Rec00455060;          /* 0x2c, records at 0x006885d0 */
MGLOBALA(Rec00455060, DAT_006885d0, 5, 0x006885d0);
void FUN_004595c0(int p0);
void FUN_00474d60(void *p0, int p1);
void *FUN_004e4800(void *p0);
void *FUN_004e45b0(void *p0, void *p1);
void *FUN_004e4450(void *p0, void *p1);
void *FUN_0045bfd0(void);
// MATCH: 0x00455060 FUN_00455060
Rec00455060 *FUN_00455060(char *obj)
{
    int i = *(int *)(obj + 0xb0);
    void *p;
    FUN_004595c0(i);
    DAT_006885d0[i].f04 = 1;
    DAT_006885d0[i].f08 = 1;
    DAT_006885d0[i].owner = obj;
    DAT_006885d0[i].f28 = 0;
    DAT_006885d0[i].f1c = 0;
    p = FUN_004e4800(DAT_006885d0[i].frame);
    if (p != FUN_0045bfd0()) {
        if (p != 0)
            FUN_004e45b0(p, DAT_006885d0[i].frame);
        FUN_004e4450(FUN_0045bfd0(), DAT_006885d0[i].frame);
    }
    FUN_00474d60(DAT_006885d0[i].frame, 1);
    return &DAT_006885d0[i];
}

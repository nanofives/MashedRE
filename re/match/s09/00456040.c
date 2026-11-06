// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00456040 (h4): 100% (regs 100%), 53/53 ins. Raw converter 96.2%: its only miss was the
   callee arity. 0x0045bfd0 takes no argument and 0x004e4450 takes two (parent, clump), which fixed the
   deferred `add esp` merge. The 0x48-B records at 0x006886b0 written as a struct array. Residual: none. */
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
typedef struct Owner00456040 { char pad[0xb0]; int slot; } Owner00456040;
typedef struct Rec00456040 {
    Owner00456040 *owner;   /* +0x00 */
    int f04;
    void *clump;            /* +0x08 */
    int f0c, f10, f14, f18, f1c, f20, f24, f28;
    int pad2c[7];
} Rec00456040;              /* 0x48, 4 records at 0x006886b0 */
MGLOBALA(Rec00456040, DAT_006886b0, 4, 0x006886b0);
void FUN_004595c0(int p0);
void FUN_00474d60(void *p0, int p1);
void *FUN_004e4450(void *p0, void *p1);
void *FUN_0045bfd0(void);
// MATCH: 0x00456040 FUN_00456040
Rec00456040 *FUN_00456040(Owner00456040 *o)
{
    FUN_004595c0(o->slot);
    DAT_006886b0[o->slot].owner = o;
    FUN_004e4450(FUN_0045bfd0(), DAT_006886b0[o->slot].clump);
    FUN_00474d60(DAT_006886b0[o->slot].clump, 1);
    DAT_006886b0[o->slot].f0c = 1;
    DAT_006886b0[o->slot].f14 = 0;
    DAT_006886b0[o->slot].f10 = 0x32;
    DAT_006886b0[o->slot].f18 = 0;
    DAT_006886b0[o->slot].f1c = 0;
    DAT_006886b0[o->slot].f28 = 0;
    DAT_006886b0[o->slot].f24 = 0;
    DAT_006886b0[o->slot].f20 = 0;
    return &DAT_006886b0[o->slot];
}

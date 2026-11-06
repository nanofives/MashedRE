// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00455b50 (h4): 100% (regs 100%), 67/67 ins. Raw converter 54.8%. The record pointer is
   EAX-passed (pilot finding 1): the file holds a stand-in caller (the 0x2c-B record loop of 0x00455c90,
   unmatched); no MATCH line on it, built only outside MATCH_PORT.
   What moved it: the pos pointer `FUN_004c0ed0(..) + 0x30` taken right away (kept in EBP), the two-way
   test as one `&&` with an else, and the final call written twice with the colour pointer chosen by
   `!= 0` (the compiler cross-jumps the shared tail, so each branch pushes its own constant; one call with
   a local or `?:` gave 92.5 / 98.5%). Residual: none. */
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
typedef struct Obj00455b50 { int f00; void *frame; } Obj00455b50;
typedef struct Rec00455b50 {
    void *owner;        /* +0x00 */
    int f04, f08;       /* +0x04 */
    int link;           /* +0x0c */
    int f10;            /* +0x10 */
    float f14;          /* +0x14 */
    Obj00455b50 *f18;   /* +0x18 */
    int f1c;            /* +0x1c */
    Obj00455b50 *f20;   /* +0x20 */
    float f24, f28;     /* +0x24 */
} Rec00455b50;          /* 0x2c, 5 records at 0x006885d0 */
MGLOBALA(Rec00455b50, DAT_006885d0, 5, 0x006885d0);
MGLOBAL(float, DAT_007f100c, 0x007f100c);
MGLOBALA(uchar, DAT_005f9814, 4, 0x005f9814);
MGLOBALA(uchar, DAT_005f9818, 4, 0x005f9818);
char *FUN_004c0ed0(void *p0);
int FUN_0045a0f0(int p0);
int FUN_0045a0d0(int p0);
int FUN_00407640(int p0);
void FUN_004c1480(void *p0, void *p1, int p2);
void FUN_00459620(int p0, float *p1, float p2, float p3, uchar *p4);
// MATCH: 0x00455b50 FUN_00455b50
static void FUN_00455b50(Rec00455b50 *r)
{
    float *pos = (float *)(FUN_004c0ed0(r->f20->frame) + 0x30);
    int i = r->link;
    if (i == -1)
        return;
    {
        int j = FUN_0045a0f0(i);
        FUN_004c1480(DAT_006885d0[i].f18->frame, DAT_006885d0[i].owner, 0);
        if (j == -1 && FUN_00407640(FUN_0045a0d0(i)) == -1) {
            DAT_006885d0[i].f14 = 0.5f;
            DAT_006885d0[i].f10 = 0;
        } else {
            DAT_006885d0[i].f14 = DAT_006885d0[i].f14 - DAT_007f100c;
        }
    }
    if (DAT_006885d0[i].f14 <= 0.0f) {
        DAT_006885d0[i].f10 = 1;
        DAT_006885d0[i].f14 = 0.0f;
    }
    if (DAT_006885d0[i].f10 != 0)
        FUN_00459620(i, pos, 8.0f, 30.0f, DAT_005f9818);
    else
        FUN_00459620(i, pos, 8.0f, 30.0f, DAT_005f9814);
}

#ifndef MATCH_PORT
/* stand-in caller: the record loop of 0x00455c90 (not matched) */
void StandIn_00455b50(void)
{
    Rec00455b50 *r;
    for (r = DAT_006885d0; r < DAT_006885d0 + 5; r++)
        if (r->f04 != 0)
            FUN_00455b50(r);
}
#endif

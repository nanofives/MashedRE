// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h5 0x004277a0 (147 B). Best 100.0% (regs 100.0%). Raw converter 36.2%.
   Static callee with a register convention (pilot finding 1): src in EAX, dst in EBX (cl 9176 picks them;
   param order (src, dst)). Callers 0x004278d0 / 0x00427990 / 0x00427ad0 (each file holds its own copy of this
   static); this file has a STAND-IN caller (not matched, not shipped) so the convention is generated.
   What moved it: `short n = *src++; short i; for (i = 0; i < n; i++) dst[i] = map(src[i])` (a short counter
   gives the movzx down-count); the converter's pointer-difference loop left `sub edx,ebx` mis-scheduled (98.0%),
   an int counter gave 52.9%. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0067d838, 0x0067d838);
MGLOBAL(int, DAT_0067d83c, 0x0067d83c);
MGLOBAL(float, DAT_008991c0, 0x008991c0);

unsigned short *FUN_00427780(int id);
int FUN_00552d10(void);
void FUN_00552d70(void);
float *FUN_00556e90(int a, void *b, void *c, void *d, void *e);
int FUN_005555b0(int a, short *text, float size, float *pos, int align, int f);

// MATCH: 0x004277a0 FUN_004277a0
static void FUN_004277a0(unsigned short *src, short *dst)
{
    short n = *src++;
    short i;
    for (i = 0; i < n; i++) {
        short c = src[i];
        if (c == 8) c = 0x81;
        else if (c == 9) c = 0x7f;
        else if (c == 10) c = 0x81;
        else if (c == 11) c = 0x8d;
        else if (c == 12) c = 0x80;
        else if (c == 13) c = 0x87;
        else if (c == 14) c = 0x8f;
        dst[i] = c;
    }
    dst[n] = 0;
}

/* stand-in caller (not matched, not shipped): text source in EAX, buffer in EBX, as 0x004278f7..0x004278fb do */
void FUN_004277a0_standin(int id)
{
    short text[512];
    FUN_004277a0(FUN_00427780(id), text);
    FUN_00552d10();
    FUN_005555b0(DAT_0067d838, text, 1.0f, &DAT_008991c0, 0, DAT_0067d83c);
}

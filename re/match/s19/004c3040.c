// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c3040 (h2): 91.2%, regs 91.2%. Hand: RwGlobals as a 0x4b-dword struct, struct copy gives rep movsd; pointer swapped to the static copy before the copy. Residual (b): add eax,0x10 / mov ecx,0x4b / pop edi each one slot off; copy-first order 79.4%, permuter 92 compiles no gain. */
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
typedef struct { int w[0x4b]; } RwGlobals_t;
typedef void (*freefn_t)(void *);
MGLOBAL(RwGlobals_t *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(freefn_t, DAT_007d3fd4, 0x007d3fd4);
MGLOBAL(int, DAT_007d3ff4, 0x007d3ff4);
MGLOBAL(RwGlobals_t, DAT_007d3ec8, 0x007d3ec8);
int FUN_004c2c90(void *p0, int p1, void *p2, int p3, int p4);
// MATCH: 0x004c3040 FUN_004c3040
int FUN_004c3040(void)
{
  int ok = FUN_004c2c90(&DAT_007d3ff8->w[4], 1, 0, 0, 0);
  if (ok) {
    RwGlobals_t *old = DAT_007d3ff8;
    DAT_007d3ff8 = &DAT_007d3ec8;
    DAT_007d3ec8 = *old;
    DAT_007d3fd4(old);
    DAT_007d3ff4--;
    DAT_007d3ff8->w[0x49] = 1;
  }
  return ok;
}

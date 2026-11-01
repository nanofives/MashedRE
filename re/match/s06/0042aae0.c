// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h11 0x0042aae0 (287 B): 100.0% exact (regs 100, ops 100), byte-identical.
   Raw converter 43.2%. RwGlobals (DAT_007d3ff8) slots +0x18 (float copied), +0x20 render-state set with 2
   arguments (the raw converter's third argument is ECX garbage), +0x30 Im2D primitive; 4 vertex records of 0x1c
   bytes at 0x0067ec30. Natural statement order matched first try. Residual: none. */
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
/* RwGlobals (DAT_007d3ff8) device slots used here: +0x18 float, +0x20 render-state set (state, value),
   +0x30 Im2D primitive (type, verts, n). Vertex records are 0x1c bytes, 4 of them at 0x0067ec30. */
typedef int (*RwStateSetFn_aae0)(int, int);
typedef int (*RwIm2DPrimFn_aae0)(int, void *, int);
typedef struct {
  char pad00[0x18];
  float f18;
  float f1c;
  RwStateSetFn_aae0 fpRenderStateSet;
  char pad24[0x0c];
  RwIm2DPrimFn_aae0 fpIm2DRenderPrimitive;
} RwGlobals_aae0;
typedef struct {
  float x;
  float y;
  float z;
  float rhw;
  unsigned int color;
  float u;
  float v;
} Vtx_aae0;
MGLOBAL(RwGlobals_aae0 *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(short, DAT_0067ea54, 0x0067ea54);
MGLOBAL(short, DAT_0067ea56, 0x0067ea56);
MGLOBALA(Vtx_aae0, DAT_0067ec30, 4, 0x0067ec30);
MGLOBAL(int, DAT_0067eca8, 0x0067eca8);
// MATCH: 0x0042aae0 FUN_0042aae0
void FUN_0042aae0(void)
{
  int i;
  DAT_007d3ff8->fpRenderStateSet(0x14, 1);
  DAT_0067ec30[0].x = 0;
  DAT_0067ec30[0].y = 0;
  DAT_0067ec30[1].x = (float)DAT_0067ea54;
  DAT_0067ec30[1].y = 0;
  DAT_0067ec30[2].x = 0;
  DAT_0067ec30[2].y = (float)DAT_0067ea56;
  DAT_0067ec30[3].x = (float)DAT_0067ea54;
  DAT_0067ec30[3].y = (float)DAT_0067ea56;
  for (i = 0; i < 4; i++) {
    DAT_0067ec30[i].color = 0xd0808080;
    DAT_0067ec30[i].z = DAT_007d3ff8->f18;
    DAT_0067ec30[i].rhw = 1.0f;
  }
  DAT_007d3ff8->fpRenderStateSet(1, 0);
  DAT_007d3ff8->fpRenderStateSet(6, 0);
  DAT_007d3ff8->fpRenderStateSet(8, 0);
  DAT_007d3ff8->fpRenderStateSet(0xc, 1);
  DAT_007d3ff8->fpRenderStateSet(10, 5);
  DAT_007d3ff8->fpRenderStateSet(0xb, 6);
  DAT_0067ec30[0].color = (unsigned char)DAT_0067eca8 << 24;
  DAT_0067ec30[1].color = DAT_0067ec30[0].color;
  DAT_0067ec30[2].color = DAT_0067ec30[0].color;
  DAT_0067ec30[3].color = DAT_0067ec30[0].color;
  if (DAT_0067eca8)
    DAT_007d3ff8->fpIm2DRenderPrimitive(4, DAT_0067ec30, 4);
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h4 0x00490020: 90.9% (regs 97.9%, ops 99.6%), 242/242 instructions, by hand from the decomp
   (raw converter 22.7%). Moved the score: ring of 30 0x50-byte records at 0x0086a4a0 as an array of
   structs with the whole spawn body inside the free-slot loop (a separate find loop unrolls 10x); the
   positional case as switch(mode) whose cases only make the FUN_0041f290 call and share the tail (the
   original cross-jumps push 0x33/0x34); p = *src struct copies (keeps y in edi); colour pointer table
   initialised after the six colours; the y-velocity temp as double (the original keeps the unrounded
   FUN_00472650 return on the x87 stack: fld [vel+4]; fadd st(1); later fstp st(0)).
   Tried without effect: declaration orders, colour array, index pointer local, int[4]/struct form of
   DAT_007f1a1c, unsigned player/mode, if-chain/ternary call (worse), permuter 2x300 s (+0.4 via unsigned i,
   not kept).
   Residual (b): eax/ecx swapped for player/mode in the head and the default branch (register colouring
   tie-break), plus the scheduling of the mov al,0xff / mov dl,0x54 colour-byte stores. */
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
typedef struct { unsigned char red, green, blue, alpha; } RwRGBA;
typedef struct { float x, y, z; } RwV3d;
typedef struct {
  int active;      /* +0x00 */
  int pad04;       /* +0x04 */
  int pad08;       /* +0x08 */
  int obj;         /* +0x0c */
  RwV3d pos;       /* +0x10 */
  RwV3d vel;       /* +0x1c */
  RwV3d scale;     /* +0x28 */
  int f34;         /* +0x34 */
  int f38;         /* +0x38 */
  int f3c;         /* +0x3c */
  float f40[3];    /* +0x40 */
  int f4c;         /* +0x4c */
} Spark0086a4a0;
MGLOBALA(int, DAT_007f1a1c, 16, 0x007f1a1c);
MGLOBALA(Spark0086a4a0, DAT_0086a4a0, 30, 0x0086a4a0);
MGLOBALA(char, s_tritex_005cf500, 7, 0x005cf500);
int FUN_0040bb50(char *name);
char *FUN_0041f290(int a, int b);
int FUN_0042fab0(int a);
float FUN_00472650(float lo, float hi);
int FUN_00472690(int lo, int hi);
int FUN_004e8090(int obj, int tex);
// MATCH: 0x00490020 FUN_00490020
void FUN_00490020(RwV3d *pos, RwRGBA *colour, int player, int mode, int tex, float size, RwV3d *vel)
{
  RwRGBA c0 = {0x78, 0x19, 0x1e, 0xff};
  RwRGBA c1 = {4, 0x54, 0xa4, 0xff};
  RwRGBA c2 = {0x3c, 0x7c, 0x4c, 0xff};
  RwRGBA c3 = {0xe4, 0xc4, 0x3c, 0xff};
  RwRGBA c4 = {0xd4, 0x54, 0x9c, 0xff};
  RwRGBA c5 = {0, 0, 0, 0xff};
  RwRGBA *cols[6] = {&c0, &c1, &c2, &c3, &c4, &c5};
  RwRGBA white = {0xff, 0xff, 0xff, 0xff};
  RwV3d p, s;
  RwRGBA col;
  char *m;
  int idx;
  int i;
  double vy;


  idx = DAT_007f1a1c[player * 4];
  switch (mode) {
  case 1:
    m = FUN_0041f290(player, 0x33);
    break;
  case 2:
    m = FUN_0041f290(player, 0x34);
    break;
  default:
    p = *pos;
    col = *colour;
    goto found;
  }
  p = *(RwV3d *)(m + 0x30);
  col.red = cols[idx]->red; col.green = cols[idx]->green;
  col.blue = cols[idx]->blue; col.alpha = cols[idx]->alpha;
found:
  for (i = 0; i < 30; i++) {
    if (DAT_0086a4a0[i].active == 0) {
      p.x += FUN_00472650(-0.001f, 0.001f);
      p.z += FUN_00472650(-0.001f, 0.001f);
      DAT_0086a4a0[i].pos = p;
      DAT_0086a4a0[i].f3c = 0;
      DAT_0086a4a0[i].f38 = FUN_00472690(0x14, 0x23);
      DAT_0086a4a0[i].f40[0] = FUN_00472650(0.0f, 1.0f);
      DAT_0086a4a0[i].f40[1] = FUN_00472650(0.0f, 1.0f);
      DAT_0086a4a0[i].f40[2] = FUN_00472650(0.0f, 1.0f);
      DAT_0086a4a0[i].vel.x = FUN_00472650(-0.03f, 0.03f);
      DAT_0086a4a0[i].vel.z = FUN_00472650(-0.03f, 0.03f);
      vy = FUN_00472650(0.01f, 0.04f);
      DAT_0086a4a0[i].vel.y = vy;
      if (vel) {
        DAT_0086a4a0[i].vel.x += vel->x;
        DAT_0086a4a0[i].vel.y = vel->y + vy;
        DAT_0086a4a0[i].vel.z = vel->z + DAT_0086a4a0[i].vel.z;
      }
      DAT_0086a4a0[i].f4c = 0;
      DAT_0086a4a0[i].f34 = 0;
      s.x = FUN_00472650(0.1f, 0.6f) * size;
      s.y = FUN_00472650(0.1f, 0.6f) * size;
      s.z = FUN_00472650(0.1f, 0.6f) * size;
      DAT_0086a4a0[i].scale = s;
      DAT_0086a4a0[i].active = 1;
      if (tex == 0) {
        if (mode == 1 && FUN_00472690(0, 3) == 3)
          tex = FUN_0042fab0(DAT_007f1a1c[player * 4]);
        else
          tex = FUN_0040bb50(s_tritex_005cf500);
        FUN_004e8090(DAT_0086a4a0[i].obj, tex);
        *(RwRGBA *)(DAT_0086a4a0[i].obj + 4) = col;
      } else {
        FUN_004e8090(DAT_0086a4a0[i].obj, tex);
        *(RwRGBA *)(DAT_0086a4a0[i].obj + 4) = white;
      }
      return;
    }
  }
}

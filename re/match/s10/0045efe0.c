// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045efe0 (h4): best 89.8% exact (regs 98.3%, ops 99.0%), 401/402 instructions. Raw 42.7%.
   Same source shape as 0x0045f5f0 / 0x0045faa0 / 0x0045e460 in this slice (Obj/slot/sound-record structs,
   start/position blocks as macros, float literals); three V3 locals give the 0x24 frame; the three-slot block
   assigns its V3 before the freq store (u, FREQ, FUN_0045e0f0, SETPOS); the phase loop over the float[3] at
   0x006130f8 does only the flag check per channel.
   Residual (one spot, cascades into register/offset diffs to the end of block 0x620): at 0x0045f12b the
   original keeps `22050.0 - sin(t2) * 500.0` on the x87 stack (fld st(0); fstp [obj+0x3c]; later fstp [esp]
   for FUN_005a6dc0, fstp st(0) on the else path). This file passes obj->freq back to FUN_005a6dc0 (same value,
   one instruction short). Tried: the expression twice (START macro) with the phase global extern (sin
   recomputed) and file-static (CSE'd but spilled to a float stack temp: fst [esp+4], frame 0x28, 80.8%),
   a float / register float / double / long double local, a block-scoped local; permuter 124 compiles on the
   static form, no gain. Class: (a) candidate: x87 CSE temp kept in st(0) vs spilled. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
typedef struct { float x, y, z; } V3_45efe0;
typedef struct { unsigned char pad[0x30]; int h; unsigned char flags; unsigned char p2[7]; float freq;
                 unsigned char p3[0x18]; V3_45efe0 pos; } Obj_45efe0;
typedef struct { Obj_45efe0 *obj; int pad; } Slot_45efe0;
typedef struct { unsigned char pad[0x80]; int snd; unsigned char p2[0xc]; } SndRec_45efe0;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(SndRec_45efe0 *, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_00690598, 0x00690598);
MGLOBAL(float, DAT_0069059c, 0x0069059c);
MGLOBALA(float, DAT_006130f8, 3, 0x006130f8);
MGLOBALA(Slot_45efe0, DAT_0068f640, 1, 0x0068f640);
int FUN_004b44f0(int a, int b);
float FUN_00472650(float lo, float hi);
double sin(double);
#pragma intrinsic(sin)
void FUN_0045e0f0(int a, float b);
void FUN_005a66d0(Obj_45efe0 *a, int b);
int FUN_005a6d60(int h, int a, int b, V3_45efe0 *v);
void FUN_005a6dc0(int h, int a, int b, float f);

#define START(I, F) \
    if ((DAT_0068f640[I].obj->flags & 2) == 0) \
      FUN_005a66d0(DAT_0068f640[I].obj, 1); \
    DAT_0068f640[I].obj->freq = F; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6dc0(DAT_0068f640[I].obj->h, 4, 1, F);
#define FREQ(I, F)     DAT_0068f640[I].obj->freq = F;     if (DAT_0068f640[I].obj->h != 0)       FUN_005a6dc0(DAT_0068f640[I].obj->h, 4, 1, F);
#define SETPOS(I, V)     DAT_0068f640[I].obj->pos = V;     if (DAT_0068f640[I].obj->h != 0)       FUN_005a6d60(DAT_0068f640[I].obj->h, 0, 1, &V);
#define POS(I, X, Y, Z) \
    v.x = X; v.y = Y; v.z = Z; \
    DAT_0068f640[I].obj->pos = v; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6d60(DAT_0068f640[I].obj->h, 0, 1, &v);
// MATCH: 0x0045efe0 FUN_0045efe0
void FUN_0045efe0(void)
{
  int i;
  int j;
  V3_45efe0 v;
  V3_45efe0 u;
  V3_45efe0 w;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1)
    return;

  DAT_00690598 = DAT_00690598 + 0.001f;
  if (DAT_00690598 > 3.14f)
    DAT_00690598 = 0.0f;
  i = DAT_0088e670[7].snd;
  START(i, FUN_00472650(0.0f, 100.0f) + 22050.0f)
  FUN_0045e0f0(i, (float)(1.0f - sin(DAT_00690598) * 0.1f));

  DAT_0069059c = DAT_0069059c + 0.001f;
  if (DAT_0069059c > 3.14f)
    DAT_0069059c = 0.0f;
  i = DAT_0088e670[10].snd;
  if ((DAT_0068f640[i].obj->flags & 2) == 0)
    FUN_005a66d0(DAT_0068f640[i].obj, 1);
  DAT_0068f640[i].obj->freq = 22050.0 - sin(DAT_0069059c) * 500.0;
  if (DAT_0068f640[i].obj->h != 0)
    FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, DAT_0068f640[i].obj->freq);
  FUN_0045e0f0(i, (float)(0.9f - sin(DAT_0069059c) * 0.1f));
  POS(i, 7.7f, 2.9f, 10.5f)

  for (j = 0; j < 3; j++) {
    DAT_006130f8[j] = DAT_006130f8[j] + 0.001f;
    if (DAT_006130f8[j] > 3.14f)
      DAT_006130f8[j] = 0.0f;
    i = DAT_0088e670[9].snd + j;
    if ((DAT_0068f640[i].obj->flags & 2) == 0)
      FUN_005a66d0(DAT_0068f640[i].obj, 1);
  }
  i = DAT_0088e670[9].snd;
  u.x = -4.0f; u.y = 2.6f; u.z = -6.0f;
  FREQ(i, 22050.0f)
  FUN_0045e0f0(i, 0.1f);
  SETPOS(i, u)
  u.x = 18.0f; u.y = -1.6f; u.z = -3.2f;
  FREQ(i + 1, 22050.0f)
  FUN_0045e0f0(i + 1, 1.0f);
  SETPOS(i + 1, u)
  u.x = 11.9f; u.y = -2.136f; u.z = -30.4f;
  FREQ(i + 2, 22050.0f)
  FUN_0045e0f0(i + 2, 1.0f);
  SETPOS(i + 2, u)

  if (FUN_004b44f0(0, 1000) == 0x19) {
    i = DAT_0088e670[8].snd;
    START(i, FUN_00472650(0.0f, 1000.0f) + 22050.0f)
    FUN_0045e0f0(i, 1.0f);
    w.x = 15.0f; w.y = 11.0f; w.z = 5.0f;
    SETPOS(i, w)
  }

  if (FUN_004b44f0(0, 700) == 0x23) {
    i = DAT_0088e670[11].snd;
    START(i, FUN_00472650(0.0f, 1000.0f) + 22050.0f)
    FUN_0045e0f0(i, 1.0f);
    w.x = 4.25f; w.y = 7.0f; w.z = 1.6f;
    SETPOS(i, w)
  }
}

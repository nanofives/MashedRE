// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045faa0 (h4): 100% exact, regs 100%, relocheck OK. Raw 61.6%. Same source shape as 0x0045f5f0
   in this slice and the pilot's 0x0045e990: Obj/slot/sound-record structs, start/position blocks as macros,
   float literals; FUN_004417e0 fills a local V3 (w.y read), FUN_0044b000 returns a V3* copied as a struct, the
   two channel loops `for (j..) i = rec.snd + j`, clamp written as `if (t < 0) .. else if (t > 1)`.
   Residual: none. */
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
typedef struct { float x, y, z; } V3_45faa0;
typedef struct { unsigned char pad[0x30]; int h; unsigned char flags; unsigned char p2[7]; float freq;
                 unsigned char p3[0x18]; V3_45faa0 pos; } Obj_45faa0;
typedef struct { Obj_45faa0 *obj; int pad; } Slot_45faa0;
typedef struct { unsigned char pad[0x80]; int snd; unsigned char p2[0xc]; } SndRec_45faa0;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(SndRec_45faa0 *, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_006905a4, 0x006905a4);
MGLOBALA(Slot_45faa0, DAT_0068f640, 1, 0x0068f640);
void FUN_004417e0(V3_45faa0 *v);
V3_45faa0 *FUN_0044b000(int n);
void FUN_0045e0f0(int a, float b);
void FUN_005a66d0(Obj_45faa0 *a, int b);
int FUN_005a6d60(int h, int a, int b, V3_45faa0 *v);
void FUN_005a6dc0(int h, int a, int b, float f);

#define START(I, F) \
    if ((DAT_0068f640[I].obj->flags & 2) == 0) \
      FUN_005a66d0(DAT_0068f640[I].obj, 1); \
    DAT_0068f640[I].obj->freq = F; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6dc0(DAT_0068f640[I].obj->h, 4, 1, F);
#define POS(I, X, Y, Z) \
    v.x = X; v.y = Y; v.z = Z; \
    DAT_0068f640[I].obj->pos = v; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6d60(DAT_0068f640[I].obj->h, 0, 1, &v);
#define POSP(I, P) \
    DAT_0068f640[I].obj->pos = *(P); \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6d60(DAT_0068f640[I].obj->h, 0, 1, P);

// MATCH: 0x0045faa0 FUN_0045faa0
void FUN_0045faa0(void)
{
  int i;
  int j;
  V3_45faa0 *p;
  V3_45faa0 v;
  V3_45faa0 w;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1)
    return;

  FUN_004417e0(&w);
  i = DAT_0088e670[7].snd;
  START(i, 22050.0f)
  DAT_006905a4 = (w.y + 2.5f) * 0.125f;
  if (DAT_006905a4 < 0.0f)
    DAT_006905a4 = 0.0f;
  else if (DAT_006905a4 > 1.0f)
    DAT_006905a4 = 1.0f;
  FUN_0045e0f0(i, DAT_006905a4);

  for (j = 0; j < 3; j++) {
    i = DAT_0088e670[8].snd + j;
    START(i, 22050.0f)
    FUN_0045e0f0(i, 1.0f);
  }
  i = DAT_0088e670[8].snd;
  POS(i, 11.8f, 10.22f, 18.96f)
  POS(i + 1, 30.6f, 10.9f, 32.5f)
  POS(i + 2, -5.4f, 5.7f, 15.4f)

  for (j = 0; j < 2; j++) {
    i = DAT_0088e670[9].snd + j;
    START(i, 22050.0f)
    FUN_0045e0f0(i, 1.0f);
  }
  i = DAT_0088e670[9].snd;
  p = FUN_0044b000(0);
  POSP(i, p)
  i++;
  p = FUN_0044b000(1);
  POSP(i, p)

  i = DAT_0088e670[10].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, -12.7f, 4.4f, 12.18f)

  i = DAT_0088e670[11].snd;
  START(i, 21050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, -34.7f, 2.0f, 20.4f)
}

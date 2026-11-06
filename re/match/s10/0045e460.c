// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045e460 (h4): 100% exact, regs 100%, relocheck OK. Raw 52.5%. Same source shape as 0x0045f5f0 /
   0x0045faa0 in this slice and the pilot's 0x0045e990: Obj/slot/sound-record structs, start/position blocks as
   macros, float literals, a local float t for the clamped volume (w.y from FUN_004417e0), FUN_00472650(float,float)
   cdecl returning float. Residual: none. */
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
typedef struct { float x, y, z; } V3_45e460;
typedef struct { unsigned char pad[0x30]; int h; unsigned char flags; unsigned char p2[7]; float freq;
                 unsigned char p3[0x18]; V3_45e460 pos; } Obj_45e460;
typedef struct { Obj_45e460 *obj; int pad; } Slot_45e460;
typedef struct { unsigned char pad[0x80]; int snd; unsigned char p2[0xc]; } SndRec_45e460;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(SndRec_45e460 *, DAT_0088e670, 0x0088e670);
MGLOBALA(Slot_45e460, DAT_0068f640, 1, 0x0068f640);
void FUN_004417e0(V3_45e460 *v);
float FUN_00472650(float lo, float hi);
void FUN_0045e0f0(int a, float b);
void FUN_005a66d0(Obj_45e460 *a, int b);
int FUN_005a6d60(int h, int a, int b, V3_45e460 *v);
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
// MATCH: 0x0045e460 FUN_0045e460
void FUN_0045e460(void)
{
  int i;
  float t;
  V3_45e460 v;
  V3_45e460 w;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1)
    return;

  FUN_004417e0(&w);
  i = DAT_0088e670[9].snd;
  START(i, 22050.0f)
  t = (w.y + 1.0f) * 0.1f;
  if (t < 0.0f)
    t = 0.0f;
  else if (t > 1.0f)
    t = 1.0f;
  FUN_0045e0f0(i, t);

  i = DAT_0088e670[8].snd;
  START(i, FUN_00472650(1000.0f, 2000.0f) + 22050.0f)
  FUN_0045e0f0(i, 0.3f);
  POS(i, 6.08f, 0.35f, 24.3f)

  i = DAT_0088e670[11].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, -16.0f, 1.6f, 39.0f)

  i = DAT_0088e670[11].snd + 1;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, 13.0f, 1.0f, 54.0f)

  i = DAT_0088e670[12].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, 24.9f, 3.8f, 14.8f)

  i = DAT_0088e670[13].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, -20.0f, 0.2f, -31.0f)

  i = DAT_0088e670[14].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, -21.0f, 7.2f, -5.0f)
}

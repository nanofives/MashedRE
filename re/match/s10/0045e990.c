// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045e990 (h4): best 99.5% exact (regs 99.5%, ops 99.5%), 444/444 instructions, relocheck OK.
   Raw 44.6%. Body taken from the pilot's re/match/pilot/a1_0045e990.c (99.3%, see its header for the structs,
   macros and callee arities) moved to the S10 form (MGLOBAL, FUN_ names, per-file type names). Writing the
   constants 22050.0f, 20.0f, 70.0f as literals (0x005ce6b8, 0x005ccd6c, 0x005ccd0c are the compiler's
   __real pool) instead of named globals removed the pilot's first residual (0x0045ea87).
   Residual (one spot, same instructions, scheduling only): at 0x0045eb73 the original issues
   `mov [v.z],-70.0 ; add eax,0x58` between `fsub [20.0]` and `fstp [v.x]`; ours puts them
   after the fstp. Tried: all 6 orders of the v.x/v.y/v.z stores, a temp x in 4 positions, `-20.0f + r`;
   permuter 170 compiles, no gain (the pilot's 212 compiles too).
   Class: (a) candidate: integer ops hoisted into the x87 fsub->fstp latency window. */
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
typedef struct { float x, y, z; } V3_45e990;
typedef struct { unsigned char pad[0x30]; int h; unsigned char flags; unsigned char p2[7]; float freq;
                 unsigned char p3[0x18]; V3_45e990 pos; } Obj_45e990;
typedef struct { Obj_45e990 *obj; int pad; } Slot_45e990;
typedef struct { unsigned char pad[0x80]; int snd; unsigned char p2[0xc]; } SndRec_45e990;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(SndRec_45e990 *, DAT_0088e670, 0x0088e670);
MGLOBALA(Slot_45e990, DAT_0068f640, 1, 0x0068f640);
void FUN_0045e0f0(int a, float b);
float FUN_00472650(float lo, float hi);
int FUN_004b44f0(int a, int b);
void FUN_005a66d0(Obj_45e990 *a, int b);
int FUN_005a6d60(int h, int a, int b, V3_45e990 *v);
void FUN_005a6dc0(int h, int a, int b, float f);

// MATCH: 0x0045e990 FUN_0045e990
void FUN_0045e990(void)
{
  int i;
  float x;
  V3_45e990 v;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1)
    return;

  if (FUN_004b44f0(0, 0x32) == 0x19) {
    i = DAT_0088e670[FUN_004b44f0(0, 3) + 7].snd;
    if ((DAT_0068f640[i].obj->flags & 2) == 0)
      FUN_005a66d0(DAT_0068f640[i].obj, 1);
    DAT_0068f640[i].obj->freq = FUN_00472650(0.0f, 10000.0f) + 22050.0f;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, FUN_00472650(0.0f, 10000.0f) + 22050.0f);
    FUN_0045e0f0(i, 1.0f);
    x = FUN_00472650(0.0f, 40.0f) - 20.0f;
    v.z = FUN_00472650(0.0f, 140.0f) - 70.0f;
    v.x = x;
    v.y = -1.0f;
    DAT_0068f640[i].obj->pos = v;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6d60(DAT_0068f640[i].obj->h, 0, 1, &v);
  }

  if (FUN_004b44f0(0, 0x19) == 0x19) {
    i = DAT_0088e670[FUN_004b44f0(0, 2) + 11].snd;
    if ((DAT_0068f640[i].obj->flags & 2) == 0)
      FUN_005a66d0(DAT_0068f640[i].obj, 1);
    DAT_0068f640[i].obj->freq = 22050.0f;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6dc0(DAT_0068f640[i].obj->h, 4, 1, 22050.0f);
    FUN_0045e0f0(i, 1.0f);
    v.x = FUN_00472650(0.0f, 40.0f) - 20.0f;
    v.y = 2.0f;
    v.z = -70.0f;
    DAT_0068f640[i].obj->pos = v;
    if (DAT_0068f640[i].obj->h != 0)
      FUN_005a6d60(DAT_0068f640[i].obj->h, 0, 1, &v);
  }

#define START(I, F) \
    if ((DAT_0068f640[I].obj->flags & 2) == 0) \
      FUN_005a66d0(DAT_0068f640[I].obj, 1); \
    DAT_0068f640[I].obj->freq = F; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6dc0(DAT_0068f640[I].obj->h, 4, 1, F); \
    FUN_0045e0f0(I, 1.0f);
#define POS(I, X, Y, Z) \
    v.x = X; v.y = Y; v.z = Z; \
    DAT_0068f640[I].obj->pos = v; \
    if (DAT_0068f640[I].obj->h != 0) \
      FUN_005a6d60(DAT_0068f640[I].obj->h, 0, 1, &v);

  i = DAT_0088e670[14].snd;
  START(i, 12050.0f)
  POS(i, -13.5f, 0.6f, -3.22f)

  i = DAT_0088e670[15].snd;
  START(i, 22050.0f)
  POS(i, 8.9f, 1.3f, 69.6f)

  i = DAT_0088e670[16].snd;
  START(i, 22050.0f)
  POS(i, 0.0f, 2.0f, -74.0f)
  i++;
  START(i, 22050.0f)
  POS(i, 8.9f, 1.3f, 69.6f)

  i = DAT_0088e670[17].snd;
  START(i, 22050.0f)
  POS(i, -3.86f, 6.716f, 24.4f)

  i = DAT_0088e670[18].snd;
  START(i, 22050.0f)
  POS(i, 4.7f, 12.6f, -71.5f)
}

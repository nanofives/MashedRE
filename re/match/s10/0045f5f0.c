// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045f5f0 (h4): 100% exact, regs 100%, relocheck OK. Raw 54.1%. Same source shape as the pilot's
   0x0045e990 (re/match/pilot/a1_0045e990.c): slot table of Obj* at 0x0068f640, 0x90-byte sound records at
   *0x0088e670 (snd id at +0x80, records 7..12), the start/position blocks as macros, FUN_00472650(float,float)
   cdecl returning float, FUN_005a6dc0 with 4 args. The phase step 0.001f, limit 3.14f and 22050.0f are literal
   constants (0x005cc558, 0x005ce6d8, 0x005ce6b8 are the compiler's __real pool): as named globals the first
   `t += step` came out as fld step / fadd t (wrong relocation order, same bytes otherwise); 0.7 is the
   double 0.7f promoted (0x005ce6e0) and sin is the intrinsic. Residual: none. */
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
typedef struct { float x, y, z; } V3_45f5f0;
typedef struct { unsigned char pad[0x30]; int h; unsigned char flags; unsigned char p2[7]; float freq;
                 unsigned char p3[0x18]; V3_45f5f0 pos; } Obj_45f5f0;
typedef struct { Obj_45f5f0 *obj; int pad; } Slot_45f5f0;
typedef struct { unsigned char pad[0x80]; int snd; unsigned char p2[0xc]; } SndRec_45f5f0;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(SndRec_45f5f0 *, DAT_0088e670, 0x0088e670);
MGLOBAL(float, DAT_00613104, 0x00613104);
MGLOBAL(float, DAT_006905a0, 0x006905a0);
MGLOBALA(Slot_45f5f0, DAT_0068f640, 1, 0x0068f640);
double sin(double);
#pragma intrinsic(sin)
void FUN_0045e0f0(int a, float b);
float FUN_00472650(float lo, float hi);
int FUN_004b44f0(int a, int b);
void FUN_005a66d0(Obj_45f5f0 *a, int b);
int FUN_005a6d60(int h, int a, int b, V3_45f5f0 *v);
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

// MATCH: 0x0045f5f0 FUN_0045f5f0
void FUN_0045f5f0(void)
{
  int i;
  V3_45f5f0 v;

  if (DAT_0068f4c8 != 0 && DAT_008aa254 > 1)
    return;

  DAT_006905a0 = DAT_006905a0 + 0.001f;
  if (DAT_006905a0 > 3.14f)
    DAT_006905a0 = 0.0f;
  i = DAT_0088e670[7].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, (float)(0.7f - sin(DAT_006905a0) * 0.7f));

  DAT_00613104 = DAT_00613104 + 0.001f;
  if (DAT_00613104 > 3.14f)
    DAT_00613104 = 0.0f;
  i = DAT_0088e670[8].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, (float)(0.7f - sin(DAT_00613104) * 0.7f));

  i = DAT_0088e670[9].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, -11.9f, 1.0f, -19.13f)

  if (FUN_004b44f0(0, 0xfa) == 0x19) {
    i = DAT_0088e670[11].snd;
    START(i, FUN_00472650(0.0f, 3000.0f) + 22050.0f)
    FUN_0045e0f0(i, 1.0f);
    POS(i, -11.9f, 1.0f, -19.13f)
  }

  if (FUN_004b44f0(0, 500) == 0x23) {
    i = DAT_0088e670[10].snd;
    START(i, FUN_00472650(0.0f, 3000.0f) + 22050.0f)
    FUN_0045e0f0(i, 1.0f);
    POS(i, -41.9f, 2.8f, 41.38f)
  }

  i = DAT_0088e670[12].snd;
  START(i, 22050.0f)
  FUN_0045e0f0(i, 1.0f);
  POS(i, -6.96f, 1.9f, 17.77f)
}

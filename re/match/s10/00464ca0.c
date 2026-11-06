// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x00464ca0 (h8): 92.7% (regs 92.7%), by hand (raw 65.7%). V3 struct copy of the 0x0044c370 position
   into record+0x58, 22050.0f literal, sin intrinsic on the double constants. Remaining: the original keeps the
   sin(..) * c0 + c8 double on the x87 stack after the store to +0x3c (fld st(0) / fstp st(0) on the no-handle
   path) and passes it to 0x005a6dc0; ours recomputes the expression in the branch. A double local gives
   fst [esp] + reload (82.4%), passing the field back gives a dword copy (88.0%). Tried: double/float local,
   literal 0.001f / 5000.0 / 22050.0 (CSE then rounds into a float stack temp, 82.4%), cast placement, field reread, permuter 109 compiles no gain. Class (b). */
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
typedef struct { float x, y, z; } MatchV3_464ca0;
typedef struct { char *p; float f; } MatchRec0068f640;
MGLOBAL(int, DAT_0068f4c8, 0x0068f4c8);
MGLOBAL(int, DAT_008aa254, 0x008aa254);
MGLOBAL(char *, DAT_0088e670, 0x0088e670);
MGLOBAL(double, DAT_005ce9c8, 0x005ce9c8);
MGLOBAL(double, DAT_005ce9c0, 0x005ce9c0);
MGLOBAL(double, DAT_005ce6c8, 0x005ce6c8);
MGLOBALA(MatchRec0068f640, DAT_0068f640, 1, 0x0068f640);
double sin(double);
#pragma intrinsic(sin)
MatchV3_464ca0 *FUN_0044c370(void);
void FUN_0045e0f0(int p0, float p1);
void FUN_005a66d0(char *p, int a);
void FUN_005a6d60(int h, int a, int b, MatchV3_464ca0 *v);
void FUN_005a6dc0(int h, int a, int b, float f);
// MATCH: 0x00464ca0 FUN_00464ca0
void FUN_00464ca0(void)
{
  MatchV3_464ca0 *pos;
  int i;

  if (DAT_0068f4c8 == 0 || DAT_008aa254 <= 1) {
    pos = FUN_0044c370();
    i = *(int *)(DAT_0088e670 + 0x470);
    if ((*(unsigned char *)(DAT_0068f640[i].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[i].p, 1);
      *(float *)(DAT_0068f640[i].p + 0x3c) = 22050.0f;
      if (*(int *)(DAT_0068f640[i].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[i].p + 0x30), 4, 1, 22050.0f);
      }
    }
    FUN_0045e0f0(i, 1.0f);
    *(MatchV3_464ca0 *)(DAT_0068f640[i].p + 0x58) = *pos;
    if (*(int *)(DAT_0068f640[i].p + 0x30) != 0) {
      FUN_005a6d60(*(int *)(DAT_0068f640[i].p + 0x30), 0, 1, pos);
    }
    i = *(int *)(DAT_0088e670 + 0x620);
    if ((*(unsigned char *)(DAT_0068f640[i].p + 0x34) & 2) == 0) {
      FUN_005a66d0(DAT_0068f640[i].p, 1);
      *(float *)(DAT_0068f640[i].p + 0x3c) = (float)(sin(DAT_005ce9c8) * DAT_005ce9c0 + DAT_005ce6c8);
      if (*(int *)(DAT_0068f640[i].p + 0x30) != 0) {
        FUN_005a6dc0(*(int *)(DAT_0068f640[i].p + 0x30), 4, 1, (float)(sin(DAT_005ce9c8) * DAT_005ce9c0 + DAT_005ce6c8));
      }
    }
    FUN_0045e0f0(i, 1.0f);
    *(MatchV3_464ca0 *)(DAT_0068f640[i].p + 0x58) = *pos;
    if (*(int *)(DAT_0068f640[i].p + 0x30) != 0) {
      FUN_005a6d60(*(int *)(DAT_0068f640[i].p + 0x30), 0, 1, pos);
    }
  }
}

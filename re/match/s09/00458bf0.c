// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458bf0: 100% by hand (raw 19.5%). Count-down while loop (i--, then &arr[i]) over the
   0x50-byte records at 0x0068b198; pos = home and vel = a zeroed local V3 are struct copies. */
/* ---- MATCH-S09 preamble (same text in every re/match/s09 source; not part of any function body) ----
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
typedef struct V3_00458bf0 {
  float x;
  float y;
  float z;
} V3_00458bf0;
typedef struct Rec00458bf0 {
  int clump;
  undefined4 f04[6];
  int f1c;
  int state;
  undefined4 f24[2];
  V3_00458bf0 home;
  V3_00458bf0 pos;
  V3_00458bf0 vel;
} Rec00458bf0;
MGLOBAL(int, DAT_0068b9a8, 0x0068b9a8);
MGLOBALA(Rec00458bf0, DAT_0068b198, 25, 0x0068b198);
// MATCH: 0x00458bf0 FUN_00458bf0
void FUN_00458bf0(void)
{
  int i;
  Rec00458bf0 *r;
  V3_00458bf0 zero;

  zero.x = 0.0f;
  zero.y = 0.0f;
  zero.z = 0.0f;
  i = DAT_0068b9a8;
  while (i != 0) {
    i--;
    r = &DAT_0068b198[i];
    r->pos = r->home;
    r->vel = zero;
    r->state = 1;
    r->f1c = 0;
  }
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x00458f80: 100% by hand (raw 26.3%). The original keeps the loop counter in EDX across the call
   to 0x00458f20, so cl 9176 must see 0x00458f20's body (it uses only EAX/ECX) earlier in the same TU: the file
   holds FUN_00458f20 (same body as 00458f20.c, 100%), NON-static (static turns it into a register-arg
   function, 41.7%). In the port this is a second definition of FUN_00458f20: include only one of the two. */
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
typedef struct V3_00458f80 {
  float x;
  float y;
  float z;
} V3_00458f80;
typedef struct Rec_00458f80 {
  int clump;
  undefined4 f04[5];
  float f18;
  int f1c;
  int state;
  int type;
  int flag;
  V3_00458f80 home;
  V3_00458f80 pos;
  V3_00458f80 vel;
} Rec_00458f80;
MGLOBALA(Rec_00458f80, DAT_0068b198, 25, 0x0068b198);
// HELD: 0x00458f20 (body elsewhere; this copy exists only so the DDK compiles the caller as the original; not compiled in the port)
#ifndef MATCH_PORT
int FUN_00458f20(int i, int on)
{
  Rec_00458f80 *r;

  if (i < 0 || i >= 25) return 0;
  r = &DAT_0068b198[i];
  if (on) {
    if (r->state == 3) {
      r->state = 1;
    }
  } else {
    r->state = 3;
  }
  r->f1c = 0;
  return 1;
}
#else
int FUN_00458f20(int i, int on);
#endif

// MATCH: 0x00458f80 FUN_00458f80
void FUN_00458f80(int on)
{
  int i;

  for (i = 0; i < 25; i++) {
    FUN_00458f20(i, on);
  }
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045a590 (h9, by hand): 100% (regs 100%). Raw converter 38.8% (in_EAX). The object pointer
   arrives in EAX: the function is `static` with a stand-in caller below (pilot finding 1), so the file is not the
   real TU. Moved it: float literals instead of the .rdata globals (0x005cc358 = 5.0f, 0x005cc32c = 0.5f,
   0x005cc320 = 1.0f, 0x005ce5f4 = 0.18181819f): with the globals the compiler commuted `x + c` / `x * c` to
   fld c (95.0%). FUN_00472650 is cdecl float (lo, hi). Residual: none. */
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
typedef struct { float x, y, z; } V3_590;
typedef struct { char pad[0xb0]; int fb0; } Sub04_590;
typedef struct { int f0; int frame; } Sub24_590;
typedef struct {
  int f00;
  Sub04_590 *p04;
  int f08;
  float f0c;
  int f10, f14, f18;
  int state;
  int f20;
  Sub24_590 *p24;
  float m28[16];
} Obj590;
void FUN_00465f40(int p0, V3_590 *p1, int p2, float p3);
float FUN_00472650(float lo, float hi);
char *FUN_004c0ed0(int frame);
int FUN_004c13e0(int frame, V3_590 *v, int combine);
int FUN_004c1480(int frame, float *m, int combine);
// MATCH: 0x0045a590 FUN_0045a590
static void FUN_0045a590(Obj590 *o)
{
  int frame;
  float scale;
  V3_590 s;
  V3_590 pos;
  frame = o->p24->frame;
  s.x = 1.0f;
  s.y = 1.0f;
  s.z = 1.0f;
  scale = 1.0f;
  if (o->state == 1) {
    if (o->f0c < 5.0f) {
      o->f0c += 0.5f;
    }
    scale = 1.5f;
  } else if (o->f0c > 1.0f) {
    scale = 1.0f;
    o->f0c -= 0.5f;
  }
  pos = *(V3_590 *)(FUN_004c0ed0(o->p24->frame) + 0x30);
  FUN_00465f40(0x16, &pos, o->p04->fb0, o->f0c * 0.18181819f);
  scale += FUN_00472650(0.0f, 0.1f);
  o->f0c += FUN_00472650(0.0f, 0.3f);
  s.z = o->f0c;
  s.x = scale;
  s.y = scale;
  FUN_004c1480(frame, o->m28, 0);
  FUN_004c13e0(frame, &s, 1);
}
/* stand-in for the real caller 0x0045ae80 (call site 0x0045aef1: mov eax,edi; call), only there so cl 9176
   gives the static callee its EAX convention; not part of the port. */
void caller_0045a590(Obj590 *o)
{
  FUN_0045a590(o);
  o->f10 = 5;
}

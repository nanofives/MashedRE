// FLAGS: /O2 /GS /Ob1
/* MATCH-S09 0x0045a530 (h9, by hand): 100% (regs 100%). Raw converter 27.8% (in_EAX). The vec3 pointer
   arrives in EAX: the function is `static` with a stand-in caller below (pilot finding 1), so the file is not the
   real TU. Moved it: 0x8c-byte request record as a struct with an int pad (a char pad brought in the /GS cookie),
   source store order x,y,z,0.15f,+0x14,+0x18,+0x10,callback. Callback 0x0045a420 is cdecl (2 stack args, ret).
   Residual: none. */
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
typedef struct {
  float x, y, z;
  float f0c;
  int i10;
  int i14;
  int i18;
  void (*cb)(int, void *);
  int pad[(0x8c - 0x20) / 4];
} Req530;
void FUN_0045a420(int a, void *b);
int FUN_00484cf0(Req530 *r);
// MATCH: 0x0045a530 FUN_0045a530
static void FUN_0045a530(float *v, int param_1)
{
  Req530 r;
  r.x = v[0];
  r.y = v[1];
  r.z = v[2];
  r.f0c = 0.15f;
  r.i14 = 0;
  r.i18 = param_1;
  r.i10 = 0x10;
  r.cb = FUN_0045a420;
  FUN_00484cf0(&r);
}
/* stand-in for the real caller 0x0045ae80 (call site 0x0045b0e1: lea eax,[esi-0x18]; push arg), only there so
   cl 9176 gives the static callee its EAX convention; not part of the port. */
void caller_0045a530(float *p, int x)
{
  if (*p > 1.0f)
    FUN_0045a530(p - 6, x);
}

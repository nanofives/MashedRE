// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044df80: 100% by hand (raw 79.3%). An int index 0..79 over the 0xf8-byte record array at
   0x00890080 (strength-reduced to a pointer, hence the signed jl against 0x894e00), param != 0 branch first. */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { int h; char pad04[0xf4]; } Rec_0044df80;
MGLOBAL(int, DAT_00684b7c, 0x00684b7c);
MGLOBALA(Rec_0044df80, DAT_00890080, 80, 0x00890080);
int FUN_00557fb0(int p0, int p1);
int FUN_00558100(int p0);
int FUN_00558140(int p0);
// MATCH: 0x0044df80 FUN_0044df80
void FUN_0044df80(int param_1)
{
  int i;

  if (DAT_00684b7c != param_1) {
    for (i = 0; i < 80; i++) {
      if (param_1 != 0) {
        FUN_00558100(DAT_00890080[i].h);
      }
      else {
        FUN_00557fb0(DAT_00890080[i].h, 0);
        FUN_00558140(DAT_00890080[i].h);
      }
    }
    DAT_00684b7c = param_1;
  }
}

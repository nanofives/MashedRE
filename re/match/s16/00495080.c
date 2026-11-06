// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495080 (h1): 100% (regs 100%), by hand. This file also holds 0x00494fd0 and 0x00494f40.
   All three BYTE-IDENTICAL (match.py --bytes). Residual: none.
   Pilot finding 1: 0x00494f40 takes the gamma ramp pointer in EAX and 0x00494fd0 the Direct3D device in ESI
   (plus two float stack args each). Both are static functions defined before their caller here, so 9176
   picks those registers itself. 0x00495080 (cdecl, 2 floats) is their real caller and passes DAT_00771a0c
   and 2*x + DAT_005cc32c.
   0x00494f40: 256-step loop, ramp R/G/B[i] = (WORD)ftol((float)(pow(i*k, c/g) * scale) * DAT_005cfe34).
   What moved it: pow called as FUN_004a2d30 (the CRT stack entry. The name pow made 9176 use the _CIpow
   intrinsic, cf. pilot finding 5), the exponent in its own float local (not the parameter slot), and the
   (float) cast on pow*scale (puts the fmul before the add esp). Ghidra's size (125 B) stops before the loop
   end. The whole 133 B symbol is byte-identical.
   0x00494fd0: D3DCAPS9 local (0x130 B) then a 0x600-byte ramp, GetDeviceCaps (+0x1c), Caps2 & 0x20000,
   GetGammaRamp (+0x58), SetGammaRamp (+0x54), /GS cookie. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct { ushort red[256]; ushort green[256]; ushort blue[256]; } GammaRamp;
typedef int (__stdcall *GetCapsFn)(int *self, void *caps);
typedef void (__stdcall *GetGammaFn)(int *self, uint swap, GammaRamp *ramp);
typedef void (__stdcall *SetGammaFn)(int *self, uint swap, uint flags, GammaRamp *ramp);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005ceb90, 0x005ceb90);
MGLOBAL(float, DAT_005cfe34, 0x005cfe34);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(int *, DAT_00771a0c, 0x00771a0c);
double __cdecl FUN_004a2d30(double x, double y); /* CRT pow, stack entry */
// MATCH: 0x00494f40 FUN_00494f40
static void FUN_00494f40(GammaRamp *ramp, float gamma, float scale)
{
  int i;
  ushort v;
  float e;
  e = DAT_005cc320 / gamma;
  for (i = 0; i < 0x100; i++) {
    v = (ushort)(long)((float)(FUN_004a2d30((double)((float)i * DAT_005ceb90), (double)e) * scale) * DAT_005cfe34);
    ramp->red[i] = v;
    ramp->green[i] = v;
    ramp->blue[i] = v;
  }
}
// MATCH: 0x00494fd0 FUN_00494fd0
static undefined4 FUN_00494fd0(int *dev, float gamma, float scale)
{
  uint caps[0x4c];
  GammaRamp ramp;
  if ((*(GetCapsFn *)(*dev + 0x1c))(dev, caps) >= 0 && (caps[3] & 0x20000) != 0) {
    (*(GetGammaFn *)(*dev + 0x58))(dev, 0, &ramp);
    FUN_00494f40(&ramp, gamma, scale);
    (*(SetGammaFn *)(*dev + 0x54))(dev, 0, 0, &ramp);
    return 1;
  }
  return 0;
}
// MATCH: 0x00495080 FUN_00495080
void FUN_00495080(float param_1, float param_2)
{
  FUN_00494fd0(DAT_00771a0c, param_1 + param_1 + DAT_005cc32c, param_2);
}

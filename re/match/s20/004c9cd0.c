// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c9cd0 (h4): 90.2% (regs 90.2%), 143/143 instructions (raw converter did not compile: C2050).
   D3D9 lost-device restore: TestCooperativeLevel (+0xc); on D3DERR_DEVICENOTRESET (0x88760869) FUN_004c9ad0, Reset
   (+0x40) with the present parameters at 0x009120e0; on D3DERR_INVALIDCALL-style 0x8876086c and Windowed it reads
   the adapter display mode (IDirect3D9 +0x20) into 0x00911f90 / BackBufferFormat, sets 0x00911f94 to 32/16/0 by
   format and resets again; on success re-gets the render target (+0x98) and depth surface (+0xa0) (released at
   once), FUN_004d1e30 && FUN_004dc970 -> FUN_004d6200, SetLight/LightEnable (+0xcc/+0xd4) for each 0x6c-byte
   light slot, FUN_004db3e0 -> FUN_004e0920; then the hook at 0x007d459c. Failure stores 0 in 0x007d4140; else
   BeginScene (+0xa4) once when 0x007d4140 is 0.
   What moved it: `return DAT_007d4140 = x` returns, a depth local for the switch, hr2 initialised to 0 (67.6 ->
   90.2). Residual: push esi is scheduled after the two checks instead of at the top of the restore block, and the
   return-0 block layout (ours `mov [g], 0; xor eax, eax` before the success path, original `xor eax, eax;
   mov [g], eax` after it). Tried: single `return DAT` exit (76.4), i declared at three scopes, Ghidra's goto loop
   form, permuter 117 compiles. Class (b). */
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
typedef struct ComObj { void **lpVtbl; } ComObj;
typedef long (__stdcall *Fn1)(ComObj *self);
typedef long (__stdcall *Fn2)(ComObj *self, void *a);
typedef long (__stdcall *Fn3)(ComObj *self, int a, void *b);
typedef long (__stdcall *Fn3i)(ComObj *self, int a, int b);
#define VCALL(T, o, i) ((T)(o)->lpVtbl[(i)])
typedef struct D3DDISPLAYMODE { unsigned int Width; unsigned int Height; unsigned int RefreshRate; int Format; } D3DDISPLAYMODE;
typedef struct D3DPRESENT_PARAMETERS {
  unsigned int BackBufferWidth; unsigned int BackBufferHeight; int BackBufferFormat; unsigned int BackBufferCount;
  int MultiSampleType; unsigned long MultiSampleQuality; int SwapEffect; void *hDeviceWindow; int Windowed;
  int EnableAutoDepthStencil; int AutoDepthStencilFormat; unsigned long Flags;
  unsigned int FullScreen_RefreshRateInHz; unsigned int PresentationInterval;
} D3DPRESENT_PARAMETERS;
typedef struct LightSlot { unsigned char light[0x68]; int enabled; } LightSlot;
typedef void (*CallbackFn)(void);
MGLOBAL(ComObj *, DAT_007d4110, 0x007d4110);
MGLOBAL(ComObj *, DAT_007d4108, 0x007d4108);
MGLOBAL(int, DAT_007d410c, 0x007d410c);
MGLOBAL(ComObj *, DAT_007d4114, 0x007d4114);
MGLOBAL(ComObj *, DAT_007d4118, 0x007d4118);
MGLOBAL(int, DAT_007d4140, 0x007d4140);
MGLOBAL(int, DAT_007d456c, 0x007d456c);
MGLOBAL(LightSlot *, DAT_007d4570, 0x007d4570);
MGLOBAL(CallbackFn, DAT_007d459c, 0x007d459c);
MGLOBAL(int, DAT_00911f90, 0x00911f90);
MGLOBAL(int, DAT_00911f94, 0x00911f94);
MGLOBAL(D3DPRESENT_PARAMETERS, DAT_009120e0, 0x009120e0);
void FUN_004c9ad0(void);
int FUN_004d1e30(void);
int FUN_004dc970(void);
void FUN_004d6200(void);
int FUN_004db3e0(void);
void FUN_004e0920(void);

// MATCH: 0x004c9cd0 FUN_004c9cd0
int FUN_004c9cd0(void)
{
  long hr;
  hr = VCALL(Fn1, DAT_007d4110, 3)(DAT_007d4110);
  if (hr == (long)0x88760869) {
    FUN_004c9ad0();
    hr = VCALL(Fn2, DAT_007d4110, 16)(DAT_007d4110, &DAT_009120e0);
    if (hr == (long)0x8876086c) {
      D3DDISPLAYMODE mode;
      int depth;
      if (!DAT_009120e0.Windowed) {
        return DAT_007d4140 = 0;
      }
      VCALL(Fn3, DAT_007d4108, 8)(DAT_007d4108, DAT_007d410c, &mode);
      DAT_00911f90 = mode.Format;
      DAT_009120e0.BackBufferFormat = mode.Format;
      switch (mode.Format) {
      case 0x15:
      case 0x16:
      case 0x23:
        depth = 0x20;
        break;
      case 0x17:
      case 0x18:
      case 0x19:
        depth = 0x10;
        break;
      default:
        depth = 0;
        break;
      }
      DAT_00911f94 = depth;
      hr = VCALL(Fn2, DAT_007d4110, 16)(DAT_007d4110, &DAT_009120e0);
    }
    if (hr >= 0) {
      VCALL(Fn3, DAT_007d4110, 38)(DAT_007d4110, 0, &DAT_007d4118);
      VCALL(Fn1, DAT_007d4118, 2)(DAT_007d4118);
      VCALL(Fn2, DAT_007d4110, 40)(DAT_007d4110, &DAT_007d4114);
      VCALL(Fn1, DAT_007d4114, 2)(DAT_007d4114);
      if (FUN_004d1e30() && FUN_004dc970()) {
        int i;
        long hr2 = 0;
        FUN_004d6200();
        for (i = 0; i < DAT_007d456c; i++) {
          VCALL(Fn3, DAT_007d4110, 51)(DAT_007d4110, i, DAT_007d4570[i].light);
          hr2 = VCALL(Fn3i, DAT_007d4110, 53)(DAT_007d4110, i, DAT_007d4570[i].enabled);
        }
        if (hr2 >= 0) {
          if (FUN_004db3e0()) {
            FUN_004e0920();
          }
        }
      }
      if (DAT_007d459c) {
        DAT_007d459c();
      }
    }
  }
  if (hr < 0) {
    return DAT_007d4140 = 0;
  }
  if (!DAT_007d4140) {
    if (VCALL(Fn1, DAT_007d4110, 41)(DAT_007d4110) >= 0) {
      return DAT_007d4140 = 1;
    }
  }
  return DAT_007d4140;
}

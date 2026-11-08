// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c9fb0 (h1): 81.3% (regs 85.6%). Raw converter did not compile (Ghidra lost the
   stack-aliased locals). The original keeps the back-buffer surface pointer in param_2's stack slot (hwnd lives in
   ebx); the file reproduces that by copying param_2 to a local hwnd and passing &param_2 as the surface out
   pointer (66.2% with a separate local). Residual: the hwnd copy is loaded at entry (original: after the first
   test), the return-0 `xor eax, eax` sits in the prologue in the original, and the D3D device load is scheduled
   before the template rep movsd. Tried: a ret variable (70.5%), the copy inside the if (80.6%), ==/!/?: forms of
   the two size flags, a block-scoped surface local, permuter 569 compiles (83.5% only with the template copy moved
   below a field store, not kept). Class (b). When DAT_00912100 is nonzero and param_2 differs from DAT_007d4104:
   GetWindowRect(param_2), destroys param_1's +0x60 and +0x64 rasters (FUN_004c7650, field zeroed first), copies the
   56-byte template DAT_009120e0 and sets dword 0 = (right == left), dword 1 = (bottom == top), dword 7 = param_2,
   dword 9 = 0, then calls the D3D device (DAT_007d4110) vtable +0x34 (__stdcall) with (device, &params, &chain).
   On result 0: chain vtable +0x14 (chain, 0, 0, &param_2), surf vtable +0x30 (param_2, &desc 32 bytes), then
   FUN_004c77c0(desc+0x18, desc+0x1c, 0, 0x82) -> r: r+0xc/+0x10 = the two sizes, r+0x14 = byte 1 and r+0x23 = byte
   3 of the 4-byte record DAT_00911b00[desc+0]; at r + DAT_00911ae4: +0 surf, +8 byte 0 of that record, +0x18 desc+0,
   +0x20 param_2, +0x1c chain; param_1+0x60 = r. FUN_004c77c0(.., .., 0, 1) nonzero -> param_1+0x64. Releases surf
   (vtable +8), returns 1. Otherwise returns 0. FLAGS /G5 (RW core). */
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
typedef struct IUnk { void **v; } IUnk;
typedef int (__stdcall *UnkFn0)(IUnk *self);
typedef int (__stdcall *CreateChainFn)(IUnk *self, void *pp, IUnk **out);
typedef int (__stdcall *GetBackBufFn)(IUnk *self, unsigned int a, int b, IUnk **out);
typedef int (__stdcall *GetDescFn)(IUnk *self, void *desc);
typedef struct Rect4 { int left; int top; int right; int bottom; } Rect4;
typedef struct Present14 { unsigned int v[14]; } Present14;
typedef struct SurfDesc { int format; int type; unsigned int usage; int pool; int ms; unsigned int msq; unsigned int width; unsigned int height; } SurfDesc;
typedef struct FmtInfo { unsigned char a; unsigned char depth; unsigned char b; unsigned char c; } FmtInfo;
typedef struct Raster { char pad0[0xc]; unsigned int width; unsigned int height; unsigned int depth; char pad18[0xb]; unsigned char b23; } Raster;
typedef struct RasExt { IUnk *surf; int pad4; unsigned char b8; char pad9[0xf]; int format; IUnk *chain; void *hwnd; } RasExt;
typedef struct Camera { char pad[0x60]; Raster *frameBuffer; Raster *zBuffer; } Camera;
MGLOBAL(int, DAT_00912100, 0x00912100);
MGLOBAL(void *, DAT_007d4104, 0x007d4104);
MGLOBAL(IUnk *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBALA(FmtInfo, DAT_00911b00, 0x80, 0x00911b00);
MGLOBAL(Present14, DAT_009120e0, 0x009120e0);
__declspec(dllimport) int __stdcall GetWindowRect(void *hWnd, Rect4 *lpRect);
void FUN_004c7650(Raster *r);
Raster *FUN_004c77c0(unsigned int w, unsigned int h, int depth, int flags);

// MATCH: 0x004c9fb0 FUN_004c9fb0
int FUN_004c9fb0(Camera *param_1, IUnk *param_2)
{
  Rect4 rect;
  Present14 pp;
  IUnk *chain;
  void *hwnd;
  SurfDesc desc;
  Raster *r;
  RasExt *ext;
  hwnd = param_2;
  if (DAT_00912100 != 0 && DAT_007d4104 != hwnd) {
    GetWindowRect(hwnd, &rect);
    if (param_1->frameBuffer != 0) {
      r = param_1->frameBuffer;
      param_1->frameBuffer = 0;
      FUN_004c7650(r);
    }
    if (param_1->zBuffer != 0) {
      r = param_1->zBuffer;
      param_1->zBuffer = 0;
      FUN_004c7650(r);
    }
    pp = DAT_009120e0;
    pp.v[0] = rect.right - rect.left == 0;
    pp.v[1] = rect.bottom - rect.top == 0;
    pp.v[7] = (unsigned int)hwnd;
    pp.v[9] = 0;
    if (((CreateChainFn)DAT_007d4110->v[0x34 / 4])(DAT_007d4110, &pp, &chain) == 0) {
      ((GetBackBufFn)chain->v[0x14 / 4])(chain, 0, 0, &param_2);
      ((GetDescFn)param_2->v[0x30 / 4])(param_2, &desc);
      r = FUN_004c77c0(desc.width, desc.height, 0, 0x82);
      if (r != 0) {
        r->width = desc.width;
        r->height = desc.height;
        r->depth = DAT_00911b00[desc.format].depth;
        r->b23 = DAT_00911b00[desc.format].c;
        ext = (RasExt *)((char *)r + DAT_00911ae4);
        ext->surf = param_2;
        ext->b8 = DAT_00911b00[desc.format].a;
        ext->format = desc.format;
        ext->hwnd = hwnd;
        ext->chain = chain;
        param_1->frameBuffer = r;
      }
      r = FUN_004c77c0(desc.width, desc.height, 0, 1);
      if (r != 0) {
        param_1->zBuffer = r;
      }
      ((UnkFn0)param_2->v[8 / 4])(param_2);
      return 1;
    }
  }
  return 0;
}

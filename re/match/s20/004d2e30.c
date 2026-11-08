// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d2e30 (h1): 100% (regs 100%). Raw converter did not compile. Z-buffer raster create: picks the depth format into ext+0x18 by probing the D3D9
   object (DAT_007d4108) with vtable +0x28 (adapter DAT_007d410c, type DAT_006181b0, adapter format DAT_00911f90,
   usage 2, resource 1, fmt) and +0x30 (.., adapter format twice, fmt): depth DAT_00911f9c 16 -> 0x49 else 0x50;
   32 -> 0x4b, 0x4f, 0x47, else 0x4d. Clears ext byte +8. When GetClientRect(DAT_007d4104) equals the raster's
   +0xc/+0x10 size, ext+0 = DAT_007d4114; otherwise device (DAT_007d4110) vtable +0x14, then +0x74 (width, height,
   format, DAT_009120f0, DAT_009120f4, 0, &ext+0, 0); hr 0x8876017c and other failures raise RWERROR(2, string)
   and return 0. Returns 1. The probe is a file-static __forceinline helper (plain __inline is not expanded under /Ob1: 33.5%).
   FLAGS /G5 (RW core). */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct IUnk { void **v; } IUnk;
typedef int (__stdcall *CheckFmtFn)(IUnk *self, int adapter, int type, int adapterFmt, unsigned int usage, int rtype, int fmt);
typedef int (__stdcall *CheckDSFn)(IUnk *self, int adapter, int type, int adapterFmt, int rtFmt, int dsFmt);
typedef int (__stdcall *DevFn0)(IUnk *self);
typedef int (__stdcall *CreateDSFn)(IUnk *self, unsigned int w, unsigned int h, int fmt, int ms, unsigned int msq, int discard, void **out, void *shared);
typedef struct Rect4 { int left; int top; int right; int bottom; } Rect4;
typedef struct Raster { char pad[0xc]; int width; int height; } Raster;
typedef struct ZExt { void *surface; int pad4; unsigned char b8; char pad9[0xf]; int format; } ZExt;
MGLOBAL(int, DAT_00911f9c, 0x00911f9c);
MGLOBAL(int, DAT_00911f90, 0x00911f90);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(IUnk *, DAT_007d4108, 0x007d4108);
MGLOBAL(int, DAT_007d410c, 0x007d410c);
MGLOBAL(void *, DAT_007d4104, 0x007d4104);
MGLOBAL(void *, DAT_007d4114, 0x007d4114);
MGLOBAL(IUnk *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_009120f0, 0x009120f0);
MGLOBAL(unsigned int, DAT_009120f4, 0x009120f4);
MGLOBALA(char, s_Failed_to_create_raster__006183c4, 25, 0x006183c4);
MGLOBALA(char, s_Out_of_video_memory__Failed_to_c_006183e0, 46, 0x006183e0);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
__declspec(dllimport) int __stdcall GetClientRect(void *hWnd, Rect4 *lpRect);

static __forceinline int CheckZFormat(int fmt)
{
  int adapterFmt = DAT_00911f90;
  if (((CheckFmtFn)DAT_007d4108->v[0x28 / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFmt, 2, 1, fmt) >= 0 &&
      ((CheckDSFn)DAT_007d4108->v[0x30 / 4])(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFmt, adapterFmt, fmt) >= 0) {
    return 1;
  }
  return 0;
}

// MATCH: 0x004d2e30 FUN_004d2e30
int FUN_004d2e30(Raster *param_1, ZExt *param_2)
{
  Rect4 rect;
  int hr;
  if (DAT_00911f9c == 16) {
    if (CheckZFormat(0x49)) {
      param_2->format = 0x49;
    } else {
      param_2->format = 0x50;
    }
  } else if (DAT_00911f9c == 32) {
    if (CheckZFormat(0x4b)) {
      param_2->format = 0x4b;
    } else if (CheckZFormat(0x4f)) {
      param_2->format = 0x4f;
    } else if (CheckZFormat(0x47)) {
      param_2->format = 0x47;
    } else {
      param_2->format = 0x4d;
    }
  }
  param_2->b8 = 0;
  GetClientRect(DAT_007d4104, &rect);
  if (rect.right == param_1->width && rect.bottom == param_1->height) {
    param_2->surface = DAT_007d4114;
    return 1;
  }
  ((DevFn0)DAT_007d4110->v[0x14 / 4])(DAT_007d4110);
  hr = ((CreateDSFn)DAT_007d4110->v[0x74 / 4])(DAT_007d4110, param_1->width, param_1->height, param_2->format,
                                               DAT_009120f0, DAT_009120f4, 0, &param_2->surface, 0);
  if (hr == (int)0x8876017c) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(2, s_Out_of_video_memory__Failed_to_c_006183e0);
    FUN_004d8480(&err);
    return 0;
  }
  if (hr < 0) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(2, s_Failed_to_create_raster__006183c4);
    FUN_004d8480(&err);
    return 0;
  }
  return 1;
}

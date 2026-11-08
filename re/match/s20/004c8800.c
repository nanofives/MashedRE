// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c8800 (h4): 100% (regs 100%), relocheck OK (GetWindowRect import unchecked by name). Raw converter
   did not compile (C2146). Fills the D3DPRESENT_PARAMETERS at 0x009120e0: fullscreen (flags & 1) from the display
   mode (refresh clamped to 0x006181c4, interval 0x80000000, swap effect 2), else windowed from GetWindowRect of
   0x007d4104 (width/height stored as (right - left) == 0 / (bottom - top) == 0, as the bytes show), swap effect 3;
   back-buffer format = adapter format, or for 0x16 CheckDeviceType(0x16, 0x15) < 0 ? 0x16 : 0x15 (swap effect 1
   instead when 0x007d413c); display depth 0x00911f9c by format; depth/stencil format by CheckDeviceFormat(usage 2,
   rtype 1) == 0 && CheckDepthStencilMatch == 0 over 0x4b, 0x4f (stencil flag 0x007d4124 = 1), 0x47, 0x4d (0),
   falling back to 16-bit 0x49 (1) / 0x50 (0); multisample type/quality from 0x006181b8 / 0x006181c0 via
   CheckDeviceMultiSampleType, swap effect 1 when multisampling.
   What moved it: adapter format read first in the fullscreen branch, the 0x007d413c branch as a three-way
   if/else with the plain assignment repeated (86.3 -> 100). */
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
typedef struct IDirect3D9 IDirect3D9;
typedef struct IDirect3D9Vtbl {
  void *QueryInterface; void *AddRef; void *Release; void *RegisterSoftwareDevice; void *GetAdapterCount;
  void *GetAdapterIdentifier; void *GetAdapterModeCount; void *EnumAdapterModes; void *GetAdapterDisplayMode;
  long (__stdcall *CheckDeviceType)(IDirect3D9 *self, unsigned int adapter, int deviceType, int adapterFormat,
                                    int backBufferFormat, int windowed);
  long (__stdcall *CheckDeviceFormat)(IDirect3D9 *self, unsigned int adapter, int deviceType, int adapterFormat,
                                      unsigned long usage, int rType, int checkFormat);
  long (__stdcall *CheckDeviceMultiSampleType)(IDirect3D9 *self, unsigned int adapter, int deviceType,
                                               int surfaceFormat, int windowed, int multiSampleType,
                                               unsigned long *qualityLevels);
  long (__stdcall *CheckDepthStencilMatch)(IDirect3D9 *self, unsigned int adapter, int deviceType,
                                           int adapterFormat, int renderTargetFormat, int depthStencilFormat);
} IDirect3D9Vtbl;
struct IDirect3D9 { IDirect3D9Vtbl *lpVtbl; };
typedef struct D3DDISPLAYMODE { unsigned int Width; unsigned int Height; unsigned int RefreshRate; int Format; } D3DDISPLAYMODE;
typedef struct D3DPRESENT_PARAMETERS {
  unsigned int BackBufferWidth; unsigned int BackBufferHeight; int BackBufferFormat; unsigned int BackBufferCount;
  int MultiSampleType; unsigned long MultiSampleQuality; int SwapEffect; void *hDeviceWindow; int Windowed;
  int EnableAutoDepthStencil; int AutoDepthStencilFormat; unsigned long Flags;
  unsigned int FullScreen_RefreshRateInHz; unsigned int PresentationInterval;
} D3DPRESENT_PARAMETERS;
typedef struct RECT { long left; long top; long right; long bottom; } RECT;
MGLOBAL(IDirect3D9 *, DAT_007d4108, 0x007d4108);
MGLOBAL(unsigned int, DAT_007d410c, 0x007d410c);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(void *, DAT_007d4104, 0x007d4104);
MGLOBAL(int, DAT_007d413c, 0x007d413c);
MGLOBAL(int, DAT_007d4124, 0x007d4124);
MGLOBAL(unsigned int, DAT_006181b8, 0x006181b8);
MGLOBAL(unsigned int, DAT_006181c0, 0x006181c0);
MGLOBAL(unsigned int, DAT_006181c4, 0x006181c4);
MGLOBAL(int, DAT_00911f9c, 0x00911f9c);
MGLOBAL(D3DPRESENT_PARAMETERS, DAT_009120e0, 0x009120e0);
__declspec(dllimport) int __stdcall GetWindowRect(void *hWnd, RECT *lpRect);

#define CHECKDEPTH(_adapterFormat, _fmt) \
  (DAT_007d4108->lpVtbl->CheckDeviceFormat(DAT_007d4108, DAT_007d410c, DAT_006181b0, (_adapterFormat), 2, 1, (_fmt)) == 0 && \
   DAT_007d4108->lpVtbl->CheckDepthStencilMatch(DAT_007d4108, DAT_007d410c, DAT_006181b0, (_adapterFormat), \
                                                DAT_009120e0.BackBufferFormat, (_fmt)) == 0)

// MATCH: 0x004c8800 FUN_004c8800
void FUN_004c8800(D3DDISPLAYMODE *mode, unsigned int flags, int windowFormat)
{
  int adapterFormat;
  int backBufferFormat;
  int depth;
  if (flags & 1) {
    adapterFormat = mode->Format;
    DAT_009120e0.Windowed = 0;
    DAT_009120e0.BackBufferCount = 1;
    DAT_009120e0.FullScreen_RefreshRateInHz = mode->RefreshRate;
    if (DAT_009120e0.FullScreen_RefreshRateInHz >= DAT_006181c4) {
      DAT_009120e0.FullScreen_RefreshRateInHz = DAT_006181c4;
    }
    DAT_009120e0.PresentationInterval = 0x80000000;
    DAT_009120e0.BackBufferWidth = mode->Width;
    DAT_009120e0.BackBufferHeight = mode->Height;
    DAT_009120e0.SwapEffect = 2;
  } else {
    RECT rect;
    DAT_009120e0.Windowed = 1;
    DAT_009120e0.BackBufferCount = 1;
    DAT_009120e0.FullScreen_RefreshRateInHz = 0;
    DAT_009120e0.PresentationInterval = 0x80000000;
    GetWindowRect(DAT_007d4104, &rect);
    DAT_009120e0.SwapEffect = 3;
    DAT_009120e0.BackBufferWidth = (rect.right - rect.left) == 0;
    DAT_009120e0.BackBufferHeight = (rect.bottom - rect.top) == 0;
    adapterFormat = windowFormat;
  }
  if (DAT_007d413c) {
    DAT_009120e0.SwapEffect = 1;
    backBufferFormat = adapterFormat;
  } else if (adapterFormat == 0x16) {
    backBufferFormat = (DAT_007d4108->lpVtbl->CheckDeviceType(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFormat,
                                                               0x15, DAT_009120e0.Windowed) < 0) + 0x15;
  } else {
    backBufferFormat = adapterFormat;
  }
  DAT_009120e0.BackBufferFormat = backBufferFormat;
  DAT_009120e0.EnableAutoDepthStencil = 1;
  switch (backBufferFormat) {
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
  DAT_00911f9c = depth;
  if (depth == 0x20) {
    if (CHECKDEPTH(adapterFormat, 0x4b)) {
      DAT_009120e0.AutoDepthStencilFormat = 0x4b;
      DAT_007d4124 = 1;
    } else if (CHECKDEPTH(adapterFormat, 0x4f)) {
      DAT_009120e0.AutoDepthStencilFormat = 0x4f;
      DAT_007d4124 = 1;
    } else if (CHECKDEPTH(adapterFormat, 0x47)) {
      DAT_009120e0.AutoDepthStencilFormat = 0x47;
      DAT_007d4124 = 0;
    } else if (CHECKDEPTH(adapterFormat, 0x4d)) {
      DAT_009120e0.AutoDepthStencilFormat = 0x4d;
      DAT_007d4124 = 0;
    } else {
      DAT_00911f9c = 0x10;
    }
  }
  if (DAT_00911f9c == 0x10) {
    if (CHECKDEPTH(adapterFormat, 0x49)) {
      DAT_009120e0.AutoDepthStencilFormat = 0x49;
      DAT_007d4124 = 1;
    } else {
      DAT_009120e0.AutoDepthStencilFormat = 0x50;
      DAT_007d4124 = 0;
    }
  }
  DAT_009120e0.MultiSampleType = 0;
  DAT_009120e0.MultiSampleQuality = 0;
  if (!DAT_007d413c && (DAT_006181b8 > 1 || DAT_006181c0 > 1)) {
    unsigned long levels;
    levels = 0;
    if (DAT_007d4108->lpVtbl->CheckDeviceMultiSampleType(DAT_007d4108, DAT_007d410c, DAT_006181b0,
                                                         DAT_009120e0.BackBufferFormat, DAT_009120e0.Windowed,
                                                         DAT_006181b8, &levels) >= 0 && levels > 0) {
      DAT_009120e0.MultiSampleType = DAT_006181b8;
      DAT_009120e0.MultiSampleQuality = levels - 1;
    } else {
      levels = 0;
      if (DAT_007d4108->lpVtbl->CheckDeviceMultiSampleType(DAT_007d4108, DAT_007d410c, DAT_006181b0,
                                                           DAT_009120e0.BackBufferFormat, DAT_009120e0.Windowed,
                                                           1, &levels) >= 0 && levels > 0) {
        unsigned long quality = DAT_006181c0 - 2;
        if (quality > levels - 1) {
          quality = levels - 1;
        }
        DAT_009120e0.MultiSampleType = 1;
        DAT_009120e0.SwapEffect = 1;
        DAT_009120e0.MultiSampleQuality = quality;
        return;
      }
    }
    if (DAT_009120e0.MultiSampleType) {
      DAT_009120e0.SwapEffect = 1;
    }
  }
}

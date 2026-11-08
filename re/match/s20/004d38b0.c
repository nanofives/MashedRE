// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004d38b0 (h4): 100% (regs 100%). IDirect3D9::CheckDeviceFormat(adapter 0x007d410c, device type 0x006181b0, adapter format 0x00911f90,
   usage 0, D3DRTYPE 3, format) >= 0.
   What moved it: the adapter format (0x00911f90) read once into a local before the call (the original keeps it
   in a register across both calls). IDirect3D9 vtable typed (+0x28 CheckDeviceFormat, +0x30 CheckDepthStencilMatch). */
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
  void *CheckDeviceType;
  long (__stdcall *CheckDeviceFormat)(IDirect3D9 *self, unsigned int adapter, int deviceType, int adapterFormat,
                                      unsigned long usage, int rType, int checkFormat);
  void *CheckDeviceMultiSampleType;
  long (__stdcall *CheckDepthStencilMatch)(IDirect3D9 *self, unsigned int adapter, int deviceType,
                                           int adapterFormat, int renderTargetFormat, int depthStencilFormat);
} IDirect3D9Vtbl;
struct IDirect3D9 { IDirect3D9Vtbl *lpVtbl; };
MGLOBAL(IDirect3D9 *, DAT_007d4108, 0x007d4108);
MGLOBAL(unsigned int, DAT_007d410c, 0x007d410c);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(int, DAT_00911f90, 0x00911f90);

// MATCH: 0x004d38b0 FUN_004d38b0
int FUN_004d38b0(int format)
{
  int adapterFormat = DAT_00911f90;
  long hr;
  hr = DAT_007d4108->lpVtbl->CheckDeviceFormat(DAT_007d4108, DAT_007d410c, DAT_006181b0, adapterFormat, 0, 3, format);
  return hr >= 0;
}

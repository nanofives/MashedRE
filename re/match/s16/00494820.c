// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00494820 (h1): best 37.0% (regs 52.6%) in this required C form, class (c) C++ EH + thiscall.
   The original is a __thiscall member (ECX = this, 1 stack arg, ret 4) with a C++ EH frame (handler
   0x005ca32b, state -1 stores at [esp+0x174]) for a smart-pointer local (the surface from GetSurfaceLevel,
   Release on every exit). C under the DDK cannot emit the EH frame. Body: width / abs(height) / pitch
   ((w+1)*3 & ~3) into this+0x164/0x168/0x16c, GetDeviceCaps (+0x1c), power-of-two sizes when TextureCaps & 2,
   floats to 0x007719ec..f8 (the last two unsigned), two CreateTexture (+0x5c) into 0x00771a10 / 0x00771a14,
   GetLevelDesc (+0x44), GetSurfaceLevel (+0x48), surface GetDesc (+0x30), format must be 0x16 or 0x19
   (else 0x8004022a).
   Tried: the same body as C++ under the DDK (match.py C++ mode, /EHs, a struct with an inline Release destructor,
   extern "C" globals, scratch D:/MashedWT/MATCH-S16_data/scratch/h1/x2_full.cpp.c) emits the same EH prologue
   and scores 44.3% (regs 69.8%). Remaining there (b): th kept in memory and the pitch computed after the
   0x168 store (inc / lea, the original keeps (w+1) unfolded). Struct-pair, self-field and bpp-local forms did
   not move it. The permuter (345 compiles) only found semantics-changing rewrites. */
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
#ifdef __cplusplus
extern "C"
#endif
int __cdecl abs(int);
#pragma intrinsic(abs)
#ifdef __cplusplus
extern "C"
#endif
void * __cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
typedef struct { int pad[0xd]; int w34; int h38; } TexSrcHdr;
typedef struct { int pad[0x11]; TexSrcHdr *p44; } TexSrc;
typedef struct { uint Format, Type, Usage, Pool, MultiSampleType, MultiSampleQuality, Width, Height; } SurfDesc;
MGLOBAL(int *, DAT_00771a0c, 0x00771a0c);
MGLOBAL(float, DAT_007719ec, 0x007719ec);
MGLOBAL(float, DAT_007719f0, 0x007719f0);
MGLOBAL(float, DAT_007719f4, 0x007719f4);
MGLOBAL(float, DAT_007719f8, 0x007719f8);
MGLOBAL(int *, DAT_00771a10, 0x00771a10);
MGLOBAL(int *, DAT_00771a14, 0x00771a14);
MGLOBAL(uint, DAT_007719fc, 0x007719fc);
MGLOBALA(char, s_Could_not_create_the_D3DX_textur_005cfcc4, 44, 0x005cfcc4);
MGLOBALA(char, s_Texture_is_format_we_can_t_handl_005cfc90, 49, 0x005cfc90);
typedef int (__stdcall *GetCapsFn)(int *self, void *caps);
typedef int (__stdcall *CreateTexFn)(int *self, uint w, uint h, uint levels, uint usage, uint fmt, uint pool, int **out, void *share);
typedef int (__stdcall *GetLevelDescFn)(int *self, uint level, SurfDesc *desc);
typedef int (__stdcall *GetSurfFn)(int *self, uint level, int **out);
typedef int (__stdcall *GetDescFn)(int *self, SurfDesc *desc);
typedef int (__stdcall *ReleaseFn)(int *self);
void FUN_00493f00(const char *fmt, ...);
// MATCH: 0x00494820 FUN_00494820
int __fastcall FUN_00494820(int *self, int edx_unused, TexSrc *src)
{
  uint caps[0x4c];
  int tw, th;
  int hr;
  int i;
  TexSrcHdr *h = src->p44;
  int w = h->w34;
  self[0x164 / 4] = w;
  self[0x168 / 4] = abs(h->h38);
  self[0x16c / 4] = (w + 1) * 3 & ~3;
  memset(caps, 0, sizeof caps);
  tw = 2;
  th = 2;
  (*(GetCapsFn *)(*DAT_00771a0c + 0x1c))(DAT_00771a0c, caps);
  if (caps[0x3c / 4] & 2) {
    while (tw < self[0x164 / 4]) tw <<= 1;
    while (th < self[0x168 / 4]) th <<= 1;
  } else {
    tw = self[0x164 / 4];
    th = self[0x168 / 4];
  }
  DAT_007719ec = (float)self[0x164 / 4];
  DAT_007719f0 = (float)self[0x168 / 4];
  DAT_007719f4 = (float)(uint)tw;
  DAT_007719f8 = (float)(uint)th;
  if ((hr = (*(CreateTexFn *)(*DAT_00771a0c + 0x5c))(DAT_00771a0c, tw, th, 1, 0, 0x16, 1, &DAT_00771a10, 0)) < 0 ||
      (hr = (*(CreateTexFn *)(*DAT_00771a0c + 0x5c))(DAT_00771a0c, tw, th, 1, 0, 0x16, 1, &DAT_00771a14, 0)) < 0) {
    FUN_00493f00(s_Could_not_create_the_D3DX_textur_005cfcc4, hr);
    return hr;
  }
  {
    SurfDesc desc;
    int *surf;
    memset(&desc, 0, sizeof desc);
    (*(GetLevelDescFn *)(*DAT_00771a10 + 0x44))(DAT_00771a10, 0, &desc);
    surf = 0;
    if ((*(GetSurfFn *)(*DAT_00771a10 + 0x48))(DAT_00771a10, 0, &surf) >= 0)
      (*(GetDescFn *)(*surf + 0x30))(surf, &desc);
    DAT_007719fc = desc.Format;
    if (desc.Format != 0x16 && desc.Format != 0x19) {
      FUN_00493f00(s_Texture_is_format_we_can_t_handl_005cfc90, desc.Format);
      if (surf != 0) (*(ReleaseFn *)(*surf + 8))(surf);
      return 0x8004022a;
    }
    if (surf != 0) (*(ReleaseFn *)(*surf + 8))(surf);
    return 0;
  }
}

// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* RWC-DRIVER-2 (2026-11-11): 100% under TOOLCHAIN rw9044 (L-131). The two RWERROR sites are written out, each with
   its own message and `return 0`. VC6 + 9044 cross-jumps the common tail after the per-site push, as the original;
   the DDK compiles the same text to 82.1% (first site shrink-wrapped). The `msg` local and the goto are gone. */
/* RWC-DRIVER round 2: no change. Tried the shared RWERROR tail as a static __inline helper (82.1%, early-return shrink-wrap comes back), the helper called from the goto label (97.1%, same), a ternary message on a flag (94.7%), and both RWERROR sites with &width/&height pointer locals set at entry (95.8%: the compiler then cross-jumps the two tails with a per-site message push like the original, but the two lea's sit at entry instead of after the palette block). */
/* RWC-DRIVER 2026-11-11: 53.3% -> 97.1% (regs 97.9%). Rewritten from the disassembly: RwRaster / raster-extension / format-table structs (as in s20/004d2c80), palette block +0x400 index and the ushort free-index stack, all IDirect3DDevice9 calls __stdcall (+0x64 CreateCubeTexture, +0x5c CreateTexture; the depth-format arm 0x46..0x50 uses levels 1, usage 2, pool 0 and links the raster), (fmt & 0xf00) in a local before the 0x6000 test, the palettized error as 'goto' into the shared RWERROR tail with the message in a local (the original pushes each string then jumps; one shared call site keeps ebx/ebp saved at entry). Residual: the two message pushes are 'mov eax, imm' + one 'push eax' in ours, and one register pick in the palette alloc call. */
/* MATCH-RWL01 0x004d2980: best 53.3% (regs ~56%, ops 64%), class (b). Texture creation (IDirect3DDevice9 slots 0x5c/0x64 via cdecl-typed Ghidra pointers, still to be typed per call) with the flag byte read as byte<<8 (xor eax,eax; mov ah) and the format tests on that word; strings as MGLOBALA char arrays (26 and 72 bytes). Remaining: the original hoists (flags & 0xf00) into eax before the 0x6000 test, the cube-texture branch is placed out of line, and the call-cleanup (stdcall vs cdecl per slot) is not yet matched. */
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef unsigned char uchar;
typedef struct { int pluginID; int errorCode; } RwError;
typedef struct { int d3dFormat; uchar b4; uchar alpha; uchar pad[2]; } FmtEnt;
typedef struct {
  void *parent; uchar *cpPixels; uchar *palette; int width; int height; int depth; int stride;
  short nOffsetX; short nOffsetY; uchar cType; uchar cFlags; uchar privateFlags; uchar cFormat;
} RwRaster;
typedef struct { uchar entries[0x400]; uint index; } Pal;
typedef struct {
  void *texture; Pal *palette; uchar alpha; uchar cube; uchar automipmap; uchar compressed;
  int pad0c; int pad10; int pad14; int d3dFormat;
} RasExt;
typedef long (__stdcall *ctex_fn)(int *, uint, uint, uint, uint, int, int, void *, void *);
typedef long (__stdcall *ccube_fn)(int *, uint, uint, uint, int, int, void *, void *);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(uint, DAT_007d46d0, 0x007d46d0);
MGLOBAL(uint, DAT_007d46cc, 0x007d46cc);
MGLOBAL(unsigned short *, DAT_007d46d8, 0x007d46d8);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d46dc, 0x007d46dc);
MGLOBAL(undefined4, DAT_007d46e4, 0x007d46e4);
MGLOBAL(int *, DAT_007d46e0, 0x007d46e0);
MGLOBAL(uint, DAT_007d46d4, 0x007d46d4);
MGLOBALA(FmtEnt, DAT_005d8bf0, 16, 0x005d8bf0);
int FUN_004cc7f0(int p0, int p1, int p2, int p3);
int FUN_004d7ff0(int p0, ...);
RwError *FUN_004d8480(RwError *p0);
void FUN_004ddcb0(int *p0, int *p1, uint p2);
MGLOBALA(char, s_Failed_to_create_texture__0061832c, 26, 0x0061832c);
MGLOBALA(char, s_Palletized_texture_can_only_be_r_00618348, 72, 0x00618348);
// MATCH: 0x004d2980 FUN_004d2980
int FUN_004d2980(RwRaster *param_1,RasExt *param_2)
{
  uint fmt;
  uint idx;
  long hr;
  int *node;
  uint mask;
  RwError err;

  fmt = (uint)param_1->cFormat << 8;
  mask = fmt & 0xf00;
  if ((fmt & 0x6000) == 0) {
    param_2->d3dFormat = DAT_005d8bf0[(fmt >> 8) & 0xf].d3dFormat;
  }
  else {
    if (mask != 0x600 && mask != 0x500) {
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(2, s_Palletized_texture_can_only_be_r_00618348);
      FUN_004d8480(&err);
      return 0;
    }
    if (DAT_007d46dc == 0) {
      DAT_007d46dc = FUN_004cc7f0(0x404,0x40,4,0x30411);
    }
    param_2->palette = (Pal *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d46dc,0x30411);
    if (param_2->palette != 0) {
      if (DAT_007d46cc != 0) {
        DAT_007d46cc--;
        idx = DAT_007d46d8[DAT_007d46cc];
      }
      else {
        idx = DAT_007d46d4++;
      }
      param_2->palette->index = idx;
    }
    param_2->d3dFormat = 0x29;
  }
  param_2->alpha = DAT_005d8bf0[(fmt >> 8) & 0xf].alpha;
  FUN_004ddcb0(&param_1->width, &param_1->height, param_1->cType | fmt);
  if (param_2->cube & 0xf) {
    hr = (*(ccube_fn *)(*DAT_007d4110 + 0x64))(DAT_007d4110, param_1->width, (~fmt >> 15) & 1,
          (param_2->automipmap & 0xf) ? 0x400 : 0, param_2->d3dFormat, 1, param_2, 0);
  }
  else if (param_2->d3dFormat >= 0x46 && param_2->d3dFormat <= 0x50) {
    hr = (*(ctex_fn *)(*DAT_007d4110 + 0x5c))(DAT_007d4110, param_1->width, param_1->height, 1, 2,
          param_2->d3dFormat, 0, param_2, 0);
    if (hr >= 0) {
      node = (int *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d46e4,0x30411);
      node[0] = (int)param_1;
      node[1] = (int)DAT_007d46e0;
      DAT_007d46e0 = node;
    }
  }
  else {
    hr = (*(ctex_fn *)(*DAT_007d4110 + 0x5c))(DAT_007d4110, param_1->width, param_1->height, (~fmt >> 15) & 1,
          (param_2->automipmap & 0xf) ? 0x400 : 0, param_2->d3dFormat, 1, param_2, 0);
  }
  if (hr < 0) {
    if (param_2->d3dFormat == 0x29 && param_2->palette != 0) {
      idx = param_2->palette->index;
      if (DAT_007d46cc >= DAT_007d46d0) {
        DAT_007d46d0 += 0x200;
        if (DAT_007d46d8 == 0) {
          DAT_007d46d8 = (unsigned short *)(**(code **)(DAT_007d3ff8 + 0x108))(DAT_007d46d0 * 2,0x1030411);
        }
        else {
          DAT_007d46d8 = (unsigned short *)(**(code **)(DAT_007d3ff8 + 0x110))(DAT_007d46d8,DAT_007d46d0 * 2,0x1030411);
        }
      }
      DAT_007d46d8[DAT_007d46cc] = (unsigned short)idx;
      DAT_007d46cc++;
      (**(code **)(DAT_007d3ff8 + 0x11c))(DAT_007d46dc,param_2->palette);
      param_2->palette = 0;
    }
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(2, s_Failed_to_create_texture__0061832c);
    FUN_004d8480(&err);
    return 0;
  }
  if (param_2->palette != 0) {
    node = (int *)(**(code **)(DAT_007d3ff8 + 0x118))(DAT_007d46e4,0x30411);
    node[0] = (int)param_1;
    node[1] = (int)DAT_007d46e0;
    DAT_007d46e0 = node;
  }
  return 1;
}

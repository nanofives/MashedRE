// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d6e70 (h1, by hand): 100.0% exact, regs 100.0% (raw 44.4%). Texture -> stage bind: addressing U/V
   (texture+0x50 bits 8..11 / 12..15, map 0x005d8c94), max anisotropy byte at texture + DAT_007d6c10 (sampler state
   10, filter += 7 when > 1), filter pair table 0x005d8ca8 into sampler states 6/5/7 (mag 3 only with caps bit
   0x4000000 at 0x00911fe0, else 2), then 0x004d6ce0(raster, stage). NULL texture on stage 0 resets render states
   0x1b and 0xf through the deferred set, and SetTexture(stage, NULL) via device slot +0x104. Two local macros
   (sampler-state set with cache [8][14] at 0x007d4f60, deferred render-state set), #undef after. Per-stage cache
   as a 24-byte struct at 0x007d6b30. Array bounds come from the gaps between globals. relocheck OK, C++ ok. */
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
typedef struct { int value; int changed; } FUN_004d6e70_rs;
typedef struct { int raster; uint addressU; uint addressV; uint filter; int pad10; uint maxAniso; } FUN_004d6e70_stage;
typedef struct { int raster; int pad04[19]; uint filterAddressing; } FUN_004d6e70_tex;
typedef int FUN_004d6e70_samp[14];
typedef int FUN_004d6e70_filt[2];
typedef int (__stdcall *FUN_004d6e70_setss)(int *self, int stage, int type, int value);
typedef int (__stdcall *FUN_004d6e70_settex)(int *self, int stage, int tex);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d6c10, 0x007d6c10);
MGLOBAL(uint, DAT_00911fe0, 0x00911fe0);
MGLOBAL(int, DAT_007d6b14, 0x007d6b14);
MGLOBAL(int, DAT_007d6b10, 0x007d6b10);
MGLOBAL(uint, DAT_007d6c14, 0x007d6c14);
MGLOBALA(int, DAT_007d5168, 210, 0x007d5168);
MGLOBALA(FUN_004d6e70_rs, DAT_007d57f8, 210, 0x007d57f8);
MGLOBALA(int, DAT_005d8c94, 5, 0x005d8c94);
MGLOBALA(FUN_004d6e70_filt, DAT_005d8ca8, 14, 0x005d8ca8);
MGLOBALA(FUN_004d6e70_samp, DAT_007d4f60, 8, 0x007d4f60);
MGLOBALA(FUN_004d6e70_stage, DAT_007d6b30, 8, 0x007d6b30);
int FUN_004d6ce0(int raster, int stage);
#define FUN_004d6e70_SS(st, ty, v)                                                   \
  do {                                                                               \
    DAT_007d4f60[st][ty] = (v);                                                      \
    (*(FUN_004d6e70_setss *)(*DAT_007d4110 + 0x114))(DAT_007d4110, st, ty, v);       \
  } while (0)
#define FUN_004d6e70_RS(s, v)                                                        \
  do {                                                                               \
    DAT_007d57f8[s].value = (v);                                                     \
    if (!DAT_007d57f8[s].changed) {                                                  \
      DAT_007d57f8[s].changed = 1;                                                   \
      DAT_007d5168[DAT_007d6c14++] = (s);                                            \
    }                                                                                \
  } while (0)
// MATCH: 0x004d6e70 FUN_004d6e70
int FUN_004d6e70(FUN_004d6e70_tex *param_1,int param_2)

{
  uint mode;
  uint filter;
  int aniso;
  int minmag;
  int mip;

  if (param_1 != 0) {
    mode = (param_1->filterAddressing >> 8) & 0xf;
    if (mode != DAT_007d6b30[param_2].addressU) {
      DAT_007d6b30[param_2].addressU = mode;
      FUN_004d6e70_SS(param_2, 1, DAT_005d8c94[mode]);
    }
    mode = (param_1->filterAddressing >> 12) & 0xf;
    if (mode != DAT_007d6b30[param_2].addressV) {
      DAT_007d6b30[param_2].addressV = mode;
      FUN_004d6e70_SS(param_2, 2, DAT_005d8c94[mode]);
    }
    filter = param_1->filterAddressing & 0xff;
    if (DAT_007d6c10 > 0) {
      aniso = *((uchar *)param_1 + DAT_007d6c10);
      if (DAT_007d6b30[param_2].maxAniso != (uint)aniso) {
        DAT_007d6b30[param_2].maxAniso = (uint)aniso;
        FUN_004d6e70_SS(param_2, 10, aniso);
      }
      if (aniso > 1) {
        filter += 7;
      }
    }
    if (DAT_007d6b30[param_2].filter != filter) {
      DAT_007d6b30[param_2].filter = filter;
      minmag = DAT_005d8ca8[filter][0];
      mip = DAT_005d8ca8[filter][1];
      if (minmag == 3) {
        FUN_004d6e70_SS(param_2, 6, 3);
        if (DAT_00911fe0 & 0x4000000) {
          FUN_004d6e70_SS(param_2, 5, 3);
        }
        else {
          FUN_004d6e70_SS(param_2, 5, 2);
        }
      }
      else {
        FUN_004d6e70_SS(param_2, 6, minmag);
        FUN_004d6e70_SS(param_2, 5, minmag);
      }
      FUN_004d6e70_SS(param_2, 7, mip);
    }
    FUN_004d6ce0(param_1->raster, param_2);
    return 1;
  }
  if (param_2 == 0 && DAT_007d6b14 != 0) {
    DAT_007d6b14 = 0;
    if (DAT_007d6b10 == 0) {
      FUN_004d6e70_RS(0x1b, 0);
      if (DAT_007d57f8[0xf].value != 0) {
        FUN_004d6e70_RS(0xf, 0);
      }
    }
  }
  if (DAT_007d6b30[param_2].raster != 0) {
    DAT_007d6b30[param_2].raster = 0;
    (*(FUN_004d6e70_settex *)(*DAT_007d4110 + 0x104))(DAT_007d4110, param_2, 0);
  }
  return 1;
}
#undef FUN_004d6e70_SS
#undef FUN_004d6e70_RS

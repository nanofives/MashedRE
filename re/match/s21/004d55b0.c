// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d55b0 (h1, by hand): 100.0% exact, regs 100.0% (raw 6.5%). Surface properties -> D3DMATERIAL9 at
   0x007d5120 + SetMaterial (device slot +0xc4, returns hr >= 0). Early-out when the cached diffuse/ambient
   (0x007d6c24/28), colour (0x007d6c20), flags & 0x48 (0x007d6c1c) and ambient colour (0x007d6c00..08 vs
   0x006181a0..a8) are unchanged; cache compares are float-as-int (local FI macro). Modulate (0x40) and prelit (8)
   paths set render states 0x8b/0x8d/0x93/0x94 through the deferred set-if-different macro. Moves: float-as-int
   compares on both sides (29.2 -> 63.2), 1.0f/255.0f as a literal instead of the DAT_005ceb90 global (x87 operand
   order, 63.8), masking param_3 in place instead of a flags local (99.7), the flags cache store after the colour
   store (100), and the Diffuse.b = .g = .r chain order (relocheck). relocheck OK, C++ ok. */
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
typedef struct { int value; int changed; } FUN_004d55b0_rs;
typedef struct { float r, g, b, a; } FUN_004d55b0_col;
typedef struct {
  FUN_004d55b0_col Diffuse;
  FUN_004d55b0_col Ambient;
  FUN_004d55b0_col Specular;
  FUN_004d55b0_col Emissive;
  float Power;
} FUN_004d55b0_mat;
typedef struct { float ambient; float specular; float diffuse; } FUN_004d55b0_surf;
typedef struct { uchar red, green, blue, alpha; } FUN_004d55b0_rgba;
typedef int (__stdcall *FUN_004d55b0_setmat)(int *self, FUN_004d55b0_mat *mat);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(uint, DAT_007d6c14, 0x007d6c14);
MGLOBALA(int, DAT_007d5168, 210, 0x007d5168);
MGLOBALA(FUN_004d55b0_rs, DAT_007d57f8, 210, 0x007d57f8);
MGLOBAL(FUN_004d55b0_mat, DAT_007d5120, 0x007d5120);
MGLOBAL(float, DAT_006181a0, 0x006181a0);
MGLOBAL(float, DAT_006181a4, 0x006181a4);
MGLOBAL(float, DAT_006181a8, 0x006181a8);
MGLOBAL(float, DAT_007d6c00, 0x007d6c00);
MGLOBAL(float, DAT_007d6c04, 0x007d6c04);
MGLOBAL(float, DAT_007d6c08, 0x007d6c08);
MGLOBAL(uint, DAT_007d6c1c, 0x007d6c1c);
MGLOBAL(uint, DAT_007d6c20, 0x007d6c20);
MGLOBAL(float, DAT_007d6c24, 0x007d6c24);
MGLOBAL(float, DAT_007d6c28, 0x007d6c28);
#define FUN_004d55b0_FI(f) (*(const uint *)&(f))
#define FUN_004d55b0_RS(s, v)                                                        \
  do {                                                                               \
    if (DAT_007d57f8[s].value != (int)(v)) {                                         \
      DAT_007d57f8[s].value = (int)(v);                                              \
      if (!DAT_007d57f8[s].changed) {                                                \
        DAT_007d57f8[s].changed = 1;                                                 \
        DAT_007d5168[DAT_007d6c14++] = (s);                                          \
      }                                                                              \
    }                                                                                \
  } while (0)
// MATCH: 0x004d55b0 FUN_004d55b0
int FUN_004d55b0(FUN_004d55b0_surf *param_1,FUN_004d55b0_rgba *param_2,uint param_3)

{
  float coef;
  uint argb;

  param_3 &= 0x48;
  if (FUN_004d55b0_FI(DAT_007d6c24) == FUN_004d55b0_FI(param_1->diffuse) &&
      FUN_004d55b0_FI(DAT_007d6c28) == FUN_004d55b0_FI(param_1->ambient) &&
      DAT_007d6c20 == *(uint *)param_2 &&
      DAT_007d6c1c == param_3 &&
      FUN_004d55b0_FI(DAT_007d6c00) == FUN_004d55b0_FI(DAT_006181a0) &&
      FUN_004d55b0_FI(DAT_007d6c04) == FUN_004d55b0_FI(DAT_006181a4) &&
      FUN_004d55b0_FI(DAT_007d6c08) == FUN_004d55b0_FI(DAT_006181a8)) {
    return 1;
  }
  DAT_007d6c24 = param_1->diffuse;
  DAT_007d6c28 = param_1->ambient;
  DAT_007d6c20 = *(uint *)param_2;
  DAT_007d6c1c = param_3;
  DAT_007d6c00 = DAT_006181a0;
  DAT_007d6c04 = DAT_006181a4;
  DAT_007d6c08 = DAT_006181a8;
  if ((param_3 & 0x40) && *(uint *)param_2 != 0xffffffff) {
    coef = param_1->diffuse * (1.0f / 255.0f);
    DAT_007d5120.Diffuse.r = (float)param_2->red * coef;
    DAT_007d5120.Diffuse.g = (float)param_2->green * coef;
    DAT_007d5120.Diffuse.b = (float)param_2->blue * coef;
    DAT_007d5120.Diffuse.a = (float)param_2->alpha * (1.0f / 255.0f);
    coef = param_1->ambient * (1.0f / 255.0f);
    if (param_3 & 8) {
      argb = ((uint)param_2->alpha << 24) | ((uint)param_2->red << 16) |
             ((uint)param_2->green << 8) | param_2->blue;
      FUN_004d55b0_RS(0x8b, argb);
      FUN_004d55b0_RS(0x8d, 1);
      FUN_004d55b0_RS(0x93, 1);
      FUN_004d55b0_RS(0x94, 0);
      DAT_007d5120.Ambient.r = 0.0f;
      DAT_007d5120.Ambient.g = 0.0f;
      DAT_007d5120.Ambient.b = 0.0f;
      DAT_007d5120.Ambient.a = 1.0f;
      DAT_007d5120.Emissive.r = (float)param_2->red * DAT_006181a0 * coef;
      DAT_007d5120.Emissive.g = (float)param_2->green * DAT_006181a4 * coef;
      DAT_007d5120.Emissive.b = (float)param_2->blue * DAT_006181a8 * coef;
      DAT_007d5120.Emissive.a = 1.0f;
      goto specular;
    }
    FUN_004d55b0_RS(0x8b, 0xffffffff);
    FUN_004d55b0_RS(0x8d, 0);
    FUN_004d55b0_RS(0x93, 0);
    FUN_004d55b0_RS(0x94, 0);
    DAT_007d5120.Ambient.r = (float)param_2->red * DAT_006181a0 * coef;
    DAT_007d5120.Ambient.g = (float)param_2->green * DAT_006181a4 * coef;
    DAT_007d5120.Ambient.b = (float)param_2->blue * DAT_006181a8 * coef;
    DAT_007d5120.Emissive.r = 0.0f;
    DAT_007d5120.Emissive.g = 0.0f;
    DAT_007d5120.Emissive.b = 0.0f;
    DAT_007d5120.Emissive.a = 1.0f;
  }
  else {
    DAT_007d5120.Diffuse.b = DAT_007d5120.Diffuse.g = DAT_007d5120.Diffuse.r = param_1->diffuse;
    DAT_007d5120.Diffuse.a = 1.0f;
    FUN_004d55b0_RS(0x8b, 0xffffffff);
    FUN_004d55b0_RS(0x93, 0);
    if (param_3 & 8) {
      FUN_004d55b0_RS(0x8d, 1);
      FUN_004d55b0_RS(0x94, 1);
    }
    else {
      FUN_004d55b0_RS(0x8d, 0);
      FUN_004d55b0_RS(0x94, 0);
    }
    DAT_007d5120.Emissive.r = 0.0f;
    DAT_007d5120.Emissive.g = 0.0f;
    DAT_007d5120.Emissive.b = 0.0f;
    DAT_007d5120.Emissive.a = 1.0f;
    if (FUN_004d55b0_FI(param_1->ambient) != 0x3f800000) {
      DAT_007d5120.Ambient.r = DAT_006181a0 * param_1->ambient;
      DAT_007d5120.Ambient.g = DAT_006181a4 * param_1->ambient;
      DAT_007d5120.Ambient.b = DAT_006181a8 * param_1->ambient;
    }
    else {
      DAT_007d5120.Ambient.r = DAT_006181a0;
      DAT_007d5120.Ambient.g = DAT_006181a4;
      DAT_007d5120.Ambient.b = DAT_006181a8;
    }
  }
  DAT_007d5120.Ambient.a = 1.0f;
specular:
  DAT_007d5120.Specular.r = 0.0f;
  DAT_007d5120.Specular.g = 0.0f;
  DAT_007d5120.Specular.b = 0.0f;
  DAT_007d5120.Specular.a = 1.0f;
  DAT_007d5120.Power = 0.0f;
  return (*(FUN_004d55b0_setmat *)(*DAT_007d4110 + 0xc4))(DAT_007d4110, &DAT_007d5120) >= 0;
}
#undef FUN_004d55b0_RS
#undef FUN_004d55b0_FI

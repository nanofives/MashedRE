// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d6200 (h4): 90.5% (regs 90.5%, ops ~100%). Render-state reset rewritten from odis (raw 74.1%). An RSSET(state, value) macro over the {value, dirty} cache at 0x007d57f8 (dirty = 1 before the queue store), the three saved cache values as a local array, the -1 fill through one local, the 8x0x21 stage-cache fill as counted do-whiles over an int* at .dirty, DAT_007d6c24/6c28/6c00/6c04/6c08 as floats (gives the original's eax zero stores), RSSET(0x17) and RSSET(7) as two if/else pairs, the 8-stage SetTexture/sampler loop, and the tail call to FUN_004d53b0. Residual: one zero store. The original keeps eax = 0 for all four float stores and loads 0x7d5928 into ecx first. Ours runs out of eax after three and stores imm 0, so every later jump displacement shifts. The permuter moved the DAT_007d6c08 store below the first two RSSETs (that is this file, 716 compiles, +0.7). Array lengths in MGLOBALA are placeholders [UNCERTAIN]. Class (b). */
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
typedef struct RSPair { int value; int dirty; } RSPair;
typedef struct TSPair { int value; int dirty; } TSPair;
typedef struct Stage { int tex; int addrU; int addrV; int filter; int border; int aniso; } Stage;
typedef struct FilterMode { int minmag; int mip; } FilterMode;
typedef long (__stdcall *SetTexFn)(int *dev, unsigned int stage, void *tex);
MGLOBALA(int, DAT_007d54b0, 210, 0x007d54b0);
MGLOBALA(RSPair, DAT_007d57f8, 210, 0x007d57f8);
MGLOBALA(int, DAT_007d5168, 210, 0x007d5168);
MGLOBAL(int, DAT_007d6c14, 0x007d6c14);
MGLOBALA(int, DAT_007d5e88, 0x108, 0x007d5e88);
typedef TSPair TSRow[0x21];
MGLOBALA(TSRow, DAT_007d4720, 8, 0x007d4720);
MGLOBAL(int, DAT_007d6c18, 0x007d6c18);
MGLOBALA(int, DAT_007d4f60, 0x70, 0x007d4f60);
MGLOBAL(float, DAT_007d6c24, 0x007d6c24);
MGLOBALA(int, DAT_007d5120, 0x11, 0x007d5120);
MGLOBAL(float, DAT_007d6c28, 0x007d6c28);
MGLOBAL(float, DAT_007d6c00, 0x007d6c00);
MGLOBAL(float, DAT_007d6c04, 0x007d6c04);
MGLOBAL(float, DAT_007d6c08, 0x007d6c08);
MGLOBAL(int, DAT_007d6c1c, 0x007d6c1c);
MGLOBAL(int, DAT_007d6c20, 0x007d6c20);
MGLOBAL(int, DAT_007d6b24, 0x007d6b24);
MGLOBAL(int, DAT_007d6b1c, 0x007d6b1c);
MGLOBAL(unsigned int, DAT_00911fc4, 0x00911fc4);
MGLOBAL(int, DAT_007d6b20, 0x007d6b20);
MGLOBAL(int, DAT_007d6b28, 0x007d6b28);
MGLOBAL(int, DAT_007d6b2c, 0x007d6b2c);
MGLOBAL(int, DAT_007d6ae8, 0x007d6ae8);
MGLOBAL(int, DAT_007d6aec, 0x007d6aec);
MGLOBAL(int, DAT_007d6af0, 0x007d6af0);
MGLOBAL(int, DAT_007d6af4, 0x007d6af4);
MGLOBAL(int, DAT_007d6af8, 0x007d6af8);
MGLOBAL(int, DAT_007d6afc, 0x007d6afc);
MGLOBAL(int, DAT_007d6b00, 0x007d6b00);
MGLOBAL(int, DAT_007d6b04, 0x007d6b04);
MGLOBAL(int, DAT_007d6b08, 0x007d6b08);
MGLOBAL(int, DAT_007d6b0c, 0x007d6b0c);
MGLOBALA(Stage, DAT_007d6b30, 8, 0x007d6b30);
MGLOBAL(int *, DAT_007d4110, 0x007d4110);
MGLOBAL(int, DAT_007d6bf0, 0x007d6bf0);
MGLOBAL(int, DAT_007d6bf4, 0x007d6bf4);
MGLOBAL(int, DAT_007d6bf8, 0x007d6bf8);
MGLOBAL(int, DAT_007d6bfc, 0x007d6bfc);
MGLOBAL(int, DAT_007d6b10, 0x007d6b10);
MGLOBAL(int, DAT_007d6b14, 0x007d6b14);
MGLOBAL(int, DAT_007d6b18, 0x007d6b18);
MGLOBALA(int, DAT_005d8c48, 4, 0x005d8c48);
MGLOBALA(int, DAT_005d8c54, 4, 0x005d8c54);
MGLOBALA(int, DAT_005d8c64, 12, 0x005d8c64);
MGLOBALA(int, DAT_005d8c94, 5, 0x005d8c94);
MGLOBALA(FilterMode, DAT_005d8ca8, 7, 0x005d8ca8);
MGLOBALA(int, DAT_005d8d18, 4, 0x005d8d18);
MGLOBALA(int, DAT_005d8d28, 9, 0x005d8d28);
MGLOBALA(int, DAT_005d8d4c, 9, 0x005d8d4c);
void FUN_004d53b0(void);
void FUN_004d5480(int state, int value);
void FUN_004d54f0(int stage, int type, int value);
void FUN_004d5570(int stage, int type, int value);
#define RS (DAT_007d57f8)
#define RSSET(st, v) \
  if (RS[st].value != (v)) { \
    RS[st].value = (v); \
    if (!RS[st].dirty) { \
      RS[st].dirty = 1; \
      DAT_007d5168[DAT_007d6c14] = (st); \
      DAT_007d6c14++; \
    } \
  }
// MATCH: 0x004d6200 FUN_004d6200
void FUN_004d6200(void)
{
  int saved[3];
  unsigned int i;
  int s, k;
  int inv;
  saved[0] = RS[0x24].value;
  saved[1] = RS[0x25].value;
  saved[2] = RS[0x18].value;
  inv = -1;
  for (i = 0; i < 210; i++) DAT_007d54b0[i] = inv;
  DAT_007d54b0[34] = 0;
  DAT_007d54b0[139] = 0;
  DAT_007d54b0[58] = 0;
  DAT_007d54b0[59] = 0;
  for (i = 0; i < 210; i++) {
    RS[i].value = DAT_007d54b0[i];
    RS[i].dirty = 0;
  }
  DAT_007d6c14 = 0;
  for (i = 0; i < 0x108; i++) DAT_007d5e88[i] = inv;
  {
    int *p = &DAT_007d4720[0][0].dirty;
    s = 8;
    do {
      k = 0x21;
      do {
        p[-1] = inv;
        *p = 0;
        p += 2;
      } while (--k);
    } while (--s);
  }
  DAT_007d6c18 = 0;
  for (i = 0; i < 0x70; i++) DAT_007d4f60[i] = inv;
  DAT_007d6c24 = 0.0f;
  for (i = 0; i < 0x11; i++) DAT_007d5120[i] = 0;
  DAT_007d6c28 = 0.0f;
  DAT_007d6c00 = 0.0f;
  DAT_007d6c04 = 0.0f;
  DAT_007d6c1c = 0;
  DAT_007d6c20 = 0;
  RSSET(0x26, DAT_007d6b24);
  RSSET(0x1c, DAT_007d6b1c);
  DAT_007d6c08 = 0.0f;
  if ((DAT_00911fc4 & 0x100) && (DAT_00911fc4 & 0x100000)) {
    RSSET(0x23, DAT_005d8c54[DAT_007d6b20]);
    RSSET(0x8c, 0);
  }
  else {
    RSSET(0x23, 0);
    RSSET(0x8c, DAT_005d8c54[DAT_007d6b20]);
  }
  RSSET(0x24, saved[0]);
  RSSET(0x25, saved[1]);
  RSSET(0x22, DAT_007d6b28);
  RSSET(9, DAT_005d8c48[DAT_007d6b2c]);
  RSSET(0xe, DAT_007d6ae8);
  if (DAT_007d6aec) {
    RSSET(0x17, 4);
  }
  else {
    RSSET(0x17, 8);
  }
  if (DAT_007d6aec || DAT_007d6ae8) {
    RSSET(7, 1);
  }
  else {
    RSSET(7, 0);
  }
  RSSET(0x34, DAT_007d6af0);
  RSSET(0x35, DAT_005d8d28[DAT_007d6af4]);
  RSSET(0x36, DAT_005d8d28[DAT_007d6af8]);
  RSSET(0x37, DAT_005d8d28[DAT_007d6afc]);
  FUN_004d5480(0x38, DAT_005d8d4c[DAT_007d6b00]);
  FUN_004d5480(0x39, DAT_007d6b04);
  FUN_004d5480(0x3a, DAT_007d6b08);
  FUN_004d5480(0x3b, DAT_007d6b0c);
  for (s = 0; s < 8; s++) {
    (*(SetTexFn *)(*DAT_007d4110 + 0x104))(DAT_007d4110, s, 0);
    DAT_007d6b30[s].tex = 0;
    FUN_004d5570(s, 6, DAT_005d8ca8[DAT_007d6b30[s].filter].minmag);
    FUN_004d5570(s, 5, DAT_005d8ca8[DAT_007d6b30[s].filter].minmag);
    FUN_004d5570(s, 7, DAT_005d8ca8[DAT_007d6b30[s].filter].mip);
    FUN_004d5570(s, 1, DAT_005d8c94[DAT_007d6b30[s].addrU]);
    FUN_004d5570(s, 2, DAT_005d8c94[DAT_007d6b30[s].addrV]);
    FUN_004d5570(s, 4, DAT_007d6b30[s].border);
    FUN_004d5570(s, 10, DAT_007d6b30[s].aniso);
    if (s != 0) {
      FUN_004d54f0(s, 1, 1);
      FUN_004d54f0(s, 4, 1);
    }
  }
  FUN_004d5480(0x13, DAT_005d8c64[DAT_007d6bf0]);
  FUN_004d5480(0x14, DAT_005d8c64[DAT_007d6bf4]);
  FUN_004d5480(0x19, DAT_005d8d4c[DAT_007d6bf8]);
  FUN_004d5480(0x18, saved[2]);
  if (DAT_007d6b10) {
    FUN_004d5480(0x1b, 1);
    FUN_004d5480(0xf, DAT_007d6bfc);
  }
  else {
    FUN_004d5480(0x1b, 0);
    FUN_004d5480(0xf, 0);
  }
  DAT_007d6b14 = 0;
  FUN_004d5480(0x89, 0);
  FUN_004d5480(0x16, DAT_005d8d18[DAT_007d6b18]);
  FUN_004d54f0(0, 1, 3);
  FUN_004d54f0(0, 3, 0);
  FUN_004d54f0(0, 4, 3);
  FUN_004d54f0(0, 6, 0);
  FUN_004d5480(0x91, 0);
  FUN_004d5480(0x92, 0);
  FUN_004d5480(0x93, 0);
  FUN_004d5480(0x1a, DAT_00911fc4 & 1);
  FUN_004d5480(0x1d, 0);
  FUN_004d5480(0x8e, 0);
  FUN_004d5480(0x8b, -1);
  FUN_004d5480(0x8f, 0);
  FUN_004d53b0();
}

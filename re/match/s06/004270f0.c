// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004270f0: 100% by hand (h4), regs 100% (198/198 ins; the jump table after the ret is outside Ghidra's
   size). Raw g2s was 49.0%. What moved it: FUN_00450b10 takes (int, V2 pos, V2 size, RGBA4 col, float *uv) with
   the two float pairs and the 4-byte colour passed BY VALUE (struct members are stored then reloaded and pushed);
   as 4 scalar floats the constants propagated (77.8%). Ghidra's FUN_0041e8d0/FUN_0041e8e0/FUN_00478cd0 take
   &DAT_00646e58 (wrong arity in Ghidra). Callee 0x004e5660 is Ghidra's RwEngineForAllPlugins, called as FUN_004e5660.
   The fog colour is (0x40 << 24) | (c()[0] << 16) | (c()[1] << 8) | c()[2] with three FUN_004924e0 calls.
   The 7..11 switch is one FUN_004292d0 + FUN_004292c0 pair per case (tail-merged by the compiler), as in
   re/match/s02/0040b090.c. RS is a file-local macro for the RwRenderStateSet slot (+0x20), #undef'd at the end.
   relocheck OK. Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
typedef int (*RwRenderStateSetFn)(int state, int value);
typedef struct { uchar r, g, b, a; } RGBA4;
typedef struct { float x, y; } V2;
MGLOBAL(int, DAT_0066d704, 0x0066d704);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0066d700, 0x0066d700);
MGLOBAL(int, DAT_00656ee8, 0x00656ee8);
MGLOBAL(int, DAT_0066d6fc, 0x0066d6fc);
MGLOBAL(int, DAT_0066d708, 0x0066d708);
MGLOBAL(int, DAT_0066d70c, 0x0066d70c);
MGLOBAL(int, DAT_0066d710, 0x0066d710);
MGLOBAL(int, DAT_0066d714, 0x0066d714);
MGLOBAL(int, DAT_0066d718, 0x0066d718);
MGLOBALA(char, DAT_00646e58, 4, 0x00646e58);
MGLOBALA(int, DAT_00656f08, 4, 0x00656f08);
MGLOBAL(undefined1, DAT_00657498, 0x00657498);
MGLOBAL(undefined4, DAT_0066d728, 0x0066d728);
void FUN_0041e8d0(void *p0);
void FUN_0041e8e0(void *p0);
int FUN_0041e9b0(int p0);
int FUN_0041e9f0(void);
int FUN_0041ea00(void);
void FUN_00426d20(int p0);
void FUN_00426d90(void);
void FUN_004292c0(undefined4 *p0, int p1);
void FUN_004292d0(undefined4 *p0, void *p1);
void FUN_00450b10(int p0, V2 p1, V2 p2, RGBA4 p3, float *p4);
int FUN_004671a0(int p0);
int FUN_00467210(int p0);
void FUN_00472ad0(int p0);
void FUN_00478220(void *p0);
void FUN_00478cd0(void *p0, int p1);
void FUN_004914b0(void);
uchar *FUN_004924e0(void);
void FUN_004b5af0(void *p0, int p1, float p2);
int FUN_0055dec0(undefined4 *p0);
int FUN_004e5660(int p0);

#define RS(st, v) (*(RwRenderStateSetFn *)(DAT_007d3ff8 + 0x20))((st), (v))

// MATCH: 0x004270f0 FUN_004270f0
void FUN_004270f0(int param_1)
{
  RGBA4 col;
  V2 pos;
  V2 size;
  float uv[4];

  if (DAT_0066d704 != 0) {
    RS(0xe, 0);
    RS(8, 0);
    RS(6, 0);
    FUN_0041e9b0(param_1);
    if (FUN_0041ea00() != 0) {
      FUN_0041e8e0(DAT_00646e58);
    } else {
      FUN_00426d20(FUN_00467210(0));
      FUN_00426d90();
      pos.x = 0.0f;
      pos.y = 0.0f;
      size.x = 512.0f;
      size.y = 512.0f;
      col.r = 0;
      col.g = 0;
      col.b = 0;
      col.a = 0;
      uv[0] = 0.0f;
      uv[1] = 0.0f;
      uv[2] = 1.0f;
      uv[3] = 1.0f;
      RS(10, 1);
      RS(0xb, 2);
      FUN_00450b10(0, pos, size, col, uv);
      RS(10, 5);
      RS(0xb, 6);
    }
    RS(8, 1);
    RS(6, 1);
    RS(0xe, 1);
    (*(RwRenderStateSetFn *)(DAT_007d3ff8 + 0x20))(0xf,
        (0x40 << 24) | (FUN_004924e0()[0] << 16) | (FUN_004924e0()[1] << 8) | FUN_004924e0()[2]);
    RS(0x10, 1);
    if (DAT_0066d700 != 0) {
      FUN_004e5660(DAT_00656ee8);
    } else {
      FUN_00478cd0(DAT_00646e58, FUN_004671a0(0));
    }
    if (DAT_0066d6fc != 0) {
      FUN_00478220(&DAT_00657498);
    }
    FUN_0041e9b0(param_1);
    if (FUN_0041e9f0() != 0) {
      FUN_0041e8d0(DAT_00646e58);
    }
  }
  FUN_004914b0();
  RS(0xe, 0);
  switch (FUN_0055dec0(&DAT_0066d728)) {
  case 7:
    FUN_004292d0(&DAT_0066d728, &DAT_0066d708);
    FUN_004292c0(&DAT_0066d728, 0);
    break;
  case 8:
    FUN_004292d0(&DAT_0066d728, &DAT_0066d70c);
    FUN_004292c0(&DAT_0066d728, 0);
    break;
  case 9:
    FUN_004292d0(&DAT_0066d728, &DAT_0066d710);
    FUN_004292c0(&DAT_0066d728, 0);
    break;
  case 10:
    FUN_004292d0(&DAT_0066d728, &DAT_0066d714);
    FUN_004292c0(&DAT_0066d728, 0);
    break;
  case 11:
    FUN_004292d0(&DAT_0066d728, &DAT_0066d718);
    FUN_004292c0(&DAT_0066d728, 0);
    break;
  }
  if (DAT_0066d708 != 0 && DAT_00656f08[DAT_0066d70c] != 0) {
    FUN_00472ad0(DAT_00656f08[DAT_0066d70c]);
  }
  if (DAT_0066d710 != 0) {
    FUN_004b5af0(DAT_00646e58 + (DAT_0066d714 * 0x40 + DAT_0066d718) * 0x40, 0, 1.0f);
  }
}
#undef RS

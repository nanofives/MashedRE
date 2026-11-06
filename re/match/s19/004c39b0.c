// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c39b0 (h3): best 69.9% (regs 88.0%, ops 92.8%). RwV3dNormalize: dot, table sqrt (RwGlobals+DAT_007d3ffc) and table
   inverse sqrt (+4) through two RwSplitBits unions (volatile int members), scale, RWERROR(0x19) when length <= 
   DAT_005d757c (RwError {1, FUN_004d7ff0(0x19)} then FUN_004d8480). Structure and every op match; residual is
   register choice (in pointer in eax in the original, ecx here, so the table/test temps take eax) and the
   sqrt-result store after `pop edi`. Tried decl order, inline table expression, out/in aliases, r local,
   condition forms, permuter (184). Class (b). */
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
typedef struct RwV3d { float x, y, z; } RwV3d;
typedef union RwSplitBits { float nReal; volatile int nInt; volatile unsigned int nUInt; } RwSplitBits;
typedef struct RwError { int pluginID; int errorCode; } RwError;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d3ffc, 0x007d3ffc);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
int FUN_004d7ff0(int code);
RwError *FUN_004d8480(RwError *err);

// MATCH: 0x004c39b0 FUN_004c39b0
float FUN_004c39b0(RwV3d *out, const RwV3d *in)
{
  RwSplitBits s;
  RwSplitBits u;
  float len2;
  len2 = in->z * in->z + (in->x * in->x + in->y * in->y);
  s.nReal = len2;
  if (s.nInt) {
    const unsigned int *tab = *(unsigned int **)(DAT_007d3ffc + DAT_007d3ff8);
    unsigned int idx;
    s.nUInt += 0x800;
    idx = tab[(s.nUInt >> 12) & 0xfff];
    idx += (s.nUInt >> 1) & 0x3fc00000;
    s.nUInt = idx;
  }
  u.nReal = len2;
  if (u.nInt) {
    const unsigned int *tab = *(unsigned int **)(DAT_007d3ffc + DAT_007d3ff8 + 4);
    unsigned int idx;
    u.nUInt += 0x800;
    idx = tab[(u.nUInt >> 12) & 0xfff];
    idx += (~u.nUInt >> 1) & 0x3fc00000;
    u.nUInt = idx;
  }
  out->x = u.nReal * in->x;
  out->y = in->y * u.nReal;
  out->z = in->z * u.nReal;
  if (s.nReal <= DAT_005d757c) {
    RwError err;
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x19);
    FUN_004d8480(&err);
  }
  return s.nReal;
}

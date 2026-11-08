// FLAGS: /O2 /Ob1 /G5 /Oy-
/* MATCH-S22 0x004fb2a0 (h7): best 78.5% (regs 81.0%, ops 91.1%) under /O2 /Ob1 /G5 /Oy- (ebp frame; raw converter
   36.7%). What moved it: /Oy-; the orthonormal ((flags & 3) == 3) branch first (50.6 -> 72.2); the 3-float result
   copy written per field in both copy paths (72.2 -> 78.5); float 0.0f pad stores. Residual: the same
   mov edx,eax; and edx,0x20000; cmp edx,0x20000 flags test as 0x004fb130/0x004fb3e0 (9176 folds it to
   test eax,0x20000), plus the argument load order around the 0x004ecf53 calls (d3dx9-psgp thunk, __stdcall 3
   args). Tried: m kept in a local, the call taking DAT_007d9908. Permuter: 171 compiles, 0 gains. Class (a) for
   the folded compare [UNCERTAIN], (b) for the argument order. */
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
typedef struct { float m[16]; } M_4fb2a0;
typedef struct { float x, y, z; } V3_4fb2a0;
MGLOBAL(M_4fb2a0 *, DAT_007d9904, 0x007d9904);
MGLOBAL(M_4fb2a0 *, DAT_007d9908, 0x007d9908);
MGLOBAL(char *, DAT_007d9900, 0x007d9900);
MGLOBAL(M_4fb2a0, DAT_005daf10, 0x005daf10);
MGLOBAL(M_4fb2a0, DAT_006199f0, 0x006199f0);
MGLOBAL(M_4fb2a0, DAT_00619a30, 0x00619a30);
float * FUN_004c4dc0(void * p0, void * p1);
void FUN_004fb210(void * p0, void * p1);
void FUN_004c4670(void * p0, void * p1);
void * __stdcall FUN_004ecf53(V3_4fb2a0 *out, V3_4fb2a0 *in, M_4fb2a0 *m);
// MATCH: 0x004fb2a0 FUN_004fb2a0
void FUN_004fb2a0(V3_4fb2a0 *param_1, V3_4fb2a0 *param_2)
{
  if (DAT_007d9908 == 0) {
    if (DAT_007d9900 != 0 && (*(unsigned int *)(DAT_007d9900 + 0xc) & 0x20000) != 0x20000) {
      if ((*(unsigned int *)(DAT_007d9900 + 0xc) & 3) == 3) {
        if (DAT_007d9904 == 0) {
          FUN_004fb210(&DAT_006199f0, DAT_007d9900);
          DAT_007d9904 = &DAT_006199f0;
        }
        DAT_007d9908 = DAT_007d9904;
      } else {
        if (DAT_007d9904 == 0) {
          FUN_004c4dc0(&DAT_006199f0, DAT_007d9900);
          DAT_006199f0.m[3] = 0.0f;
          DAT_006199f0.m[7] = 0.0f;
          DAT_006199f0.m[11] = 0.0f;
          DAT_006199f0.m[15] = 1.0f;
          DAT_007d9904 = &DAT_006199f0;
        }
        FUN_004c4670(&DAT_00619a30, DAT_007d9904);
        DAT_00619a30.m[3] = 0.0f;
        DAT_007d9908 = &DAT_00619a30;
        DAT_00619a30.m[7] = 0.0f;
        DAT_00619a30.m[11] = 0.0f;
        DAT_00619a30.m[15] = 1.0f;
        FUN_004ecf53(param_2, param_1, DAT_007d9908);
        return;
      }
    } else {
      DAT_007d9908 = DAT_007d9904 = &DAT_005daf10;
      param_2->x = param_1->x;
      param_2->y = param_1->y;
      param_2->z = param_1->z;
      return;
    }
  }
  if (DAT_007d9908 != &DAT_005daf10) {
    FUN_004ecf53(param_2, param_1, DAT_007d9908);
    return;
  }
  param_2->x = param_1->x;
  param_2->y = param_1->y;
  param_2->z = param_1->z;
}

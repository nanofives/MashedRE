// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00442a20: by hand (h7). Raw 33.3%. The 16-dword loop is a 64-byte struct assignment (rep movsd);
   +0x58 is a float (fld/fstp). */
/* ---- MATCH-S08 preamble (same text in every re/match/s08 source; not part of any function body) ----
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
typedef struct { float m[16]; } Mat64_00442a20;
MGLOBAL(float, DAT_00896580, 0x00896580);
MGLOBAL(Mat64_00442a20, DAT_0089650c, 0x0089650c);
void FUN_00441700(int p0);
// MATCH: 0x00442a20 FUN_00442a20
void FUN_00442a20(int param_1)
{
  *(float *)(param_1 + 0x58) = DAT_00896580;
  FUN_00441700(param_1);
  *(Mat64_00442a20 *)(*(int *)(*(int *)(param_1 + 0x84) + 4) + 0x10) = DAT_0089650c;
}

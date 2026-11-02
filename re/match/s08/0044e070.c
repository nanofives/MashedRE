// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044e070: 100% by hand (raw 87.5%). The float at 0x005cc8f4 is 0x3e2aaaab, an .rdata literal:
   `* (1.0f / 6.0f)` (also `/ 6.0f`, or a float temp times an MGLOBAL) loads the table entry first; a plain
   `table * DAT_005cc8f4` put the constant first (75%). Table is float[][4] at 0x00894de0, indexed by the
   record's +0xc and +0x10 ints. */
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
typedef struct { char pad00[0xc]; int i0c; int i10; char pad14[0xe4]; } Rec_0044e070;
MGLOBALA(Rec_0044e070, DAT_00890080, 80, 0x00890080);
typedef float Row_0044e070[4];
MGLOBALA(Row_0044e070, DAT_00894de0, 8, 0x00894de0);
// MATCH: 0x0044e070 FUN_0044e070
float FUN_0044e070(int param_1)
{
  return DAT_00894de0[DAT_00890080[param_1].i0c][DAT_00890080[param_1].i10] * (1.0f/6.0f);
}

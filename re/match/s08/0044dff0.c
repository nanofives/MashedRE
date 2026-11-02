// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x0044dff0: 100% by hand (raw 30.8%). A 3-float struct copy from (record.h->[+4]) + 0x40 into
   *param_2 (the add edx, 0x40 then three dword moves). */
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
typedef struct { float x, y, z; } V3_0044dff0;
typedef struct { char pad00[4]; char *frame; } Obj_0044dff0;
typedef struct { Obj_0044dff0 *h; char pad04[0xf4]; } Rec_0044dff0;
MGLOBALA(Rec_0044dff0, DAT_00890080, 80, 0x00890080);
// MATCH: 0x0044dff0 FUN_0044dff0
void FUN_0044dff0(int param_1, V3_0044dff0 *param_2)
{
  *param_2 = *(V3_0044dff0 *)(DAT_00890080[param_1].h->frame + 0x40);
}

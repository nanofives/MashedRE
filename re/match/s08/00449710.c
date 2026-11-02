// FLAGS: /O2 /GS /Ob1
/* MATCH-S08 0x00449710: 100% (regs 100%) by hand (h9). Raw 81.2%. Same source as 0x004496a0 with a constant
   id 6000 and flag 0x005f96c8 (the loop counter becomes the dec edi countdown). */
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
typedef struct { float x, y, z; } MatchV3_00449710;
MGLOBAL(int, DAT_005f96c8, 0x005f96c8);
int FUN_0047ce40(int p0);
void FUN_0047cea0(int p0, MatchV3_00449710 *p1);
int FUN_0047d150(int p0);
void FUN_00465da0(int p0, int p1);
// MATCH: 0x00449710 FUN_00449710
void FUN_00449710(void)
{
  MatchV3_00449710 v;
  int i;
  int obj;

  v.x = 200.0f;
  v.y = 400.0f;
  v.z = 200.0f;
  for (i = 0; i < 3; i++) {
    obj = FUN_0047ce40(6000);
    FUN_0047cea0(obj, &v);
  }
  if (DAT_005f96c8 != 0) {
    FUN_00465da0(8, FUN_0047d150(obj) + 0x30);
    DAT_005f96c8 = 0;
  }
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429660 (h9, by hand): 100% (regs 100%).
   Raw converter 34.4%: FUN_0040dc80 takes no args (the pushes before it are FUN_00427e00's args), the colour is a
   4-byte RGBA by value, param_1 is an RGBA*, the slot table at 0x007f1a18 has stride 16 (+0 flag, +4 id).
   Both branches carry the full draw tail in source (cross-jumping merges only the last call). The .rdata floats
   0x005cc31c (3.0f) and 0x005cd618 (640.0f) are source literals: as MGLOBALs the shadow sum loads the global
   first (96.9%, permuter found nothing); as literals `fld x; fadd 3.0f` matches. Residual: none. */
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
typedef struct { unsigned char r, g, b, a; } RGBA;
typedef struct { int flag; int id; int pad[2]; } Slot;
MGLOBALA(Slot, DAT_007f1a18, 4, 0x007f1a18);
float FUN_0040dc80(void);
int FUN_0040dc90(void);
void FUN_00427e00(int p0, float p1, float p2, RGBA p3, float p4, int p5);
float FUN_00429620(void);
void FUN_0042c7c0(int p0);
int FUN_0042f500(void);
// MATCH: 0x00429660 FUN_00429660
/* [C1 2026-06-01] Renders a player name label (with drop shadow), choosing solo vs multiplayer
   string slot. */
void FUN_00429660(RGBA *param_1, int param_2)
{
  int id;
  RGBA black;
  float x;

  id = DAT_007f1a18[FUN_0040dc90()].id;
  black.r = 0;
  black.g = 0;
  black.b = 0;
  black.a = 0xff;
  x = FUN_00429620();
  if (FUN_0042f500() != 0) {
    FUN_0042c7c0(0);
    FUN_0042c7c0(1);
    id = 0xa4;
    if (DAT_007f1a18[FUN_0040dc90()].flag != 0) {
      id = 0xa5;
    }
    if (param_2 != 0) {
      FUN_00427e00(id, x + 3.0f, 403.0f, black, 1.0f, 1);
      FUN_00427e00(id, x, 400.0f, *param_1, 1.0f, 1);
      return;
    }
    FUN_00427e00(id, 640.0f - FUN_0040dc80() + 3.0f, 93.0f, black, 1.0f, 2);
    FUN_00427e00(id, 640.0f - FUN_0040dc80(), 90.0f, *param_1, 1.0f, 2);
  }
  else {
    id = id + 0x79;
    if (param_2 != 0) {
      FUN_00427e00(id, x + 3.0f, 403.0f, black, 1.0f, 1);
      FUN_00427e00(id, x, 400.0f, *param_1, 1.0f, 1);
      return;
    }
    FUN_00427e00(id, 640.0f - FUN_0040dc80() + 3.0f, 93.0f, black, 1.0f, 2);
    FUN_00427e00(id, 640.0f - FUN_0040dc80(), 90.0f, *param_1, 1.0f, 2);
  }
}

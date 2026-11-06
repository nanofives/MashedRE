// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x00485420 (h2): 100% (raw 65.2%), and 0x00485380 100% (raw 31.2%). This file holds 0x00485380:
   its id arrives in EAX, so it is a static callee defined before its caller 0x00485420 (pilot finding 1).
   FUN_004a332b(-11) must be declared __declspec(noreturn) (else an extra add esp,4). */
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
typedef struct SmplFzxState {
  int f00[24];
  float *pos;
} SmplFzxState;
typedef struct SmplFzxSlot {
  SmplFzxState **state;
  int f04;
  int f08;
  int f0c;
} SmplFzxSlot;
typedef struct SmplFzxWorld {
  int f00;
  int f04;
  int f08;
  SmplFzxSlot *slots;
} SmplFzxWorld;
MGLOBAL(SmplFzxWorld *, DAT_006e71cc, 0x006e71cc);
int FUN_00485340(int id);
__declspec(noreturn) void FUN_004a332b(int code);
// MATCH: 0x00485380 FUN_00485380
static SmplFzxState *FUN_00485380(int id)
{
  if (FUN_00485340(id) == 0) {
    FUN_004a332b(-11);
  }
  return *DAT_006e71cc->slots[id].state;
}
// MATCH: 0x00485420 FUN_00485420
int FUN_00485420(int id, float *out)
{
  float *pos;
  if (FUN_00485340(id) != 0) {
    pos = FUN_00485380(id)->pos;
    out[0] = pos[0];
    out[1] = pos[1];
    out[2] = pos[2];
    return 1;
  }
  return 0;
}

// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c1300 (h1): 100% (regs 100%), relocheck OK. RwFrameForAllChildren shape (next read before the callback, break on NULL). FLAGS add /G5 (game flags: 57.7% raw). */
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
typedef struct RwFrame RwFrame;
typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);
// MATCH: 0x004c1300 FUN_004c1300
RwFrame *FUN_004c1300(RwFrame *frame, RwFrameCallBack callBack, void *data)
{
  RwFrame *child = *(RwFrame **)((char *)frame + 0x98);
  while (child) {
    RwFrame *next = *(RwFrame **)((char *)child + 0x9c);
    if (!callBack(child, data)) {
      break;
    }
    child = next;
  }
  return frame;
}

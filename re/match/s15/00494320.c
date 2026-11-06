// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00494320 (h8): 100% (regs 100%). COM teardown: slot 0x24 call on DAT_00771a34, then per global
   "copy, null the global, Release (slot 8) the copy" (the store precedes the vtable load at 0x0049433b), then
   unconditional Release on DAT_00771a10 / DAT_00771a14. Slot layout matches IUnknown + IMediaControl Stop.
   Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct IUnk_ IUnk_;
typedef struct IUnkVtbl_ {
  void *QueryInterface;
  unsigned long (__stdcall *AddRef)(IUnk_ *self);
  unsigned long (__stdcall *Release)(IUnk_ *self);
  void *f0c[6];
  long (__stdcall *Stop)(IUnk_ *self);
} IUnkVtbl_;
struct IUnk_ { IUnkVtbl_ *lpVtbl; };
MGLOBAL(IUnk_ *, DAT_00771a34, 0x00771a34);
MGLOBAL(IUnk_ *, DAT_00771a28, 0x00771a28);
MGLOBAL(IUnk_ *, DAT_00771a2c, 0x00771a2c);
MGLOBAL(IUnk_ *, DAT_00771a30, 0x00771a30);
MGLOBAL(IUnk_ *, DAT_00771a24, 0x00771a24);
MGLOBAL(IUnk_ *, DAT_00771a20, 0x00771a20);
MGLOBAL(IUnk_ *, DAT_00771a10, 0x00771a10);
MGLOBAL(IUnk_ *, DAT_00771a14, 0x00771a14);
// MATCH: 0x00494320 FUN_00494320
void FUN_00494320(void)
{
  IUnk_ *t;

  if (DAT_00771a34 != 0) {
    DAT_00771a34->lpVtbl->Stop(DAT_00771a34);
    t = DAT_00771a34;
    if (t != 0) {
      DAT_00771a34 = 0;
      t->lpVtbl->Release(t);
    }
  }
  t = DAT_00771a28;
  if (t != 0) {
    DAT_00771a28 = 0;
    t->lpVtbl->Release(t);
  }
  t = DAT_00771a2c;
  if (t != 0) {
    DAT_00771a2c = 0;
    t->lpVtbl->Release(t);
  }
  t = DAT_00771a30;
  if (t != 0) {
    DAT_00771a30 = 0;
    t->lpVtbl->Release(t);
  }
  t = DAT_00771a24;
  if (t != 0) {
    DAT_00771a24 = 0;
    t->lpVtbl->Release(t);
  }
  t = DAT_00771a20;
  if (t != 0) {
    DAT_00771a20 = 0;
    t->lpVtbl->Release(t);
  }
  DAT_00771a10->lpVtbl->Release(DAT_00771a10);
  DAT_00771a14->lpVtbl->Release(DAT_00771a14);
}

// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004943f0 (h8): 100% (regs 100%). Takes a float stack parameter (Ghidra's local_4 at [esp+8] is
   param_1); the object at DAT_00771a20 is called through vtable slots 0x20 (get, long* out) and 0x1c (put, 1 arg),
   stdcall, the IBasicAudio get_Volume/put_Volume slot layout. The out long is reused for the result (the dead
   store at 0x0049444b). Constants as float literals 0.1f / -10000.0f / -2000.0f (0x005cc56c, 0x005ce038,
   0x005cd904): with globals the compiler emits fld const; fmul param in each branch instead of hoisting the fld.
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
typedef struct IBasicAudio_ IBasicAudio_;
typedef struct IBasicAudioVtbl_ {
  void *IDispatch[7];
  long (__stdcall *put_Volume)(IBasicAudio_ *self, long v);
  long (__stdcall *get_Volume)(IBasicAudio_ *self, long *v);
} IBasicAudioVtbl_;
struct IBasicAudio_ { IBasicAudioVtbl_ *lpVtbl; };
MGLOBAL(IBasicAudio_ *, DAT_00771a20, 0x00771a20);
// MATCH: 0x004943f0 FUN_004943f0
undefined4 FUN_004943f0(float param_1)
{
  long vol;

  if (DAT_00771a20->lpVtbl->get_Volume(DAT_00771a20, &vol) == (long)0x80004001) {
    return 0;
  }
  if (param_1 < 0.1f) {
    vol = -10000 - (long)(param_1 * -10000.0f);
  }
  else {
    vol = -2000 - (long)(param_1 * -2000.0f);
  }
  DAT_00771a20->lpVtbl->put_Volume(DAT_00771a20, vol);
  return 1;
}

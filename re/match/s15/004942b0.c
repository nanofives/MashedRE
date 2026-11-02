// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004942b0 (h8): 100% (regs 100%). DAT_00771a28 is called through vtable slots 0x20 (5 args: 3 long*
   outs + 0) and 0x30 (3 longs), DAT_00771a2c through slot 0x20 with one double 0.0 (the two push 0); the slot
   layouts match IMediaEvent GetEvent/FreeEventParams and IMediaPosition put_CurrentPosition. Three named long
   locals in declaration order give the original frame. Residual: none. */
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
typedef struct IMediaEvent_ IMediaEvent_;
typedef struct IMediaEventVtbl_ {
  void *IDispatch[7];
  void *f1c;
  long (__stdcall *GetEvent)(IMediaEvent_ *self, long *lEventCode, long *lParam1, long *lParam2, long msTimeout);
  void *f24;
  void *f28;
  void *f2c;
  long (__stdcall *FreeEventParams)(IMediaEvent_ *self, long lEventCode, long lParam1, long lParam2);
} IMediaEventVtbl_;
struct IMediaEvent_ { IMediaEventVtbl_ *lpVtbl; };
typedef struct IMediaPosition_ IMediaPosition_;
typedef struct IMediaPositionVtbl_ {
  void *IDispatch[7];
  void *f1c;
  long (__stdcall *put_CurrentPosition)(IMediaPosition_ *self, double t);
} IMediaPositionVtbl_;
struct IMediaPosition_ { IMediaPositionVtbl_ *lpVtbl; };
MGLOBAL(IMediaEvent_ *, DAT_00771a28, 0x00771a28);
MGLOBAL(IMediaPosition_ *, DAT_00771a2c, 0x00771a2c);
MGLOBAL(int, DAT_00771a08, 0x00771a08);
MGLOBAL(int, DAT_00771a04, 0x00771a04);
// MATCH: 0x004942b0 FUN_004942b0
void FUN_004942b0(void)
{
  long evCode;
  long param1;
  long param2;

  if (DAT_00771a28 != 0) {
    if (DAT_00771a28->lpVtbl->GetEvent(DAT_00771a28, &evCode, &param1, &param2, 0) >= 0) {
      if (evCode == 1) {
        DAT_00771a2c->lpVtbl->put_CurrentPosition(DAT_00771a2c, 0.0);
        if (DAT_00771a08 == 0) {
          DAT_00771a04 = 0;
        }
      }
      DAT_00771a28->lpVtbl->FreeEventParams(DAT_00771a28, evCode, param1, param2);
    }
  }
}

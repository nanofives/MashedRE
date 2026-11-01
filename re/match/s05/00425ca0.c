// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00425ca0 (175 B), h9 by hand: 86.5% (regs 86.5%). Raw 32.7%. DAT_005cc33c is the .rdata float -1.0,
   written as the literal -1.0f (a float global loads it first and multiplies the local, the original does fld local;
   fmul [c]). RwFrame/RwMatrix typed (+0x40 = modelling.pos, +0x20 = modelling.up, FUN_004c1520 = (frame, axis,
   float angle, op)). Residual (b): each fmul is scheduled one instruction later than the original (after the next
   struct-copy mov / push). Tried: literal vs const/global, -1.0f*v vs v*-1.0f, *=, double -1.0, a scale local, an
   __inline V3Scale helper, a matrix pointer temp, untyped int offsets; permuter 144 compiles: no change. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
typedef struct { float x; float y; float z; } V3;
typedef struct { V3 right; int flags; V3 up; int pad1; V3 at; int pad2; V3 pos; int pad3; } RwMatrix;
typedef struct { int obj[2]; int link[2]; RwMatrix modelling; } RwFrame;
typedef struct { int a; RwFrame *frame; } Obj;
RwFrame *FUN_004c1340(RwFrame *f, V3 *v, int op);
RwFrame *FUN_004c1520(RwFrame *f, V3 *axis, float angle, int op);
// MATCH: 0x00425ca0 FUN_00425ca0
void FUN_00425ca0(Obj *param_1, float param_2)
{
  V3 v;
  v = param_1->frame->modelling.pos;
  v.x = v.x * -1.0f;
  v.y = v.y * -1.0f;
  v.z = v.z * -1.0f;
  FUN_004c1340(param_1->frame, &v, 2);
  FUN_004c1520(param_1->frame, &param_1->frame->modelling.up, param_2, 2);
  v.x = v.x * -1.0f;
  v.y = v.y * -1.0f;
  v.z = v.z * -1.0f;
  FUN_004c1340(param_1->frame, &v, 2);
}
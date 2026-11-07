// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cafd0 (h1): 100% (regs 100%). Moved by: both if/else orders swapped (73.7% -> 93.0%) and ext->obj read in each branch instead of a local (-> 100%). With param_2 nonzero, reads the plugin block at (*param_2 + DAT_00911ae4):
   byte +9 low nibble zero -> object (+0) vtable +0x48 (__stdcall) (ext->obj, 0, &surf), else (ext->obj, byte >> 4, 0, &surf).
   Then the DAT_007d4578[param_1] cache: on change store surf and call the device (DAT_007d4110) vtable +0x94 with
   (device, param_1, surf), result = (hr >= 0); unchanged -> 1. Releases surf through its vtable +8 when nonzero.
   FLAGS /G5 (RW core). */
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
typedef struct IUnk { struct IUnkVtbl *v; } IUnk;
typedef int (__stdcall *UnkFn0)(IUnk *self);
typedef int (__stdcall *GetSurf3)(IUnk *self, unsigned int level, IUnk **out);
typedef int (__stdcall *GetSurf4)(IUnk *self, unsigned int face, unsigned int level, IUnk **out);
typedef int (__stdcall *DevFn2)(IUnk *self, int a, IUnk *b);
typedef struct IUnkVtbl { void *f[0x26]; } IUnkVtbl;
typedef struct RasExt { IUnk *obj; int pad4; char pad8; unsigned char flags; } RasExt;
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
MGLOBAL(IUnk *, DAT_007d4110, 0x007d4110);
MGLOBALA(IUnk *, DAT_007d4578, 16, 0x007d4578);

// MATCH: 0x004cafd0 FUN_004cafd0
int FUN_004cafd0(int param_1, int *param_2)
{
  RasExt *ext;
  IUnk *surf;
  int ok;
  surf = 0;
  if (param_2 != 0) {
    ext = (RasExt *)(*param_2 + DAT_00911ae4);
    if ((ext->flags & 0xf) != 0) {
      ((GetSurf4)ext->obj->v->f[0x48 / 4])(ext->obj, ext->flags >> 4, 0, &surf);
    } else {
      ((GetSurf3)ext->obj->v->f[0x48 / 4])(ext->obj, 0, &surf);
    }
  }
  if (DAT_007d4578[param_1] != surf) {
    DAT_007d4578[param_1] = surf;
    ok = ((DevFn2)DAT_007d4110->v->f[0x94 / 4])(DAT_007d4110, param_1, surf) >= 0;
  } else {
    ok = 1;
  }
  if (surf != 0) {
    ((UnkFn0)surf->v->f[8 / 4])(surf);
  }
  return ok;
}

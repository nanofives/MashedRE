// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004c8c70 (h3): 66.2% (regs 89.7%, ops 98.6%). D3D9 device close: frees 0x007d4130 (RwGlobals+0x10c), FUN_005cb42b(), clears MXCSR bit 15 (_mm_getcsr/_mm_setcsr intrinsics) when 0x007d458c, frees the 260 slots at 0x007d4158 to the freelist 0x007d4568 (+0x11c) and destroys it (FUN_004cc9f0), frees 0x007d4570, resets device state through the IDirect3DDevice9 vtable (+0x104 x8, +0x1a0, +0x190 x4, +0x1ac, +0x15c, +0x170), releases every 12-byte entry at 0x007d414c until its refcount is 0 and frees its +8 block, frees the array, FUN_004cfe40/004dcdc0/004db550, Release (+8) the device. The raw converter could not emit the MXCSR access (C file was not generated). Residual: the MXCSR mask compiles to "and ch, 0x7f" (ours) vs "and eax, 0xffff7fff" (original), which shifts every later register choice (the freelist-loop operand load order, the alternating ecx/edx vtable loads) and accounts for nearly all of the miss. Tried: mask as & 0xffff7fff, & ~0x8000 | 0 (the _MM_SET_FLUSH_ZERO_MODE form), int / unsigned / unsigned long locals, ^= form; /G6, game flags and plain /O2 (64.0%); permuter 224 compiles (its 85.5% variant moved FUN_004cc9f0 after the global was zeroed, which changes behaviour: rejected). Class (a): 9176 always narrows the AND to a byte op here. */
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
unsigned int _mm_getcsr(void);
void _mm_setcsr(unsigned int csr);
#pragma intrinsic(_mm_getcsr, _mm_setcsr)
typedef struct IDev { struct IDevVtbl *lpVtbl; } IDev;
typedef unsigned long (__stdcall *fpRelease)(void *self);
typedef long (__stdcall *fpSet2)(void *self, unsigned int a, void *b);
typedef long (__stdcall *fpSet1)(void *self, void *a);
typedef long (__stdcall *fpSet4)(void *self, unsigned int a, void *b, unsigned int c, unsigned int d);
typedef struct IDevVtbl {
  void *QueryInterface;
  void *AddRef;
  fpRelease Release;
  void *pad0[62];
  fpSet2 SetTexture;           /* +0x104 */
  void *pad1[21];
  fpSet1 Set15c;               /* +0x15c */
  void *pad2[4];
  fpSet1 Set170;               /* +0x170 */
  void *pad3[7];
  fpSet4 SetStreamSource;      /* +0x190 */
  void *pad4[3];
  fpSet1 Set1a0;               /* +0x1a0 */
  void *pad5[2];
  fpSet1 Set1ac;               /* +0x1ac */
} IDevVtbl;
typedef struct IUnk { struct IUnkVtbl *lpVtbl; } IUnk;
typedef struct IUnkVtbl { void *QueryInterface; void *AddRef; fpRelease Release; } IUnkVtbl;
typedef struct ResEntry { IUnk *obj; int pad; void *mem; } ResEntry;
typedef void (*fpFree)(void *mem);
typedef void (*fpFreeListFree)(void *fl, void *mem);
MGLOBAL(int, DAT_007d4120, 0x007d4120);
MGLOBAL(void *, DAT_007d4130, 0x007d4130);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4128, 0x007d4128);
MGLOBAL(int, DAT_007d458c, 0x007d458c);
MGLOBALA(void *, DAT_007d4158, 260, 0x007d4158);
MGLOBAL(void *, DAT_007d4568, 0x007d4568);
MGLOBAL(int, DAT_007d456c, 0x007d456c);
MGLOBAL(void *, DAT_007d4570, 0x007d4570);
MGLOBAL(IDev *, DAT_007d4110, 0x007d4110);
MGLOBAL(unsigned int, DAT_007d4144, 0x007d4144);
MGLOBAL(ResEntry *, DAT_007d414c, 0x007d414c);
MGLOBAL(int, DAT_007d4148, 0x007d4148);
void FUN_005cb42b(void);
void FUN_004cc9f0(void *fl);
void FUN_004cfe40(void);
void FUN_004dcdc0(void);
void FUN_004db550(void);

// MATCH: 0x004c8c70 FUN_004c8c70
int FUN_004c8c70(void)
{
  unsigned int i;

  DAT_007d4120 = 0;
  if (DAT_007d4130) {
    (*(fpFree *)(DAT_007d3ff8 + 0x10c))(DAT_007d4130);
    DAT_007d4130 = 0;
    DAT_007d4128 = 0;
  }
  FUN_005cb42b();
  if (DAT_007d458c) {
    _mm_setcsr((_mm_getcsr() & ~0x8000) | 0);
  }
  for (i = 0; i < 260; i++) {
    if (DAT_007d4158[i]) {
      (*(fpFreeListFree *)(DAT_007d3ff8 + 0x11c))(DAT_007d4568, DAT_007d4158[i]);
      DAT_007d4158[i] = 0;
    }
  }
  FUN_004cc9f0(DAT_007d4568);
  DAT_007d4568 = 0;
  DAT_007d456c = 0;
  if (DAT_007d4570) {
    (*(fpFree *)(DAT_007d3ff8 + 0x10c))(DAT_007d4570);
    DAT_007d4570 = 0;
  }
  for (i = 0; i < 8; i++) {
    DAT_007d4110->lpVtbl->SetTexture(DAT_007d4110, i, 0);
  }
  DAT_007d4110->lpVtbl->Set1a0(DAT_007d4110, 0);
  for (i = 0; i < 4; i++) {
    DAT_007d4110->lpVtbl->SetStreamSource(DAT_007d4110, i, 0, 0, 0);
  }
  DAT_007d4110->lpVtbl->Set1ac(DAT_007d4110, 0);
  DAT_007d4110->lpVtbl->Set15c(DAT_007d4110, 0);
  DAT_007d4110->lpVtbl->Set170(DAT_007d4110, 0);
  for (i = 0; i < DAT_007d4144; i++) {
    if (DAT_007d414c[i].obj) {
      while (DAT_007d414c[i].obj->lpVtbl->Release(DAT_007d414c[i].obj) > 0) {
      }
    }
    if (DAT_007d414c[i].mem) {
      (*(fpFree *)(DAT_007d3ff8 + 0x10c))(DAT_007d414c[i].mem);
    }
  }
  if (DAT_007d414c) {
    (*(fpFree *)(DAT_007d3ff8 + 0x10c))(DAT_007d414c);
    DAT_007d414c = 0;
  }
  DAT_007d4144 = 0;
  DAT_007d4148 = 0;
  FUN_004cfe40();
  FUN_004dcdc0();
  FUN_004db550();
  DAT_007d4110->lpVtbl->Release(DAT_007d4110);
  DAT_007d4110 = 0;
  return 1;
}

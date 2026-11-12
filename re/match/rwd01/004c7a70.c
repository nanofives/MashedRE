// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWD01 0x004c7a70 _rwDeviceSystemFn (2811 B): by hand, best 93.2% (regs 93.9%, ops 98.3%, 814/821 ins). Not matched; relocheck
   BAD only because the code is not yet byte-identical (offsets shift).
   Mechanically: switch on param_1 (0..0x16, jump table, default returns 0).
     7: when DAT_007d4120 == 0 and 0 <= in < DAT_007d4128, stores in to DAT_007d4100, copies the 20-byte record
        DAT_007d4130[in] to DAT_00911f84 (16 B) / DAT_00911f98, sets DAT_00911f94 from the format (0x15/0x16/0x23
        -> 0x20, 0x17/0x18/0x19 -> 0x10, else 0), then the vtable +0x2c loop (2..16, NULL out) into DAT_006181b4,
        DAT_006181b4..c0 = 1 first, then one +0x2c call with type 1 and &level, DAT_006181bc = level + 1, raised
        into DAT_006181b4 when larger. 5: FUN_004c90a0 when DAT_007d4130 == 0, *out = DAT_007d4128.
     6: same guard, out[0..5] = width, height, depth (table above), +0x10, +8, 0x500/0x600/0x200/0x100/0xa00/0.
     0xa: *out = DAT_007d4100. 8: DAT_007d4110 != 0 -> ShowWindow(DAT_007d4104, in ? 9 : 0) unless DAT_00912100.
     4: 56-byte copy of DAT_00618220 to out, DAT_00911d64 = inOut.
     0: DAT_007d4104 = *(HWND *)inOut, DAT_007d4108 = FUN_004dd140(0x1f) (thunk), RwGlobals +0xcc copy of
        DAT_00618298 to DAT_00911d80, +0x10 count, adapter loop on +0x38 (DAT_00911fa0) with DAT_007d410c /
        DAT_006181b0 = 1, Release (+8) and NULL on no adapter, else +0x18 over the 3 formats at DAT_005d8b80,
        +0x20 into DAT_00911f84, depth, copy to DAT_007d40b0, FUN_004c8740 and four FUN_004dcxxx results.
     1: RwGlobals +0x10c free of DAT_007d4130, Release of DAT_007d4108. 0xd/0xc: *out = vtable +0x10 of
        DAT_007d4108 / DAT_007d4110. 0xe: +0x14 identifier into a 0x44c stack record, +0xcc copy of its +0x200.
     0xf: *out = DAT_007d410c. 0x10: case 0 middle part for adapter `in`, FUN_004c90a0, the case 7 level search.
     2: +0x20 mode, 14-dword clear of DAT_009120e0, FUN_004c8800, +0x38, +0x14 (vendor 0x1002 device 0x5157 ->
        DAT_00912074 &= ~1), behaviour flags (0/4 | 0x20 / 0x40 / 0x50), CreateDevice (+0x40) into DAT_007d4110,
        result stored in DAT_007d4120 and returned. 3/0xb: FUN_004c8c70 / FUN_004c8e50 (out, inOut, in).
     0x11: FUN_005cb404, MXCSR |= 0x8000 when DAT_007d458c, FUN_004c40b0(0x5cb2a0), FUN_004c35f0 with one of
        three 4-pointer sets. 0x13: *out = max(DAT_00911ff8, DAT_00911ffc) unsigned. 0x16: *(u16 *)out = 2.
   What moved it: the depth switch into a temp (63.6 -> 72.8), locals scoped per case (frame 0x474 -> 0x470,
   78.2), case 0 as a __forceinline helper (ebx save leaves the prologue, 88.3: the original saves ebx only in
   case 0), case 2 as a helper whose result is stored to DAT_007d4120 by the case (90.6), case 7 as a helper
   (92.0), case 8 helper, the windowed flag computed after the four DAT_006181b4..c0 stores (92.6), `in > -1`
   in case 6 (93.2, a size effect only).
   Residual (most of the gap is jump displacements to the default exit, which move with every size difference):
   (1) case 0: the NULL return of Create gets no epilogue copy of its own (ours cross-jumps into the Release path
   tail, so the DAT_007d4108 = 0 store is not scheduled between the pops); the found test after the adapter loop
   is `cmp [DAT_007d410c], esi` (original loads it into eax before the jge). Tried: early return / nested /
   goto / result variable / do-while / hr local / null check in the case / Release helper, permuter 710
   compiles on the helper. (2) case 2: DevCaps not loaded before the DAT_007d4138 branch and the two stores in
   the software arm swap; a DevCaps local hoists it but moves the flags into ecx (84%). Tried arm orders,
   ternaries, nesting, permuter 779 compiles. (3) case 0x11: `or ah, 0x80` where the original has
   `or eax, 0x8000` (every C form tried gives the byte op in 9176). (4) case 5 / 6: register choice
   (original keeps eax free in case 5 and loads out after the first record field in case 6). Main-body
   permuter 746 compiles: only gain was a read through an uninitialised pointer, rejected. */
/* RWC-DRIVER-2 (2026-11-11): 93.2% -> 97.0% (ops 100%, 821/821 ins) under TOOLCHAIN rw9044 (L-131; 93.1% there
   before these edits). Moved by: case 7 written in the switch (`if (...) { ...; return 1; } break;`) instead of the
   D3D9UseMode helper, whose return value VC6 kept in a stack temp with an extra epilogue; case 0 returns early on
   a NULL Direct3DCreate9 result (VC6 gives it its own epilogue, as the original at 0x004c7c95); `in >= 0` in case 6
   (the DDK-only `in > -1` is gone); the case 0x11 MXCSR update as a 5-instruction inline __asm block (stmxcsr / mov
   eax / or eax, 0x8000 / mov / ldmxcsr), since every intrinsic form gives `or ah, 0x80` under both compilers. The body
   stays compiled C (CONFIDENCE.md C4 gate 3 allows inline __asm inside real C work).
   Residual: only jump displacements. Case 7's two multisample-failure branches (jl / jbe after the vtable +0x2c call
   with type 1) go to case 16's copy of the `mov eax, edi` return tail (near jumps, 8 bytes more) where the original
   sends them to case 7's own copy at 0x004c7bcd and sends case 16's branches there too. Every later target is 8 bytes
   off. Tried: case 16 before / after case 7 or last (80-82%), case 16 as an int helper, MultisampleLevels expanded in
   either case or returning int, a goto from case 16 into case 7 (85.0%, drops case 16's copy), plain static helpers
   under /Ob2, permuter 319 compiles. 0x004db3e0 shows the same choice (original: the earlier copy of a duplicated
   return tail; ours: the later). */
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
typedef struct ComObj { void **lpVtbl; } ComObj;
typedef unsigned int (__stdcall *VFn0)(ComObj *self);
typedef long (__stdcall *VFn1)(ComObj *self, void *a);
typedef long (__stdcall *VFn2)(ComObj *self, unsigned int a, void *b);
typedef long (__stdcall *VFn3)(ComObj *self, unsigned int a, int b, void *c);
typedef long (__stdcall *VFnMS)(ComObj *self, unsigned int adapter, int devType, int format, int windowed,
                                unsigned int msType, unsigned long *quality);
typedef long (__stdcall *VFnCD)(ComObj *self, unsigned int adapter, int devType, void *hwnd, unsigned long flags,
                                void *pp, ComObj **device);
#define VCALL(T, o, i) ((T)(o)->lpVtbl[(i)])
typedef struct D3DDISPLAYMODE { unsigned int Width; unsigned int Height; unsigned int RefreshRate; int Format; } D3DDISPLAYMODE;
typedef struct ModeRec { D3DDISPLAYMODE mode; int flags; } ModeRec;
typedef struct AdapterIdent {
  char Driver[512]; char Description[512]; char DeviceName[32]; unsigned int DriverVersion[2];
  unsigned int VendorId; unsigned int DeviceId; unsigned int SubSysId; unsigned int Revision;
  unsigned int DeviceIdentifier[4]; unsigned int WHQLLevel;
} AdapterIdent;
typedef struct RECT { long left; long top; long right; long bottom; } RECT;
typedef struct Block56 { int v[14]; } Block56;
typedef struct RwGlobalsFns { char pad[0xcc]; void (*strcpyFn)(char *, const char *); char pad2[0x10c - 0xd0];
                              void (*freeFn)(void *); } RwGlobalsFns;
MGLOBAL(RwGlobalsFns *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d4100, 0x007d4100);
MGLOBAL(void *, DAT_007d4104, 0x007d4104);
MGLOBAL(ComObj *, DAT_007d4108, 0x007d4108);
MGLOBAL(unsigned int, DAT_007d410c, 0x007d410c);
MGLOBAL(ComObj *, DAT_007d4110, 0x007d4110);
MGLOBAL(ComObj *, DAT_007d4114, 0x007d4114);
MGLOBAL(ComObj *, DAT_007d4118, 0x007d4118);
MGLOBAL(int, DAT_007d4120, 0x007d4120);
MGLOBAL(int, DAT_007d4128, 0x007d4128);
MGLOBAL(ModeRec *, DAT_007d4130, 0x007d4130);
MGLOBAL(int, DAT_007d4134, 0x007d4134);
MGLOBAL(int, DAT_007d4138, 0x007d4138);
MGLOBAL(int, DAT_007d413c, 0x007d413c);
MGLOBAL(D3DDISPLAYMODE, DAT_007d40b0, 0x007d40b0);
MGLOBAL(int, DAT_007d456c, 0x007d456c);
MGLOBAL(void *, DAT_007d4570, 0x007d4570);
MGLOBAL(int, DAT_007d4574, 0x007d4574);
MGLOBAL(int, DAT_007d4578, 0x007d4578);
MGLOBAL(int, DAT_007d457c, 0x007d457c);
MGLOBAL(int, DAT_007d4580, 0x007d4580);
MGLOBAL(int, DAT_007d4584, 0x007d4584);
MGLOBAL(int, DAT_007d4588, 0x007d4588);
MGLOBAL(int, DAT_007d458c, 0x007d458c);
MGLOBAL(int, DAT_007d4590, 0x007d4590);
MGLOBAL(int, DAT_007d4594, 0x007d4594);
MGLOBAL(void *, DAT_00911d64, 0x00911d64);
MGLOBALA(char, DAT_00911d80, 32, 0x00911d80);
MGLOBALA(char, DAT_00618298, 32, 0x00618298);
MGLOBAL(int, DAT_00911f80, 0x00911f80);
MGLOBAL(D3DDISPLAYMODE, DAT_00911f84, 0x00911f84);
MGLOBAL(int, DAT_00911f94, 0x00911f94);
MGLOBAL(int, DAT_00911f98, 0x00911f98);
MGLOBALA(int, DAT_00911fa0, 0x4c, 0x00911fa0);
MGLOBAL(unsigned int, DAT_00911fbc, 0x00911fbc);
MGLOBAL(unsigned int, DAT_00911ff8, 0x00911ff8);
MGLOBAL(unsigned int, DAT_00911ffc, 0x00911ffc);
MGLOBAL(unsigned int, DAT_00912074, 0x00912074);
MGLOBAL(int, DAT_0091208c, 0x0091208c);
MGLOBALA(int, DAT_009120e0, 14, 0x009120e0);
MGLOBAL(void *, DAT_009120fc, 0x009120fc);
MGLOBAL(int, DAT_00912100, 0x00912100);
MGLOBAL(int, DAT_0091210c, 0x0091210c);
MGLOBAL(int, DAT_006181b0, 0x006181b0);
MGLOBAL(unsigned int, DAT_006181b4, 0x006181b4);
MGLOBAL(unsigned int, DAT_006181b8, 0x006181b8);
MGLOBAL(unsigned int, DAT_006181bc, 0x006181bc);
MGLOBAL(unsigned int, DAT_006181c0, 0x006181c0);
MGLOBAL(Block56, DAT_00618220, 0x00618220);
MGLOBALA(int, DAT_005d8b80, 3, 0x005d8b80);
void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
__declspec(dllimport) int __stdcall ShowWindow(void *hWnd, int nCmdShow);
__declspec(dllimport) int __stdcall GetClientRect(void *hWnd, RECT *lpRect);
unsigned int __cdecl _mm_getcsr(void);
void __cdecl _mm_setcsr(unsigned int);
ComObj *__stdcall FUN_004dd140(unsigned int sdkVersion);
void FUN_004c8740(void);
int FUN_004dcf90(void);
int FUN_004dcff0(void);
int FUN_004dd050(void);
int FUN_004dd0b0(void);
void FUN_004c90a0(void);
void FUN_004c8800(D3DDISPLAYMODE *mode, int flags, int format);
void FUN_004c8650(void);
void FUN_004c8690(void);
void FUN_004cfe20(void);
void FUN_004db3e0(void);
void FUN_004d5bc0(void);
void FUN_004dcd50(void);
int FUN_004c8c70(void *out, void *inOut, int in);
int FUN_004c8e50(void *out, void *inOut, int in);
void FUN_005cb404(void);
void FUN_004c40b0(void *fn);
void FUN_004c35f0(void *a, void *b, void *c, void *d);
void FUN_005cb2a0(void);
void FUN_005cb0ef(void); void FUN_005cb000(void); void FUN_005cb160(void); void FUN_005cb07f(void);
void FUN_004dc4a0(void); void FUN_004dc370(void); void FUN_004dc530(void); void FUN_004dc410(void);
void FUN_004dc280(void); void FUN_004dc160(void); void FUN_004dc300(void); void FUN_004dc1f0(void);

#define FORMAT_DEPTH(_dst, _fmt) { \
  int _depth; \
  switch (_fmt) { \
  case 0x15: case 0x16: case 0x23: _depth = 0x20; break; \
  case 0x17: case 0x18: case 0x19: _depth = 0x10; break; \
  default: _depth = 0; break; \
  } \
  _dst = _depth; }

static __forceinline void MultisampleLevels(void)
{
    int windowed;
    unsigned int level;
    DAT_006181b4 = 1;
    DAT_006181b8 = 1;
    DAT_006181bc = 1;
    DAT_006181c0 = 1;
    windowed = ~DAT_00911f98 & 1;
    for (level = 2; level <= 16; level++) {
      if (VCALL(VFnMS, DAT_007d4108, 11)(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911f84.Format,
                                         windowed, level, 0) >= 0)
        DAT_006181b4 = level;
    }
    if (VCALL(VFnMS, DAT_007d4108, 11)(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911f84.Format,
                                       windowed, 1, (unsigned long *)&level) >= 0) {
      DAT_006181bc = level + 1;
      if (DAT_006181bc > DAT_006181b4)
        DAT_006181b4 = DAT_006181bc;
    }
}

static __forceinline int D3D9Focus(int in)
{
    if (DAT_007d4110 != 0) {
      if (DAT_00912100 == 0) {
        if (in != 0)
          ShowWindow(DAT_007d4104, 9);
        else
          ShowWindow(DAT_007d4104, 0);
      }
      return 1;
    }
    return 0;
}

static __forceinline int D3D9Open(void *inOut)
{
    unsigned int numAdapters;
    int i;
    DAT_007d4104 = *(void **)inOut;
    DAT_007d4108 = FUN_004dd140(0x1f);
    if (DAT_007d4108 == 0)
      return 0;
    {
    DAT_007d3ff8->strcpyFn(DAT_00911d80, DAT_00618298);
    numAdapters = VCALL(VFn0, DAT_007d4108, 4)(DAT_007d4108);
    DAT_007d410c = 0;
    DAT_006181b0 = 1;
    while (DAT_007d410c < numAdapters) {
      if (VCALL(VFn3, DAT_007d4108, 14)(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911fa0) >= 0)
        break;
      DAT_007d410c++;
    }
    if (DAT_007d410c >= numAdapters) {
      VCALL(VFn0, DAT_007d4108, 2)(DAT_007d4108);
      DAT_007d4108 = 0;
      return 0;
    }
    DAT_00911f80 = 0;
    for (i = 0; i < 3; i++)
      DAT_00911f80 += VCALL(VFn2, DAT_007d4108, 6)(DAT_007d4108, DAT_007d410c, (void *)DAT_005d8b80[i]);
    VCALL(VFn2, DAT_007d4108, 8)(DAT_007d4108, DAT_007d410c, &DAT_00911f84);
    FORMAT_DEPTH(DAT_00911f94, DAT_00911f84.Format)
    DAT_00911f98 = 0;
    DAT_007d40b0 = DAT_00911f84;
    DAT_007d4100 = 0;
    FUN_004c8740();
    DAT_007d4588 = FUN_004dcf90();
    DAT_007d458c = FUN_004dcff0();
    DAT_007d4590 = FUN_004dd050();
    DAT_007d4594 = FUN_004dd0b0();
    return 1;
    }
}

static __forceinline int D3D9Start(void)
{
    unsigned long flags;
    RECT rect;
    D3DDISPLAYMODE mode;
    AdapterIdent ident;
    VCALL(VFn2, DAT_007d4108, 8)(DAT_007d4108, DAT_007d410c, &mode);
    memset(DAT_009120e0, 0, sizeof(DAT_009120e0));
    FUN_004c8800(&DAT_00911f84, DAT_00911f98, mode.Format);
    DAT_009120fc = DAT_007d4104;
    DAT_0091210c = DAT_007d413c != 0;
    VCALL(VFn3, DAT_007d4108, 14)(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_00911fa0);
    VCALL(VFn3, DAT_007d4108, 5)(DAT_007d4108, DAT_007d410c, 0, &ident);
    if (ident.VendorId == 0x1002 && ident.DeviceId == 0x5157)
      DAT_00912074 &= ~1u;
    flags = 0;
    if (DAT_007d4134 != 0)
      flags = 4;
    if (DAT_007d4138 != 0) {
      flags |= 0x20;
      DAT_00911fbc &= ~0x10000u;
      DAT_0091208c = 0;
    } else if (DAT_00911fbc & 0x10000) {
      if (DAT_00911fbc & 0x100000)
        flags |= 0x50;
      else
        flags |= 0x40;
    } else {
      flags |= 0x20;
    }
    if (VCALL(VFnCD, DAT_007d4108, 16)(DAT_007d4108, DAT_007d410c, DAT_006181b0, DAT_007d4104, flags,
                                       DAT_009120e0, &DAT_007d4110) < 0) {
      return 0;
    }
    if (DAT_007d413c != 0)
      VCALL(VFn1, DAT_007d4110, 20)(DAT_007d4110, (void *)1);
    if (DAT_007d4104 != 0) {
      GetClientRect(DAT_007d4104, &rect);
      DAT_00911f84.Width = rect.right;
      DAT_00911f84.Height = rect.bottom;
    }
    VCALL(VFn2, DAT_007d4110, 38)(DAT_007d4110, 0, &DAT_007d4118);
    VCALL(VFn0, DAT_007d4118, 2)(DAT_007d4118);
    VCALL(VFn1, DAT_007d4110, 40)(DAT_007d4110, &DAT_007d4114);
    VCALL(VFn0, DAT_007d4114, 2)(DAT_007d4114);
    DAT_007d4574 = 0;
    memset(&DAT_007d4578, 0, 16);
    FUN_004c8650();
    FUN_004c8690();
    DAT_007d456c = 0;
    if (DAT_007d4570 != 0) {
      DAT_007d3ff8->freeFn(DAT_007d4570);
      DAT_007d4570 = 0;
    }
    FUN_004cfe20();
    FUN_004db3e0();
    FUN_004d5bc0();
    FUN_004dcd50();
    return 1;
  }

// MATCH: 0x004c7a70 _rwDeviceSystemFn
int rwDeviceSystemFn(int request, void *out, void *inOut, int in)
{
  switch (request) {
  case 7:
    if (DAT_007d4120 == 0 && in >= 0 && in < DAT_007d4128) {
      ModeRec *rec;
      DAT_007d4100 = in;
      rec = &DAT_007d4130[in];
      DAT_00911f84 = rec->mode;
      DAT_00911f98 = rec->flags;
      FORMAT_DEPTH(DAT_00911f94, rec->mode.Format)
      MultisampleLevels();
      return 1;
    }
    break;
  case 5:
    if (DAT_007d4130 == 0)
      FUN_004c90a0();
    *(int *)out = DAT_007d4128;
    return 1;
  case 6:
    if (DAT_007d4130 == 0)
      FUN_004c90a0();
    if (in >= 0 && in < DAT_007d4128) {
      int *vm = (int *)out;
      vm[0] = DAT_007d4130[in].mode.Width;
      vm[1] = DAT_007d4130[in].mode.Height;
      FORMAT_DEPTH(vm[2], DAT_007d4130[in].mode.Format)
      vm[3] = DAT_007d4130[in].flags;
      vm[4] = DAT_007d4130[in].mode.RefreshRate;
      {
        int fmt;
        switch (DAT_007d4130[in].mode.Format) {
        case 0x15: case 0x23: fmt = 0x500; break;
        case 0x16: fmt = 0x600; break;
        case 0x17: fmt = 0x200; break;
        case 0x19: fmt = 0x100; break;
        case 0x18: fmt = 0xa00; break;
        default: fmt = 0; break;
        }
        vm[5] = fmt;
      }
      return 1;
    }
    break;
  case 10:
    *(int *)out = DAT_007d4100;
    return 1;
  case 8:
    return D3D9Focus(in);
  case 4:
    *(Block56 *)out = DAT_00618220;
    DAT_00911d64 = inOut;
    return 1;
  case 0:
    return D3D9Open(inOut);
  case 1:
    if (DAT_007d4130 != 0) {
      DAT_007d3ff8->freeFn(DAT_007d4130);
      DAT_007d4130 = 0;
      DAT_007d4128 = 0;
    }
    if (DAT_007d4108 != 0) {
      VCALL(VFn0, DAT_007d4108, 2)(DAT_007d4108);
      DAT_007d4108 = 0;
    }
    return 1;
  case 13:
    *(unsigned int *)out = VCALL(VFn0, DAT_007d4108, 4)(DAT_007d4108);
    return 1;
  case 14: {
    AdapterIdent ident;
    VCALL(VFn3, DAT_007d4108, 5)(DAT_007d4108, in, 0, &ident);
    DAT_007d3ff8->strcpyFn((char *)out, ident.Description);
    return 1;
  }
  case 15:
    *(unsigned int *)out = DAT_007d410c;
    return 1;
  case 16: {
    int i;
    DAT_007d410c = in;
    DAT_00911f80 = 0;
    for (i = 0; i < 3; i++)
      DAT_00911f80 += VCALL(VFn2, DAT_007d4108, 6)(DAT_007d4108, DAT_007d410c, (void *)DAT_005d8b80[i]);
    VCALL(VFn2, DAT_007d4108, 8)(DAT_007d4108, DAT_007d410c, &DAT_00911f84);
    FUN_004c90a0();
    MultisampleLevels();
    return 1;
  }
  case 2:
    DAT_007d4120 = D3D9Start();
    return DAT_007d4120;
  case 3:
    return FUN_004c8c70(out, inOut, in);
  case 11:
    return FUN_004c8e50(out, inOut, in);
  case 12:
    *(unsigned int *)out = VCALL(VFn0, DAT_007d4110, 4)(DAT_007d4110);
    return 1;
  case 17:
    FUN_005cb404();
    if (DAT_007d458c != 0) {
      unsigned int csr;
      __asm {
        stmxcsr csr
        mov eax, csr
        or eax, 0x8000
        mov csr, eax
        ldmxcsr csr
      }
    }
    FUN_004c40b0((void *)FUN_005cb2a0);
    if (DAT_007d458c != 0) {
      if (DAT_007d4590 != 0 && DAT_007d4594 == 0)
        FUN_004c35f0((void *)FUN_004dc4a0, (void *)FUN_004dc370, (void *)FUN_004dc530, (void *)FUN_004dc410);
      else
        FUN_004c35f0((void *)FUN_004dc280, (void *)FUN_004dc160, (void *)FUN_004dc300, (void *)FUN_004dc1f0);
    } else {
      FUN_004c35f0((void *)FUN_005cb0ef, (void *)FUN_005cb000, (void *)FUN_005cb160, (void *)FUN_005cb07f);
    }
    return 1;
  case 19:
    if (DAT_00911ff8 >= DAT_00911ffc)
      *(unsigned int *)out = DAT_00911ff8;
    else
      *(unsigned int *)out = DAT_00911ffc;
    return 1;
  case 22:
    *(unsigned short *)out = 2;
    return 1;
  }
  return 0;
}

// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005ac210: rewritten from the listing (RO-AUDIOINIT-45D460-REACH, 2026-11-21); earlier note in 005ac210_before.c of the row's data folder: permuter 57.1%, not port-ready. */
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
/* RO-AUDIOINIT-45D460-REACH (2026-11-21): body rewritten from the listing of MASHED.exe.unpatched 0x005ac210..0x005ac533. The
   Ghidra form computed the allocation base from param_3 (the original takes [desc+0x34] + 0x5c, 0x005ac442 / 0x005ac44d), used
   one `code` type for three RwGlobals slot arities, and declared the callbacks 0x005ac770 / 0x005aaaa0 as data (the port would
   have passed original code addresses to 0x005aa0c0 / 0x005aaac0, which call them). Stack: locals at [esp+0x10] (s = str,
   v = vec, k = key record {k0, k1}, sel, cb = {cb0, cb1}, f[7] at [esp+0x2c], fmt[28] at [esp+0x48]); param_1's slot holds
   the flags word (0x005ac22e); param_3's slot is rewritten at 0x005ac43e. */
typedef int (*RwStrLen_005ac210)(const void *);
typedef void (*RwStrCpy_005ac210)(void *, const void *);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(uint, DAT_00914700, 0x00914700);
MGLOBAL(undefined1, DAT_005e6194, 0x005e6194);
MGLOBAL(undefined1, DAT_005e6e60, 0x005e6e60);
int FUN_005aaaa0(int p0, int p1);
int FUN_005ac770(int p0, int p1);
int FUN_005aa060(int p0, void *p1, int p2);
int FUN_005aa0c0(int p0, int p1, int (*p2)(int, int), int *p3, int p4);
void FUN_005aaac0(int (*p0)(int, int), int *p1);
int FUN_005aba20(int p0, int p1, void *p2, int *p3, uint p4, int p5);
int FUN_005ac7b0(int p0, uint *p1, uint *p2);
int FUN_005ac900(void *p0);
void FUN_005ac980(int p0, int *p1, int p2);
int FUN_005aca80(void *p0);
void FUN_005acaa0(void *p0, void *p1, int p2, uint p3);
int FUN_005acd10(int p0, int *p1);
int FUN_005adf30(int p0, void *p1);
int FUN_005ae0c0(void *p0, void *p1, uint p2);
int FUN_005aea00(uint p0, int p1);
void FUN_005aec00(void *p0, int p1);
// MATCH: 0x005ac210 FUN_005ac210
int FUN_005ac210(uint *param_1,int param_2,int param_3,int *param_4,uint param_5)

{
  uint *s;
  uint *v;
  int k[2];
  uint sel;
  int cb[2];
  int f[7];
  uint fmt[7];
  uint sw;
  char *p;
  int desc;
  int key;
  int base;
  uint size;
  int mem;
  int r;

  sw = param_5 >> 4 & 1;                                       /* 0x005ac21f .. 0x005ac22b */
  p = (char *)(param_1 + 1);
  param_1 = (uint *)*param_1;                                  /* 0x005ac22e: flags word in param_1's slot */
  v = (uint *)0x0;
  s = (uint *)0x0;
  k[0] = 0;
  if (sw != 0) {
    FUN_005aec00(&param_1,4);
  }
  FUN_005acaa0(fmt,p,0,sw);                                    /* 0x005ac257 */
  p = p + FUN_005aca80(fmt);
  FUN_005acaa0(f,p,0,sw);                                      /* 0x005ac270 */
  p = p + FUN_005aca80(f);
  if ((param_5 & 1) != 0) {
    sel = param_5 >> 1 & 1;
  }
  else {
    sel = *(uint *)p;
    if (sw != 0) {
      FUN_005aec00(p,4);
    }
  }
  p = p + 4;
  if (((uint)param_1 & 1) != 0) {
    if ((param_5 & 8) == 0) {
      v = (uint *)p;
      FUN_005ae0c0(p,p,sw);
    }
    p = p + 0x10;
  }
  if (((uint)param_1 & 2) != 0) {
    if ((param_5 & 4) == 0) {
      s = (uint *)p;
    }
    p = p + ((uint)((*(RwStrLen_005ac210 *)(DAT_007d3ff8 + 0xf4))(p) + DAT_00914700) & ~(DAT_00914700 - 1));   /* 0x005ac2e4 .. 0x005ac2fb */
  }
  desc = param_2;
  if (((uint)param_1 & 4) != 0) {
    if (desc == 0) {
      if (param_3 != 0) {
        cb[0] = param_3;
        cb[1] = 0;
        FUN_005aa0c0(0,0,FUN_005ac770,cb,1);                   /* 0x005ac333 */
        desc = cb[1];
        if (desc == 0) {
          return 0;
        }
      }
      else {
        FUN_005ae0c0(p,p,sw);
        desc = FUN_005ac900(p);
        if (desc == 0) {
          return 0;
        }
        if (FUN_005adf30(*(int *)desc,&DAT_005e6194) == 0) {
          desc = FUN_005ac900(&DAT_005e6e60);
          if (FUN_005acd10(desc,f) == 0) {
            FUN_005ac980(*(int *)(desc + 0x28) == 0 ? 0 :
                         *(int *)(*(int *)(desc + 0x28) + *(int *)(desc + 0x2c) * 4),f,0);   /* 0x005ac397 .. 0x005ac3b0 */
          }
        }
      }
    }
    p = p + 0x10;
  }
  if (param_4 == (int *)0x0) {
    param_4 = f;
  }
  if ((*param_4 == 0) || ((param_5 & 0x20) != 0)) {
    *param_4 = f[0];
  }
  if (((uint)param_1 & 8) != 0) {
    if (param_3 != 0) goto alloc;
    if (desc == 0) {
      FUN_005ae0c0(p,p,sw);
      key = FUN_005aa060(0,p,0);                                /* 0x005ac3f7 */
      if (key == 0) {
        return 0;
      }
    }
    else {
      k[0] = *(int *)(desc + 0x30);
      key = k[0];
    }
  }
  else {
    if (param_3 != 0) goto alloc;
    key = k[0];
  }
  k[1] = key;
  k[0] = 0;
  FUN_005aaac0(FUN_005aaaa0,k);                                /* 0x005ac432 */
  param_3 = k[0];
alloc:
  base = *(int *)(desc + 0x34) + 0x5c;                         /* 0x005ac442 / 0x005ac44d */
  size = (uint)(-(v != (uint *)0x0) & 0x10) + base;
  size = size + (s == (uint *)0x0 ? 0 : (*(RwStrLen_005ac210 *)(DAT_007d3ff8 + 0xf4))(s) + 1);
  mem = FUN_005aea00(size,0x30806);
  if (mem == 0) {
    return 0;
  }
  if (v != (uint *)0x0) {
    *(uint *)(base + mem) = v[0];
    *(uint *)(base + mem + 4) = v[1];
    *(uint *)(base + mem + 8) = v[2];
    *(uint *)(base + mem + 0xc) = v[3];
    v = (uint *)(base + mem);
    base = base + 0x10;
  }
  if (s != (uint *)0x0) {
    (*(RwStrCpy_005ac210 *)(DAT_007d3ff8 + 0xcc))((void *)(base + mem),s);   /* 0x005ac4ca */
    s = (uint *)(base + mem);
  }
  r = FUN_005aba20(desc,param_3,fmt,param_4,sel,mem);          /* 0x005ac4ed */
  if ((r == 0) || (size == 0)) {
    return 0;
  }
  *(uint *)(r + 0x54) = *(uint *)(r + 0x54) & 0xfffffffb;
  if (FUN_005ac7b0(r,v,s) == 0) {
    return 0;
  }
  return r;
}

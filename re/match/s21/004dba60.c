// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWT-A 2026-11-12: 100% under rw9044 (TOOLCHAIN line added, ox converted before oy). verify/match_20261112/RESULT_MATCH-RWT-A.md
   MATCH-S21 0x004dba60 (h3): best 87.6% (regs 96.6%, ops 97.8%). Raw did not compile; written from odis.py + decomp.json.
   Im2D-style draw: camera from DAT_00911d60+0x60 or FUN_004c75c0, primitive count by switch (jump table
   at 0x004dbc78, past the Ghidra size), vertex buffer lock (slot 0x2c, __stdcall, 0x2800 discard / 0x1800
   nooverwrite at 0x2492 vertices) or FUN_004dcbb0, copy or offset by the camera's short x/y at +0x1c/+0x1e,
   unlock, FUN_004db770, device slot 0x144 with the type table at 0x005d8d8c. What moved it: p != 0 block with
   'return 0' last, a local copy of param_3 (float temps reuse param_3's home), ox/oy order (permuter).
   Residual: ecx/edx swapped in the n/3 sequence, the lock call and the copy-loop setup. Class (b). */
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
typedef struct Vtx_ba60 { float x; float y; float z; float rhw; unsigned int col; float u; float v; } Vtx_ba60;
typedef struct Cam_ba60 { char pad[0x1c]; short ox; short oy; } Cam_ba60;
typedef struct Com_ba60 { void **vtbl; } Com_ba60;
typedef int (__stdcall *Lock_ba60)(Com_ba60 *, unsigned int, unsigned int, void **, unsigned int);
typedef int (__stdcall *Unlock_ba60)(Com_ba60 *);
typedef int (__stdcall *Draw_ba60)(Com_ba60 *, int, unsigned int, unsigned int);
MGLOBAL(int, DAT_00911d60, 0x00911d60);
MGLOBAL(int, DAT_007d7098, 0x007d7098);
MGLOBAL(unsigned int, DAT_007d709c, 0x007d709c);
MGLOBAL(Com_ba60 *, DAT_007d70a0, 0x007d70a0);
MGLOBAL(Com_ba60 *, DAT_007d4110, 0x007d4110);
MGLOBALA(int, DAT_005d8d8c, 6, 0x005d8d8c);
Cam_ba60 *FUN_004c75c0(void);
void FUN_004db770(void);
int FUN_004dcbb0(int stride, int count, Com_ba60 **vb, void **lock, unsigned int *base);
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
// MATCH: 0x004dba60 FUN_004dba60
int FUN_004dba60(int param_1, Vtx_ba60 *param_2, int param_3)
{
  Vtx_ba60 *p = 0;
  Cam_ba60 *cam;
  int prims;
  int i;
  int hr;
  int n = param_3;
  if (DAT_00911d60 != 0) {
    cam = *(Cam_ba60 **)(DAT_00911d60 + 0x60);
  } else {
    cam = FUN_004c75c0();
  }
  switch (param_1) {
  case 1: prims = n / 2; break;
  case 2: prims = n - 1; break;
  case 3: prims = n / 3; break;
  case 4:
  case 5: prims = n - 2; break;
  default: prims = 0; break;
  }
  if (DAT_007d7098 != 0) {
    if (DAT_007d709c + n > 0x2492) {
      DAT_007d709c = 0;
      ((Lock_ba60)DAT_007d70a0->vtbl[0x2c / 4])(DAT_007d70a0, 0, n * sizeof(Vtx_ba60), (void **)&p, 0x2800);
    } else {
      ((Lock_ba60)DAT_007d70a0->vtbl[0x2c / 4])(DAT_007d70a0, DAT_007d709c * sizeof(Vtx_ba60), n * sizeof(Vtx_ba60), (void **)&p, 0x1800);
    }
  } else {
    FUN_004dcbb0(sizeof(Vtx_ba60), n, &DAT_007d70a0, (void **)&p, &DAT_007d709c);
  }
  if (p != 0) {
  if (cam->ox == 0 && cam->oy == 0) {
    memcpy(p, param_2, n * sizeof(Vtx_ba60));
  } else {
    float ox, oy;
    ox = (float)cam->ox;
    oy = (float)cam->oy;
    for (i = 0; i < n; i++) {
      p[i].x = ox + param_2[i].x;
      p[i].y = oy + param_2[i].y;
      p[i].z = param_2[i].z;
      p[i].rhw = param_2[i].rhw;
      p[i].col = param_2[i].col;
      p[i].u = param_2[i].u;
      p[i].v = param_2[i].v;
    }
  }
  ((Unlock_ba60)DAT_007d70a0->vtbl[0x30 / 4])(DAT_007d70a0);
  FUN_004db770();
  hr = ((Draw_ba60)DAT_007d4110->vtbl[0x144 / 4])(DAT_007d4110, DAT_005d8d8c[param_1], DAT_007d709c, prims);
  DAT_007d709c += param_3;
  return hr >= 0;
  }
  return 0;
}

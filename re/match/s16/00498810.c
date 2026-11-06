// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 h5 0x00498810 (69 B): best 86.7% (regs 100, ops 100). Static callee: Camera* in EAX (pilot finding 1),
   defined before its real caller FUN_00498860 (also in this file, 100%). Residual (b): the two loads pick
   edx/eax where the original picks eax/ecx (raster[0xc] and the name arg). Tried: nested-if vs && form,
   void* image, struct vs int* raster, (name, cam) order, extra call site, int-returning callees, permuter
   96 compiles: no move. */
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
typedef struct Timeb64 { __int64 time; unsigned short millitm; short timezone; short dstflag; } Timeb64;
typedef struct Raster { int r0, r4, r8, w, h; } Raster;
typedef struct Camera { char pad[0x60]; Raster *raster; } Camera;
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_005d0118, 8, 0x005d0118);
MGLOBALA(char, s_fmt_005d0120, 10, 0x005d0120);
int FUN_004cdca0(int w, int h, int d);
void FUN_004cdd00(int img);
void FUN_004cdd60(int img);
void FUN_004ce8e0(int img, char *name);
void FUN_004d52d0(int img, Raster *raster);
void FUN_004a43d2(Timeb64 *tb);
char *FUN_004a43b9(__int64 *t);
int FUN_004a2b60(char *out, const char *fmt, ...);
typedef void (*FsFn)(char *name, char *ext);

// MATCH: 0x00498810 FUN_00498810
static void FUN_00498810(Camera *cam, char *name)
{
  Raster *raster = cam->raster;
  int img;
  if (raster != 0) {
    img = FUN_004cdca0(raster->w, raster->h, 0x20);
    if (img != 0) {
      FUN_004cdd60(img);
      FUN_004d52d0(img, raster);
      FUN_004ce8e0(img, name);
      FUN_004cdd00(img);
    }
  }
}

// MATCH: 0x00498860 FUN_00498860
void FUN_00498860(Camera *cam)
{
  Timeb64 tb;
  char buf[256];
  char *p;
  char *s;
  FUN_004a43d2(&tb);
  s = FUN_004a43b9(&tb.time);
  FUN_004a2b60(buf, s_fmt_005d0120, s, tb.millitm);
  for (p = buf; *p; p++) {
    if (*p == ' ' || *p == ':' || *p == '.')
      *p = '_';
  }
  (*(FsFn *)(DAT_007d3ff8 + 0xd4))(buf, DAT_005d0118);
  FUN_00498810(cam, buf);
}

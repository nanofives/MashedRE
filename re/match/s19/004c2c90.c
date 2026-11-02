// FLAGS: /O2 /GS /Ob1
/* MATCH-S19 0x004c2c90 (h2): 93.2%, regs 93.2%. Hand: device fpSystem call, switch 0xd..0x12 (jump table matches), shared error tail. Residual (b): two arg loads before sub esp, the *out=1 store, and the error-code store/lea pair scheduled differently; permuter 97 compiles no gain. */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, s_Only_rendering_sub_system_006180f8, 1, 0x006180f8);
typedef struct { int type; int code; } RwError_t;
typedef int (*sysfn_t)(int, int *, int, int);
typedef struct { int pad; sysfn_t fpSystem; } RwDevice_t;
typedef void (*msgfn_t)(int *, char *);
int FUN_004d7ff0(int p0, ...);
RwError_t *FUN_004d8480(RwError_t *p0);
// MATCH: 0x004c2c90 FUN_004c2c90
int FUN_004c2c90(RwDevice_t *dev, int req, int *out, int a4, int a5)
{
  int r;
  RwError_t err;
  r = dev->fpSystem(req, out, a4, a5);
  if (r) {
    return r;
  }
  switch (req) {
  case 0xd:
    *out = 1;
    return 1;
  case 0xe:
    r = (a5 == 0);
    if (r) {
      (*(msgfn_t *)(DAT_007d3ff8 + 0xcc))(out, s_Only_rendering_sub_system_006180f8);
    }
    break;
  case 0xf:
    *out = 0;
    return 1;
  case 0x10:
    r = (a5 == 0);
    break;
  case 0x11:
  case 0x12:
    return 1;
  }
  if (!r) {
    err.type = 1;
    err.code = FUN_004d7ff0(0x18, req);
    FUN_004d8480(&err);
  }
  return r;
}

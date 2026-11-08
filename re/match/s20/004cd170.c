// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cd170 (h3): 100% (regs 100%); the jump table at 0x004cd2b0 sits past Ghidra's size. Needs module+0x3c (else RWERROR 0x23); fills the request at module+0x44 (handle +0x20 = 0, type, data, size in declaration order), switch on type picks the handle from module+4/8/0xc/0x10/0x14 (cases written in the binary's body order 3,5,4,1,2) with size rounded down to a multiple of 3 / 2; default RWERROR(0x25, type) via variadic FUN_004d7ff0; returns FUN_004d40d0(handle, req, 0) != 0. From 31.5%: structs, success branch first, case order, store order (permuter move). Relocheck OK. */
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
typedef struct RwError { int pluginID; int errorCode; } RwError;
typedef struct Req2 { int pad[8]; void *handle; int type; void *data; int size; } Req2;
typedef struct Mod {
  void *handle;
  void *h4;
  void *h8;
  void *hc;
  void *h10;
  void *h14;
  int pad[9];
  void *reqptr;
  int pad2[1];
  Req2 req;
} Mod;
MGLOBAL(int, DAT_00911d00, 0x00911d00);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_004d40d0(void *p0, Req2 *p1, int p2);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_00911d00))

// MATCH: 0x004cd170 FUN_004cd170
int FUN_004cd170(int param_1, void *param_2, int param_3)
{
  RwError err;
  Req2 *req;
  if (MOD->reqptr) {
    req = &MOD->req;
    req->handle = 0;
    req->type = param_1;
    req->data = param_2;
    req->size = param_3;
    switch (param_1) {
    case 3:
      req->handle = MOD->h4;
      req->size = param_3 - param_3 % 3;
      break;
    case 5:
      req->handle = MOD->h8;
      break;
    case 4:
      req->handle = MOD->hc;
      break;
    case 1:
      req->handle = MOD->h10;
      req->size = param_3 - param_3 % 2;
      break;
    case 2:
      req->handle = MOD->h14;
      break;
    default:
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(0x25, param_1);
      FUN_004d8480(&err);
      break;
    }
    if (FUN_004d40d0(req->handle, req, 0)) {
      return 1;
    }
  } else {
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x23);
    FUN_004d8480(&err);
  }
  return 0;
}

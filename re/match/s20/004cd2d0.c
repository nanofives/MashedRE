// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cd2d0 (h3): 100% (regs 100%); jump table at 0x004cd410 past Ghidra's size. FLAGS /G5 (the raw converter's /GS pick was a converter artifact; no cookie). ok = module+0x3c != 0 taken before FUN_004d40c0() (result unused here, the callee loads 0x007d4710), then the 004cd170 request shape: handle 0, type, data 0, size = (unsigned short) module+0x38; same case order 3,5,4,1,2; RWERROR 0x25 / 0x23. Relocheck OK. */
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
  int pad[8];
  unsigned short size;
  short pad1;
  void *reqptr;
  int pad2[1];
  Req2 req;
} Mod;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00911d00, 0x00911d00);
void *FUN_004d40c0(void);
int FUN_004d40d0(void *p0, Req2 *p1, int p2);
int FUN_004d7ff0(int code, ...);
RwError *FUN_004d8480(RwError *err);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_00911d00))

// MATCH: 0x004cd2d0 FUN_004cd2d0
int FUN_004cd2d0(int param_1)
{
  RwError err;
  Req2 *req;
  int ok;
  ok = MOD->reqptr != 0;
  FUN_004d40c0();
  if (ok) {
    req = &MOD->req;
    req->handle = 0;
    req->type = param_1;
    req->data = 0;
    req->size = MOD->size;
    switch (param_1) {
    case 3:
      req->handle = MOD->h4;
      break;
    case 5:
      req->handle = MOD->h8;
      break;
    case 4:
      req->handle = MOD->hc;
      break;
    case 1:
      req->handle = MOD->h10;
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

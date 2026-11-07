// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cd430 (h3): 100% (regs 100%). heap = FUN_004d40c0(); needs module+0x3c (else RWERROR 0x23); request handle = module+0x10, type 1, data 0, size 2, then data = FUN_004e14a0(heap, 4) holding the two ushort args; FUN_004d40d0(handle, req, 0), FUN_004e12b0(heap, data) on both paths, returns 1 / 0. From 51.5%: request struct, success branch first. Relocheck OK. */
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
void FUN_004e12b0(void *heap, void *mem);
void *FUN_004e14a0(void *heap, int size);
#define MOD ((Mod *)(DAT_007d3ff8 + DAT_00911d00))

// MATCH: 0x004cd430 FUN_004cd430
int FUN_004cd430(unsigned short param_1, unsigned short param_2)
{
  RwError err;
  Req2 *req;
  void *heap;
  heap = FUN_004d40c0();
  if (MOD->reqptr) {
    req = &MOD->req;
    req->handle = MOD->h10;
    req->type = 1;
    req->data = 0;
    req->size = 2;
    req->data = FUN_004e14a0(heap, 4);
    ((unsigned short *)req->data)[0] = param_1;
    ((unsigned short *)req->data)[1] = param_2;
    if (FUN_004d40d0(req->handle, req, 0)) {
      FUN_004e12b0(heap, req->data);
      return 1;
    }
    FUN_004e12b0(heap, req->data);
    return 0;
  }
  err.pluginID = 1;
  err.errorCode = FUN_004d7ff0(0x23);
  FUN_004d8480(&err);
  return 0;
}

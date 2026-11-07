// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cbd30 (h2): 100% (regs 100%), --bytes BYTE-IDENTICAL (jump table included). RwStreamRead shape:
   RwStream struct with the memory/file/custom union, RwError {pluginID, errorCode} local, memcpy intrinsic. Key
   move: in the memory case assign length = avail BEFORE the error call (keeps length in ebp). Flags /G5 (raw run
   picked the game flags at 48.8%). Residual: none. relocheck OK. */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
typedef struct { int pluginID; int errorCode; } RwError;
typedef uint (*sfnread_t)(void *, void *, uint);
typedef struct {
  int type; int accessType; int position;
  union {
    struct { int position; uint nSize; unsigned char *memBlock; } memory;
    struct { void *fpFile; } file;
    struct { void *sfnclose; sfnread_t sfnread; void *sfnwrite; void *sfnskip; void *data; } custom;
  } Type;
} RwStream;
int FUN_004d7ff0(int p0);
RwError *FUN_004d8480(RwError *p0);
uint FUN_00550950(void *p0, uint p1, uint p2, void *p3);
int FUN_00550af0(void *p0);
// MATCH: 0x004cbd30 FUN_004cbd30
uint FUN_004cbd30(RwStream *param_1,void *param_2,uint param_3)
{
  RwError err;
  switch (param_1->type) {
  case 1:
  case 2:
    {
      void *fp = param_1->Type.file.fpFile;
      uint n = FUN_00550950(param_2, 1, param_3, fp);
      if (n != param_3) {
        if (FUN_00550af0(fp)) {
          err.pluginID = 1;
          err.errorCode = FUN_004d7ff0(5);
          FUN_004d8480(&err);
        } else {
          err.pluginID = 1;
          err.errorCode = FUN_004d7ff0(0x8000001a);
          FUN_004d8480(&err);
        }
      }
      return n;
    }
  case 3:
    if (param_3 > param_1->Type.memory.nSize - param_1->Type.memory.position) {
      param_3 = param_1->Type.memory.nSize - param_1->Type.memory.position;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(5);
      FUN_004d8480(&err);
    }
    memcpy(param_2, param_1->Type.memory.memBlock + param_1->Type.memory.position, param_3);
    param_1->Type.memory.position += param_3;
    return param_3;
  case 4:
    return param_1->Type.custom.sfnread(param_1->Type.custom.data, param_2, param_3);
  default:
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0xe);
    FUN_004d8480(&err);
    return 0;
  }
}

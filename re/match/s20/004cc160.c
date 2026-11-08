// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc160 (h2): 100% (regs 100%). RwStreamClose shape: switch with case 2 = (fclose(fp) == 0), case 3
   memory export to RwMemory when accessType != 1, case 4 custom close, default RWERROR(0xe); then free-list free
   through RwGlobals (DAT_007d3ff8 + 0x11c) with the module offset second. Raw 57.6%. Residual: none. relocheck OK. */
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
typedef struct { int pluginID; int errorCode; } RwError;
typedef void (*sfnclose_t)(void *);
typedef void (*freefn_t)(int, void *);
typedef struct { void *start; uint length; } RwMemory;
typedef struct {
  int type; int accessType; int position;
  union {
    struct { uint position; uint nSize; unsigned char *memBlock; } memory;
    struct { void *fpFile; } file;
    struct { sfnclose_t sfnclose; void *sfnread; void *sfnwrite; void *sfnskip; void *data; } custom;
  } Type;
  int rwOwned;
} RwStream;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d45c4, 0x007d45c4);
int FUN_004d7ff0(int p0);
RwError *FUN_004d8480(RwError *p0);
int FUN_00550910(void *p0);
// MATCH: 0x004cc160 FUN_004cc160
int FUN_004cc160(RwStream *param_1,RwMemory *param_2)
{
  int result;
  RwError err;
  switch (param_1->type) {
  case 1:
    result = 1;
    break;
  case 2:
    result = (FUN_00550910(param_1->Type.file.fpFile) == 0);
    break;
  case 3:
    result = 1;
    if (param_1->accessType != 1 && param_2 != 0) {
      param_2->start = param_1->Type.memory.memBlock;
      param_2->length = param_1->Type.memory.position;
    }
    break;
  case 4:
    if (param_1->Type.custom.sfnclose) {
      param_1->Type.custom.sfnclose(param_1->Type.custom.data);
    }
    result = 1;
    break;
  default:
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0xe);
    FUN_004d8480(&err);
    return 0;
  }
  if (param_1->rwOwned) {
    (*(freefn_t *)(DAT_007d3ff8 + 0x11c))(*(int *)(DAT_007d3ff8 + DAT_007d45c4), param_1);
  }
  return result;
}

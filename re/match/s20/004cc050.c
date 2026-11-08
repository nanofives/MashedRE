// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc050 (h2): 78.5% (regs 81.7%). Raw 61.3%. RwStreamSkip shape (same RwStream / RwError types as
   0x004cbd30). Tried: inverted memory-case test (73.1%), ?: for the custom case, block-scoped RWERROR (do {..}
   while (0)), permuter (no gain). Residual (b): the original pushes esi at entry and edi only inside the file case
   (edi is used only there); ours pushes edi at entry for every path, which shifts the error-struct offsets and the
   epilogues. */
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
typedef int (*sfnskip_t)(void *, uint);
typedef struct {
  int type; int accessType; int position;
  union {
    struct { uint position; uint nSize; unsigned char *memBlock; } memory;
    struct { void *fpFile; } file;
    struct { void *sfnclose; void *sfnread; void *sfnwrite; sfnskip_t sfnskip; void *data; } custom;
  } Type;
} RwStream;
int FUN_004d7ff0(int p0);
RwError *FUN_004d8480(RwError *p0);
int FUN_005509b0(void *p0, uint p1, int p2);
int FUN_00550af0(void *p0);
// MATCH: 0x004cc050 FUN_004cc050
RwStream *FUN_004cc050(RwStream *param_1,uint param_2)
{
  RwError err;
  if (param_2 == 0) return param_1;
  switch (param_1->type) {
  case 1:
  case 2:
    {
      void *fp = param_1->Type.file.fpFile;
      if (FUN_005509b0(fp, param_2, 1)) {
        if (FUN_00550af0(fp)) {
          err.pluginID = 1;
          err.errorCode = FUN_004d7ff0(5);
          FUN_004d8480(&err);
        }
        return 0;
      }
      return param_1;
    }
  case 3:
    if (param_1->Type.memory.position + param_2 > param_1->Type.memory.nSize) {
      param_1->Type.memory.position = param_1->Type.memory.nSize;
      err.pluginID = 1;
      err.errorCode = FUN_004d7ff0(5);
      FUN_004d8480(&err);
      return 0;
    }
    param_1->Type.memory.position += param_2;
    return param_1;
  case 4:
    if (!param_1->Type.custom.sfnskip(param_1->Type.custom.data, param_2)) return 0;
    return param_1;
  default:
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0xe);
    FUN_004d8480(&err);
    return 0;
  }
}

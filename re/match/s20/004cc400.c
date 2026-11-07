// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc400 (h2): 100% (regs 100%). _rwStreamReadChunkHeader shape: RwMark local (12 B) read through
   0x004cbd30, RwChunkHeaderInfo local (20 B) with isComplex = 0x004cc4f0(&info); version/buildNum unpack as if ((id
   & 0xffff0000) == 0) {..} else {..} (the ternary form gave 70.7%, the inverted if 92.9%). Raw 47.1%. Residual:
   none. relocheck OK. */
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
typedef struct { uint type; uint length; uint libraryID; } RwMark;
typedef struct { uint type; uint length; uint version; uint buildNum; int isComplex; } RwChunkHeaderInfo;
uint FUN_004cbd30(void *p0, void *p1, uint p2);
int FUN_004cc4f0(RwChunkHeaderInfo *p0);
int FUN_004d7ff0(int p0);
RwError *FUN_004d8480(RwError *p0);
// MATCH: 0x004cc400 FUN_004cc400
int FUN_004cc400(void *param_1,uint *param_2,uint *param_3,uint *param_4,uint *param_5)
{
  RwMark mark;
  RwChunkHeaderInfo info;
  RwError err;
  if (FUN_004cbd30(param_1, &mark, sizeof(mark)) != sizeof(mark)) {
    err.pluginID = 1;
    err.errorCode = FUN_004d7ff0(0x8000001a);
    FUN_004d8480(&err);
    return 0;
  }
  info.type = mark.type;
  info.length = mark.length;
  if ((mark.libraryID & 0xffff0000) == 0) {
    info.version = mark.libraryID << 8;
    info.buildNum = 0;
  } else {
    info.version = (((mark.libraryID >> 14) & 0x3ff00) + 0x30000) | ((mark.libraryID >> 16) & 0x3f);
    info.buildNum = mark.libraryID & 0xffff;
  }
  info.isComplex = FUN_004cc4f0(&info);
  if (param_2) *param_2 = info.type;
  if (param_3) *param_3 = info.length;
  if (param_5) *param_5 = info.buildNum;
  if (param_4) *param_4 = info.version;
  return 1;
}

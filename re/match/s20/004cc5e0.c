// FLAGS: /O2 /Ob1 /G5
/* MATCH-S20 0x004cc5e0 (h2): 59.8% (regs 60.9%). Raw 46.0% (game flags). RwStreamFindChunk shape: chunk-header read
   0x004cc400, skip 0x004cc050, version window 0x35000..0x37002, RWERROR(0x80000004). Permuter moved err.pluginID
   (+1.4). Residual (a): the original frame is 0xc: readLength lives in the dead home slot of the stream parameter
   (Ghidra shows &param_1) and the RwError overlaps readVersion; 9176 gives a 0x14 frame with separate slots. Tried
   all 24 declaration orders, block-scoped RWERROR, /G5 vs /GS vs no /G (flag change made it worse), /Ox, /Og forms.
   Same param-home packing shows in 0x004d3510 and 0x004d07b0. */
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
void *FUN_004cc050(void *p0, uint p1);
int FUN_004cc400(void *p0, uint *p1, uint *p2, uint *p3, uint *p4);
int FUN_004d7ff0(int p0);
RwError *FUN_004d8480(RwError *p0);
// MATCH: 0x004cc5e0 FUN_004cc5e0
int FUN_004cc5e0(void *param_1,uint param_2,uint *param_3,uint *param_4)
{
  uint readType;
  uint readLength;
  uint readVersion;
  RwError err;
  if (!FUN_004cc400(param_1, &readType, &readLength, &readVersion, 0)) return 0;
  while (readType != param_2) {
    if (!FUN_004cc050(param_1, readLength)) return 0;
    if (!FUN_004cc400(param_1, &readType, &readLength, &readVersion, 0)) return 0;
  }
  if (readVersion < 0x35000) {
    err.errorCode = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&err);
    err.pluginID = 1;
    return 0;
  }
  if (readVersion > 0x37002) {
    err.errorCode = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&err);
    err.pluginID = 1;
    return 0;
  }
  if (param_3) *param_3 = readLength;
  if (param_4) *param_4 = readVersion;
  return 1;
}
